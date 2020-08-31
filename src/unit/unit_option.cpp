#include "unit/unit_option.h"

namespace graphtinker{
    UnitOption::UnitOption(){}
    UnitOption::~UnitOption(){}
    void UnitOption::InitModuleUnitParams(vertexid_t adjvtx_id, edgeweight_t edge_weight)
	{
		module_params.rolledover = NO;
		module_params.clustered = NO;
		module_params.cptr = -1;
		module_params.adjvtx_id = adjvtx_id;
		module_params.edge_weight = edge_weight;
		return;
	}

    void UnitOption::InitLoadUnit()
	{
		load_unit_cmd.load = YES;
		return;
	}
    void UnitOption::InitInsertUnit(vertexid_t adjvtx_id, bucket_t adjvtx_id_hash, edgeweight_t edge_weight)
	{
		insert_params.adjvtx_id = adjvtx_id;
		insert_params.initial_bucket = adjvtx_id_hash;
		insert_params.edge_weight = edge_weight;

		insert_report.exittype = PASSEDTHROUGH;
		insert_report.validbuckets_incr = 0;
		insert_report.overflowbkt = FIRST;
		insert_report.lastbktloadedinto = NONE;
		return;
	}
	void UnitOption::InitFindUnit(vertexid_t adjvtx_id, bucket_t adjvtx_id_hash, edgeweight_t edge_weight)
	{
		find_params.adjvtx_id = adjvtx_id; // edge info
		find_params.initial_bucket = adjvtx_id_hash;
		find_params.edge_weight = edge_weight;
		find_params.is_start_blk = true;

		find_report.local_offset = NULLL;
		find_report.is_found = false;
		find_report.is_deleted = false;
		find_report.is_reach_max_prob_length = false;
		find_report.is_empty = false;
		return;
	}
	void UnitOption::InitWritebackUnit()
	{
		writeback_unit_cmd.writeback = NO;
		writeback_unit_cmd.addr = 0;
		writeback_unit_cmd.markasclustered = NO;
		return;
    }


    void UnitOption::InitModuleUnitParams2()
	{
		module_params.rolledover = NO;
		module_params.clustered = NO;
		module_params.cptr = -1;
		return;
	}
    void UnitOption::InitInsertUnit2()
	{
		insert_params.is_start_blk = false; 

		insert_report.exittype = PASSEDTHROUGH;
		insert_report.validbuckets_incr = 0;
		insert_report.overflowbkt = FIRST;
		insert_report.lastbktloadedinto = NONE;
		return;
	}
    void UnitOption::InitFindUnit2()
	{
		find_params.is_start_blk = false;

		find_report.local_offset = NULLL;
		find_report.is_found = NO;
		find_report.is_deleted = NO;
		find_report.is_reach_max_prob_length = NO;
		find_report.is_empty = NO;
		return;
	}

#ifdef EN_CAL
	void UnitOption::InitCalUnit(){
		cal_unit_cmd.verdict = NOCMD;
		return;
	}
	void UnitOption::ClearCalAdr()
	{
		module_params.ll_localbaseaddrptr_x = 0; // we don't know the value yet  --edge.LLlocalbaseaddrptr;
		module_params.ll_localaddrptr_x = 0;	 // we don't know the value yet  --edge.LLlocaladdrptr;
		return;
	}
#endif
}