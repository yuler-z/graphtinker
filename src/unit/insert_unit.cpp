#include "unit/unit_option.h"
#include "unit/unit_flow.h"
;
namespace graphtinker
{
	uint debug_totaledgeinsertions;

	void UnitFlow::insert_unit(
		margin_t workblock_margin,
		bucket_t adjvtx_id_hash,
		uint geni,
		workblock_t *workblock,
		edge_t *edge)
	{
		bucket_t local_offset;

		vertexid_t vtx_id = edge->vtx_id;
		vertexid_t entry_i;
		bucket_t initialbucket_i;
		edge_type_t type_i;
		edge_weight_t edgeweight_i;
		flag_t flag_i;
#ifdef EN_CAL
		vertexid_t cal_localbaseaddrptr_i;
		vertexid_t cal_localaddrptr_i;
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
		beginbkt = insert_params.is_start_blk? adjvtx_id_hash : workblock_margin.top;

		// traverse entries in block
		for (currbkt = beginbkt; currbkt <= workblock_margin.bottom; currbkt++)
		{

			// get entry's local offset
			local_offset = currbkt - workblock_margin.top;

			// retrieve current entry
			entry_i = workblock->edges[local_offset].adjvtx_id;
			initialbucket_i = workblock->edges[local_offset].initial_bucket;
			type_i = workblock->edges[local_offset].type;
			edgeweight_i = workblock->edges[local_offset].weight;
			flag_i = workblock->edges[local_offset].flag;

#ifdef EN_CAL
			cal_localbaseaddrptr_i = workblock->edges[local_offset].cal_localbaseaddrptr;
			cal_localaddrptr_i = workblock->edges[local_offset].cal_localaddrptr;
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

					workblock->edges[local_offset].adjvtx_id = insert_params.adjvtx_id;
					workblock->edges[local_offset].initial_bucket = insert_params.initial_bucket;
					workblock->edges[local_offset].type = insert_params.type;
					workblock->edges[local_offset].weight = insert_params.weight;
					memcpy(workblock->edges[local_offset].properties, edge->properties, sizeof(edge_property_t) * EDGE_PROPERTY_NUM);
					workblock->edges[local_offset].flag = VALID;
#ifdef EN_CAL
					workblock->edges[local_offset].cal_localbaseaddrptr = module_params.cal_localbaseaddrptr;
					workblock->edges[local_offset].cal_localaddrptr = module_params.cal_localaddrptr;
#endif

#ifdef EN_CAL
					// one-time-assignments : if initial edge, track its location
					if (insert_params.adjvtx_id == edge->adjvtx_id)
					{
						edge->heba_hvtx_id = vtx_id;
						edge->heba_workblockid = workblock_margin.top / WORK_BLOCK_HEIGHT;
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

					workblock->edges[local_offset].adjvtx_id = insert_params.adjvtx_id;
					workblock->edges[local_offset].initial_bucket = insert_params.initial_bucket;
					workblock->edges[local_offset].type = insert_params.type;
					workblock->edges[local_offset].weight = insert_params.weight;
					memcpy(workblock->edges[local_offset].properties, edge->properties, sizeof(edge_property_t) * EDGE_PROPERTY_NUM);
					workblock->edges[local_offset].flag = VALID;
#ifdef EN_CAL
					workblock->edges[local_offset].cal_localbaseaddrptr = module_params.cal_localbaseaddrptr;
					workblock->edges[local_offset].cal_localaddrptr = module_params.cal_localaddrptr;
#endif

#ifdef EN_CAL
					// one-time-assignments : if initial edge, track its location
					if (insert_params.adjvtx_id == edge->adjvtx_id)
					{
						edge->heba_hvtx_id = vtx_id;
						edge->heba_workblockid = workblock_margin.top / WORK_BLOCK_HEIGHT;
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