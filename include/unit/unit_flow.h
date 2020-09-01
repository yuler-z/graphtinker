#ifndef UNIT_FLOW_H
#define UNIT_FLOW_H
#include "core/graphtinker.h"
#include "unit/unit_option.h"

namespace graphtinker{
    class UnitFlow
    {
    public:
        Graphtinker *gt_;
        UnitOption *unit_option;

        UnitFlow(Graphtinker* gt);
        ~UnitFlow();

        void load_unit(
                margin_t work_block_margin,
                vertexid_t vtx_id,
                vector<work_block_t> &edge_block_array,
                work_block_t *work_block);
        
        void load_params(
                margin_t work_block_margin,
                bucket_t adjvtx_id_hash); 

        void find_unit(
                margin_t work_block_margin,
                bucket_t adjvtx_id_hash,
                work_block_t *work_block,
                uint edge_update_cmd);

        void insert_unit(
                margin_t work_block_margin,
                bucket_t adjvtx_id_hash,
                work_block_t *work_block,
                edge_t *edge,
                uint geni);

        void inference_unit(
                uint edge_update_cmd,
                margin_t *work_block_margin,
                margin_t sub_block_margin,
                margin_t start_wblkmargin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd = nullptr,
                crumple_in_cmd_t *heba_popoutpopin_cmd = nullptr);
        // writeback unit
        void writeback_unit(
                edge_t edge,
                work_block_t *work_block,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_,
                tracker_t *lvatracker_,
                vertexid_t hvtx_id,
                margin_t first_wblkmargin,
                margin_t sub_block_margin,
                uint geni,
                uint edge_update_cmd
                #ifdef EN_CAL
                ,
                vector<cal_edgeblock_t> &cal_edgeblock_array_
                #endif
                #ifdef EN_DCI
                ,
                vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
                ,
                uint lastgenworkblockaddr, vector<supervertex_t> &svs
                #endif
        );

        void interval_unit(
                margin_t *work_block_margin,
                margin_t *sub_block_margin,
                margin_t *start_wblkmargin,
                margin_t *first_wblkmargin,
                vertexid_t *vtx_id,
                bucket_t *adjvtx_id_hash,
                edge_t edge,
                uint edge_update_cmd,
                uint *tripiteration_lcs,
                uint *geni,
                uint *quitstatus
#ifdef EN_CAL
                ,
                cal_unit_cmd_t *cal_unit_cmd
#endif
#ifdef EN_DCI
                ,
                crumple_in_cmd_t *heba_deleteandcrumplein_cmd,
                uint *lastgenworkblockaddr
#endif
        );

        void findonlymode_searchstopped_searchunsuccessful_x_x(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findonlymode_searchstopped_searchsuccessful_x_x(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id ,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
                margin_t *work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
                margin_t *work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd 
        );

        void insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void insertonlymode_x_x_loadedintoemptybucket_x(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void insertonlymode_x_x_foundandupdateditself_x(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void insertonlymode_x_x_passedthrough_notlastworkblock(
                margin_t *work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd = nullptr,
                crumple_in_cmd_t *heba_popoutpopin_cmd = nullptr
        );

        void insertonlymode_x_x_passedthrough_lastworkblock(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findanddeletemode_searchstopped_searchunsuccessful_x_x(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findanddeletemode_searchstopped_searchsuccessful_x_x(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
                margin_t work_block_margin,
                margin_t sub_block_margin,
                vertexid_t vtx_id,
                cal_unit_cmd_t *cal_unit_cmd,
                crumple_in_cmd_t *heba_popoutpopin_cmd
        );

        void set_intervalunitcmd_continue_from_first_generation();

        void set_intervalunitcmd_continue_in_lower_generation();

        void
        set_intervalunitcmd_continue_in_current_generation(margin_t *work_block_margin,
                                                           margin_t sub_block_margin);

        void set_intervalunitcmd_quit_to_next_edge();

        void set_loadunitcmd_loadnextEB(uint cmd);

        void set_moduleunitcmd(uint cmd);

        void
        set_moduleunitparamsedgefields(vertexid_t adjvtx_id, edge_weight_t weight);

        void set_writebackunitcmd_writebackcurrentEB(uint cmd, uint addr,
                                                     margin_t sub_block_margin);

        void setwritebackunitcmd_markasclustered(uint cmd, uint addr);

        void set_cal_unit_cmd(uint cmd);

        void update_margin_and_rollover_status(margin_t *work_block_margin, margin_t sub_block_margin);

        bool is_last_workblock_in_subblock(margin_t work_block_margin, margin_t start_wblkmargin, margin_t sub_block_margin);

// ----------------CAL Unit-----------------------

	void cal_unit(
		edge_t edge,
		vector<work_block_t> &edge_block_array_m_,
		vector<work_block_t> &edge_block_array_o_,
		uint geni
#ifdef EN_CAL
		,
		vector<cal_edgeblock_t> &cal_edgeblock_array_, vector<cal_logical_vertex_entity_t> &cal_lva_, cal_edgeblock_tracker_t *cal_edgeblock_tracker_
#endif
	);
        void cal_insert(
                edge_t edge,
                vector<cal_edgeblock_t> &cal_edgeblock_array_,
                vector<cal_logical_vertex_entity_t> &cal_lva_,
                cal_edgeblock_tracker_t *cal_edgeblock_tracker_,
                uint geni);

        void cal_update(
                edge_t edge,
                vector<cal_edgeblock_t> &cal_edgeblock_array_);

        void cal_delete(
                edge_t edge,
                vector<cal_edgeblock_t> &cal_edgeblock_array_);

        void cal_delete_and_crumple_in(
                edge_t edge,
                vector<cal_edgeblock_t> &cal_edgeblock_array_,
                vector<cal_logical_vertex_entity_t> &cal_lva_,
                cal_edgeblock_tracker_t *cal_edgeblock_tracker_,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_,
                uint geni);

        void cal_update_edge_ptrs( edge_t edge, vector<cal_edgeblock_t> &cal_edgeblock_array_);
    };
    


}

#endif