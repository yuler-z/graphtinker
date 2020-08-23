#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;
namespace gt
{
    void Graphtinker::FindWorkBlockMargin(margin_t *blkmargin, bucket_t hadjvtx_id)
    {
        bucket_t a;
        bucket_t ttop;

        a = hadjvtx_id / WORK_BLOCK_HEIGHT;
        ttop = a * WORK_BLOCK_HEIGHT;

        blkmargin->top = ttop;
        blkmargin->bottom = ttop + (WORK_BLOCK_HEIGHT - 1);
        return;
    }

    void Graphtinker::FindSubBlockMargin(margin_t *sub_block_margin, bucket_t hadjvtx_id)
    {
        bucket_t a;
        bucket_t ttop;

        a = hadjvtx_id / sub_block_height_;
        ttop = a * sub_block_height_;

        sub_block_margin->top = ttop;
        sub_block_margin->bottom = ttop + (sub_block_height_ - 1);
        return;
    }

    uint Graphtinker::findsubblkId(vertexid_t vid)
    {
        uint a = vid % page_block_height_;
        uint subblockid = a / sub_block_height_;
        return subblockid;
    }
} // namespace gt