
#include <ctime>
#include <random>
#include "core/graphtinker.h"

using std::vector;

namespace graphtinker
{

    Graphtinker::Graphtinker()
    {
        _config = new Config("file_path");
        CreateGraph();
    };

    Graphtinker::Graphtinker(string file_path)
    {
        _config = new Config(file_path);
        CreateGraph();
    }

    Graphtinker::~Graphtinker()
    {
        DLOG(INFO) << "Graphtinker::~Graphtinker : Graphtinker destructor called";

        edge_block_array_m_.clear();
        edge_block_array_o_.clear();

        if (translator_handler_ != nullptr)
        {
            delete translator_handler_;
        }
        if (vertices_handler_ != nullptr)
        {
            delete vertices_handler_;
        }

#ifdef EN_CAL
        cal_edgeblock_array_.clear();
        cal_lva_.clear();
#endif
    }

    void Graphtinker::CreateGraph()
    {

        _config->ConfigGraph(this);
        _config->PrintAll();

        work_blocks_per_page_ = page_block_height_ / WORK_BLOCK_HEIGHT;
        work_blocks_per_subblock_ = sub_block_height_ / WORK_BLOCK_HEIGHT;

        edge_block_array_m_.resize((num_vertices_ * work_blocks_per_page_));
        edge_block_array_o_.resize((100 * work_blocks_per_page_));
        lvatracker_.mark = 100;

        LOG(INFO) << "lvatracker_ start mark : " << lvatracker_.mark;

#ifdef EN_CAL
        cal_edgeblock_array_.resize(((num_vertices_ * page_block_height_) / LLEDGEBLOCKSIZE));
        cal_lva_.resize(((num_vertices_ + (LVACOARSENESSWIDTH - 1)) / LVACOARSENESSWIDTH));
        cal_edgeblock_tracker_.ptraddr = 0;
        initialize_lvas();
#endif

        // vertices & translator
        if (updatev_ == SELF)
        {
            vertices_handler_ = new Vertices(vertex_range_, 0.15);
        }

        if ((sgh_for_vtx_id_ == SELF) || (sgh_for_adjvtx_id_ == SELF))
        {
            translator_handler_ = new Translator(vertex_range_);
        }

// metadata (for delete and crumple in)
// enable ---- crumple in on delete
#ifdef EN_DCI
        edgeblock_parentinfo.resize((num_edges_ / (page_block_height_ / 2)));
#endif
        LOG(INFO) << "edge_block_array_m_.size() : " << edge_block_array_m_.size();
        LOG(INFO) << "edge_block_array_o_.size() : " << edge_block_array_o_.size();
// enable ---- linked-list graph data structure
#ifdef EN_CAL
        LOG(INFO) << "cal_edgeblock_array_.size() : " << cal_edgeblock_array_.size();
        LOG(INFO) << "cal_lva_.size() : " << cal_lva_.size();
        LOG(INFO) << "cal_edgeblock_tracker_.ptraddr : " << cal_edgeblock_tracker_.ptraddr;
#endif
        return;
    }

    void Graphtinker::InsertEdge(Edge &edge)
    {
        uint edge_update_cmd = INSERTEDGE;
        if (sgh_for_vtx_id_ == SELF)
        {
            edge.src = translator_handler_->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == SELF)
        {
            edge.dst = translator_handler_->GetLocalVid((vertexid_t)edge.dst);
        }
        check_whether_to_resize_edgeblockarray_m(edge.src);
        UpdateEdge(edge, edge_update_cmd, vertices_handler_);
        return;
    }

    void Graphtinker::InsertEdge(Edge &edge, Vertices *external_vertices_handler)
    {
        uint edge_update_cmd = INSERTEDGE;
        if (sgh_for_vtx_id_ == SELF)
        {
            edge.src = (uint)translator_handler_->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == SELF)
        {
            edge.dst = (uint)translator_handler_->GetLocalVid((vertexid_t)edge.dst);
        }
        check_whether_to_resize_edgeblockarray_m(edge.src);
        UpdateEdge(edge, edge_update_cmd, external_vertices_handler);
        return;
    }

    void Graphtinker::InsertEdge(Edge &edge, Translator *translator_handler)
    {
        if (edge.src > vertex_range_)
        {
            LOG(ERROR) << "Graphtinker::InsertEdge : bug, out of range19. src : " << edge.src << ", vertex_range_ : "
                       << vertex_range_;
        }
        uint edge_update_cmd = INSERTEDGE;
        if (sgh_for_vtx_id_ == OTHER)
        {
            edge.src = (uint)translator_handler->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == OTHER)
        {
            edge.dst = (uint)translator_handler->GetLocalVid((vertexid_t)edge.dst);
        }
        check_whether_to_resize_edgeblockarray_m(edge.src);
        UpdateEdge(edge, edge_update_cmd, vertices_handler_);
        return;
    }

    void Graphtinker::InsertEdge(Edge &edge, Vertices *external_vertices_handler, Translator *translator_handler)
    {
        if (edge.src > vertex_range_)
        {
            LOG(ERROR) << "Graphtinker::InsertEdge : bug, out of range199. src : " << edge.src << ", vertex_range_ : "
                       << vertex_range_;
        }
        uint edge_update_cmd = INSERTEDGE;
        if (sgh_for_vtx_id_ == OTHER)
        {
            edge.src = (uint)translator_handler->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == OTHER)
        {
            edge.dst = (uint)translator_handler->GetLocalVid((vertexid_t)edge.dst);
        }
        check_whether_to_resize_edgeblockarray_m(edge.src);
        UpdateEdge(edge, edge_update_cmd, external_vertices_handler);
        return;
    }

    void Graphtinker::BatchInsertEdge(const char *path, uint batch_size)
    {
        LOG(INFO) << std::endl;
        LOG(INFO) << "----------Bath insertion edge-----------";
        vector<Edge> edges;
        uint edges_inserted = 0;
        uint total_edges_inserted = 0;
        float insertion_timelapse_ms = 0;
        float total_insertion_time_lapse_ms = 0;
        float throughput = 0;
        float runningthroughput = 0;
        const auto& edge_type_array = _config->edge_type_array();
        const auto& edge_type_map = _config->edge_type_map();
        size_t type_num = edge_type_array.size();
        // std::default_random_engine e;
        // std::uniform_int_distribution<uint> u(0,edge_type_map.size() - 1);
        
        uint batchid = 0;

        FILE *file = fopen(path, "r");
        if (file == NULL)
        {
            LOG(ERROR) << "error : cannot find the location of input file rmat500k8m.edges (test_updatesingleedge)";
        }

        while (!feof(file))
        {
            edges.clear();

            // load batch of edges
            uint src = 0;
            uint dst = 0;
            uint ew = 0;
            for (uint i = 0; i < BATCH_SIZE; i++)
            {
                if (feof(file))
                {
                    LOG(INFO) << "finished reading from file (updates_test)";
                    break;
                }
                if (fscanf(file, "%i", &src) < 0)
                {
                    LOG(ERROR) << "fscan error (updates_test)";
                    break;
                }

                if (fscanf(file, "%i", &dst) < 0)
                {
                    LOG(ERROR) << "fscan error (updates_test)";
                    break;
                }

                Edge edge;
                edge.src = src;
                edge.dst = dst;
                edge.type= edge_type_map.at(edge_type_array.at(i % type_num));
                edge.weight = 1;
                edges.push_back(edge);
            }

            // insert edges
            edges_inserted = 0;
            std::clock_t start = std::clock();
            for (auto&& _ : edges)
            {
                edges_inserted += 1;
                total_edges_inserted += 1;
                InsertEdge(_);
                if (!is_directed)
                {
                    edges_inserted += 1;
                    total_edges_inserted += 1;
                    InsertEdge(_);
                }
            }
            insertion_timelapse_ms = 0;
            insertion_timelapse_ms = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
            total_insertion_time_lapse_ms += insertion_timelapse_ms;
            throughput = ((edges_inserted / insertion_timelapse_ms) * 1000) / 1000000;
            runningthroughput = ((total_edges_inserted / total_insertion_time_lapse_ms) * 1000) / 1000000;
            LOG(INFO) << std::endl;
            LOG(INFO) << "batch : " << batchid;
            LOG(INFO) << "edges loaded : " << edges_inserted << " edges";
            LOG(INFO) << "number of edges : " << PrintEdgeCount() << " edges";
            LOG(INFO) << "number of unique edges : " << PrintUniqueEdgeCount() << " edges";
#ifdef EN_CAL
            LOG(INFO) << "number of edges in ll : " << PrintCALEdgeCount() << " edges";
#endif
            LOG(INFO) << "timelapse (ms) : " << (float)insertion_timelapse_ms << " ms";
            LOG(INFO) << "total timelapse (ms) : " << (float)total_insertion_time_lapse_ms << " ms";
            LOG(INFO) << "running throughput : " << (float)runningthroughput << " million edges per sec";
            batchid += 1;
        }

        fclose(file);
        edges.clear();
        return;
    }

    void Graphtinker::DeleteEdge(Edge& edge)
    {
        uint edge_update_cmd = DELETEEDGE;
        if (sgh_for_vtx_id_ == SELF)
        {
            edge.src = translator_handler_->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == SELF)
        {
            edge.dst = translator_handler_->GetLocalVid((vertexid_t)edge.dst);
        }
        UpdateEdge(edge, edge_update_cmd, vertices_handler_);
        return;
    }

    void Graphtinker::DeleteEdge(Edge& edge, Vertices *external_vertices_handler)
    {
        uint edge_update_cmd = DELETEEDGE;
        if (sgh_for_vtx_id_ == SELF)
        {
            edge.src = translator_handler_->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == SELF)
        {
            edge.dst = translator_handler_->GetLocalVid((vertexid_t)edge.dst);
        }
        UpdateEdge(edge, edge_update_cmd, external_vertices_handler);
        return;
    }

    void Graphtinker::BatchDeleteEdge(const char *path, uint batch_size)
    {
        LOG(INFO) << std::endl;
        LOG(INFO) << "-----------deletions started--------------";
        vector<Edge> edges;
        uint edges_deleted = 0;
        uint total_edges_deleted = 0;
        float deletion_timelapse_ms = 0;
        float total_deletion_time_lapse_ms = 0;
        float throughput = 0;
        float runningthroughput = 0;
        uint batchid = 0;
        const auto& edge_type_array = _config->edge_type_array();
        const auto& edge_type_map = _config->edge_type_map();
        size_t type_num = edge_type_array.size();
        // std::default_random_engine e;
        // std::uniform_int_distribution<uint> u(0,edge_type_map.size() - 1);

        FILE *file = fopen(path, "r");
        if (file == NULL)
        {
            LOG(ERROR) << "error : cannot find the location of input file rmat500k8m.edges (test_updatesingleedge)";
        }

        while (!feof(file))
        {
            edges.clear();

            // load batch of edges
            uint src = 0;
            uint dst = 0;
            uint ew = 0;
            for (uint i = 0; i < BATCH_SIZE; i++)
            {
                if (feof(file))
                {
                    LOG(INFO) << "finished reading from file (updates_test)";
                    break;
                }
                if (fscanf(file, "%i", &src) < 0)
                {
                    LOG(ERROR) << "fscan dataset file error";
                    break;
                }
                if (fscanf(file, "%i", &dst) < 0)
                {
                    LOG(ERROR) << "fscan data file error";
                    break;
                }
                Edge edge;
                edge.src = src;
                edge.dst = dst;
                edge.type= edge_type_map.at(edge_type_array.at(i % type_num));
                edge.weight = 1;
                edges.push_back(edge);
            }

            edges_deleted = 0;
            std::clock_t start = std::clock();
            for (auto&& _ : edges)
            {
                edges_deleted += 1;
                total_edges_deleted += 1;
                DeleteEdge(_);
                if (!is_directed)
                {
                    edges_deleted += 1;
                    total_edges_deleted += 1;
                    DeleteEdge(_);
                }
            }
            
            // print batch delete info
            deletion_timelapse_ms = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
            total_deletion_time_lapse_ms += deletion_timelapse_ms;
            throughput = ((edges_deleted / deletion_timelapse_ms) * 1000) / 1000000;
            runningthroughput = ((total_edges_deleted / total_deletion_time_lapse_ms) * 1000) / 1000000;
            LOG(INFO) << std::endl;
            LOG(INFO) << "batch : " << batchid;
            LOG(INFO) << "edges loaded : " << edges_deleted << " edges";
            LOG(INFO) << "number of edges : " << PrintEdgeCount() << " edges";
            LOG(INFO) << "number of unique edges : " << PrintUniqueEdgeCount() << " edges";
#ifdef EN_CAL
            LOG(INFO) << "number of unique edges in ll : " << PrintCALEdgeCount() << " edges";
#endif
            LOG(INFO) << "timelapse (ms) : " << (float)deletion_timelapse_ms << " ms";
            LOG(INFO) << "running throughput : " << (float)runningthroughput << " million edges per sec";
#ifdef EN_DCI
            LOG(INFO) << "svs's size : " << tinker.print_svs_size();
            LOG(INFO) << "freed edgeblock list's size : " << tinker.print_freed_edgeblock_list_size();
#endif
            batchid += 1;
        }

        fclose(file);
        edges.clear();
        return;
    }
    /**
     * insert_edge和delete_edge的主逻辑函数。
     * 该函数包含以下几个单元:
     *  
     */
    void
    Graphtinker::UpdateEdge(const Edge &e, uint edge_update_cmd, Vertices *external_vertices_handler)
    {
        DLOG(INFO) << "<< Update Edge >>";
        DLOG(INFO) << "edge (" << e.src <<  ", " << e.dst << ")";
        // create an edge object
        edge_t edge;
        edge.vtx_id = e.src;
        edge.adjvtx_id = e.dst;
        edge.type = e.type;
        edge.weight = e.weight;
        edge.flag = VALID;
#ifdef EN_CAL
        edge.heba_hvtx_id = -1;
        edge.heba_workblockid = -1;
        edge.heba_loffset = -1;
#endif

        // variables & parameter registers and RAM declared
        uint geni;
        vertexid_t vtx_id;
        uint tripiteration_lcs = 0;

        bucket_t adjvtx_id_hash = 0;
        work_block_t work_block;

        //margins
        margin_t work_block_margin;
        margin_t start_wblkmargin;
        margin_t first_wblkmargin;
        margin_t sub_block_margin;

        // unit flow负责状态机全流程
        UnitFlow *unit_flow = new UnitFlow(this);
        UnitOption *unit_option = unit_flow->unit_option;

//LL unit
#ifdef EN_CAL
        cal_unit_cmd_t cal_unit_cmd;
#endif

#ifdef EN_DCI
        crumple_in_cmd_t deleteandcrumpleincmd;
#endif

        //exit condition (this should be after: 'insert-unit parameters and reports')
        uint quitstatus = 0;

        // compute hash and margin
        geni = 1;
        vtx_id = edge.vtx_id;
        tripiteration_lcs = 0;
        adjvtx_id_hash = GoogleHash(edge.adjvtx_id, edge.type, geni);
        FindWorkBlockMargin(adjvtx_id_hash, &work_block_margin); //find work block margins
        FindSubBlockMargin(adjvtx_id_hash, &sub_block_margin);
        start_wblkmargin = work_block_margin;
        first_wblkmargin = work_block_margin;
        DLOG(INFO) << "adjvtx_id_hash = " << adjvtx_id_hash;
        DLOG(INFO) << "work_block_margin = [" << work_block_margin.top << " , " << work_block_margin.bottom << "]";
#ifdef EN_DCI
        // keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
        uint lastgenworkblockaddr = GetEdgeblockOffset(vtx_id) + work_block_margin.top / WORK_BLOCK_HEIGHT;
#endif

#ifdef EN_CAL
        unit_option->InitCalUnit();
        unit_option->ClearCalAdr();
#endif
#ifdef EN_DCI
        init_deleteandcrumplein_verdictcmd(&deleteandcrumpleincmd);
#endif

        //init the options of the whole unit flow
        unit_option->module_unit_cmd.mode = FIND_MODE;
        unit_option->InitModuleUnitParams(edge);
        unit_option->InitLoadUnit();
        unit_option->InitInsertUnit(edge, adjvtx_id_hash);
        unit_option->InitFindUnit(edge, adjvtx_id_hash);
        unit_option->InitWritebackUnit();

        // 首先更新vertex property array
        if (updatev_ != OFF)
        {
            vertexid_t vtx_id_g = edge.vtx_id;
            vertexid_t adjvtx_id_g = edge.adjvtx_id;
            if (sgh_for_vtx_id_ == SELF)
            {
                vtx_id_g = translator_handler_->GetGlobalVid(edge.vtx_id);
            }
            if (sgh_for_adjvtx_id_ == SELF)
            {
                adjvtx_id_g = translator_handler_->GetGlobalVid(edge.adjvtx_id);
            }
            external_vertices_handler->UpdateVertexProperty(vtx_id_g, adjvtx_id_g, edge_update_cmd, is_directed_);
        }

        //******************** main loop ***********
        uint infiniti;
        for (infiniti = 0; infiniti < SOMELARGENO; infiniti++)
        {
            // 1. load work_block
            if (geni == 1) {
                unit_flow->LoadUnit(work_block_margin, vtx_id, edge_block_array_m_, &work_block);
            } else {
                unit_flow->LoadUnit(work_block_margin, vtx_id, edge_block_array_o_, &work_block);
            }

            // 2. load compute-unit params
            unit_flow->LoadParams(work_block_margin, adjvtx_id_hash);

            // 3. find_unit or insert_unit
            if (unit_option->module_unit_cmd.mode == FIND_MODE) {
                unit_flow->FindUnit(work_block_margin, adjvtx_id_hash, &work_block, edge_update_cmd);
            } else if (unit_option->module_unit_cmd.mode == INSERT_MODE) {
                unit_flow->InsertUnit(work_block_margin, adjvtx_id_hash, &work_block, &edge, geni);
            }

            // 4. 
            unit_flow->InferenceUnit( edge_update_cmd, &work_block_margin, sub_block_margin,
                start_wblkmargin, vtx_id
#ifdef EN_CAL
                ,
                &cal_unit_cmd
#endif
#ifdef EN_DCI
                ,
                &deleteandcrumpleincmd
#endif
            );

            // 5.
            unit_flow->WritebackUnit(
                edge,
                &work_block,
                edge_block_array_m_,
                edge_block_array_o_,
                &lvatracker_,
                vtx_id,
                first_wblkmargin,
                sub_block_margin,
                geni,
                edge_update_cmd
#ifdef EN_CAL
                ,
                cal_edgeblock_array_
#endif
#ifdef EN_DCI
                ,
                edgeblock_parentinfo
                ,
                lastgenworkblockaddr, svs
#endif
            );

            // Optional Feature: delete and crumple in
#ifdef EN_DCI
            CrumpleInUnit(
                writeback_unit_cmd,
                find_report,
                edge,
                edge_block_array_m_,
                edge_block_array_o_,
#ifdef EN_CAL
                cal_edgeblock_array_,
#endif
                edgeblock_parentinfo,
                vtx_id,
                work_block_margin,
                sub_block_margin,
                geni,
                deleteandcrumpleincmd, svs, freed_edgeblock_list);
#endif
            // Optional Feature: CAL
#ifdef EN_CAL
            unit_flow->CalUnit(
                edge,
                edge_block_array_m_,
                edge_block_array_o_,
                geni,
                cal_edgeblock_array_, cal_lva_, &cal_edgeblock_tracker_);
#endif
            // Interval unit
            unit_flow->IntervalUnit(
                &work_block_margin,
                &sub_block_margin,
                &start_wblkmargin,
                &first_wblkmargin,
                &vtx_id,
                &adjvtx_id_hash,
                edge,
                edge_update_cmd,
                &tripiteration_lcs,
                &geni,
                &quitstatus
#ifdef EN_CAL
                ,
                &cal_unit_cmd
#endif
#ifdef EN_DCI
                ,
                &lastgenworkblockaddr,
                &deleteandcrumpleincmd
#endif
            );


            if (quitstatus == 1)
            {
                DLOG(INFO) << "Update signle edge success !";
                return;
            }

            if (tripiteration_lcs > 100)
            {
                LOG(ERROR) << "bug! : trip iteration too much!, exiting...";
                return;
            }

            if (geni > 500000)
            {
                LOG(ERROR) << " too many generations (" << geni << ") spun!, exiting...";
                LOG(ERROR) << "search_report.is_search_stop = " << unit_option->search_report.is_search_stop;
                LOG(ERROR) << "search_report.is_search_success = " << unit_option->search_report.is_search_success;
                LOG(ERROR) << "insert_report.exittype = " << unit_option->insert_report.exittype;
                LOG(ERROR) << "module_unit_cmd.mode = " << unit_option->module_unit_cmd.mode;
                return;
            }
        }
        LOG(ERROR) << "bug! : infiniti exhausted : infiniti = " << infiniti << "";
        LOG(ERROR) << "search_report.is_search_stop = " << unit_option->search_report.is_search_stop;
        LOG(ERROR) << "search_report.is_search_success = " << unit_option->search_report.is_search_success;
        LOG(ERROR) << "insert_report.exittype = " << unit_option->insert_report.exittype;
        LOG(ERROR) << "module_unit_cmd.mode = " << unit_option->module_unit_cmd.mode;
        return;
    }

    vertexid_t Graphtinker::retrieve_edges(vertexid_t vid, vector<edge_tt> &edges)
    {
        vector<clusterptr_t> clusterptrs;
        vertexid_t basevid = vid;
        uint _geni = 1;

        // load edges
        clusterptrs.push_back(vid);
        uint len = clusterptrs.size();
        uint wps = work_blocks_per_subblock_;

        while (true)
        {
            for (uint i = 0; i < len; i++)
            {
                vid = clusterptrs.back();
                clusterptrs.pop_back();

                uint ebaoffset = GetEdgeblockOffset(vid);
                for (uint t = 0; t < work_blocks_per_page_; t++)
                {
                    work_block_t edgeset;
                    if (_geni == 1)
                    {
                        edgeset = edge_block_array_m_[(ebaoffset + t)];
                    }
                    else
                    {
                        edgeset = edge_block_array_o_[(ebaoffset + t)];
                    }

                    for (uint k = 0; k < WORK_BLOCK_HEIGHT; k++)
                    {
                        if (edgeset.edges[k].flag == VALID)
                        {
                            edges.push_back(edgeset.edges[k]);
                        }
                    }

                    if (((t % wps) == 0) && (edgeset.clusterinfo.flag == VALID))
                    {
                        clusterptrs.push_back(edgeset.clusterinfo.data);
                    }
                }
            }

            len = clusterptrs.size();
            if (len == 0)
            {
                break;
            }
            else
            {
                _geni += 1;
            }
        }
        return basevid;
    }

    uint Graphtinker::PrintEdgeCount()
    {
        uint edgecount = 0;
        edgecount += Graphtinker::GetEdgeCount(edge_block_array_m_, GetEdgeBlockArrayHeight(edge_block_array_m_));
        edgecount += Graphtinker::GetEdgeCount(edge_block_array_o_, lvatracker_.mark);
        return edgecount;
    }

    uint Graphtinker::GetEdgeCount(vector<work_block_t> edge_block_array_x, uint height)
    {
        uint edgecount = 0;
        for (vertexid_t vid = 0; vid < height; vid++)
        {
            uint offset = GetEdgeblockOffset(vid);
            for (uint j = offset; j < (offset + work_blocks_per_page_); j++)
            {
                for (uint k = 0; k < WORK_BLOCK_HEIGHT; k++)
                {
                    if (edge_block_array_x[j].edges[k].flag == VALID)
                    {
                        edgecount += edge_block_array_x[j].edges[k].weight;
                    }
                }
            }
        }
        return edgecount;
    }

    uint Graphtinker::PrintUniqueEdgeCount()
    {
        uint edgecount = 0;
        edgecount += Graphtinker::GetUniqueEdgeCount(edge_block_array_m_,
                                                     GetEdgeBlockArrayHeight(edge_block_array_m_));
        edgecount += Graphtinker::GetUniqueEdgeCount(edge_block_array_o_, lvatracker_.mark);
        return edgecount;
    }

    uint Graphtinker::GetUniqueEdgeCount(vector<work_block_t> edge_block_array_x, uint height)
    {
        uint edgecount = 0;
        for (vertexid_t vid = 0; vid < height; vid++)
        {
            uint offset = GetEdgeblockOffset(vid);
            for (uint j = offset; j < (offset + work_blocks_per_page_); j++)
            {
                for (uint k = 0; k < WORK_BLOCK_HEIGHT; k++)
                {
                    if (edge_block_array_x[j].edges[k].flag == VALID)
                    {
                        edgecount += 1;
                    }
                }
            }
        }
        return edgecount;
    }

    uint Graphtinker::PrintCALEdgeCount()
    {
        uint uniqueedgecount = 0;
        uniqueedgecount += ll_countedges(cal_edgeblock_array_);
        return uniqueedgecount;
    }

    uint Graphtinker::ll_countedges(vector<cal_edgeblock_t> &cal_edgeblock_array_)
    {
        uint uniqueedgecount = 0;
        for (uint i = 0; i < cal_edgeblock_array_.size(); i++)
        { //ll_edgeblock_array_size
            for (uint j = 0; j < LLEDGEBLOCKSIZE; j++)
            {
                if (cal_edgeblock_array_[i].ll_edgeblock[j].flag == VALID)
                {
                    uniqueedgecount += cal_edgeblock_array_[i].ll_edgeblock[j].weight;
                }
            }
        }
        return uniqueedgecount;
    }

#ifdef EN_DCI
    uint Graphtinker::print_svs_size()
    {
        return svs.size();
    }

    uint Graphtinker::print_freed_edgeblock_list_size()
    {
        return freed_edgeblock_list.size();
    }
#endif

    void Graphtinker::initialize_lvas()
    {
        for (uint i = 0; i < cal_lva_.size(); i++)
        {
            cal_lva_[i].baseaddr = 0;
            cal_lva_[i].lastlocalbaseaddr = 0;
            cal_lva_[i].lastlocaladdr = 0;
            cal_lva_[i].totaledgecount = 0;
            cal_lva_[i].flag = INVALID;
        }
        return;
    }

    // getter and setter
    const uint Graphtinker::vertex_range() const { return vertex_range_; }
    const bool Graphtinker::is_directed() const { return is_directed_; }
    const uint Graphtinker::translator_tracker_mark() const { return translator_handler_->translator_tracker_mark(); }
    const uint Graphtinker::work_blocks_per_page() const { return work_blocks_per_page_; }
    const uint Graphtinker::work_blocks_per_subblock() const { return work_blocks_per_subblock_; }
    const uint Graphtinker::sub_block_height() const { return sub_block_height_; }
    const Translator *Graphtinker::translator() const { return translator_handler_; }
    const Vertices *Graphtinker::vertices() const { return vertices_handler_; }
    const vector<work_block_t> &Graphtinker::edge_block_array_m() const { return edge_block_array_m_; }
    const vector<work_block_t> &Graphtinker::edge_block_array_o() const { return edge_block_array_o_; }
    const vector<cal_logical_vertex_entity_t> &Graphtinker::ll_logicalvertexentity() const { return cal_lva_; }
    const vector<cal_edgeblock_t> &Graphtinker::ll_edge_block_array() const { return cal_edgeblock_array_; }

    void Graphtinker::PrintEdgeblockArray(vertexid_t begin, vertexid_t end)
    {
        vector<edge_tt> edges;

        for (uint i = begin; i < end; i++)
        {
            retrieve_edges(0, edges);
            for (uint j = 0; j < edges.size(); j++)
            {
                LOG(INFO) << "Graphtinker::PrintEdgeblockArray : srcvid : " << i << ", dstvid : " << edges[j].adjvtx_id
                          << ", weight : " << edges[j].weight;
            }
            edges.clear();
        }
        return;
    }
    /**
     * 计算出edge_block_array的高度，也就是page的个数
     */
    uint Graphtinker::GetEdgeBlockArrayHeight(const vector<work_block_t> &edge_block_array)
    {
        return (edge_block_array.size() / work_blocks_per_page_);
    }

    /**
     * 根据vertex id判断是否需要扩容。如果vertex id > height, 则需要
     */
    void Graphtinker::check_whether_to_resize_edgeblockarray_m(uint vid)
    {
        if (vid >= GetEdgeBlockArrayHeight(edge_block_array_m_))
        {
            uint newsz = (vid + eba_m_expansion_addition_height_) * work_blocks_per_page_;
            edge_block_array_m_.resize(newsz);
            LOG(INFO) << "Graphtinker::resize_edgeblockarray_m : resizing... : vid : " << vid
                      << ", new edge_block_array_m_ size : " << edge_block_array_m_.size()
                      << ", new edge_block_array_m_ height : " << GetEdgeBlockArrayHeight(edge_block_array_m_);
        }
        return;
    }

    uint Graphtinker::GetEdgeblockOffset(vertexid_t vid)
    {
        uint offset3rmbase = vid * work_blocks_per_page_;
        
        DLOG(INFO) << "vid = " << vid;
        DLOG(INFO) << "work_blocks_per_page_ = " << work_blocks_per_page_;
        DLOG(INFO) << "value = " << offset3rmbase;
        return offset3rmbase;
    }

   	bucket_t Graphtinker::GoogleHash(vertexid_t vid, edge_type_t etype, uint geni) const
	{
        const uint KMULTFACTOR = 1;
		bucket_t hashval = 0;
		bucket_t exthashval = 0;

		// get extended edgeblock range
		uint ext_edgeblock_range = page_block_height_ + (KMULTFACTOR * (geni - 1));

		// do basicHash on extended edgeblock range
		exthashval = (vid ^ (etype << 4)) % ext_edgeblock_range;

		// now hash to edgeblock range
		hashval = exthashval % page_block_height_;
		return hashval;
	}
	uint Graphtinker::add_page(tracker_t *tracker, vector<work_block_t> &edge_block_array)
	{
		if ((tracker->mark * work_blocks_per_page_) >= edge_block_array.size())
		{ // resize only when filled
			LOG(INFO) << "add_page : resizing edge_block_array_o_..."  ;
			edge_block_array.resize((edge_block_array.size() + (eba_o_expansion_addition_height_ * work_blocks_per_page_)));
		}
		uint pos = tracker->mark;
		tracker->mark += 1;
		return pos;
	}
	uint Graphtinker::getsvtracker(markertracker_t *svtracker)
	{
		uint ret = svtracker->marker;
		svtracker->marker++;
		return ret;
    }
    void Graphtinker::FindWorkBlockMargin(bucket_t adjvtx_id_hash, margin_t *blkmargin)
    {
        bucket_t a;
        bucket_t ttop;

        a = adjvtx_id_hash / WORK_BLOCK_HEIGHT;
        ttop = a * WORK_BLOCK_HEIGHT;

        blkmargin->top = ttop;
        blkmargin->bottom = ttop + (WORK_BLOCK_HEIGHT - 1);
        return;
    }

    void Graphtinker::FindSubBlockMargin(bucket_t adjvtx_id_hash, margin_t *sub_block_margin)
    {
        bucket_t a;
        bucket_t ttop;

        a = adjvtx_id_hash / sub_block_height_;
        ttop = a * sub_block_height_;

        sub_block_margin->top = ttop;
        sub_block_margin->bottom = ttop + (sub_block_height_ - 1);
        return;
    }
} // namespace graphtinker
