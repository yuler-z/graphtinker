/*
#include <string.h>
#include <iostream>
#include <string>
#include "core/graphtinker.h"
using namespace std;
namespace graphtinker
{
	void Graphtinker::initallmoduleunit(module_unit_cmd_t *module_unit_cmd, module_params_t *module_params, vertexid_t adjvtx_id, edgeweight_t edge_weight)
	{
		module_unit_cmd->mode = FIND_MODE;

		module_params->rolledover = NO;
		module_params->clustered = NO;
		module_params->cptr = -1;
		module_params->adjvtx_id = adjvtx_id;
		module_params->edge_weight = edge_weight;
		return;
	}

	void Graphtinker::initialize_llebaverdictcmd(cal_unit_cmd_t *cal_unit_cmd)
	{
		cal_unit_cmd->verdict = NOCMD;
		return;
	}

#ifdef EN_CAL
	void Graphtinker::clear_lleba_addresses_in_moduleparams(module_params_t *module_params)
	{
		module_params->ll_localbaseaddrptr_x = 0; // we don't know the value yet  --edge.LLlocalbaseaddrptr;
		module_params->ll_localaddrptr_x = 0;	 // we don't know the value yet  --edge.LLlocaladdrptr;
		return;
	}
#endif

#ifdef EN_CRUMPLE_IN
	void Graphtinker::init_deleteandcrumplein_verdictcmd(crumple_in_cmd_t *heba_deleteandcrumplein_cmd)
	{
		heba_deleteandcrumplein_cmd->verdict = DCI_NOCMD;
		return;
	}

#endif
} // namespace graphtinker
*/