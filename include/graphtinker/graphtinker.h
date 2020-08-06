#ifndef GRAPHTINKER_H
#define GRAPHTINKER_H

#include <iostream>
#include <vector>
#include "common.h"
#include "vertices.h"
#include "translator.h"
#include "config/config.h"
using namespace std;
using std::string;
/// classes
namespace gt {

    class Graphtinker {
    private:


    public:
        Graphtinker();

        Graphtinker(string file_path);

        ~Graphtinker();


        Config *_config;
        // config
        uint _sgh_for_xvtxid;
        uint _sgh_for_xadjvtxid;
        uint _updatev;
        uint _vertex_range;
        uint _num_vertices;
        uint _num_edges;
        uint _graphdirectiontype;

        uint _sub_block_height;
        uint _page_block_height;
        uint _sub_blocks_per_page;
        //compute
        uint _work_blocks_per_page;
        uint _work_blocks_per_subblock;

        uint _eba_m_expansion_addition_height;
        uint _eba_o_expansion_addition_height;
        uint _ll_eba_expansion_addition_height;
        uint _ll_lva_expansion_addition_height;
        uint _eba_expansion_padding;

        // edge block array
        vector<edge_nt> _edge_block_array_m;
        vector<edge_nt> _edge_block_array_o;
        tracker_t _lvatracker;

        // ll edge block array
        vector<ll_edgeblock_t> _ll_edge_block_array;
        vector<ll_logicalvertexentity_t> _ll_lva;
        ll_eba_tracker_t _ll_eba_tracker;

        // vertices
        vertices _vertices_handler;

        // translator
        translator _translator_handler;

// metadata (for delete and crumple in)
#ifdef EN_CRUMPLEINONDELETE
        vector<supervertex_t> svs;
        vector<vertexid_t> freed_edgeblock_list;
#endif
#ifdef EN_CRUMPLEINONDELETE
        vector<edgeblock_parentinfo_t> edgeblock_parentinfo;
#endif


        // member functions
        void CreateGraph();

        void insert_edge(uint src, uint dst, uint ew);

        void insert_edge(uint src, uint dst, uint ew, vertices &_vertices_handler);

        void insert_edge(uint src, uint dst, uint ew, translator &_translator_handler);

        void
        insert_edge(uint src, uint dst, uint ew, vertices &external_vertices_handler, translator &_translator_handler);

        void delete_edge(uint src, uint dst, uint ew);

        void delete_edge(uint src, uint dst, uint ew, vertices &_vertices_handler);

        void update_edge(uint src, uint dst, uint ew, uint edgeupdatecmd, vertices &_vertices_handler);

        vertexid_t retrieve_edges(vertexid_t vid, vector<edge_tt> &edges);

        vector<edge_nt> &get_edge_block_array_m();

        vector<edge_nt> &get_edge_block_array_c();

        vector<ll_logicalvertexentity_t> &get_ll_logicalvertexentity();

        vector<ll_edgeblock_t> &get_ll_edge_block_array();

        uint printv_edgecount();

        uint get_edgecount(vector<edge_nt> edge_block_array_x, uint height);

        uint printv_uniqueedgecount();

        uint get_uniqueedgecount(vector<edge_nt> edge_block_array_x, uint height);

        uint printll_edgecount();

        uint ll_countedges(vector<ll_edgeblock_t> &_ll_edge_block_array);

        uint print_svs_size();

        uint print_freed_edgeblock_list_size();

        void initialize_lvas();

        vertices &get_vertices_handler();

        uint get_graphdirectiontype();

        translator &gettranslator();

        uint get_translator_tracker_mark();

        uint get_work_blocks_per_page();

        uint get_work_blocks_per_subblock();

        void print_edgeblockarray(vertexid_t begin, vertexid_t end);

        uint get_edge_block_array_height(vector<edge_nt> &edge_block_array);

        void check_whether_to_resize_edgeblockarray_m(uint vid);

        uint get_vertex_range();

        // compute unit
        void compute_unit(
                module_unit_cmd_t moduleunitcmd,
                module_params_t *moduleparams,
                insert_params_t *insertparams,
                insert_report_t *insertreport,
                find_params_t findparams,
                find_report_t *findreport,
                margin_t wblkmargin,
                margin_t subblkmargin,
                margin_t start_wblkmargin,
                bucket_t hadjvtx_id,
                edge_nt *edgeblock,
                edge_t *edge,
                vertexid_t xvtx_id,
                uint rolledover,
                uint edgeupdatecmd,
                uint geni);

        // find margin
        void findwblkmargin(margin_t *blkmargin, bucket_t hadjvtx_id);

        void findsubblkmargin(margin_t *subblkmargin, bucket_t hadjvtx_id);

        uint findsubblkId(vertexid_t vid);

        // commonutility
        uint max(uint A, uint B);

        // eba_manager
        uint get_edgeblock_offset(vertexid_t vid);

        // find unit
        void find_unit(
                module_params_t *moduleparams,
                find_params_t findparams,
                find_report_t *findreport,
                margin_t wblkmargin,
                margin_t subblkmargin,
                bucket_t hadjvtx_id,
                edge_nt *edgeblock,
                uint edgeupdatecmd);

        // gds_utility
        uint getsvtracker(markertracker_t *svtracker);

        // hash scripts
        bucket_t googlehash(vertexid_t vid, uint geni);

        // inference unit
        void inference_unit(
                uint edgeupdatecmd,
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                find_report_t findreport,
                insert_params_t insertparams, //***
                insert_report_t insertreport,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t *wblkmargin,
                margin_t subblkmargin,
                margin_t start_wblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void findonlymode_searchstopped_searchunsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void findonlymode_searchstopped_searchsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams, //***
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t *wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams, //***
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams, //***
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t *wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void insertonlymode_x_x_loadedintoemptybucket_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void insertonlymode_x_x_foundandupdateditself_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void insertonlymode_x_x_passedthrough_notlastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t *wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void insertonlymode_x_x_passedthrough_lastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void findanddeletemode_searchstopped_searchunsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void findanddeletemode_searchstopped_searchsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                find_report_t findreport,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                find_report_t findreport,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t wblkmargin,
                margin_t subblkmargin,
                vertexid_t xvtx_id
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
        );

        void set_intervalunitcmd_continue_from_first_generation(interval_unit_cmd_t *intervalunitcmd);

        void set_intervalunitcmd_continue_in_lower_generation(interval_unit_cmd_t *intervalunitcmd);

        void
        set_intervalunitcmd_continue_in_current_generation(interval_unit_cmd_t *intervalunitcmd, margin_t *wblkmargin,
                                                           margin_t subblkmargin, module_params_t *moduleparams);

        void set_intervalunitcmd_quit_to_next_edge(interval_unit_cmd_t *intervalunitcmd);

        void set_loadunitcmd_loadnextEB(load_unit_cmd_t *loadunitcmd, uint cmd);

        void set_moduleunitcmd(module_unit_cmd_t *moduleunitcmd, uint cmd);

        void
        set_moduleunitparamsedgefields(module_params_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight);

        void set_writebackunitcmd_writebackcurrentEB(writeback_unit_cmd_t *writebackunitcmd, uint cmd, uint addr,
                                                     margin_t subblkmargin);

        void setwritebackunitcmd_markasclustered(writeback_unit_cmd_t *writebackunitcmd, uint cmd, uint addr);

        void set_llgdsunitcmd(llgds_unit_cmd_t *llgdsunitcmd, uint cmd);

        void updatemarginandrolloverstatus(margin_t *wblkmargin, margin_t subblkmargin, module_params_t *moduleparams);

        uint isthelastworkblock(margin_t wblkmargin, margin_t start_wblkmargin, margin_t subblkmargin,
                                module_params_t moduleparams);

        // initialize
        uint add_page(tracker_t *tracker, vector<edge_nt> &edge_block_array);

        // insert unit
        void insert_unit(
                module_params_t *moduleparams,
                insert_params_t *insertparams,
                insert_report_t *insertreport,
                margin_t wblkmargin,
                margin_t subblkmargin,
                bucket_t hadjvtx_id,
                edge_nt *edgeblock,
                edge_t *edge,
                vertexid_t xvtx_id,
                uint rolledover,
                uint geni);

        bucket_t getdib(bucket_t currbkt, bucket_t initialbucket, margin_t subblkmargin, uint rolledover);

        // interval unit
        void interval_unit_updateedge(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t *insertparams,
                insert_report_t *insertreport,
                find_params_t *findparams,
                find_report_t *findreport,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t intervalunitcmd,
                margin_t *wblkmargin,
                margin_t *subblkmargin,
                margin_t *start_wblkmargin,
                margin_t *first_wblkmargin,
                vertexid_t *xvtx_id,
                bucket_t *hadjvtx_id,
                edge_t edge,
                uint edgeupdatecmd,
                uint *tripiteration_lcs,
                uint *geni,
                uint *quitstatus,
                uint infiniti
#ifdef EN_CRUMPLEINONDELETE
        ,
        uint *lastgenworkblockaddr
#endif
#ifdef EN_LLGDS
                ,
                llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
        ,
        delete_and_crumple_in_cmd_t *heba_deleteandcrumplein_cmd
#endif
        );

        // interval unit functions
        void initallmoduleunit(module_unit_cmd_t *moduleunitcmd, module_params_t *moduleparams, vertexid_t xadjvtx_id,
                               edgeweight_t edge_weight);

        void
        initialize_moduleunit_params(module_params_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight);

        void initialize_moduleunit_params2(module_params_t *moduleparams);

        void initialize_loadunit(load_unit_cmd_t *loadunitcmd);

        void initialize_insertunit(insert_params_t *insertparams, insert_report_t *insertreport, vertexid_t xadjvtx_id,
                                   bucket_t hadjvtx_id, edgeweight_t edge_weight);

        void initialize_insertunit2(insert_params_t *insertparams, insert_report_t *insertreport);

        void initialize_findunit(find_params_t *findparams, find_report_t *findreport, vertexid_t xadjvtx_id,
                                 bucket_t hadjvtx_id, edgeweight_t edge_weight);

        void initialize_findunit2(find_params_t *findparams, find_report_t *findreport);

        void initialize_writebackunit(writeback_unit_cmd_t *writebackunitcmd);

        void initialize_llebaverdictcmd(llgds_unit_cmd_t *llgdsunitcmd);

#ifdef EN_LLGDS

        void clear_lleba_addresses_in_moduleparams(module_params_t *moduleparams);

#endif

        void init_deleteandcrumplein_verdictcmd(delete_and_crumple_in_cmd_t *heba_deleteandcrumplein_cmd);

        // ll functions
        void ll_insert(
                edge_t edge,
                module_params_t *moduleparams,
                insert_params_t *insertparams,
                vector<ll_edgeblock_t> &_ll_edge_block_array,
                vector<ll_logicalvertexentity_t> &_ll_lva,
                ll_eba_tracker_t *_ll_eba_tracker,
                uint geni);

        void ll_update(
                edge_t edge,
                vertexid_t ll_localbaseaddrptr,
                vertexid_t ll_localaddrptr,
                vector<ll_edgeblock_t> &_ll_edge_block_array);

        void ll_delete(
                edge_t edge,
                vertexid_t ll_localbaseaddrptr,
                vertexid_t ll_localaddrptr,
                vector<ll_edgeblock_t> &_ll_edge_block_array);

        void ll_deleteandcrumplein(
                edge_t edge,
                vertexid_t ll_localbaseaddrptr,
                vertexid_t ll_localaddrptr,
                vector<ll_edgeblock_t> &_ll_edge_block_array,
                vector<ll_logicalvertexentity_t> &_ll_lva,
                ll_eba_tracker_t *_ll_eba_tracker,
                vector<edge_nt> &_edge_block_array_m,
                vector<edge_nt> &_edge_block_array_o,
                uint geni);

        void ll_updateedgeptrs(
                edge_t edge,
                module_params_t moduleparams,
                vector<ll_edgeblock_t> &_ll_edge_block_array);

        // load parameters
        void load_params(
                module_params_t moduleparams,
                insert_params_t *insertparams,
                find_params_t *findparams,
                margin_t wblkmargin,
                bucket_t hadjvtx_id);

        // load unit
        void load_unit(
                module_params_t *moduleparams,
                load_unit_cmd_t loadunitcmd,
                insert_params_t *insertparams,
                find_params_t *findparams,
                margin_t wblkmargin,
                vertexid_t hvtx_id,
                edge_t edge,
                edge_nt *edgeblock,
                vector<edge_nt> &edge_block_array,
                uint *prevLoadAddr,
                uint geni);

        // super vertices
        int sv_get_next_edge(
                uint xvtx_id,
                vector<supervertex_t> &svs,
                vector<vertexid_t> &freed_edgeblock_list,
                edge_tt *edgett,
                margin_t wblkmargin,
                writeback_unit_cmd_t writebackunitcmd,
                int *tailhvtx_id,
                uint *svs_index,
                uint *numclusteredworkblocks,
                uint geni,
                vector<edge_nt> &_edge_block_array_m,
                vector<edge_nt> &_edge_block_array_o);

        int sv_get_tail_edgeblock(
                vector<supervertex_t> &svs,
                uint currworkblockaddr,
                uint *svs_index,
                uint geni,
                vector<edge_nt> &_edge_block_array_m,
                vector<edge_nt> &_edge_block_array_o);

        int sv_pick_edge(
                uint xvtx_id,
                uint offset,
                edge_tt *edgett,
                uint *numclusteredworkblocks,
                vector<edge_nt> &edge_block_array);

        // utility
        uint bitcombine_AB(uint A, uint B);

        uint bitselect_ABC(uint A, uint B, uint C);

        float min_float(float A, float B);

        vertexdata_t min_type1(vertexdata_t A, vertexdata_t B);

        uint getpartitionid(uint vid);

        // vertices
        vertexproperty_t readproperty(vertexid_t vertexid);

        void writeproperty(vertexid_t vertexid, vertexdata_t vertexdata);

        void update_vertex_property(edge_t edge, uint edgedirection, uint edgeupdatecmd);

        void initialize();

        // writeback unit
        void writeback_unit(
                module_params_t *moduleparams,
                writeback_unit_cmd_t writebackunitcmd,
                edge_t edge,
                edge_nt *edgeblock,
                vector<edge_nt> &_edge_block_array_m,
                vector<edge_nt> &_edge_block_array_o,
#ifdef EN_CRUMPLEINONDELETE
                vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
#endif
                tracker_t *_lvatracker,
                vertexid_t hvtx_id,
                margin_t first_wblkmargin,
                margin_t subblkmargin,
                uint geni,
                uint edgeupdatecmd
#ifdef EN_CRUMPLEINONDELETE
        ,
        uint lastgenworkblockaddr, vector<supervertex_t> &svs
#ifdef EN_LLGDS
        ,
        vector<ll_edgeblock_t> &_ll_edge_block_array
#endif
#endif
        );

        // delete and crumple in unit
        void deleteandcrumplein_unit(
                writeback_unit_cmd_t writebackunitcmd,
                find_report_t findreport,
                edge_t edge,
                vector<edge_nt> &_edge_block_array_m,
                vector<edge_nt> &_edge_block_array_o,
#ifdef EN_LLGDS
                vector<ll_edgeblock_t> &_ll_edge_block_array,
#endif
                vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
                vertexid_t xvtx_id,
                margin_t wblkmargin,
                margin_t subblkmargin,
                uint geni, delete_and_crumple_in_cmd_t deleteandcrumpleincmd, vector<supervertex_t> &svs,
                vector<vertexid_t> &freed_edgeblock_list);

        // ll unit
        void ll_unit(
                llgds_unit_cmd_t llgdsunitcmd,
                module_params_t *moduleparams,
                insert_params_t *insertparams,
                edge_t edge,
                vector<edge_nt> &_edge_block_array_m,
                vector<edge_nt> &_edge_block_array_o,
                uint geni
#ifdef EN_LLGDS
                ,
                vector<ll_edgeblock_t> &_ll_edge_block_array, vector<ll_logicalvertexentity_t> &_ll_lva,
                ll_eba_tracker_t *_ll_eba_tracker
#endif
        );

    private:
    };

}
#endif
