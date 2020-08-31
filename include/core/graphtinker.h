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
;
using std::string;
/// classes
namespace graphtinker {

    class Graphtinker {
    private:


    public:
        Graphtinker(); 
        Graphtinker(string file_path); 
        ~Graphtinker();


        Config *_config;
        // config
        uint sgh_for_vtx_id_;
        uint sgh_for_adjvtx_id_;
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
        uint cal_eba_expansion_addition_height_;
        uint cal_lva_expansion_addition_height_;
        uint eba_expansion_padding_;

        // edge block array
        vector<work_block_t> edge_block_array_m_;
        vector<work_block_t> edge_block_array_o_;
        tracker_t lvatracker_;

        // ll edge block array
        vector<cal_edgeblock_t> cal_edgeblock_array_;
        vector<cal_logical_vertex_entity_t> cal_lva_;
        cal_edgeblock_tracker_t cal_edgeblock_tracker_;

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

        void UpdateEdge(uint src, uint dst, uint ew, uint edge_update_cmd, Vertices* vertices_handler_);


        vertexid_t retrieve_edges(vertexid_t vid, vector<edge_tt> &edges);

        uint PrintCALEdgeCount();
        uint ll_countedges(vector<cal_edgeblock_t> &cal_edgeblock_array_);
        uint print_svs_size();
        uint print_freed_edgeblock_list_size();
        void initialize_lvas();

        // getter
        const uint vertex_range() const;
        const Translator* translator() const;
        const Vertices* vertices() const;
        const uint graphdirectiontype() const;
        const uint translator_tracker_mark() const;
        const uint work_blocks_per_page() const;
        const uint work_blocks_per_subblock() const;
        const uint sub_block_height() const;
        const vector<work_block_t> &edge_block_array_m() const;
        const vector<work_block_t> &edge_block_array_o() const;
        const vector<cal_logical_vertex_entity_t> &ll_logicalvertexentity() const;
        const vector<cal_edgeblock_t> &ll_edge_block_array() const;

        // print function
        uint PrintEdgeCount();
        uint GetEdgeCount(vector<work_block_t> edge_block_array_x, uint height);
        uint PrintUniqueEdgeCount();
        uint GetUniqueEdgeCount(vector<work_block_t> edge_block_array_x, uint height);
        void PrintEdgeblockArray(const vertexid_t begin, const vertexid_t end);
        uint GetEdgeBlockArrayHeight(const vector<work_block_t> &edge_block_array); 
        void check_whether_to_resize_edgeblockarray_m(uint vid);

        // find margin
        void FindWorkBlockMargin(bucket_t adjvtx_id_hash,margin_t *blkmargin);
        void FindSubBlockMargin(bucket_t adjvtx_id_hash,margin_t *sub_block_margin);
        uint GetEdgeblockOffset(vertexid_t vid);

        // gds_utility
        uint getsvtracker(markertracker_t *svtracker);

        // hash scripts
        bucket_t GoogleHash(vertexid_t vid, uint geni) const;

        // initialize
        uint add_page(tracker_t *tracker, vector<work_block_t> &edge_block_array);

        // bucket_t getdib(bucket_t currbkt, bucket_t initial_bucket, margin_t sub_block_margin, uint rolledover);


#ifdef EN_CRUMPLE_IN
        void init_deleteandcrumplein_verdictcmd(crumple_in_cmd_t *heba_deleteandcrumplein_cmd);

        // super Vertices
        int sv_get_next_edge(
                uint vtx_id,
                vector<supervertex_t> &svs,
                vector<vertexid_t> &freed_edgeblock_list,
                edge_tt *edgett,
                margin_t work_block_margin,
                writeback_unit_cmd_t writeback_unit_cmd,
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
                uint vtx_id,
                uint offset,
                edge_tt *edgett,
                uint *numclusteredworkblocks,
                vector<work_block_t> &edge_block_array);

        // delete and crumple in unit
        void CrumpleInUnit(
                writeback_unit_cmd_t writeback_unit_cmd,
                find_report_t find_report,
                edge_t edge,
                vector<work_block_t> &edge_block_array_m_,
                vector<work_block_t> &edge_block_array_o_,
#ifdef EN_CAL
                vector<cal_edgeblock_t> &cal_edgeblock_array_,
#endif
                vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
                vertexid_t vtx_id,
                margin_t work_block_margin,
                margin_t sub_block_margin,
                uint geni, crumple_in_cmd_t deleteandcrumpleincmd, vector<supervertex_t> &svs,
                vector<vertexid_t> &freed_edgeblock_list);

#endif
    };

}
#endif
