#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;
namespace gt
{
	void Graphtinker::initallmoduleunit(module_unit_cmd_t *moduleunitcmd, module_params_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight)
	{
		moduleunitcmd->mode = FIND_MODE;

		moduleparams->rolledover = NO;
		moduleparams->clustered = NO;
		moduleparams->cptr = -1;
		moduleparams->xadjvtx_id = xadjvtx_id;
		moduleparams->edge_weight = edge_weight;
		return;
	}

	void Graphtinker::InitModuleUnitParams(module_params_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight)
	{
		moduleparams->rolledover = NO;
		moduleparams->clustered = NO;
		moduleparams->cptr = -1;
		moduleparams->xadjvtx_id = xadjvtx_id;
		moduleparams->edge_weight = edge_weight;
		return;
	}

	/**
initializes module unit's:
- traversal info fields
- cluster info fields
- additional info fields
(does not initialize the edge fields)
*/
	void Graphtinker::initialize_moduleunit_params2(module_params_t *moduleparams)
	{
		moduleparams->rolledover = NO;
		moduleparams->clustered = NO;
		moduleparams->cptr = -1;
		return;
	}

	void Graphtinker::InitLoadUnit(load_unit_cmd_t *loadunitcmd)
	{
		loadunitcmd->load = YES;
		return;
	}

	void Graphtinker::InitInsertUnit(insert_params_t *insertparams, insert_report_t *insertreport, vertexid_t xadjvtx_id, bucket_t hadjvtx_id, edgeweight_t edge_weight)
	{
		insertparams->xadjvtx_id = xadjvtx_id;
		insertparams->initialbucket_x = hadjvtx_id;
		insertparams->edge_weight = edge_weight;

		insertreport->exittype = PASSEDTHROUGH;
		insertreport->validbuckets_incr = 0;
		insertreport->overflowbkt = FIRST;
		insertreport->lastbktloadedinto = NONE;
		return;
	}

	/**
initializes insert unit's:
- additional info fields
(does not initialize the edge fields)
*/
	void Graphtinker::initialize_insertunit2(insert_params_t *insertparams, insert_report_t *insertreport)
	{
		insertparams->isstartblk = 0;

		insertreport->exittype = PASSEDTHROUGH;
		insertreport->validbuckets_incr = 0;
		insertreport->overflowbkt = FIRST;
		insertreport->lastbktloadedinto = NONE;
		return;
	}

	void Graphtinker::InitFindUnit(find_params_t *findparams, find_report_t *findreport, vertexid_t xadjvtx_id, bucket_t hadjvtx_id, edgeweight_t edge_weight)
	{
		findparams->xadjvtx_id = xadjvtx_id; // edge info
		findparams->initialbucket_x = hadjvtx_id;
		findparams->edge_weight = edge_weight;
		findparams->isstartblk = 0;

		findreport->local_offset = NULLL;
		findreport->is_found = NO;
		findreport->is_deleted = NO;
		findreport->is_reach_max_prob_length = NO;
		findreport->is_empty = NO;
		return;
	}

	/**
initializes find unit's:
- additional info fields
(does not initialize the edge fields)
*/
	void Graphtinker::initialize_findunit2(find_params_t *findparams, find_report_t *findreport)
	{
		findparams->isstartblk = 0;

		findreport->local_offset = NULLL;
		findreport->is_found = NO;
		findreport->is_deleted = NO;
		findreport->is_reach_max_prob_length = NO;
		findreport->is_empty = NO;
		return;
	}

	void Graphtinker::InitWritebackUnit(writeback_unit_cmd_t *writebackunitcmd)
	{
		writebackunitcmd->writeback = NO;
		writebackunitcmd->addr = 0;
		writebackunitcmd->markasclustered = NO;
		return;
	}

	void Graphtinker::initialize_llebaverdictcmd(cal_unit_cmd_t *cal_unit_cmd)
	{
		cal_unit_cmd->verdict = NOCMD;
		return;
	}

#ifdef EN_CAL
	void Graphtinker::clear_lleba_addresses_in_moduleparams(module_params_t *moduleparams)
	{
		moduleparams->ll_localbaseaddrptr_x = 0; // we don't know the value yet  --edge.LLlocalbaseaddrptr;
		moduleparams->ll_localaddrptr_x = 0;	 // we don't know the value yet  --edge.LLlocaladdrptr;
		return;
	}
#endif

	void Graphtinker::init_deleteandcrumplein_verdictcmd(crumple_in_cmd_t *heba_deleteandcrumplein_cmd)
	{
		heba_deleteandcrumplein_cmd->verdict = DCI_NOCMD;
		return;
	}
} // namespace gt