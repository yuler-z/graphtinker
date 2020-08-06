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
	void Graphtinker::ll_unit(
		llgds_unit_cmd_t llgdsunitcmd,
		module_params_t *moduleparams,
		insert_params_t *insertparams,
		edge_t edge,
		vector<edge_nt> &_edge_block_array_m,
		vector<edge_nt> &_edge_block_array_o,
		uint geni
#ifdef EN_LLGDS
		,
		vector<ll_edgeblock_t> &_ll_edge_block_array, vector<ll_logicalvertexentity_t> &_ll_lva, ll_eba_tracker_t *_ll_eba_tracker
#endif
	)
	{
// if searchstopped and unsuccessful
// append to LL, add {lastlocalbaseaddr, lastlocaladdr} to edge
// off we go to insert unit !
// NB: this unit should be before interval unit***
#ifdef EN_LLGDS
		if (llgdsunitcmd.verdict == INSERTCMD)
		{
			ll_insert(
				edge,
				moduleparams,
				insertparams,
				_ll_edge_block_array,
				_ll_lva,
				_ll_eba_tracker,
				geni);
		}
		else if (llgdsunitcmd.verdict == UPDATECMD)
		{
			ll_update(
				edge,
				moduleparams->ll_localbaseaddrptr_x,
				moduleparams->ll_localaddrptr_x,
				_ll_edge_block_array);
		}
		else if (llgdsunitcmd.verdict == DELETECMD)
		{
			ll_delete(
				edge,
				moduleparams->ll_localbaseaddrptr_x,
				moduleparams->ll_localaddrptr_x,
				_ll_edge_block_array);
		}
		else if (llgdsunitcmd.verdict == DELETEANDCRUMPLEINCMD)
		{
			ll_deleteandcrumplein(
				edge,
				moduleparams->ll_localbaseaddrptr_x,
				moduleparams->ll_localaddrptr_x,
				_ll_edge_block_array,
				_ll_lva,
				_ll_eba_tracker,
				_edge_block_array_m,
				_edge_block_array_o,
				geni);
		}
		else if (llgdsunitcmd.verdict == UPDATEEDGEPTRSCMD)
		{
			ll_updateedgeptrs(
				edge,
				*moduleparams,
				_ll_edge_block_array);
		}
		else if (llgdsunitcmd.verdict == NOCMD)
		{
			// do nothing
		}
		else
		{
			cout << "bug! : should never get here (update_edge45)" << endl;
		}
#endif
	}
} // namespace gt