#include <functional>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	void Graphtinker::CALUnit(
		cal_unit_cmd_t cal_unit_cmd,
		module_params_t *moduleparams,
		insert_params_t *insertparams,
		edge_t edge,
		vector<work_block_t> &edge_block_array_m_,
		vector<work_block_t> &edge_block_array_o_,
		uint geni
#ifdef EN_CAL
		,
		vector<ll_edgeblock_t> &ll_edge_block_array_, vector<ll_logicalvertexentity_t> &ll_lva_, ll_eba_tracker_t *ll_eba_tracker_
#endif
	)
	{
// if searchstopped and unsuccessful
// append to LL, add {lastlocalbaseaddr, lastlocaladdr} to edge
// off we go to insert unit !
// NB: this unit should be before interval unit***
#ifdef EN_CAL
		if (cal_unit_cmd.verdict == INSERTCMD)
		{
			ll_insert(
				edge,
				moduleparams,
				insertparams,
				ll_edge_block_array_,
				ll_lva_,
				ll_eba_tracker_,
				geni);
		}
		else if (cal_unit_cmd.verdict == UPDATECMD)
		{
			ll_update(
				edge,
				moduleparams->ll_localbaseaddrptr_x,
				moduleparams->ll_localaddrptr_x,
				ll_edge_block_array_);
		}
		else if (cal_unit_cmd.verdict == DELETECMD)
		{
			ll_delete(
				edge,
				moduleparams->ll_localbaseaddrptr_x,
				moduleparams->ll_localaddrptr_x,
				ll_edge_block_array_);
		}
		else if (cal_unit_cmd.verdict == DELETEANDCRUMPLEINCMD)
		{
			ll_deleteandcrumplein(
				edge,
				moduleparams->ll_localbaseaddrptr_x,
				moduleparams->ll_localaddrptr_x,
				ll_edge_block_array_,
				ll_lva_,
				ll_eba_tracker_,
				edge_block_array_m_,
				edge_block_array_o_,
				geni);
		}
		else if (cal_unit_cmd.verdict == UPDATEEDGEPTRSCMD)
		{
			ll_updateedgeptrs(
				edge,
				*moduleparams,
				ll_edge_block_array_);
		}
		else if (cal_unit_cmd.verdict == NOCMD)
		{
			// do nothing
		}
		else
		{
			LOG(ERROR) << " should never get here (update_edge45)"  ;
		}
#endif
	}
} // namespace gt