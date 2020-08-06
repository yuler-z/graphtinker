#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;
namespace gt
{
    void Graphtinker::findwblkmargin(margin_t *blkmargin, bucket_t hadjvtx_id)
    {
        bucket_t a;
        bucket_t ttop;

        a = hadjvtx_id / WORK_BLOCK_HEIGHT;
        ttop = a * WORK_BLOCK_HEIGHT;

        blkmargin->top = ttop;
        blkmargin->bottom = ttop + (WORK_BLOCK_HEIGHT - 1);
        return;
    }

    void Graphtinker::findsubblkmargin(margin_t *subblkmargin, bucket_t hadjvtx_id)
    {
        bucket_t a;
        bucket_t ttop;

        a = hadjvtx_id / _sub_block_height;
        ttop = a * _sub_block_height;

        subblkmargin->top = ttop;
        subblkmargin->bottom = ttop + (_sub_block_height - 1);
        return;
    }

    uint Graphtinker::findsubblkId(vertexid_t vid)
    {
        uint a = vid % _page_block_height;
        uint subblockid = a / _sub_block_height;
        return subblockid;
    }
} // namespace gt