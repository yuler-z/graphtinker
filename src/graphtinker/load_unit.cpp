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

#define OPTIMIZEUSINGPREVADDR 0

namespace gt
{

	void Graphtinker::load_unit(
		module_params_t *moduleparams,
		load_unit_cmd_t loadunitcmd,
		insert_params_t *insertparams,
		find_params_t *findparams,
		margin_t wblkmargin,
		vertexid_t hvtx_id,
		edge_t edge,
		edge_nt *edgeblock,
		vector<edge_nt> &edge_block_array,
		uint *prevLoadAddr,
		uint geni)
	{
		uint trueoffset4rmbase = get_edgeblock_offset(hvtx_id);
		uint addr = trueoffset4rmbase + wblkmargin.top / WORK_BLOCK_HEIGHT;

#ifdef OPTIMIZEUSINGPREVADDR
		if (loadunitcmd.load == YES && addr != *prevLoadAddr)
		{
#ifdef cpuem_bugs_b1
			if (addr >= edge_block_array.size())
			{
				cout << "bug! : addr out-of-range (load_unit) : hvtx_id : " << hvtx_id << ", wblkmargin.top/WORK_BLOCK_HEIGHT : " << wblkmargin.top / WORK_BLOCK_HEIGHT << " addr : " << addr << ", edge_block_array.size() : " << edge_block_array.size() << ", geni : " << geni << endl;
				return;
			}
#endif

			*edgeblock = edge_block_array[addr];
			*prevLoadAddr = NAv; //reset
		}
#endif
#ifndef OPTIMIZEUSINGPREVADDR
		if (loadunitcmd.load == YES)
		{
#ifdef cpuem_bugs_b1
			if (addr >= edge_block_array.size())
			{
				cout << "bug! : addr out-of-range (load_unit) : hvtx_id = " << hvtx_id << ", wblkmargin.top/WORK_BLOCK_HEIGHT = " << wblkmargin.top / WORK_BLOCK_HEIGHT << " addr = " << (trueoffset4rmbase + wblkmargin.top / WORK_BLOCK_HEIGHT) << endl;
				return;
			}
#endif

			*edgeblock = edge_block_array[addr];
			*prevLoadAddr = NAv; //reset
		}
#endif

		*prevLoadAddr = addr; //assign

#ifdef cpuem_bugs_b1
		if (addr >= edge_block_array.size())
		{
			cout << "bug! : addr out-of-range (load_unit) (B) : addr = " << addr << endl;
		}
#endif

		clusterinfo_t clusterinfo = edgeblock->clusterinfo; //retreive cluster info
		if (clusterinfo.flag == VALID)
		{
			moduleparams->clustered = YES;
			moduleparams->cptr = clusterinfo.data;
		}
		return;
	}
} // namespace gt