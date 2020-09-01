#include "unit/unit_flow.h"
;
namespace graphtinker
{
	// 实际的update edge和delete edge操作
	void UnitFlow::find_unit( margin_t work_block_margin, bucket_t adjvtx_id_hash, work_block_t *work_block, uint edge_update_cmd)
	{
		bucket_t local_offset;

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

		find_report_t &find_report = unit_option->find_report;
		find_params_t &find_params = unit_option->find_params;
		// if this isn't altered in the for-loop, then this would be the report
		find_report.local_offset = NULLL; // local offset from first bucket in the current workblock
		find_report.is_found = false;
		find_report.is_deleted = false;
		find_report.is_reach_max_prob_length = false;
		find_report.is_empty = false;

		// get begin bucket
		beginbkt = find_params.is_start_blk? adjvtx_id_hash : work_block_margin.top;

		// robin hood hash 的查找过程
		for (currbkt = beginbkt; currbkt <= work_block_margin.bottom; currbkt++)
		{

			// get entry's local offset
			local_offset = currbkt - work_block_margin.top;

			// retrieve current entry
			entry_i = work_block->edges[local_offset].adjvtx_id;
			type_i = work_block->edges[local_offset].type;
			initialbucket_i = work_block->edges[local_offset].initial_bucket;
			edgeweight_i = work_block->edges[local_offset].weight;
			flag_i = work_block->edges[local_offset].flag;
#ifdef EN_CAL
			ll_localbaseaddrptr_i = work_block->edges[local_offset].ll_localbaseaddrptr;
			ll_localaddrptr_i = work_block->edges[local_offset].ll_localaddrptr;
#endif
			// 判断该位置是否就是要查找的边 
			if (entry_i == find_params.adjvtx_id && type_i == find_params.type)
			{
				if (flag_i == VALID)
				{
					find_report.local_offset = local_offset;
					find_report.is_found = true;

					if (edge_update_cmd == INSERTEDGE)
					{
						work_block->edges[local_offset].weight += 1; // update it!
					}
#ifdef EN_DCI
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
					unit_option->module_params.ll_localbaseaddrptr_x = ll_localbaseaddrptr_i;
					unit_option->module_params.ll_localaddrptr_x = ll_localaddrptr_i;
#endif
					break;
				}
				else if (flag_i == DELETED)
				{
					// continue searching...
				}
				else
				{
					// LOG(INFO) <<"something fishy if this is seen too much (find_unit5)" ;
					// empty bucket found, stop search
					find_report.is_empty = true;
					break;
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
					find_report.is_empty = true;
					break;
				}
			}
		}
		// not found. to be continued in next block
		DLOG(INFO) << "----Find Unit----";
		if(find_report.is_deleted) DLOG(INFO) << "find_report.is_deleted";
		if(find_report.is_empty) DLOG(INFO) << "find_report.is_empty";
		if(find_report.is_found) DLOG(INFO) << "find_report.is_found";
		if(find_report.is_reach_max_prob_length) DLOG(INFO) << "find_report.is_reach_max_prob_length";
		DLOG(INFO) << "find_report.local_offset = " << find_report.local_offset;
		return;
	}
} // namespace graphtinker