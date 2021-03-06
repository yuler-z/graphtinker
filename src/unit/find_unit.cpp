#include "unit/unit_flow.h"
;
namespace graphtinker
{
	// 实际的update edge和delete edge操作
	void UnitFlow::find_unit( margin_t workblock_margin, bucket_t adjvtx_id_hash, bool is_insert_edge, workblock_t *workblock)
	{
		bucket_t local_offset;

		vertexid_t entry_i;
		edge_type_t type_i;
		flag_t flag_i;
#ifdef EN_CAL
		vertexid_t cal_localbaseaddrptr_i;
		vertexid_t cal_localaddrptr_i;
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
		beginbkt = find_params.is_start_blk? adjvtx_id_hash : workblock_margin.top;

		// robin hood hash 的查找过程
		for (currbkt = beginbkt; currbkt <= workblock_margin.bottom; currbkt++)
		{

			// get entry's local offset
			local_offset = currbkt - workblock_margin.top;

			// retrieve current entry
			entry_i = workblock->edges[local_offset].adjvtx_id;
			type_i = workblock->edges[local_offset].type;
			flag_i = workblock->edges[local_offset].flag;
#ifdef EN_CAL
			cal_localbaseaddrptr_i = workblock->edges[local_offset].ll_localbaseaddrptr;
			cal_localaddrptr_i = workblock->edges[local_offset].ll_localaddrptr;
#endif
			// 判断该位置是否就是要查找的边 
			if (entry_i == find_params.adjvtx_id && type_i == find_params.type)
			{
				if (flag_i == VALID)
				{
					find_report.local_offset = local_offset;
					find_report.is_found = true;

					if (is_insert_edge)
					{
						workblock->edges[local_offset].weight += 1; // update it!
					}
#ifdef EN_DCI
					else if (!is_insert_edge)/*delete edge*/
					{
						workblock->edges[local_offset].flag = DELETED; // remove it!
					}
#else
					else if (!is_insert_edge)
					{
						workblock->edges[local_offset].flag = DELETED; // remove it!
					}
#endif
					else
					{
						DLOG(ERROR) << "NEVER GET HERE";
					}

#ifdef EN_CAL
					unit_option->module_params.cal_localbaseaddrptr = cal_localbaseaddrptr_i;
					unit_option->module_params.cal_localaddrptr = cal_localaddrptr_i;
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
					DLOG(INFO) << "edge in the bucket is valid, so the bucket is empty";
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
					DLOG(INFO) << "bucket is empty";
					find_report.is_empty = true;
					break;
				}
			}
		} // not found. to be continued in next block

		// Debug
		DLOG(INFO) << "----Find Unit----";
		if(find_report.is_deleted) DLOG(INFO) << "find_report.is_deleted";
		if(find_report.is_empty) DLOG(INFO) << "find_report.is_empty";
		if(find_report.is_found) DLOG(INFO) << "find_report.is_found";
		if(find_report.is_reach_max_prob_length) DLOG(INFO) << "find_report.is_reach_max_prob_length";
		DLOG(INFO) << "find_report.local_offset = " << find_report.local_offset;

		return;
	}
} // namespace graphtinker