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
// TODO: 1. 计划把unit中的参数，即状态机过程中用到的所有信息，全部封装到UnitOption中, 以简化代码
namespace gt {

    class Graphtinker {
    private:


    public:
        Graphtinker();

        Graphtinker(string file_path);

        ~Graphtinker();


        Config *_config;
        // config
        uint sgh_for_xvtxid_;
        uint sgh_for_xadjvtxid_;
        uint updatev_;
        uint vertex_range_;
        uint num_vertices_;
        uint num_edges_;
        uint graphdirectiontype_;

        uint sub_block_height_;
        uint page_block_height_;
        uint sub_blocks_per_page_;
        //compute
        uint work_blocks_per_page_;
        uint work_blocks_per_subblock_;

        uint eba_m_expansion_addition_height_;
        uint eba_o_expansion_addition_height_;
        uint ll_eba_expansion_addition_height_;
        uint ll_lva_expansion_addition_height_;
        uint eba_expansion_padding_;

        // edge block array
        vector<work_block_t> edge_block_array_m_;
        vector<work_block_t> edge_block_array_o_;
        tracker_t lvatracker_;

        // ll edge block array
        vector<ll_edgeblock_t> ll_edge_block_array_;
        vector<ll_logicalvertexentity_t> ll_lva_;
        ll_eba_tracker_t ll_eba_tracker_;

        // Vertex Property Array
        Vertices* vertices_handler_;

        // Scatter Gather Hash
        Translator* translator_handler_;

// metadata (for delete and crumple in)
#ifdef EN_CRUMPLE_IN
        vector<supervertex_t> svs;
        vector<vertexid_t> freed_edgeblock_list;
        vector<edgeblock_parentinfo_t> edgeblock_parentinfo;
#endif


        // member functions
        void CreateGraph();

        void InsertEdge(uint src, uint dst, uint ew);

        void InsertEdge(uint src, uint dst, uint ew, Vertices* vertices_handler_);

        void InsertEdge(uint src, uint dst, uint ew, Translator* translator_handler_);

        void InsertEdge(uint src, uint dst, uint ew, Vertices* external_vertices_handler, Translator* translator_handler_);

        void BatchInsertEdge(const char* path, uint batch_size);

        void DeleteEdge(uint src, uint dst, uint ew);

        void DeleteEdge(uint src, uint dst, uint ew, Vertices* vertices_handler_);

        void BatchDeleteEdge(const char* path, uint batch_size);

        void update_edge(uint src, uint dst, uint ew, uint edge_update_cmd, Vertices* vertices_handler_);

        vertexid_t retrieve_edges(vertexid_t vid, vector<edge_tt> &edges);


        uint printv_edgecount();

        uint get_edgecount(vector<work_block_t> edge_block_array_x, uint height);

        uint printv_uniqueedgecount();

        uint get_uniqueedgecount(vector<work_block_t> edge_block_array_x, uint height);

        uint printll_edgecount();

        uint ll_countedges(vector<ll_edgeblock_t> &ll_edge_block_array_);

        uint print_svs_size();

        uint print_freed_edgeblock_list_size();

        void initialize_lvas();

        // getter and setter
        const uint vertex_range() const;
        const Translator* translator() const;
        const Vertices* vertices() const;
        const uint graphdirectiontype() const;
        const uint translator_tracker_mark() const;
        const uint work_blocks_per_page() const;
        const uint work_blocks_per_subblock() const;
        const vector<work_block_t> &edge_block_array_m() const;
        const vector<work_block_t> &edge_block_array_c() const;
        const vector<ll_logicalvertexentity_t> &ll_logicalvertexentity() const;
        const vector<ll_edgeblock_t> &ll_edge_block_array() const;

        // print function
        void print_edgeblockarray(const vertexid_t begin, const vertexid_t end);
        uint GetEdgeBlockArrayHeight(const vector<work_block_t> &edge_block_array); 
        void check_whether_to_resize_edgeblockarray_m(uint vid);


        // compute unit
        void ComputeUnit(
                module_unit_cmd_t moduleunitcmd,
                module_params_t *moduleparams,
                insert_params_t *insertparams,
                insert_report_t *insertreport,
                find_params_t findparams,
                find_report_t *findreport,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                margin_t start_wblkmargin,
                bucket_t hadjvtx_id,
                work_block_t *work_block,
                edge_t *edge,
                vertexid_t xvtx_id,
                uint rolledover,
                uint edge_update_cmd,
                uint geni);

        // find margin
        void FindWorkBlockMargin(margin_t *blkmargin, bucket_t hadjvtx_id);

        void FindSubBlockMargin(margin_t *sub_block_margin, bucket_t hadjvtx_id);

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
                margin_t work_block_margin,
                margin_t sub_block_margin,
                bucket_t hadjvtx_id,
                work_block_t *work_block,
                uint edge_update_cmd);

        // gds_utility
        uint getsvtracker(markertracker_t *svtracker);

        // hash scripts
        bucket_t googlehash(vertexid_t vid, uint geni) const;

        // inference unit
        void InferenceUnit(
                uint edge_update_cmd,
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                find_report_t findreport,
                insert_params_t insertparams, //***
                insert_report_t insertreport,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t *work_block_margin,
                margin_t sub_block_margin,
                margin_t start_wblkmargin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd = nullptr,
                crumple_in_cmd_t *heba_popoutpopin_cmd = nullptr
        );

        void findonlymode_searchstopped_searchunsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findonlymode_searchstopped_searchsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams, //***
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id ,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t *work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams, //***
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams, //***
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t *work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd 
        );

        void insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void insertonlymode_x_x_loadedintoemptybucket_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void insertonlymode_x_x_foundandupdateditself_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void insertonlymode_x_x_passedthrough_notlastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t *work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd = nullptr,
                crumple_in_cmd_t *heba_popoutpopin_cmd = nullptr
        );

        void insertonlymode_x_x_passedthrough_lastworkblock(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                insert_params_t insertparams, //***
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findanddeletemode_searchstopped_searchunsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findanddeletemode_searchstopped_searchsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                find_report_t findreport,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
                module_unit_cmd_t *moduleunitcmd,
                module_params_t *moduleparams,
                load_unit_cmd_t *loadunitcmd,
                find_params_t findparams,
                find_report_t findreport,
                writeback_unit_cmd_t *writebackunitcmd,
                interval_unit_cmd_t *intervalunitcmd,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t xvtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void set_intervalunitcmd_continue_from_first_generation(interval_unit_cmd_t *intervalunitcmd);

        void set_intervalunitcmd_continue_in_lower_generation(interval_unit_cmd_t *intervalunitcmd);

        void
        set_intervalunitcmd_continue_in_current_generation(interval_unit_cmd_t *intervalunitcmd, margin_t *work_block_margin,
                                                           margin_t sub_block_margin, module_params_t *moduleparams);

        void set_intervalunitcmd_quit_to_next_edge(interval_unit_cmd_t *intervalunitcmd);

        void set_loadunitcmd_loadnextEB(load_unit_cmd_t *loadunitcmd, uint cmd);

        void set_moduleunitcmd(module_unit_cmd_t *moduleunitcmd, uint cmd);

        void
        set_moduleunitparamsedgefields(module_params_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight);

        void set_writebackunitcmd_writebackcurrentEB(writeback_unit_cmd_t *writebackunitcmd, uint cmd, uint addr,
                                                     margin_t sub_block_margin);

        void setwritebackunitcmd_markasclustered(writeback_unit_cmd_t *writebackunitcmd, uint cmd, uint addr);

        void set_cal_unit_cmd(cal_unit_cmd_t *cal_unit_cmd, uint cmd);

        void updatemarginandrolloverstatus(margin_t *work_block_margin, margin_t sub_block_margin, module_params_t *moduleparams);

        bool IsLastWorkBlockInSubBlock(margin_t work_block_margin, margin_t start_wblkmargin, margin_t sub_block_margin,
                                module_params_t moduleparams);

        // initialize
        uint add_page(tracker_t *tracker, vector<work_block_t> &edge_block_array);

        // insert unit
        void insert_unit(
                module_params_t *moduleparams,
                insert_params_t *insertparams,
                insert_report_t *insertreport,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                bucket_t hadjvtx_id,
                work_block_t *work_block,
                edge_t *edge,
                vertexid_t xvtx_id,
                uint rolledover,
                uint geni);

        bucket_t getdib(bucket_t currbkt, bucket_t initialbucket, margin_t sub_block_margin, uint rolledover);

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
                margin_t *work_block_margin,
                margin_t *sub_block_margin,
                margin_t *start_wblkmargin,
                margin_t *first_wblkmargin,
                vertexid_t *xvtx_id,
                bucket_t *hadjvtx_id,
                edge_t edge,
                uint edge_update_cmd,
                uint *tripiteration_lcs,
                uint *geni,
                uint *quitstatus,
                uint infiniti
#ifdef EN_CRUMPLE_IN
        ,
        uint *lastgenworkblockaddr
#endif
#ifdef EN_CAL
                ,
                cal_unit_cmd_t *cal_unit_cmd
#endif
#ifdef EN_CRUMPLE_IN
        ,
        crumple_in_cmd_t *heba_deleteandcrumplein_cmd
#endif
        );

        // interval unit functions
        void initallmoduleunit(module_unit_cmd_t *moduleunitcmd, module_params_t *moduleparams, vertexid_t xadjvtx_id,
                               edgeweight_t edge_weight);

        void
        InitModuleUnitParams(module_params_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight);

        void initialize_moduleunit_params2(module_params_t *moduleparams);

        void InitLoadUnit(load_unit_cmd_t *loadunitcmd);

        void InitInsertUnit(insert_params_t *insertparams, insert_report_t *insertreport, vertexid_t xadjvtx_id,
                                   bucket_t hadjvtx_id, edgeweight_t edge_weight);

        void initialize_insertunit2(insert_params_t *insertparams, insert_report_t *insertreport);

        void InitFindUnit(find_params_t *findparams, find_report_t *findreport, vertexid_t xadjvtx_id,
                                 bucket_t hadjvtx_id, edgeweight_t edge_weight);

        void initialize_findunit2(find_params_t *findparams, find_report_t *findreport);

        void InitWritebackUnit(writeback_unit_cmd_t *writebackunitcmd);

        void initialize_llebaverdictcmd(cal_unit_cmd_t *cal_unit_cmd);

#ifdef EN_CAL

        void clear_lleba_addresses_in_moduleparams(module_params_t *moduleparams);

#endif

        void init_deleteandcrumplein_verdictcmd(crumple_in_cmd_t *heba_deleteandcrumplein_cmd);

        // ll functions
        void ll_insert(
                edge_t edge,
                module_params_t *moduleparams,
                insert_params_t *insertparams,
                vector<ll_edgeblock_t> &ll_edge_block_array_,
                vector<ll_logicalvertexentity_t> &ll_lva_,
                ll_eba_tracker_t *ll_eba_tracker_,
                uint geni);

        void ll_update(
                edge_t edge,
                vertexid_t ll_localbaseaddrptr,
                vertexid_t ll_localaddrptr,
                vector<ll_edgeblock_t> &ll_edge_block_array_);

        void ll_delete(
                edge_t edge,
                vertexid_t ll_localbaseaddrptr,
                vertexid_t ll_localaddrptr,
                vector<ll_edgeblock_t> &ll_edge_block_array_);

        void ll_deleteandcrumplein(
                edge_t edge,
                vertexid_t ll_localbaseaddrptr,
                vertexid_t ll_localaddrptr,
                vector<ll_edgeblock_t> &ll_edge_block_array_,
                vector<ll_logicalvertexentity_t> &ll_lva_,
                ll_eba_tracker_t *ll_eba_tracker_,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_,
                uint geni);

        void ll_updateedgeptrs(
                edge_t edge,
                module_params_t moduleparams,
                vector<ll_edgeblock_t> &ll_edge_block_array_);

        // load parameters
        void LoadParams(
                module_params_t moduleparams,
                insert_params_t *insertparams,
                find_params_t *findparams,
                margin_t work_block_margin,
                bucket_t hadjvtx_id);

        // load unit
        void LoadUnit(
                module_params_t *moduleparams,
                load_unit_cmd_t loadunitcmd,
                insert_params_t *insertparams,
                find_params_t *findparams,
                margin_t work_block_margin,
                vertexid_t hvtx_id,
                edge_t edge,
                work_block_t *work_block,
                vector<work_block_t> &edge_block_array,
                uint *prevLoadAddr,
                uint geni);

        // super Vertices
        int sv_get_next_edge(
                uint xvtx_id,
                vector<supervertex_t> &svs,
                vector<vertexid_t> &freed_edgeblock_list,
                edge_tt *edgett,
                margin_t work_block_margin,
                writeback_unit_cmd_t writebackunitcmd,
                int *tailhvtx_id,
                uint *svs_index,
                uint *numclusteredworkblocks,
                uint geni,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_);

        int sv_get_tail_edgeblock(
                vector<supervertex_t> &svs,
                uint currworkblockaddr,
                uint *svs_index,
                uint geni,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_);

        int sv_pick_edge(
                uint xvtx_id,
                uint offset,
                edge_tt *edgett,
                uint *numclusteredworkblocks,
                vector<work_block_t> &edge_block_array);

        // utility
        uint bitcombine_AB(uint A, uint B);

        uint bitselect_ABC(uint A, uint B, uint C);

        float min_float(float A, float B);

        vertexdata_t min_type1(vertexdata_t A, vertexdata_t B);

        uint getpartitionid(uint vid);

        // writeback unit
        void WritebackUnit(
                module_params_t *moduleparams,
                writeback_unit_cmd_t writebackunitcmd,
                edge_t edge,
                work_block_t *work_block,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_,
#ifdef EN_CRUMPLE_IN
                vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
#endif
                tracker_t *lvatracker_,
                vertexid_t hvtx_id,
                margin_t first_wblkmargin,
                margin_t sub_block_margin,
                uint geni,
                uint edge_update_cmd
#ifdef EN_CRUMPLE_IN
        ,
        uint lastgenworkblockaddr, vector<supervertex_t> &svs
#ifdef EN_CAL
        ,
        vector<ll_edgeblock_t> &ll_edge_block_array_
#endif
#endif
        );

        // delete and crumple in unit
        void CrumpleInUnit(
                writeback_unit_cmd_t writebackunitcmd,
                find_report_t findreport,
                edge_t edge,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_,
#ifdef EN_CAL
                vector<ll_edgeblock_t> &ll_edge_block_array_,
#endif
                vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
                vertexid_t xvtx_id,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                uint geni, crumple_in_cmd_t deleteandcrumpleincmd, vector<supervertex_t> &svs,
                vector<vertexid_t> &freed_edgeblock_list);

        // ll unit
        void CALUnit(
                cal_unit_cmd_t cal_unit_cmd,
                module_params_t *moduleparams,
                insert_params_t *insertparams,
                edge_t edge,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_,
                uint geni
#ifdef EN_CAL
                ,
                vector<ll_edgeblock_t> &ll_edge_block_array_, vector<ll_logicalvertexentity_t> &ll_lva_,
                ll_eba_tracker_t *ll_eba_tracker_
#endif
        );

    private:
    };

}
#endif
