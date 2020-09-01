#include "unit/unit_option.h"
#include "unit/unit_flow.h"
;
namespace graphtinker
{
	uint debug_totaledgeinsertions;

	void UnitFlow::insert_unit(
		margin_t work_block_margin,
		bucket_t adjvtx_id_hash,
		work_block_t *work_block,
		edge_t *edge,
		uint geni)
	{
		bucket_t local_offset;

		vertexid_t vtx_id = edge->vtx_id;
		vertexid_t entry_i;
		bucket_t initialbucket_i;
		edge_type_t type_i;
		edge_weight_t edgeweight_i;
		flag_t flag_i;
#ifdef EN_CAL
		vertexid_t ll_localbaseaddrptr_i;
		vertexid_t ll_localaddrptr_i;
#endif

		bucket_t currbkt;
		bucket_t beginbkt;

		bucket_t DIB_i;
		bucket_t DIB_x;

		module_params_t &module_params = unit_option->module_params;

		// if this isn't altered in the for-loop, then this would be the report
		insert_report_t &insert_report = unit_option->insert_report;
		insert_report.exittype = PASSEDTHROUGH;
		insert_report.validbuckets_incr = 0;
		insert_report.overflowbkt = FIRST;
		insert_report.lastbktloadedinto = NONE;

		// get begin bucket
		insert_params_t &insert_params = unit_option->insert_params;
		beginbkt = insert_params.is_start_blk? adjvtx_id_hash : work_block_margin.top;

		// traverse entries in block
		for (currbkt = beginbkt; currbkt <= work_block_margin.bottom; currbkt++)
		{

			// get entry's local offset
			local_offset = currbkt - work_block_margin.top;

			// retrieve current entry
			entry_i = work_block->edges[local_offset].adjvtx_id;
			initialbucket_i = work_block->edges[local_offset].initial_bucket;
			type_i = work_block->edges[local_offset].type;
			edgeweight_i = work_block->edges[local_offset].weight;
			flag_i = work_block->edges[local_offset].flag;

#ifdef EN_CAL
			ll_localbaseaddrptr_i = work_block->edges[local_offset].ll_localbaseaddrptr;
			ll_localaddrptr_i = work_block->edges[local_offset].ll_localaddrptr;
#endif

			if (entry_i == insert_params.adjvtx_id && type_i == insert_params.type)
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

					work_block->edges[local_offset].adjvtx_id = insert_params.adjvtx_id;
					work_block->edges[local_offset].initial_bucket = insert_params.initial_bucket;
					work_block->edges[local_offset].type = insert_params.type;
					work_block->edges[local_offset].weight = insert_params.weight;
					work_block->edges[local_offset].flag = VALID;
#ifdef EN_CAL
					work_block->edges[local_offset].ll_localbaseaddrptr = module_params.ll_localbaseaddrptr_x;
					work_block->edges[local_offset].ll_localaddrptr = module_params.ll_localaddrptr_x;
#endif

#ifdef EN_CAL
					// one-time-assignments : if initial edge, track its location
					if (insert_params.adjvtx_id == edge->adjvtx_id)
					{
						edge->heba_hvtx_id = vtx_id;
						edge->heba_workblockid = work_block_margin.top / WORK_BLOCK_HEIGHT;
						edge->heba_loffset = local_offset;
						// edge->which_gen_is_the_main_copy_located = geni;
					}
#endif

					debug_totaledgeinsertions += 1;

					// update insert_report
					insert_report.exittype = LOAD_INTO_EMPTY_BUCKET;
					insert_report.validbuckets_incr = 1;
					insert_report.overflowbkt = NONE;
					insert_report.lastbktloadedinto = currbkt;
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

					work_block->edges[local_offset].adjvtx_id = insert_params.adjvtx_id;
					work_block->edges[local_offset].initial_bucket = insert_params.initial_bucket;
					work_block->edges[local_offset].type = insert_params.type;
					work_block->edges[local_offset].weight = insert_params.weight;
					work_block->edges[local_offset].flag = VALID;
#ifdef EN_CAL
					work_block->edges[local_offset].ll_localbaseaddrptr = module_params.ll_localbaseaddrptr_x;
					work_block->edges[local_offset].ll_localaddrptr = module_params.ll_localaddrptr_x;
#endif

#ifdef EN_CAL
					// one-time-assignments : if initial edge, track its location
					if (insert_params.adjvtx_id == edge->adjvtx_id)
					{
						edge->heba_hvtx_id = vtx_id;
						edge->heba_workblockid = work_block_margin.top / WORK_BLOCK_HEIGHT;
						edge->heba_loffset = local_offset;
						// edge->which_gen_is_the_main_copy_located = geni;
					}
#endif

					debug_totaledgeinsertions += 1;

					// update insert_report
					insert_report.exittype = LOAD_INTO_EMPTY_BUCKET;
					insert_report.validbuckets_incr = 1;
					insert_report.overflowbkt = NONE;
					insert_report.lastbktloadedinto = currbkt;
					break;
				}
			}
		}
		DLOG(INFO) << "----Insert Unit----";
		DLOG(INFO) << "insert_report.exittype = " << insert_report.exittype;
		DLOG(INFO) << "insert_report.validbuckets_incr = "<< insert_report.exittype;
		DLOG(INFO) << "insert_report.overflowbkt = " << insert_report.overflowbkt;
		DLOG(INFO) << "insert_report.lastbktloadedinto" << insert_report.lastbktloadedinto;
		return;
	}

} // namespace graphtinker