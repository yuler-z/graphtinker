
#include <ctime>
#include <random>
#include "core/graphtinker.h"

using std::vector;

namespace graphtinker
{

    Graphtinker::Graphtinker()
    {
        config_ = new Config("file_path");
        create_graph();
    };

    Graphtinker::Graphtinker(string file_path)
    {
        config_ = new Config(file_path);
        create_graph();
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

    void Graphtinker::create_graph()
    {

        config_->config(this);
        config_->pring_configuration();

        workblocks_per_page_ = page_block_height_ / WORK_BLOCK_HEIGHT;
        workblocks_per_subblock_ = subblock_height_ / WORK_BLOCK_HEIGHT;

        edge_block_array_m_.resize((num_vertices_ * workblocks_per_page_));
        edge_block_array_o_.resize((100 * workblocks_per_page_));
        lvatracker_.mark = 100;

        LOG(INFO) << "lvatracker_ start mark : " << lvatracker_.mark;

#ifdef EN_CAL
        cal_edgeblock_array_.resize(((num_vertices_ * page_block_height_) / LLEDGEBLOCKSIZE));
        cal_lva_.resize(((num_vertices_ + (LVACOARSENESSWIDTH - 1)) / LVACOARSENESSWIDTH));
        cal_edgeblock_tracker_.ptraddr = 0;
        init_cal_lvas();
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

    void Graphtinker::insert_edge(Edge &edge)
    {
        bool is_insert_edge = INSERTEDGE;
        if (sgh_for_vtx_id_ == SELF)
        {
            edge.src = translator_handler_->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == SELF)
        {
            edge.dst = translator_handler_->GetLocalVid((vertexid_t)edge.dst);
        }
        check_whether_to_resize_edgeblockarray_m(edge.src);
        update_edge(edge, is_insert_edge, vertices_handler_);
        return;
    }

    void Graphtinker::insert_edge(Edge &edge, Vertices *external_vertices_handler)
    {
        bool is_insert_edge = INSERTEDGE;
        if (sgh_for_vtx_id_ == SELF)
        {
            edge.src = (uint)translator_handler_->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == SELF)
        {
            edge.dst = (uint)translator_handler_->GetLocalVid((vertexid_t)edge.dst);
        }
        check_whether_to_resize_edgeblockarray_m(edge.src);
        update_edge(edge, is_insert_edge, external_vertices_handler);
        return;
    }

    void Graphtinker::insert_edge(Edge &edge, Translator *translator_handler)
    {
        if (edge.src > vertex_range_)
        {
            LOG(ERROR) << "Graphtinker::insert_edge : bug, out of range19. src : " << edge.src << ", vertex_range_ : "
                       << vertex_range_;
        }
        bool is_insert_edge = INSERTEDGE;
        if (sgh_for_vtx_id_ == OTHER)
        {
            edge.src = (uint)translator_handler->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == OTHER)
        {
            edge.dst = (uint)translator_handler->GetLocalVid((vertexid_t)edge.dst);
        }
        check_whether_to_resize_edgeblockarray_m(edge.src);
        update_edge(edge, is_insert_edge, vertices_handler_);
        return;
    }

    void Graphtinker::insert_edge(Edge &edge, Vertices *external_vertices_handler, Translator *translator_handler)
    {
        if (edge.src > vertex_range_)
        {
            LOG(ERROR) << "Graphtinker::insert_edge : bug, out of range199. src : " << edge.src << ", vertex_range_ : "
                       << vertex_range_;
        }
        bool is_insert_edge = INSERTEDGE;
        if (sgh_for_vtx_id_ == OTHER)
        {
            edge.src = (uint)translator_handler->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == OTHER)
        {
            edge.dst = (uint)translator_handler->GetLocalVid((vertexid_t)edge.dst);
        }
        check_whether_to_resize_edgeblockarray_m(edge.src);
        update_edge(edge, is_insert_edge, external_vertices_handler);
        return;
    }

    void Graphtinker::batch_insert_edge(const char *path, uint batch_size)
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
        const auto& edge_type_array = config_->edge_type_array();
        const auto& edge_type_map = config_->edge_type_map();
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
                insert_edge(_);
                if (!is_directed_)
                {
                    edges_inserted += 1;
                    total_edges_inserted += 1;
                    insert_edge(_);
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
            LOG(INFO) << "number of edges : " << print_edge_count() << " edges";
            LOG(INFO) << "number of unique edges : " << print_unique_edge_count() << " edges";
#ifdef EN_CAL
            LOG(INFO) << "number of edges in cal: " << print_cal_edge_count() << " edges";
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

    void Graphtinker::delete_edge(Edge& edge)
    {
        bool is_insert_edge = DELETEEDGE;
        if (sgh_for_vtx_id_ == SELF)
        {
            edge.src = translator_handler_->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == SELF)
        {
            edge.dst = translator_handler_->GetLocalVid((vertexid_t)edge.dst);
        }
        update_edge(edge, is_insert_edge, vertices_handler_);
        return;
    }

    void Graphtinker::delete_edge(Edge& edge, Vertices *external_vertices_handler)
    {
        bool is_insert_edge = DELETEEDGE;
        if (sgh_for_vtx_id_ == SELF)
        {
            edge.src = translator_handler_->GetLocalVid((vertexid_t)edge.src);
        }
        if (sgh_for_adjvtx_id_ == SELF)
        {
            edge.dst = translator_handler_->GetLocalVid((vertexid_t)edge.dst);
        }
        update_edge(edge, is_insert_edge, external_vertices_handler);
        return;
    }

    void Graphtinker::batch_delete_edge(const char *path, uint batch_size)
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
        const auto& edge_type_array = config_->edge_type_array();
        const auto& edge_type_map = config_->edge_type_map();
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
                delete_edge(_);
                if (!is_directed_)
                {
                    edges_deleted += 1;
                    total_edges_deleted += 1;
                    delete_edge(_);
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
            LOG(INFO) << "number of edges : " << print_edge_count() << " edges";
            LOG(INFO) << "number of unique edges : " << print_unique_edge_count() << " edges";
#ifdef EN_CAL
            LOG(INFO) << "number of unique edges in cal : " << print_cal_edge_count() << " edges";
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
    Graphtinker::update_edge(const Edge &e, bool is_insert_edge, Vertices *external_vertices_handler)
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
        workblock_t workblock;

        //margins
        margin_t workblock_margin;
        margin_t start_wblkmargin;
        margin_t first_wblkmargin;
        margin_t subblock_margin;

        // unit flow负责状态机全流程
        UnitFlow *unit_flow = new UnitFlow(this);
        UnitOption *unit_option = unit_flow->unit_option;

//LL unit
#ifdef EN_CAL
        cal_unit_cmd_t cal_unit_cmd;
#endif

#ifdef EN_DCI
        dci_cmd_t deleteandcrumpleincmd;
#endif

        //exit condition (this should be after: 'insert-unit parameters and reports')
        uint quitstatus = 0;

        // compute hash and margin
        geni = 1;
        vtx_id = edge.vtx_id;
        tripiteration_lcs = 0;
        adjvtx_id_hash = google_hash(edge.adjvtx_id, edge.type, geni);
        find_workblock_margin(adjvtx_id_hash, &workblock_margin); //find work block margins
        find_subblock_margin(adjvtx_id_hash, &subblock_margin);
        start_wblkmargin = workblock_margin;
        first_wblkmargin = workblock_margin;
        DLOG(INFO) << "adjvtx_id_hash = " << adjvtx_id_hash;
        DLOG(INFO) << "workblock_margin = [" << workblock_margin.top << " , " << workblock_margin.bottom << "]";
#ifdef EN_DCI
        // keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
        uint lastgenworkblockaddr = get_edgeblock_offset(vtx_id) + workblock_margin.top / WORK_BLOCK_HEIGHT;
#endif

#ifdef EN_CAL
        unit_option->InitCalUnit();
        unit_option->ClearCalAdr();
#endif
#ifdef EN_DCI
        init_dci_unit_cmd(&deleteandcrumpleincmd);
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
            external_vertices_handler->UpdateVertexProperty(vtx_id_g, adjvtx_id_g, is_insert_edge, is_directed_);
        }

        //******************** main loop ***********
        uint infiniti;
        for (infiniti = 0; infiniti < SOMELARGENO; infiniti++)
        {
            // 1. load workblock
            if (geni == 1) {
                unit_flow->load_unit(workblock_margin, vtx_id, edge_block_array_m_, &workblock);
            } else {
                unit_flow->load_unit(workblock_margin, vtx_id, edge_block_array_o_, &workblock);
            }

            // 2. load compute-unit params
            unit_flow->load_params(workblock_margin, adjvtx_id_hash);

            // 3. find_unit or insert_unit
            if (unit_option->module_unit_cmd.mode == FIND_MODE) {
                unit_flow->find_unit(workblock_margin, adjvtx_id_hash, &workblock, is_insert_edge);
            } else if (unit_option->module_unit_cmd.mode == INSERT_MODE) {
                unit_flow->insert_unit(workblock_margin, adjvtx_id_hash, &workblock, &edge, geni);
            }

            // 4. 
            unit_flow->inference_unit( is_insert_edge, &workblock_margin, subblock_margin,
                start_wblkmargin, vtx_id);

            // 5.
            unit_flow->writeback_unit(
                edge,
                &workblock,
                edge_block_array_m_,
                edge_block_array_o_,
                &lvatracker_,
                vtx_id,
                first_wblkmargin,
                subblock_margin,
                geni,
                is_insert_edge
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
            dci_unit(
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
                workblock_margin,
                subblock_margin,
                geni,
                deleteandcrumpleincmd, svs, freed_edgeblock_list);
#endif
            // Optional Feature: CAL
#ifdef EN_CAL
            unit_flow->cal_unit(
                edge,
                edge_block_array_m_,
                edge_block_array_o_,
                geni,
                cal_edgeblock_array_,
                cal_lva_,
                &cal_edgeblock_tracker_);
#endif
            // Interval unit
            unit_flow->interval_unit(
                &workblock_margin,
                &subblock_margin,
                &start_wblkmargin,
                &first_wblkmargin,
                &vtx_id,
                &adjvtx_id_hash,
                edge,
                is_insert_edge,
                &tripiteration_lcs,
                &geni,
                &quitstatus
#ifdef EN_DCI
                &lastgenworkblockaddr,
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
        uint wps = workblocks_per_subblock_;

        while (true)
        {
            for (uint i = 0; i < len; i++)
            {
                vid = clusterptrs.back();
                clusterptrs.pop_back();

                uint ebaoffset = get_edgeblock_offset(vid);
                for (uint t = 0; t < workblocks_per_page_; t++)
                {
                    workblock_t edgeset;
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

    uint Graphtinker::print_edge_count()
    {
        uint edgecount = 0;
        edgecount += Graphtinker::get_edge_count(edge_block_array_m_, get_edgeblock_array_height(edge_block_array_m_));
        edgecount += Graphtinker::get_edge_count(edge_block_array_o_, lvatracker_.mark);
        return edgecount;
    }

    uint Graphtinker::get_edge_count(vector<workblock_t> edge_block_array_x, uint height)
    {
        uint edgecount = 0;
        for (vertexid_t vid = 0; vid < height; vid++)
        {
            uint offset = get_edgeblock_offset(vid);
            for (uint j = offset; j < (offset + workblocks_per_page_); j++)
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

    uint Graphtinker::print_unique_edge_count()
    {
        uint edgecount = 0;
        edgecount += Graphtinker::get_unique_edge_count(edge_block_array_m_,
                                                     get_edgeblock_array_height(edge_block_array_m_));
        edgecount += Graphtinker::get_unique_edge_count(edge_block_array_o_, lvatracker_.mark);
        return edgecount;
    }

    uint Graphtinker::get_unique_edge_count(vector<workblock_t> edge_block_array_x, uint height)
    {
        uint edgecount = 0;
        for (vertexid_t vid = 0; vid < height; vid++)
        {
            uint offset = get_edgeblock_offset(vid);
            for (uint j = offset; j < (offset + workblocks_per_page_); j++)
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

#ifdef EN_CAL
    uint Graphtinker::print_cal_edge_count()
    {
        uint uniqueedgecount = 0;
        uniqueedgecount += cal_edge_count(cal_edgeblock_array_);
        return uniqueedgecount;
    }

    uint Graphtinker::cal_edge_count(vector<cal_edgeblock_t> &cal_edgeblock_array_)
    {
        uint uniqueedgecount = 0;
        for (uint i = 0; i < cal_edgeblock_array_.size(); i++)
        { //cal_edgeblock_array_size
            for (uint j = 0; j < LLEDGEBLOCKSIZE; j++)
            {
                if (cal_edgeblock_array_[i].cal_edgeblock[j].flag == VALID)
                {
                    uniqueedgecount += cal_edgeblock_array_[i].cal_edgeblock[j].weight;
                }
            }
        }
        return uniqueedgecount;
    }
    
    void Graphtinker::init_cal_lvas()
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
#endif

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

    void Graphtinker::print_edgeblock_array(vertexid_t begin, vertexid_t end)
    {
        vector<edge_tt> edges;

        for (uint i = begin; i < end; i++)
        {
            retrieve_edges(0, edges);
            for (uint j = 0; j < edges.size(); j++)
            {
                LOG(INFO) << "Graphtinker::print_edgeblock_array : srcvid : " << i << ", dstvid : " << edges[j].adjvtx_id
                          << ", weight : " << edges[j].weight;
            }
            edges.clear();
        }
        return;
    }
    /**
     * 计算出edge_block_array的高度，也就是page的个数
     */
    uint Graphtinker::get_edgeblock_array_height(const vector<workblock_t> &edge_block_array)
    {
        return (edge_block_array.size() / workblocks_per_page_);
    }

    /**
     * 根据vertex id判断是否需要扩容。如果vertex id > height, 则需要
     */
    void Graphtinker::check_whether_to_resize_edgeblockarray_m(uint vid)
    {
        if (vid >= get_edgeblock_array_height(edge_block_array_m_))
        {
            uint newsz = (vid + eba_m_expansion_addition_height_) * workblocks_per_page_;
            edge_block_array_m_.resize(newsz);
            LOG(INFO) << "Graphtinker::resize_edgeblockarray_m : resizing... : vid : " << vid
                      << ", new edge_block_array_m_ size : " << edge_block_array_m_.size()
                      << ", new edge_block_array_m_ height : " << get_edgeblock_array_height(edge_block_array_m_);
        }
        return;
    }

    uint Graphtinker::get_edgeblock_offset(vertexid_t vid)
    {
        uint offset3rmbase = vid * workblocks_per_page_;
        
        DLOG(INFO) << "vid = " << vid;
        DLOG(INFO) << "workblocks_per_page_ = " << workblocks_per_page_;
        DLOG(INFO) << "value = " << offset3rmbase;
        return offset3rmbase;
    }

   	bucket_t Graphtinker::google_hash(vertexid_t vid, edge_type_t etype, uint geni) const
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
	uint Graphtinker::add_page(tracker_t *tracker, vector<workblock_t> &edge_block_array)
	{
		if ((tracker->mark * workblocks_per_page_) >= edge_block_array.size())
		{ // resize only when filled
			LOG(INFO) << "add_page : resizing edge_block_array_o_..."  ;
			edge_block_array.resize((edge_block_array.size() + (eba_o_expansion_addition_height_ * workblocks_per_page_)));
		}
		uint pos = tracker->mark;
		tracker->mark += 1;
		return pos;
	}

    void Graphtinker::find_workblock_margin(bucket_t adjvtx_id_hash, margin_t *blkmargin)
    {
        bucket_t a;
        bucket_t ttop;

        a = adjvtx_id_hash / WORK_BLOCK_HEIGHT;
        ttop = a * WORK_BLOCK_HEIGHT;

        blkmargin->top = ttop;
        blkmargin->bottom = ttop + (WORK_BLOCK_HEIGHT - 1);
        return;
    }

    void Graphtinker::find_subblock_margin(bucket_t adjvtx_id_hash, margin_t *subblock_margin)
    {
        bucket_t a;
        bucket_t ttop;

        a = adjvtx_id_hash / subblock_height_;
        ttop = a * subblock_height_;

        subblock_margin->top = ttop;
        subblock_margin->bottom = ttop + (subblock_height_ - 1);
        return;
    }
} // namespace graphtinker
