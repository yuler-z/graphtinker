#include "unit/unit_flow.h"

namespace graphtinker{
    

    UnitFlow::UnitFlow(Graphtinker *gt_){
        unit_option = new UnitOption();
        gt = gt_;

        work_blocks_per_page_ = gt->work_blocks_per_page();
        sub_block_height_ = gt->sub_block_height();
        work_blocks_per_subblock_  = gt->work_blocks_per_subblock();

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

	bucket_t UnitFlow::GoogleHash(vertexid_t vid, uint geni) const{ return gt->GoogleHash(vid, geni); }
    uint UnitFlow::GetEdgeblockOffset(vertexid_t vid) const{ return gt->GetEdgeblockOffset(vid); }
    void UnitFlow::FindWorkBlockMargin(bucket_t adjvtx_id_hash, margin_t *blkmargin)const{ gt->FindWorkBlockMargin(adjvtx_id_hash, blkmargin); }
    void UnitFlow::FindSubBlockMargin(bucket_t adjvtx_id_hash, margin_t *blkmargin)const{ gt->FindSubBlockMargin(adjvtx_id_hash, blkmargin); }
    uint UnitFlow::add_page(tracker_t *tracker, vector<work_block_t> &edge_block_array)const{return gt->add_page(tracker, edge_block_array);}
}