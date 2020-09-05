#ifndef UNIT_FLOW_H
#define UNIT_FLOW_H
#include "core/graphtinker.h"
#include "unit/unit_option.h"
#ifdef EN_CAL
#define SET_CAL_UNIT_CMD(cmd) \
        set_cal_unit_cmd(cmd)
#else
#define SET_CAL_UNIT_CMD(cmd)
#endif

#ifdef EN_DCI
#define SET_DCI_UNIT_CMD(cmd) \
        set_dci_unit_cmd(cmd)
#else
#define SET_DCI_UNIT_CMD(cmd)
#endif

namespace graphtinker
{
        class UnitFlow
        {
        public:
                Graphtinker *gt_;
                UnitOption *unit_option;

                UnitFlow(Graphtinker *gt);
                ~UnitFlow();

                void load_unit(
                    margin_t workblock_margin,
                    vertexid_t vtx_id,
                    uint geni,
                    workblock_t *workblock);

                void load_params(
                    margin_t workblock_margin,
                    bucket_t adjvtx_id_hash);

                void find_unit(
                    margin_t workblock_margin,
                    bucket_t adjvtx_id_hash,
                    bool is_insert_edge,
                    workblock_t *workblock);

                void insert_unit(
                    margin_t workblock_margin,
                    bucket_t adjvtx_id_hash,
                    uint geni,
                    workblock_t *workblock,
                    edge_t *edge);

                void inference_unit(
                    vertexid_t vtx_id,
                    bool is_insert_edge,
                    margin_t subblock_margin,
                    margin_t start_wblkmargin,
                    margin_t *workblock_margin);

                // writeback unit
                void writeback_unit(
                    margin_t subblock_margin,
                    vertexid_t hvtx_id,
                    bool is_insert_edge,
                    uint geni,
                    workblock_t *workblock,
                    tracker_t *lvatracker_
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
                    edge_t edge,
                    margin_t *workblock_margin,
                    margin_t *subblock_margin,
                    margin_t *start_wblkmargin,
                    vertexid_t *vtx_id,
                    bucket_t *adjvtx_id_hash,
                    uint *tripiteration_lcs,
                    uint *geni,
                    uint *quitstatus
#ifdef EN_DCI
                    ,
                    uint *lastgenworkblockaddr
#endif
                );

                void find_stopped_notsuccess_insert(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void find_stopped_success_insert(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void find_notstopped_notsuccess_notlastworkblock(
                    margin_t *workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void find_notstopped_notsuccessful_lastworkblock(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void insert_passedthroughandswappedatsomepoint_notlastworkblock(
                    margin_t *workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void insert_passedthroughandswappedatsomepoint_lastworkblock(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void insert_loadedintoemptybucket(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void insert_foundandupdateditself(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void insert_passedthrough_notlastworkblock(
                    margin_t *workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void insert_passedthrough_lastworkblock(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void find_stopped_notsuccess_delete(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void find_stopped_success_delete(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void find_stopped_notsuccess_dci_delete(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void find_stopped_success_dci_delete(
                    margin_t workblock_margin,
                    margin_t subblock_margin,
                    vertexid_t vtx_id);

                void set_intervalunitcmd_continue_from_first_generation();

                void set_intervalunitcmd_continue_in_lower_generation();

                void
                set_intervalunitcmd_continue_in_current_generation(margin_t *workblock_margin,
                                                                   margin_t subblock_margin);

                void set_intervalunitcmd_quit_to_next_edge();

                void set_loadunitcmd_loadnextedgeblock(uint cmd);

                void set_moduleunitcmd(uint cmd);

                void
                set_moduleunitparams_edgefields(vertexid_t adjvtx_id, edge_weight_t weight);

                void set_writebackunitcmd_writebackcurrentedgeblock(uint cmd, uint addr,
                                                                    margin_t subblock_margin);

                void set_writebackunitcmd_markasclustered(uint cmd, uint addr);

                void update_margin_and_rollover_status(margin_t *workblock_margin, margin_t subblock_margin);

                bool is_last_workblock_in_subblock(margin_t workblock_margin, margin_t start_wblkmargin, margin_t subblock_margin);

                // ----------------CAL Unit-----------------------

#ifdef EN_CAL
                void cal_unit(
                    edge_t edge,
                    vector<workblock_t> &edge_block_array_m_,
                    vector<workblock_t> &edge_block_array_o_,
                    uint geni,
                    vector<cal_edgeblock_t> &cal_edgeblock_array_,
                    vector<cal_logical_vertex_entity_t> &cal_lva_,
                    cal_edgeblock_tracker_t *cal_edgeblock_tracker_);
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
                    vector<workblock_t> &edge_block_array_m_,
                    vector<workblock_t> &edge_block_array_o_,
                    uint geni);

                void cal_update_edge_ptrs(edge_t edge, vector<cal_edgeblock_t> &cal_edgeblock_array_);
                void set_cal_unit_cmd(uint cmd);
#endif

#ifdef EN_DCI
                void UnitFlow::set_dci_unit_cmd(uint cmd);
#endif
        };
} // namespace graphtinker
#endif