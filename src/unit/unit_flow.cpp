#include "unit/unit_flow.h"

namespace graphtinker{
    

    UnitFlow::UnitFlow(Graphtinker *gt_){
        unit_option = new UnitOption();
        gt = gt_;

        work_blocks_per_page_ = gt->work_blocks_per_page();
        sub_block_height_ = gt->sub_block_height();
        work_blocks_per_subblock_  = gt->work_blocks_per_subblock();
        cal_lva_expansion_addition_height_ = gt->cal_lva_expansion_addition_height_;
        cal_eba_expansion_addition_height_ = gt->cal_eba_expansion_addition_height_;

        DLOG(INFO) << "construct UnitFlow";
        DLOG(INFO) << "work_blocks_per_page_ = " << work_blocks_per_page_;
        DLOG(INFO) << "sub_block_height_ = " << sub_block_height_;
        DLOG(INFO) << "work_blocks_per_subblock_" << work_blocks_per_subblock_;

    }

    UnitFlow::~UnitFlow(){
        delete unit_option;
        unit_option = nullptr;

        gt = nullptr;
    }

	bucket_t UnitFlow::GoogleHash(vertexid_t vid, edge_type_t type, uint geni) const{ return gt->GoogleHash(vid, type, geni); }
    uint UnitFlow::GetEdgeblockOffset(vertexid_t vid) const{ return gt->GetEdgeblockOffset(vid); }
    void UnitFlow::FindWorkBlockMargin(bucket_t adjvtx_id_hash, margin_t *blkmargin)const{ gt->FindWorkBlockMargin(adjvtx_id_hash, blkmargin); }
    void UnitFlow::FindSubBlockMargin(bucket_t adjvtx_id_hash, margin_t *blkmargin)const{ gt->FindSubBlockMargin(adjvtx_id_hash, blkmargin); }
    uint UnitFlow::add_page(tracker_t *tracker, vector<work_block_t> &edge_block_array)const{return gt->add_page(tracker, edge_block_array);}
}