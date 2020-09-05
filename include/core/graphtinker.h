#ifndef GRAPHTINKER_H
#define GRAPHTINKER_H

#include <iostream>
#include <vector>
#include "core/common.h"
#include "core/vertices.h"
#include "core/translator.h"
#include "config/config.h"
#include "unit/unit_option.h"
#include "unit/unit_flow.h"

using std::string;
namespace graphtinker {

    class Graphtinker {
    private:
        friend class UnitFlow;
        friend class Config;

        uint sgh_for_vtx_id_;
        uint sgh_for_adjvtx_id_;
        uint updatev_;
        uint vertex_range_;
        uint num_vertices_;
        uint num_edges_;
        bool is_directed_;

        uint subblock_height_;
        uint page_block_height_;
        uint subblocks_per_page_;
        uint workblocks_per_page_;
        uint workblocks_per_subblock_;

        uint eba_m_expansion_addition_height_;
        uint eba_o_expansion_addition_height_;
        uint cal_eba_expansion_addition_height_;
        uint cal_lva_expansion_addition_height_;
        uint eba_expansion_padding_;

        Config *config_;

        // edge block array
        vector<workblock_t> edge_block_array_m_;
        vector<workblock_t> edge_block_array_o_;
        tracker_t lvatracker_;

        // Vertex Property Array
        Vertices* vertices_handler_;

        // Scatter-Gather Hash
        Translator* translator_handler_;

#ifdef EN_CAL
        vector<cal_edgeblock_t> cal_edgeblock_array_;
        vector<cal_logical_vertex_entity_t> cal_lva_;
        cal_edgeblock_tracker_t cal_edgeblock_tracker_;
#endif

// metadata (for delete and crumple in)
#ifdef EN_DCI
        vector<supervertex_t> svs;
        vector<vertexid_t> freed_edgeblock_list;
        vector<edgeblock_parentinfo_t> edgeblock_parentinfo;
#endif

    public:
        Graphtinker(); 
        Graphtinker(string file_path); 
        ~Graphtinker();
        // member functions

        void insert_edge(edge_t& edge); 
        void insert_edge(edge_t& edge, Vertices* vertices_handler_); 
        void insert_edge(edge_t& edge, Translator* translator_handler_); 
        void insert_edge(edge_t& edge, Vertices* external_vertices_handler, Translator* translator_handler_); 
        void batch_insert_edge(const char* path, uint batch_size);

        void delete_edge(edge_t& edge);
        void delete_edge(edge_t& edge, Vertices* vertices_handler_);
        void batch_delete_edge(const char* path, uint batch_size);

        // print function
        uint print_svs_size();
        uint print_freed_edgeblock_list_size();
        uint print_edge_count();
        uint print_unique_edge_count();
        void print_edgeblock_array(const vertexid_t begin, const vertexid_t end);
#ifdef EN_CAL
        uint print_cal_edge_count();
#endif

private:
        void create_graph();
        void init_cal_lvas();
        void update_edge(edge_t& edge, bool is_insert_edge, Vertices* vertices_handler_);

        uint get_edge_count(vector<workblock_t> edge_block_array_x, uint height);
        uint get_unique_edge_count(vector<workblock_t> edge_block_array_x, uint height);
        uint get_edgeblock_array_height(const vector<workblock_t> &edge_block_array); 
        vertexid_t retrieve_edges(vertexid_t vid, vector<edge_tt> &edges);
        void check_whether_to_resize_edgeblockarray_m(uint vid);
#ifdef EN_CAL
        uint cal_edge_count(vector<cal_edgeblock_t> &cal_edgeblock_array_);
#endif

        // find margin
        void find_workblock_margin(bucket_t adjvtx_id_hash,margin_t *blkmargin);
        void find_subblock_margin(bucket_t adjvtx_id_hash,margin_t *subblock_margin);
        uint get_edgeblock_offset(vertexid_t vid);
        bucket_t google_hash(vertexid_t vid, edge_type_t etype, uint geni) const;
        uint add_page(tracker_t *tracker, vector<workblock_t> &edge_block_array);

#ifdef EN_DCI
        void init_dci_unit_cmd();

        // super Vertices
        int sv_get_next_edge(
                uint vtx_id,
                vector<supervertex_t> &svs,
                vector<vertexid_t> &freed_edgeblock_list,
                edge_tt *edgett,
                margin_t workblock_margin,
                writeback_unit_cmd_t writeback_unit_cmd,
                int *tailhvtx_id,
                uint *svs_index,
                uint *numclusteredworkblocks,
                uint geni,
                vector<workblock_t> &edge_block_array_m_,
                vector<workblock_t> &edge_block_array_o_);

        int sv_get_tail_edgeblock(
                vector<supervertex_t> &svs,
                uint currworkblockaddr,
                uint *svs_index,
                uint geni,
                vector<workblock_t> &edge_block_array_m_,
                vector<workblock_t> &edge_block_array_o_);

        int sv_pick_edge(
                uint vtx_id,
                uint offset,
                edge_tt *edgett,
                uint *numclusteredworkblocks,
                vector<workblock_t> &edge_block_array);

        // delete and crumple in unit
        void dci_unit(
                writeback_unit_cmd_t writeback_unit_cmd,
                find_report_t find_report,
                vector<workblock_t> &edge_block_array_m_,
                vector<workblock_t> &edge_block_array_o_,
#ifdef EN_CAL
                vector<cal_edgeblock_t> &cal_edgeblock_array_,
#endif
                vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
                vertexid_t vtx_id,
                margin_t workblock_margin,
                margin_t subblock_margin,
                uint geni, dci_cmd_t deleteandcrumpleincmd, vector<supervertex_t> &svs,
                vector<vertexid_t> &freed_edgeblock_list);

#endif
    };

}
#endif
