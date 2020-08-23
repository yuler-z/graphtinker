#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;
namespace gt
{
	void Graphtinker::find_unit(
		module_params_t *moduleparams,
		find_params_t findparams,
		find_report_t *findreport,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		bucket_t hadjvtx_id,
		work_block_t *work_block,
		uint edge_update_cmd)
	{
		bucket_t local_offset;

		vertexid_t entry_i;
		bucket_t initialbucket_i;
		edgeweight_t edgeweight_i;
		flag_t flag_i;
#ifdef EN_CAL
		vertexid_t ll_localbaseaddrptr_i;
		vertexid_t ll_localaddrptr_i;
#endif

		bucket_t currbkt;
		bucket_t beginbkt;

		bucket_t DIB_i;
		bucket_t DIB_x;

		// if this isn't altered in the for-loop, then this would be the report
		findreport->local_offset = NULLL; // local offset from first bucket in the current workblock
		findreport->is_found = false;
		findreport->is_deleted = false;
		findreport->is_reach_max_prob_length = false;
		findreport->is_empty = false;

		// get begin bucket
		beginbkt = findparams.isstartblk == 1 ? hadjvtx_id : work_block_margin.top;

		// robin hood hash 的查找过程
		for (currbkt = beginbkt; currbkt <= work_block_margin.bottom; currbkt++)
		{

			// get entry's local offset
			local_offset = currbkt - work_block_margin.top;

			// retrieve current entry
			entry_i = work_block->edges[local_offset].xadjvtx_id;
			initialbucket_i = work_block->edges[local_offset].initialbucket;
			edgeweight_i = work_block->edges[local_offset].edge_weight;
			flag_i = work_block->edges[local_offset].flag;
#ifdef EN_CAL
			ll_localbaseaddrptr_i = work_block->edges[local_offset].ll_localbaseaddrptr;
			ll_localaddrptr_i = work_block->edges[local_offset].ll_localaddrptr;
#endif
			// 判断该位置是否就是要查找的边 
			if (entry_i == findparams.xadjvtx_id)
			{
				if (flag_i == VALID)
				{
					findreport->local_offset = local_offset;
					findreport->is_found = true;

					if (edge_update_cmd == INSERTEDGE)
					{
						work_block->edges[local_offset].edge_weight += 1; // update it!
					}
#ifdef EN_CRUMPLE_IN
					else if (edge_update_cmd == DELETEEDGE)
					{
						work_block->edges[local_offset].flag = DELETED; // remove it!
					}
#else
					else if (edge_update_cmd == DELETEEDGE)
					{
						work_block->edges[local_offset].flag = DELETED; // remove it!
					}
#endif
					else
					{
						LOG(ERROR) << " should never be seen here (find_unit56)"  ;
					}

#ifdef EN_CAL
					moduleparams->ll_localbaseaddrptr_x = ll_localbaseaddrptr_i;
					moduleparams->ll_localaddrptr_x = ll_localaddrptr_i;
#endif
					return;
				}
				else if (flag_i == DELETED)
				{
					// continue searching...
				}
				else
				{
					// LOG(INFO) <<"something fishy if this is seen too much (find_unit5)" ;
					// empty bucket found, stop search
					findreport->is_empty = true;
					return;
				}
			}
			else
			{
				if (flag_i == VALID || flag_i == DELETED)
				{
					// continue searching...
				}
				else
				{
					findreport->is_empty = true;
					return;
				}
			}
		}
		// not found. to be continued in next block
		return;
	}
} // namespace gt