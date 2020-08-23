#include <string.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <cmath>
#include <omp.h>

#include "graphtinker/vertices.h"
#include "graphtinker/graphtinker.h"

using namespace std;

namespace gt
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
        LOG(INFO) << "Graphtinker::~Graphtinker : Graphtinker destructor called";

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
        ll_edge_block_array_.clear();
        ll_lva_.clear();
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
        ll_edge_block_array_.resize(((num_vertices_ * page_block_height_) / LLEDGEBLOCKSIZE));
        ll_lva_.resize(((num_vertices_ + (LVACOARSENESSWIDTH - 1)) / LVACOARSENESSWIDTH));
        ll_eba_tracker_.ptraddr = 0;
        initialize_lvas();
#endif

        // vertices & translator
        if (updatev_ == SELF)
        {
            vertices_handler_ = new Vertices(vertex_range_, 0.15);
        }

        if ((sgh_for_xvtxid_ == SELF) || (sgh_for_xadjvtxid_ == SELF))
        {
            translator_handler_ = new Translator(vertex_range_);
        }

// metadata (for delete and crumple in)
// enable ---- crumple in on delete
#ifdef EN_CRUMPLE_IN
        edgeblock_parentinfo.resize((num_edges_ / (page_block_height_ / 2)));
#endif
        LOG(INFO) << "edge_block_array_m_.size() : " << edge_block_array_m_.size();
        LOG(INFO) << "edge_block_array_o_.size() : " << edge_block_array_o_.size();
// enable ---- linked-list graph data structure
#ifdef EN_CAL
        LOG(INFO) << "ll_edge_block_array_.size() : " << ll_edge_block_array_.size();
        LOG(INFO) << "ll_lva_.size() : " << ll_lva_.size();
        LOG(INFO) << "ll_eba_tracker_.ptraddr : " << ll_eba_tracker_.ptraddr;
#endif
        return;
    }

    void Graphtinker::InsertEdge(uint src, uint dst, uint ew)
    {
        uint edge_update_cmd = INSERTEDGE;
        if (sgh_for_xvtxid_ == SELF)
        {
            src = translator_handler_->GetLocalVid((vertexid_t)src);
        }
        if (sgh_for_xadjvtxid_ == SELF)
        {
            dst = translator_handler_->GetLocalVid((vertexid_t)dst);
        }
        check_whether_to_resize_edgeblockarray_m(src);
        update_edge(src, dst, ew, edge_update_cmd, vertices_handler_);
        return;
    }

    void Graphtinker::InsertEdge(uint src, uint dst, uint ew, Vertices *external_vertices_handler)
    {
        uint edge_update_cmd = INSERTEDGE;
        if (sgh_for_xvtxid_ == SELF)
        {
            src = (uint)translator_handler_->GetLocalVid((vertexid_t)src);
        }
        if (sgh_for_xadjvtxid_ == SELF)
        {
            dst = (uint)translator_handler_->GetLocalVid((vertexid_t)dst);
        }
        check_whether_to_resize_edgeblockarray_m(src);
        update_edge(src, dst, ew, edge_update_cmd, external_vertices_handler);
        return;
    }

    void Graphtinker::InsertEdge(uint src, uint dst, uint ew, Translator *translator_handler)
    {
        if (src > vertex_range_)
        {
            LOG(ERROR) << "Graphtinker::InsertEdge : bug, out of range19. src : " << src << ", vertex_range_ : "
                       << vertex_range_;
        }
        uint edge_update_cmd = INSERTEDGE;
        if (sgh_for_xvtxid_ == OTHER)
        {
            src = (uint)translator_handler->GetLocalVid((vertexid_t)src);
        }
        if (sgh_for_xadjvtxid_ == OTHER)
        {
            dst = (uint)translator_handler->GetLocalVid((vertexid_t)dst);
        }
        check_whether_to_resize_edgeblockarray_m(src);
        update_edge(src, dst, ew, edge_update_cmd, vertices_handler_);
        return;
    }

    void Graphtinker::InsertEdge(uint src, uint dst, uint ew, Vertices *external_vertices_handler,
                                 Translator *translator_handler)
    {
        if (src > vertex_range_)
        {
            LOG(ERROR) << "Graphtinker::InsertEdge : bug, out of range199. src : " << src << ", vertex_range_ : "
                       << vertex_range_;
        }
        uint edge_update_cmd = INSERTEDGE;
        if (sgh_for_xvtxid_ == OTHER)
        {
            src = (uint)translator_handler->GetLocalVid((vertexid_t)src);
        }
        if (sgh_for_xadjvtxid_ == OTHER)
        {
            dst = (uint)translator_handler->GetLocalVid((vertexid_t)dst);
        }
        check_whether_to_resize_edgeblockarray_m(src);
        update_edge(src, dst, ew, edge_update_cmd, external_vertices_handler);
        return;
    }

    void Graphtinker::BatchInsertEdge(const char *path, uint batch_size)
    {
        LOG(INFO) << endl;
        LOG(INFO) << "=== bath insertion edge started ===";
        vector<tuple_t> src_dst_pairs;
        uint edges_inserted = 0;
        uint total_edges_inserted = 0;
        float insertion_timelapse_ms = 0;
        float total_insertion_time_lapse_ms = 0;
        float throughput = 0;
        float runningthroughput = 0;
        uint batchid = 0;

        FILE *file = fopen(path, "r");
        if (file == NULL)
        {
            LOG(ERROR) << "error : cannot find the location of input file rmat500k8m.edges (test_updatesingleedge)";
        }

        while (!feof(file))
        {
            src_dst_pairs.clear();

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

                tuple_t t;
                t.A = src;
                t.B = dst;
                src_dst_pairs.push_back(t);
            }

            // insert edges
            edges_inserted = 0;
            std::clock_t start = std::clock();
            for (uint j = 0; j < src_dst_pairs.size(); j++)
            {
                edges_inserted += 1;
                total_edges_inserted += 1;
                InsertEdge((uint)src_dst_pairs[j].A, (uint)src_dst_pairs[j].B, 1);
                if (graphdirectiontype_ == UNDIRECTEDGRAPH)
                {
                    edges_inserted += 1;
                    total_edges_inserted += 1;
                    InsertEdge((uint)src_dst_pairs[j].B, (uint)src_dst_pairs[j].A, 1);
                }
            }
            insertion_timelapse_ms = 0;
            insertion_timelapse_ms = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
            total_insertion_time_lapse_ms += insertion_timelapse_ms;
            throughput = ((edges_inserted / insertion_timelapse_ms) * 1000) / 1000000;
            runningthroughput = ((total_edges_inserted / total_insertion_time_lapse_ms) * 1000) / 1000000;
            LOG(INFO) << endl;
            LOG(INFO) << "batch : " << batchid;
            LOG(INFO) << "edges loaded : " << edges_inserted << " edges";
            LOG(INFO) << "number of edges : " << printv_edgecount() << " edges";
            LOG(INFO) << "number of unique edges : " << printv_uniqueedgecount() << " edges";
#ifdef EN_CAL
            LOG(INFO) << "number of edges in ll : " << printll_edgecount() << " edges";
#endif
            LOG(INFO) << "timelapse (ms) : " << (float)insertion_timelapse_ms << " ms";
            LOG(INFO) << "total timelapse (ms) : " << (float)total_insertion_time_lapse_ms << " ms";
            LOG(INFO) << "running throughput : " << (float)runningthroughput << " million edges per sec";
            batchid += 1;
        }

        fclose(file);
        src_dst_pairs.clear();
        return;
    }

    void Graphtinker::DeleteEdge(uint src, uint dst, uint ew)
    {
        uint edge_update_cmd = DELETEEDGE;
        if (sgh_for_xvtxid_ == SELF)
        {
            src = translator_handler_->GetLocalVid((vertexid_t)src);
        }
        if (sgh_for_xadjvtxid_ == SELF)
        {
            dst = translator_handler_->GetLocalVid((vertexid_t)dst);
        }
        update_edge(src, dst, ew, edge_update_cmd, vertices_handler_);
        return;
    }

    void Graphtinker::DeleteEdge(uint src, uint dst, uint ew, Vertices *external_vertices_handler)
    {
        uint edge_update_cmd = DELETEEDGE;
        if (sgh_for_xvtxid_ == SELF)
        {
            src = translator_handler_->GetLocalVid((vertexid_t)src);
        }
        if (sgh_for_xadjvtxid_ == SELF)
        {
            dst = translator_handler_->GetLocalVid((vertexid_t)dst);
        }
        update_edge(src, dst, ew, edge_update_cmd, external_vertices_handler);
        return;
    }

    void Graphtinker::BatchDeleteEdge(const char *path, uint batch_size)
    {
        LOG(INFO) << endl;
        LOG(INFO) << "=== deletions started ===";
        vector<tuple_t> src_dst_pairs;
        uint edges_deleted = 0;
        uint total_edges_deleted = 0;
        float deletion_timelapse_ms = 0;
        float total_deletion_time_lapse_ms = 0;
        float throughput = 0;
        float runningthroughput = 0;
        uint batchid = 0;

        FILE *file = fopen(path, "r");
        if (file == NULL)
        {
            LOG(ERROR) << "error : cannot find the location of input file rmat500k8m.edges (test_updatesingleedge)";
        }

        while (!feof(file))
        {
            src_dst_pairs.clear();

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

                tuple_t t;
                t.A = src;
                t.B = dst;
                src_dst_pairs.push_back(t);
            }

            edges_deleted = 0;
            std::clock_t start = std::clock();
            for (uint j = 0; j < src_dst_pairs.size(); j++)
            {
                edges_deleted += 1;
                total_edges_deleted += 1;
                DeleteEdge((uint)src_dst_pairs[j].A, (uint)src_dst_pairs[j].B, 1);
                if (graphdirectiontype_ == UNDIRECTEDGRAPH)
                {
                    edges_deleted += 1;
                    total_edges_deleted += 1;
                    DeleteEdge((uint)src_dst_pairs[j].B, (uint)src_dst_pairs[j].A, 1);
                }
            }

            // print batch delete info
            deletion_timelapse_ms = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
            total_deletion_time_lapse_ms += deletion_timelapse_ms;
            throughput = ((edges_deleted / deletion_timelapse_ms) * 1000) / 1000000;
            runningthroughput = ((total_edges_deleted / total_deletion_time_lapse_ms) * 1000) / 1000000;
            LOG(INFO) << endl;
            LOG(INFO) << "batch : " << batchid;
            LOG(INFO) << "edges loaded : " << edges_deleted << " edges";
            LOG(INFO) << "number of edges : " << printv_edgecount() << " edges";
            LOG(INFO) << "number of unique edges : " << printv_uniqueedgecount() << " edges";
#ifdef EN_CAL
            LOG(INFO) << "number of unique edges in ll : " << printll_edgecount() << " edges";
#endif
            LOG(INFO) << "timelapse (ms) : " << (float)deletion_timelapse_ms << " ms";
            LOG(INFO) << "running throughput : " << (float)runningthroughput << " million edges per sec";
#ifdef EN_CRUMPLE_IN
            LOG(INFO) << "svs's size : " << tinker.print_svs_size();
            LOG(INFO) << "freed edgeblock list's size : " << tinker.print_freed_edgeblock_list_size();
#endif
            batchid += 1;
        }

        fclose(file);
        src_dst_pairs.clear();
        return;
    }
    /**
     * insert_edge和delete_edge的主逻辑函数。
     * 该函数包含以下几个单元:
     *  
     */
    void
    Graphtinker::update_edge(uint src, uint dst, uint ew, uint edge_update_cmd, Vertices *external_vertices_handler)
    {
        // create an edge object
        edge_t edge;
        edge.xvtx_id = src;
        edge.xadjvtx_id = dst;
        edge.edgew = 1;
        edge.flag = VALID;
#ifdef EN_CAL
        edge.heba_hvtx_id = -1;
        edge.heba_workblockid = -1;
        edge.heba_loffset = -1;
#endif

        // variables & parameter registers and RAM declared
        uint eit;
        uint geni;
        vertexid_t xvtx_id;
        uint tripiteration_lcs = 0;

        ///////////////////////conversion unit
        //hash xadjvtx_id
        bucket_t hadjvtx_id = 0;

        ///////////////////////work_block
        work_block_t work_block;

        ///////////////////////margins & traversal trackers
        margin_t work_block_margin; //存储work_block的start addr和end addr
        margin_t start_wblkmargin;
        margin_t first_wblkmargin;
        margin_t sub_block_margin;

        ///////////////////////params and report
        //module parameters and reports
        module_unit_cmd_t moduleunitcmd;
        module_params_t moduleparams;

        //load unit
        load_unit_cmd_t loadunitcmd;

        //insert-unit parameters and reports
        insert_params_t insertparams;
        insert_report_t insertreport;

        //find-unit parameters and reports
        find_params_t findparams;
        find_report_t findreport;
        search_report_t searchreport;

        //writeback unit
        writeback_unit_cmd_t writebackunitcmd;

        //interval unit
        interval_unit_cmd_t intervalunitcmd;

//LL unit
#ifdef EN_CAL
        cal_unit_cmd_t cal_unit_cmd;
#endif

#ifdef EN_CRUMPLE_IN
        crumple_in_cmd_t deleteandcrumpleincmd;
#endif

        //exit condition (this should be after: 'insert-unit parameters and reports')
        uint quitstatus = 0;

        ////////////////////////// initial assignments
        // initialize appropriate module globals
        geni = 1;
        xvtx_id = edge.xvtx_id;
        tripiteration_lcs = 0;
        hadjvtx_id = googlehash(edge.xadjvtx_id, geni);
        FindWorkBlockMargin(&work_block_margin, hadjvtx_id); //find margins
        start_wblkmargin = work_block_margin;
        first_wblkmargin = work_block_margin;
        FindSubBlockMargin(&sub_block_margin, hadjvtx_id);
#ifdef EN_CRUMPLE_IN
        // keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
        uint lastgenworkblockaddr = get_edgeblock_offset(xvtx_id) + work_block_margin.top / WORK_BLOCK_HEIGHT;
#endif

        //initialize module unit
        moduleunitcmd.mode = FIND_MODE;

        // initialize appropriate fields of lcs units
        // everything is initialized here
        InitModuleUnitParams(&moduleparams, edge.xadjvtx_id, edge.edgew);
        InitLoadUnit(&loadunitcmd);
        InitInsertUnit(&insertparams, &insertreport, edge.xadjvtx_id, hadjvtx_id, edge.edgew);
        InitFindUnit(&findparams, &findreport, edge.xadjvtx_id, hadjvtx_id, edge.edgew);
        InitWritebackUnit(&writebackunitcmd);
#ifdef EN_CAL
        initialize_llebaverdictcmd(&cal_unit_cmd);
        clear_lleba_addresses_in_moduleparams(&moduleparams);
#endif
#ifdef EN_CRUMPLE_IN
        init_deleteandcrumplein_verdictcmd(&deleteandcrumpleincmd);
#endif

        //initialize trip iteration
        uint infiniti = 0;
        uint prevLoadAddr = NAv;

        vertexid_t xvtxid = 0;
        vertexid_t xadjvtxid = 0;
        
        // 首先更新vertex property array
        if (updatev_ != OFF)
        {
            xvtxid = edge.xvtx_id;
            xadjvtxid = edge.xadjvtx_id;
            if (sgh_for_xvtxid_ == SELF)
            {
                xvtxid = translator_handler_->GetGlobalVid(edge.xvtx_id);
            }
            if (sgh_for_xadjvtxid_ == SELF)
            {
                xadjvtxid = translator_handler_->GetGlobalVid(edge.xadjvtx_id);
            }
            external_vertices_handler->UpdateVertexProperty(xvtxid, xadjvtxid, edge_update_cmd, graphdirectiontype_);
        }

        //*** loop
        for (infiniti = 0; infiniti < SOMELARGENO; infiniti++)
        {

            //***********************************************************************************************//Load unit
            /***
             * 通过xvtx_id计算page，根据wblkmargin计算该page中的那个work_block
             * 加载edge所处的work_block
             */
            if (geni == 1)
            {
                LoadUnit(
                    &moduleparams,
                    loadunitcmd,
                    &insertparams,
                    &findparams,
                    work_block_margin,
                    xvtx_id,
                    edge,
                    &work_block,
                    edge_block_array_m_,
                    &prevLoadAddr,
                    geni);
            }
            else
            {
                LoadUnit(
                    &moduleparams,
                    loadunitcmd,
                    &insertparams,
                    &findparams,
                    work_block_margin,
                    xvtx_id,
                    edge,
                    &work_block,
                    edge_block_array_o_,
                    &prevLoadAddr,
                    geni);
            }
            //***********************************************************************************************

            //***********************************************************************************************//load compute-params unit
            LoadParams(
                moduleparams,
                &insertparams,
                &findparams,
                work_block_margin,
                hadjvtx_id);
            //***********************************************************************************************

            //***********************************************************************************************//Compute unit
            /// one-time-fields of incomming edge is updated here
            ComputeUnit(
                moduleunitcmd,
                &moduleparams,
                &insertparams,
                &insertreport,
                findparams,
                &findreport,
                work_block_margin,
                sub_block_margin,
                start_wblkmargin,
                hadjvtx_id,
                &work_block,
                &edge,
                xvtx_id,
                moduleparams.rolledover,
                edge_update_cmd,
                geni);
            //***********************************************************************************************

            //***********************************************************************************************//Inference unit
            InferenceUnit(
                edge_update_cmd,
                &moduleunitcmd,
                &moduleparams,
                &loadunitcmd,
                findparams,
                findreport,
                insertparams,
                insertreport,
                &writebackunitcmd,
                &intervalunitcmd,
                &work_block_margin,
                sub_block_margin,
                start_wblkmargin,
                xvtx_id
#ifdef EN_CAL
                ,
                &cal_unit_cmd
#endif
#ifdef EN_CRUMPLE_IN
                ,
                &deleteandcrumpleincmd
#endif
            );
            //***********************************************************************************************

            //***********************************************************************************************//EBA port unit
            WritebackUnit(
                &moduleparams,
                writebackunitcmd,
                edge,
                &work_block,
                edge_block_array_m_,
                edge_block_array_o_,
#ifdef EN_CRUMPLE_IN
                edgeblock_parentinfo,
#endif
                &lvatracker_,
                xvtx_id,
                first_wblkmargin,
                sub_block_margin,
                geni,
                edge_update_cmd
#ifdef EN_CRUMPLE_IN
                ,
                lastgenworkblockaddr, svs
#ifdef EN_CAL
                ,
                ll_edge_block_array_
#endif
#endif
            );
//***********************************************************************************************

//***********************************************************************************************// delete and crumple in
#ifdef EN_CRUMPLE_IN
            CrumpleInUnit(
                writebackunitcmd,
                findreport,
                edge,
                edge_block_array_m_,
                edge_block_array_o_,
#ifdef EN_CAL
                ll_edge_block_array_,
#endif
                edgeblock_parentinfo,
                xvtx_id,
                work_block_margin,
                sub_block_margin,
                geni
#ifdef EN_CRUMPLE_IN
                ,
                deleteandcrumpleincmd, svs, freed_edgeblock_list
#endif
            );
#endif
//***********************************************************************************************

//***********************************************************************************************// GraphTinkerLL
#ifdef EN_CAL
            CALUnit(
                cal_unit_cmd,
                &moduleparams,
                &insertparams,
                edge,
                edge_block_array_m_,
                edge_block_array_o_,
                geni
#ifdef EN_CAL
                ,
                ll_edge_block_array_, ll_lva_, &ll_eba_tracker_
#endif
            );
#endif
            //***********************************************************************************************

            //***********************************************************************************************//Interval unit
            interval_unit_updateedge(
                &moduleunitcmd,
                &moduleparams,
                &loadunitcmd,
                &insertparams,
                &insertreport,
                &findparams,
                &findreport,
                &writebackunitcmd,
                intervalunitcmd,
                &work_block_margin,
                &sub_block_margin,
                &start_wblkmargin,
                &first_wblkmargin,
                &xvtx_id,
                &hadjvtx_id,
                edge,
                edge_update_cmd,
                &tripiteration_lcs,
                &geni,
                &quitstatus,
                infiniti
#ifdef EN_CRUMPLE_IN
                ,
                &lastgenworkblockaddr
#endif
#ifdef EN_CAL
                ,
                &cal_unit_cmd
#endif
#ifdef EN_CRUMPLE_IN
                ,
                &deleteandcrumpleincmd
#endif
            );

            //***********************************************************************************************

            if (quitstatus == 1)
            {
                DLOG(INFO) << "successful. exiting... exiting... (update_singleedge)";
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
                LOG(ERROR) << "searchreport.search_stop = " << searchreport.search_stop;
                LOG(ERROR) << "searchreport.search_success = " << searchreport.search_success;
                LOG(ERROR) << "insertreport.exittype = " << insertreport.exittype;
                LOG(ERROR) << "moduleunitcmd.mode = " << moduleunitcmd.mode;
                return;
            }
        }
        LOG(ERROR) << "bug! : infiniti exhausted : infiniti = " << infiniti << "";
        LOG(ERROR) << "searchreport.search_stop = " << searchreport.search_stop;
        LOG(ERROR) << "searchreport.search_success = " << searchreport.search_success;
        LOG(ERROR) << "insertreport.exittype = " << insertreport.exittype;
        LOG(ERROR) << "moduleunitcmd.mode = " << moduleunitcmd.mode;
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

                uint ebaoffset = get_edgeblock_offset(vid);
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

    uint Graphtinker::printv_edgecount()
    {
        uint edgecount = 0;
        edgecount += Graphtinker::get_edgecount(edge_block_array_m_, GetEdgeBlockArrayHeight(edge_block_array_m_));
        edgecount += Graphtinker::get_edgecount(edge_block_array_o_, lvatracker_.mark);
        return edgecount;
    }

    uint Graphtinker::get_edgecount(vector<work_block_t> edge_block_array_x, uint height)
    {
        uint edgecount = 0;
        for (vertexid_t vid = 0; vid < height; vid++)
        {
            uint offset = get_edgeblock_offset(vid);
            for (uint j = offset; j < (offset + work_blocks_per_page_); j++)
            {
                for (uint k = 0; k < WORK_BLOCK_HEIGHT; k++)
                {
                    if (edge_block_array_x[j].edges[k].flag == VALID)
                    {
                        edgecount += edge_block_array_x[j].edges[k].edge_weight;
                    }
                }
            }
        }
        return edgecount;
    }

    uint Graphtinker::printv_uniqueedgecount()
    {
        uint edgecount = 0;
        edgecount += Graphtinker::get_uniqueedgecount(edge_block_array_m_,
                                                      GetEdgeBlockArrayHeight(edge_block_array_m_));
        edgecount += Graphtinker::get_uniqueedgecount(edge_block_array_o_, lvatracker_.mark);
        return edgecount;
    }

    uint Graphtinker::get_uniqueedgecount(vector<work_block_t> edge_block_array_x, uint height)
    {
        uint edgecount = 0;
        for (vertexid_t vid = 0; vid < height; vid++)
        {
            uint offset = get_edgeblock_offset(vid);
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

    uint Graphtinker::printll_edgecount()
    {
        uint uniqueedgecount = 0;
        uniqueedgecount += ll_countedges(ll_edge_block_array_);
        return uniqueedgecount;
    }

    uint Graphtinker::ll_countedges(vector<ll_edgeblock_t> &ll_edge_block_array_)
    {
        uint uniqueedgecount = 0;
        for (uint i = 0; i < ll_edge_block_array_.size(); i++)
        { //ll_edgeblock_array_size
            for (uint j = 0; j < LLEDGEBLOCKSIZE; j++)
            {
                if (ll_edge_block_array_[i].ll_edgeblock[j].flag == VALID)
                {
                    uniqueedgecount += ll_edge_block_array_[i].ll_edgeblock[j].edgew;
                }
            }
        }
        return uniqueedgecount;
    }

#ifdef EN_CRUMPLE_IN
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
        for (uint i = 0; i < ll_lva_.size(); i++)
        {
            ll_lva_[i].baseaddr = 0;
            ll_lva_[i].lastlocalbaseaddr = 0;
            ll_lva_[i].lastlocaladdr = 0;
            ll_lva_[i].totaledgecount = 0;
            ll_lva_[i].flag = INVALID;
        }
        return;
    }

    // getter and setter
    const uint Graphtinker::vertex_range() const { return vertex_range_; }
    const uint Graphtinker::graphdirectiontype() const { return graphdirectiontype_; }
    const uint Graphtinker::translator_tracker_mark() const { return translator_handler_->translator_tracker_mark(); }
    const uint Graphtinker::work_blocks_per_page() const { return work_blocks_per_page_; }
    const uint Graphtinker::work_blocks_per_subblock() const { return work_blocks_per_subblock_; }
    const Translator *Graphtinker::translator() const { return translator_handler_; }
    const Vertices *Graphtinker::vertices() const { return vertices_handler_; }
    const vector<work_block_t> &Graphtinker::edge_block_array_m() const { return edge_block_array_m_; }
    const vector<work_block_t> &Graphtinker::edge_block_array_c() const { return edge_block_array_o_; }
    const vector<ll_logicalvertexentity_t> &Graphtinker::ll_logicalvertexentity() const { return ll_lva_; }
    const vector<ll_edgeblock_t> &Graphtinker::ll_edge_block_array() const { return ll_edge_block_array_; }

    void Graphtinker::print_edgeblockarray(vertexid_t begin, vertexid_t end)
    {
        vector<edge_tt> edges;

        for (uint i = begin; i < end; i++)
        {
            retrieve_edges(0, edges);
            for (uint j = 0; j < edges.size(); j++)
            {
                LOG(INFO) << "Graphtinker::print_edgeblockarray : srcvid : " << i << ", dstvid : " << edges[j].xadjvtx_id
                          << ", edgew : " << edges[j].edge_weight;
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

} // namespace gt