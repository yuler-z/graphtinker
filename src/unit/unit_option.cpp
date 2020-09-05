#include "unit/unit_option.h"

namespace graphtinker{
    UnitOption::UnitOption(){}
    UnitOption::~UnitOption(){}
    void UnitOption::InitModuleUnitParams(const edge_t &edge)
	{
		module_params.rolledover = NO;
		module_params.clustered = NO;
		module_params.cptr = -1;
		// edge
		module_params.type = edge.type;
		module_params.adjvtx_id = edge.adjvtx_id;
		module_params.weight = edge.weight;
		return;
	}

    void UnitOption::InitLoadUnit()
	{
		load_unit_cmd.load = true;
		return;
	}
    void UnitOption::InitInsertUnit(const edge_t &edge, bucket_t adjvtx_id_hash)
	{
		insert_params.adjvtx_id = edge.adjvtx_id;
		insert_params.initial_bucket = adjvtx_id_hash;
		insert_params.type = edge.type;
		insert_params.weight = edge.weight;

		insert_report.exittype = PASSEDTHROUGH;
		insert_report.validbuckets_incr = 0;
		insert_report.overflowbkt = FIRST;
		insert_report.lastbktloadedinto = NONE;
		return;
	}
	void UnitOption::InitFindUnit(const edge_t &edge, bucket_t adjvtx_id_hash)
	{
		find_params.adjvtx_id = edge.adjvtx_id; 
		find_params.initial_bucket = adjvtx_id_hash;
		find_params.type = edge.type;
		find_params.weight = edge.weight;
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
		module_params.cal_localbaseaddrptr = 0; // we don't know the value yet  --edge.LLlocalbaseaddrptr;
		module_params.cal_localaddrptr = 0;	 // we don't know the value yet  --edge.LLlocaladdrptr;
		return;
	}
#endif
}