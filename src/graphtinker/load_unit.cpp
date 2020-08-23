#include <string.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ctime>
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

	void Graphtinker::LoadUnit(
		module_params_t *moduleparams,
		load_unit_cmd_t loadunitcmd,
		insert_params_t *insertparams,
		find_params_t *findparams,
		margin_t work_block_margin,
		vertexid_t hvtx_id,
		edge_t edge,
		work_block_t *work_block,
		vector<work_block_t> &edge_block_array,
		uint *prevLoadAddr,
		uint geni)
	{
		uint trueoffset4rmbase = get_edgeblock_offset(hvtx_id);
		uint addr = trueoffset4rmbase + work_block_margin.top / WORK_BLOCK_HEIGHT;

		if (loadunitcmd.load == YES && addr != *prevLoadAddr)
		{
			if (addr >= edge_block_array.size())
			{
				LOG(ERROR) << " addr out-of-range (LoadUnit) : hvtx_id : " << hvtx_id << ", work_block_margin.top/WORK_BLOCK_HEIGHT : " << work_block_margin.top / WORK_BLOCK_HEIGHT << " addr : " << addr << ", edge_block_array.size() : " << edge_block_array.size() << ", geni : " << geni  ;
				return;
			}

			*work_block = edge_block_array[addr];
			*prevLoadAddr = NAv; //reset
		}

		*prevLoadAddr = addr; //assign

		if (addr >= edge_block_array.size())
		{
			LOG(ERROR) << " addr out-of-range (LoadUnit) (B) : addr = " << addr  ;
		}

		clusterinfo_t clusterinfo = work_block->clusterinfo; //retreive cluster info
		if (clusterinfo.flag == VALID)
		{
			moduleparams->clustered = YES;
			moduleparams->cptr = clusterinfo.data;
		}
		return;
	}
} // namespace gt