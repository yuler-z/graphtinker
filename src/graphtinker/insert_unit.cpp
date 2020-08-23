#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;
namespace gt
{
	uint debug_totaledgeinsertions;

	void Graphtinker::insert_unit(
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
		uint geni)
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
		insertreport->exittype = PASSEDTHROUGH;
		insertreport->validbuckets_incr = 0;
		insertreport->overflowbkt = FIRST;
		insertreport->lastbktloadedinto = NONE;

		// get begin bucket
		beginbkt = insertparams->isstartblk == 1 ? hadjvtx_id : work_block_margin.top;

		// traverse entries in block
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

			if (entry_i == insertparams->xadjvtx_id)
			{
				if (flag_i == VALID)
				{
					// should not be
					LOG(ERROR) << " should not be (insert_unit45)"  ;
				} /* else if (flag_i == DELETED){
				// do nothing. moving on ...			
			} */
				else
				{
					// empty bucket found, insert and exit <//*** NB: this is repeated below>

					work_block->edges[local_offset].xadjvtx_id = insertparams->xadjvtx_id;
					work_block->edges[local_offset].initialbucket = insertparams->initialbucket_x;
					work_block->edges[local_offset].edge_weight = insertparams->edge_weight;
					work_block->edges[local_offset].flag = VALID;
#ifdef EN_CAL
					work_block->edges[local_offset].ll_localbaseaddrptr = moduleparams->ll_localbaseaddrptr_x;
					work_block->edges[local_offset].ll_localaddrptr = moduleparams->ll_localaddrptr_x;
#endif

#ifdef EN_CAL
					// one-time-assignments : if initial edge, track its location
					if (insertparams->xadjvtx_id == edge->xadjvtx_id)
					{
						edge->heba_hvtx_id = xvtx_id;
						edge->heba_workblockid = work_block_margin.top / WORK_BLOCK_HEIGHT;
						edge->heba_loffset = local_offset;
						// edge->which_gen_is_the_main_copy_located = geni;
					}
#endif

					debug_totaledgeinsertions += 1;

					// update insertreport
					insertreport->exittype = LOADEDINTOEMPTYBUCKET;
					insertreport->validbuckets_incr = 1;
					insertreport->overflowbkt = NONE;
					insertreport->lastbktloadedinto = currbkt;
					return;
				}
			}
			else
			{
				if (flag_i == VALID)
				{
					// do nothing. moving on ...
				} /* else if (flag_i == DELETED){
				// do nothing. moving on ...			
			} */
				else
				{
					// empty bucket found, insert and exit

					work_block->edges[local_offset].xadjvtx_id = insertparams->xadjvtx_id;
					work_block->edges[local_offset].initialbucket = insertparams->initialbucket_x;
					work_block->edges[local_offset].edge_weight = insertparams->edge_weight;
					work_block->edges[local_offset].flag = VALID;
#ifdef EN_CAL
					work_block->edges[local_offset].ll_localbaseaddrptr = moduleparams->ll_localbaseaddrptr_x;
					work_block->edges[local_offset].ll_localaddrptr = moduleparams->ll_localaddrptr_x;
#endif

#ifdef EN_CAL
					// one-time-assignments : if initial edge, track its location
					if (insertparams->xadjvtx_id == edge->xadjvtx_id)
					{
						edge->heba_hvtx_id = xvtx_id;
						edge->heba_workblockid = work_block_margin.top / WORK_BLOCK_HEIGHT;
						edge->heba_loffset = local_offset;
						// edge->which_gen_is_the_main_copy_located = geni;
					}
#endif

					debug_totaledgeinsertions += 1;

					// update insertreport
					insertreport->exittype = LOADEDINTOEMPTYBUCKET;
					insertreport->validbuckets_incr = 1;
					insertreport->overflowbkt = NONE;
					insertreport->lastbktloadedinto = currbkt;
					return;
				}
			}
		}
		return;
	}

	bucket_t Graphtinker::getdib(bucket_t currbkt, bucket_t initialbucket, margin_t sub_block_margin, uint rolledover)
	{
		bucket_t DIB = 0;
		bucket_t rightpad = 0;
		bucket_t leftpad = 0;
		uint work_blocks_per_subblock_ = work_blocks_per_subblock_;
		uint work_block_height = WORK_BLOCK_HEIGHT;

		bucket_t diff = currbkt - initialbucket;
		if (diff >= 0)
		{
			// DIB = currbkt - initialbucket;
			if (rolledover == YES)
			{
				DIB = (work_blocks_per_subblock_ * work_block_height) + (currbkt - initialbucket);
			}
			else
			{
				DIB = currbkt - initialbucket;
			}
		}
		else
		{
			rightpad = sub_block_margin.bottom - initialbucket;
			leftpad = currbkt - sub_block_margin.top;
			DIB = rightpad + leftpad;
		}
		return DIB;
	}
} // namespace gt