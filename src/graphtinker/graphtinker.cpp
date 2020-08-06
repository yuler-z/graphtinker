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


namespace gt {

    Graphtinker::Graphtinker() {
        _config = new Config("file_path");

        CreateGraph();
    };

    Graphtinker::Graphtinker(string file_path) {
        _config = new Config(file_path);

        CreateGraph();

        // google log
        

    }

    Graphtinker::~Graphtinker() {
        printf("Graphtinker::~Graphtinker : Graphtinker destructor called \n");
        /** _edge_block_array_m.clear();
        _edge_block_array_o.clear();
        delete [] vertex_translator;
        #ifdef EN_LLGDS
        _ll_edge_block_array.clear();
        _ll_lva.clear();
        #endif */
    }

    void Graphtinker::CreateGraph() {
        
        _config->ConfigGraph(this);
        _config->PrintAll();

        _work_blocks_per_page = _page_block_height / WORK_BLOCK_HEIGHT;
        _work_blocks_per_subblock = _sub_block_height / WORK_BLOCK_HEIGHT;

        _edge_block_array_m.resize((_num_vertices * _work_blocks_per_page));
        _edge_block_array_o.resize((100 * _work_blocks_per_page));
        _lvatracker.mark = 100;

        cout << "_lvatracker start mark : " << _lvatracker.mark << endl;

#ifdef EN_LLGDS
        _ll_edge_block_array.resize(((_num_vertices * _page_block_height) / LLEDGEBLOCKSIZE));
        _ll_lva.resize(((_num_vertices + (LVACOARSENESSWIDTH - 1)) / LVACOARSENESSWIDTH));
        _ll_eba_tracker.ptraddr = 0;
        initialize_lvas();
#endif

        // vertices & translator
        if (_updatev == SELF) {
            _vertices_handler.initialize(_vertex_range, 0.15);
        } else {
        }
        if ((_sgh_for_xvtxid == SELF) || (_sgh_for_xadjvtxid == SELF)) {
            _translator_handler.initialize(_vertex_range);
        }

// metadata (for delete and crumple in)
// enable ---- crumple in on delete
#ifdef EN_CRUMPLEINONDELETE
        edgeblock_parentinfo.resize((_num_edges / (_page_block_height / 2)));
#endif
        cout << "_edge_block_array_m.size() : " << _edge_block_array_m.size() << endl;
        cout << "_edge_block_array_o.size() : " << _edge_block_array_o.size() << endl;
// enable ---- linked-list graph data structure
#ifdef EN_LLGDS
        cout << "_ll_edge_block_array.size() : " << _ll_edge_block_array.size() << endl;
        cout << "_ll_lva.size() : " << _ll_lva.size() << endl;
        cout << "_ll_eba_tracker.ptraddr : " << _ll_eba_tracker.ptraddr << endl;
#endif
        return;
    }

    void Graphtinker::insert_edge(uint src, uint dst, uint ew) {
        uint edgeupdatecmd = INSERTEDGE;
        if (_sgh_for_xvtxid == SELF) {
            src = _translator_handler.get_localvid((vertexid_t) src);
        }
        if (_sgh_for_xadjvtxid == SELF) {
            dst = _translator_handler.get_localvid((vertexid_t) dst);
        }
        check_whether_to_resize_edgeblockarray_m(src);
        update_edge(src, dst, ew, edgeupdatecmd, _vertices_handler);
        return;
    }

    void Graphtinker::insert_edge(uint src, uint dst, uint ew, vertices &external_vertices_handler) {
        uint edgeupdatecmd = INSERTEDGE;
        if (_sgh_for_xvtxid == SELF) {
            src = (uint) _translator_handler.get_localvid((vertexid_t) src);
        }
        if (_sgh_for_xadjvtxid == SELF) {
            dst = (uint) _translator_handler.get_localvid((vertexid_t) dst);
        }
        check_whether_to_resize_edgeblockarray_m(src);
        update_edge(src, dst, ew, edgeupdatecmd, external_vertices_handler);
        return;
    }

    void Graphtinker::insert_edge(uint src, uint dst, uint ew, translator &_translator_handler) {
        if (src > _vertex_range) {
            cout << "Graphtinker::insert_edge : bug, out of range19. src : " << src << ", _vertex_range : "
                 << _vertex_range << endl;
        }
        uint edgeupdatecmd = INSERTEDGE;
        if (_sgh_for_xvtxid == OTHER) {
            src = (uint) _translator_handler.get_localvid((vertexid_t) src);
        }
        if (_sgh_for_xadjvtxid == OTHER) {
            dst = (uint) _translator_handler.get_localvid((vertexid_t) dst);
        }
        check_whether_to_resize_edgeblockarray_m(src);
        update_edge(src, dst, ew, edgeupdatecmd, _vertices_handler);
        return;
    }

    void Graphtinker::insert_edge(uint src, uint dst, uint ew, vertices &external_vertices_handler,
                                  translator &_translator_handler) {
        if (src > _vertex_range) {
            cout << "Graphtinker::insert_edge : bug, out of range199. src : " << src << ", _vertex_range : "
                 << _vertex_range << endl;
        }
        uint edgeupdatecmd = INSERTEDGE;
        if (_sgh_for_xvtxid == OTHER) {
            src = (uint) _translator_handler.get_localvid((vertexid_t) src);
        }
        if (_sgh_for_xadjvtxid == OTHER) {
            dst = (uint) _translator_handler.get_localvid((vertexid_t) dst);
        }
        check_whether_to_resize_edgeblockarray_m(src);
        update_edge(src, dst, ew, edgeupdatecmd, external_vertices_handler);
        return;
    }

    void Graphtinker::delete_edge(uint src, uint dst, uint ew) {
        uint edgeupdatecmd = DELETEEDGE;
        if (_sgh_for_xvtxid == SELF) {
            src = _translator_handler.get_localvid((vertexid_t) src);
        }
        if (_sgh_for_xadjvtxid == SELF) {
            dst = _translator_handler.get_localvid((vertexid_t) dst);
        }
        update_edge(src, dst, ew, edgeupdatecmd, _vertices_handler);
        return;
    }

    void Graphtinker::delete_edge(uint src, uint dst, uint ew, vertices &external_vertices_handler) {
        uint edgeupdatecmd = DELETEEDGE;
        if (_sgh_for_xvtxid == SELF) {
            src = _translator_handler.get_localvid((vertexid_t) src);
        }
        if (_sgh_for_xadjvtxid == SELF) {
            dst = _translator_handler.get_localvid((vertexid_t) dst);
        }
        update_edge(src, dst, ew, edgeupdatecmd, external_vertices_handler);
        return;
    }

    void
    Graphtinker::update_edge(uint src, uint dst, uint ew, uint edgeupdatecmd, vertices &external_vertices_handler) {
        // create an edge object
        edge_t edge;
        edge.xvtx_id = src;
        edge.xadjvtx_id = dst;
        edge.edgew = 1;
        edge.flag = VALID;
#ifdef EN_LLGDS
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

        ///////////////////////edgeblock
        edge_nt edgeblock;

        ///////////////////////margins & traversal trackers
        margin_t wblkmargin;
        margin_t start_wblkmargin;
        margin_t first_wblkmargin;
        margin_t subblkmargin;

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
        searchreport_t searchreport;

        //writeback unit
        writeback_unit_cmd_t writebackunitcmd;

        //interval unit
        interval_unit_cmd_t intervalunitcmd;

//LL unit
#ifdef EN_LLGDS
        llgds_unit_cmd_t llgdsunitcmd;
#endif

#ifdef EN_CRUMPLEINONDELETE
        delete_and_crumple_in_cmd_t deleteandcrumpleincmd;
#endif

        //exit condition (this should be after: 'insert-unit parameters and reports')
        uint quitstatus = 0;

        ////////////////////////// initial assignments
        // initialize appropriate module globals
        geni = 1;
        xvtx_id = edge.xvtx_id;
        tripiteration_lcs = 0;
        hadjvtx_id = googlehash(edge.xadjvtx_id, geni);
        findwblkmargin(&wblkmargin, hadjvtx_id); //find margins
        start_wblkmargin = wblkmargin;
        first_wblkmargin = wblkmargin;
        findsubblkmargin(&subblkmargin, hadjvtx_id);
#ifdef EN_CRUMPLEINONDELETE
        // keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
        uint lastgenworkblockaddr = get_edgeblock_offset(xvtx_id) + wblkmargin.top / WORK_BLOCK_HEIGHT;
#endif

        //initialize module unit
        moduleunitcmd.mode = FINDONLYMODE;

        // initialize appropriate fields of lcs units
        // everything is initialized here
        initialize_moduleunit_params(&moduleparams, edge.xadjvtx_id, edge.edgew);
        initialize_loadunit(&loadunitcmd);
        initialize_insertunit(&insertparams, &insertreport, edge.xadjvtx_id, hadjvtx_id, edge.edgew);
        initialize_findunit(&findparams, &findreport, edge.xadjvtx_id, hadjvtx_id, edge.edgew);
        initialize_writebackunit(&writebackunitcmd);
#ifdef EN_LLGDS
        initialize_llebaverdictcmd(&llgdsunitcmd);
        clear_lleba_addresses_in_moduleparams(&moduleparams);
#endif
#ifdef EN_CRUMPLEINONDELETE
        init_deleteandcrumplein_verdictcmd(&deleteandcrumpleincmd);
#endif

        //initialize trip iteration
        uint infiniti = 0;
        uint prevLoadAddr = NAv;

        vertexid_t xvtxid = 0;
        vertexid_t xadjvtxid = 0;

        if (_updatev != OFF) {
            // update vertex of first edge
            xvtxid = edge.xvtx_id;
            xadjvtxid = edge.xadjvtx_id;
            if (_sgh_for_xvtxid == SELF) {
                xvtxid = _translator_handler.get_globalvid(edge.xvtx_id);
            }
            if (_sgh_for_xadjvtxid == SELF) {
                xadjvtxid = _translator_handler.get_globalvid(edge.xadjvtx_id);
            }
            external_vertices_handler.update_vertex_property(xvtxid, xadjvtxid, edgeupdatecmd, _graphdirectiontype);
        }

        //***
        //***
        //***
        //***
        //***
        //***
        //***
        //*** loop
        for (infiniti = 0; infiniti < SOMELARGENO; infiniti++) {

            //***********************************************************************************************//Load unit
            if (geni == 1) {
                load_unit(
                        &moduleparams,
                        loadunitcmd,
                        &insertparams,
                        &findparams,
                        wblkmargin,
                        xvtx_id,
                        edge,
                        &edgeblock,
                        _edge_block_array_m,
                        &prevLoadAddr,
                        geni);
            } else {
                load_unit(
                        &moduleparams,
                        loadunitcmd,
                        &insertparams,
                        &findparams,
                        wblkmargin,
                        xvtx_id,
                        edge,
                        &edgeblock,
                        _edge_block_array_o,
                        &prevLoadAddr,
                        geni);
            }
            //***********************************************************************************************

            //***********************************************************************************************//load compute-params unit
            load_params(
                    moduleparams,
                    &insertparams,
                    &findparams,
                    wblkmargin,
                    hadjvtx_id);
            //***********************************************************************************************

            //***********************************************************************************************//Compute unit
            /// one-time-fields of incomming edge is updated here
            compute_unit(
                    moduleunitcmd,
                    &moduleparams,
                    &insertparams,
                    &insertreport,
                    findparams,
                    &findreport,
                    wblkmargin,
                    subblkmargin,
                    start_wblkmargin,
                    hadjvtx_id,
                    &edgeblock,
                    &edge,
                    xvtx_id,
                    moduleparams.rolledover,
                    edgeupdatecmd,
                    geni);
            //***********************************************************************************************

            //***********************************************************************************************//Inference unit
            inference_unit(
                    edgeupdatecmd,
                    &moduleunitcmd,
                    &moduleparams,
                    &loadunitcmd,
                    findparams,
                    findreport,
                    insertparams,
                    insertreport,
                    &writebackunitcmd,
                    &intervalunitcmd,
                    &wblkmargin,
                    subblkmargin,
                    start_wblkmargin,
                    xvtx_id
#ifdef EN_LLGDS
                    ,
                    &llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
            ,
            &deleteandcrumpleincmd
#endif
            );
            //***********************************************************************************************

            //***********************************************************************************************//EBA port unit
            writeback_unit(
                    &moduleparams,
                    writebackunitcmd,
                    edge,
                    &edgeblock,
                    _edge_block_array_m,
                    _edge_block_array_o,
#ifdef EN_CRUMPLEINONDELETE
                    edgeblock_parentinfo,
#endif
                    &_lvatracker,
                    xvtx_id,
                    first_wblkmargin,
                    subblkmargin,
                    geni,
                    edgeupdatecmd
#ifdef EN_CRUMPLEINONDELETE
            ,
            lastgenworkblockaddr, svs
#ifdef EN_LLGDS
            ,
            _ll_edge_block_array
#endif
#endif
            );
//***********************************************************************************************

//***********************************************************************************************// delete and crumple in
#ifdef EN_CRUMPLEINONDELETE
            deleteandcrumplein_unit(
                writebackunitcmd,
                findreport,
                edge,
                _edge_block_array_m,
                _edge_block_array_o,
#ifdef EN_LLGDS
                _ll_edge_block_array,
#endif
                edgeblock_parentinfo,
                xvtx_id,
                wblkmargin,
                subblkmargin,
                geni
#ifdef EN_CRUMPLEINONDELETE
                ,
                deleteandcrumpleincmd, svs, freed_edgeblock_list
#endif
            );
#endif
//***********************************************************************************************

//***********************************************************************************************// GraphTinkerLL
#ifdef EN_LLGDS
            ll_unit(
                    llgdsunitcmd,
                    &moduleparams,
                    &insertparams,
                    edge,
                    _edge_block_array_m,
                    _edge_block_array_o,
                    geni
#ifdef EN_LLGDS
                    ,
                    _ll_edge_block_array, _ll_lva, &_ll_eba_tracker
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
                    &wblkmargin,
                    &subblkmargin,
                    &start_wblkmargin,
                    &first_wblkmargin,
                    &xvtx_id,
                    &hadjvtx_id,
                    edge,
                    edgeupdatecmd,
                    &tripiteration_lcs,
                    &geni,
                    &quitstatus,
                    infiniti
#ifdef EN_CRUMPLEINONDELETE
            ,
            &lastgenworkblockaddr
#endif
#ifdef EN_LLGDS
                    ,
                    &llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
            ,
            &deleteandcrumpleincmd
#endif
            );

            //***********************************************************************************************

            if (quitstatus == 1) {
#ifdef cpuem_l7
                cout << endl
                     << "successful. exiting... exiting... (update_singleedge)" << endl;
#endif
                return;
            }
            if (tripiteration_lcs > 100) {
#ifdef cpuem_bugs_b1
                cout << endl
                     << "bug! : trip iteration too much!, exiting..." << endl;
#endif
                return;
            }
            if (geni > 500000) {
#ifdef cpuem_bugs_b1
                cout << "bug! : too many generations (" << geni << ") spun!, exiting..." << endl;
                cout << "searchreport.searchstop = " << searchreport.searchstop << endl;
                cout << "searchreport.searchsuccessful = " << searchreport.searchsuccessful << endl;
                cout << "insertreport.exittype = " << insertreport.exittype << endl;
                cout << "moduleunitcmd.mode = " << moduleunitcmd.mode << endl;
#endif
                return;
            }
        }
        cout << endl
             << "bug! : infiniti exhausted : infiniti = " << infiniti << "" << endl;
        cout << "searchreport.searchstop = " << searchreport.searchstop << endl;
        cout << "searchreport.searchsuccessful = " << searchreport.searchsuccessful << endl;
        cout << "insertreport.exittype = " << insertreport.exittype << endl;
        cout << "moduleunitcmd.mode = " << moduleunitcmd.mode << endl;
        return;
    }

    vertexid_t Graphtinker::retrieve_edges(vertexid_t vid, vector<edge_tt> &edges) {
        vector<clusterptr_t> clusterptrs;
        vertexid_t basevid = vid;
        uint _geni = 1;

        // load edges
        clusterptrs.push_back(vid);
        uint len = clusterptrs.size();
        uint wps = _work_blocks_per_subblock;

        while (true) {
            for (uint i = 0; i < len; i++) {
                vid = clusterptrs.back();
                clusterptrs.pop_back();

                uint ebaoffset = get_edgeblock_offset(vid);
                for (uint t = 0; t < _work_blocks_per_page; t++) {
                    edge_nt edgeset;
                    if (_geni == 1) {
                        edgeset = _edge_block_array_m[(ebaoffset + t)];
                    } else {
                        edgeset = _edge_block_array_o[(ebaoffset + t)];
                    }

                    for (uint k = 0; k < WORK_BLOCK_HEIGHT; k++) {
                        if (edgeset.edges[k].flag == VALID) {
                            edges.push_back(edgeset.edges[k]);
                        }
                    }

                    if (((t % wps) == 0) && (edgeset.clusterinfo.flag == VALID)) {
                        clusterptrs.push_back(edgeset.clusterinfo.data);
                    }
                }
            }

            len = clusterptrs.size();
            if (len == 0) {
                break;
            } else {
                _geni += 1;
            }
        }
        return basevid;
    }

    vector<edge_nt> &Graphtinker::get_edge_block_array_m() {
        return _edge_block_array_m;
    }

    vector<edge_nt> &Graphtinker::get_edge_block_array_c() {
        return _edge_block_array_o;
    }

    vector<ll_logicalvertexentity_t> &Graphtinker::get_ll_logicalvertexentity() {
        return _ll_lva;
    }

    vector<ll_edgeblock_t> &Graphtinker::get_ll_edge_block_array() {
        return _ll_edge_block_array;
    }

    uint Graphtinker::printv_edgecount() {
        uint edgecount = 0;
        edgecount += Graphtinker::get_edgecount(_edge_block_array_m, get_edge_block_array_height(_edge_block_array_m));
        edgecount += Graphtinker::get_edgecount(_edge_block_array_o, _lvatracker.mark);
        return edgecount;
    }

    uint Graphtinker::get_edgecount(vector<edge_nt> edge_block_array_x, uint height) {
        uint edgecount = 0;
        for (vertexid_t vid = 0; vid < height; vid++) {
            uint offset = get_edgeblock_offset(vid);
            for (uint j = offset; j < (offset + _work_blocks_per_page); j++) {
                for (uint k = 0; k < WORK_BLOCK_HEIGHT; k++) {
                    if (edge_block_array_x[j].edges[k].flag == VALID) {
                        edgecount += edge_block_array_x[j].edges[k].edge_weight;
                    }
                }
            }
        }
        return edgecount;
    }

    uint Graphtinker::printv_uniqueedgecount() {
        uint edgecount = 0;
        edgecount += Graphtinker::get_uniqueedgecount(_edge_block_array_m,
                                                      get_edge_block_array_height(_edge_block_array_m));
        edgecount += Graphtinker::get_uniqueedgecount(_edge_block_array_o, _lvatracker.mark);
        return edgecount;
    }

    uint Graphtinker::get_uniqueedgecount(vector<edge_nt> edge_block_array_x, uint height) {
        uint edgecount = 0;
        for (vertexid_t vid = 0; vid < height; vid++) {
            uint offset = get_edgeblock_offset(vid);
            for (uint j = offset; j < (offset + _work_blocks_per_page); j++) {
                for (uint k = 0; k < WORK_BLOCK_HEIGHT; k++) {
                    if (edge_block_array_x[j].edges[k].flag == VALID) {
                        edgecount += 1;
                    }
                }
            }
        }
        return edgecount;
    }

    uint Graphtinker::printll_edgecount() {
        uint uniqueedgecount = 0;
        uniqueedgecount += ll_countedges(_ll_edge_block_array);
        return uniqueedgecount;
    }

    uint Graphtinker::ll_countedges(vector<ll_edgeblock_t> &_ll_edge_block_array) {
        uint uniqueedgecount = 0;
        for (uint i = 0; i < _ll_edge_block_array.size(); i++) { //ll_edgeblock_array_size
            for (uint j = 0; j < LLEDGEBLOCKSIZE; j++) {
                if (_ll_edge_block_array[i].ll_edgeblock[j].flag == VALID) {
                    uniqueedgecount += _ll_edge_block_array[i].ll_edgeblock[j].edgew;
                }
            }
        }
        return uniqueedgecount;
    }

#ifdef EN_CRUMPLEINONDELETE
    uint Graphtinker::print_svs_size()
    {
        return svs.size();
    }
#endif

#ifdef EN_CRUMPLEINONDELETE
    uint Graphtinker::print_freed_edgeblock_list_size()
    {
        return freed_edgeblock_list.size();
    }
#endif

    void Graphtinker::initialize_lvas() {
        for (uint i = 0; i < _ll_lva.size(); i++) {
            _ll_lva[i].baseaddr = 0;
            _ll_lva[i].lastlocalbaseaddr = 0;
            _ll_lva[i].lastlocaladdr = 0;
            _ll_lva[i].totaledgecount = 0;
            _ll_lva[i].flag = INVALID;
        }
        return;
    }

    vertices &Graphtinker::get_vertices_handler() {
        return _vertices_handler;
    }

    uint Graphtinker::get_graphdirectiontype() {
        return _graphdirectiontype;
    }

    translator &Graphtinker::gettranslator() {
        return _translator_handler;
    }

    uint Graphtinker::get_translator_tracker_mark() {
        return _translator_handler.get_translator_tracker_mark();
    }

    uint Graphtinker::get_work_blocks_per_page() {
        return _work_blocks_per_page;
    }

    uint Graphtinker::get_work_blocks_per_subblock() {
        return _work_blocks_per_subblock;
    }

    void Graphtinker::print_edgeblockarray(vertexid_t begin, vertexid_t end) {
        vector<edge_tt> edges;

        for (uint i = begin; i < end; i++) {
            retrieve_edges(0, edges);
            for (uint j = 0; j < edges.size(); j++) {
                cout << "Graphtinker::print_edgeblockarray : srcvid : " << i << ", dstvid : " << edges[j].xadjvtx_id
                     << ", edgew : " << edges[j].edge_weight << endl;
            }
            edges.clear();
        }
        return;
    }

    uint Graphtinker::get_edge_block_array_height(vector<edge_nt> &edge_block_array) {
        return (edge_block_array.size() / _work_blocks_per_page);
    }

    void Graphtinker::check_whether_to_resize_edgeblockarray_m(uint vid) {
        if (vid >= get_edge_block_array_height(_edge_block_array_m)) {
            uint newsz = (vid + _eba_m_expansion_addition_height) * _work_blocks_per_page;
            _edge_block_array_m.resize(newsz);
            cout << "Graphtinker::resize_edgeblockarray_m : resizing... : vid : " << vid
                 << ", new _edge_block_array_m size : " << _edge_block_array_m.size()
                 << ", new _edge_block_array_m height : " << get_edge_block_array_height(_edge_block_array_m) << endl;
        }
        return;
    }

    uint Graphtinker::get_vertex_range() {
        return _vertex_range;
    }
}