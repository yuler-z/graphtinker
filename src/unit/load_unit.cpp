#include "unit/unit_flow.h"

using namespace std;


namespace graphtinker
{

	void UnitFlow::LoadUnit(
		margin_t work_block_margin,
		vertexid_t vtx_id,
		vector<work_block_t> &edge_block_array,
		work_block_t *work_block)
	{

		uint trueoffset4rmbase = GetEdgeblockOffset(vtx_id);
		uint addr = trueoffset4rmbase + work_block_margin.top / WORK_BLOCK_HEIGHT;

		DLOG(INFO) << "----Load Unit----";
		DLOG(INFO) << "trueoffset4rmbase = " << trueoffset4rmbase;
		DLOG(INFO) << "addr of work_block = " << addr;

		// TODO: prevLoadAddr accelerating
		static uint preLoadAddr = NAv;
		if (unit_option->load_unit_cmd.load == YES && preLoadAddr != addr )
		{
			if (addr >= edge_block_array.size())
			{
				LOG(ERROR) << " addr out-of-range (LoadUnit) : vtx_id : " << vtx_id << ", work_block_margin.top/WORK_BLOCK_HEIGHT : " << work_block_margin.top / WORK_BLOCK_HEIGHT << " addr : " << addr << ", edge_block_array.size() : " << edge_block_array.size(); // << ", geni : " << geni  ;
				return;
			}

			*work_block = edge_block_array[addr];
			preLoadAddr = NAv; //reset
		}

		 	preLoadAddr = addr; //assign

		if (addr >= edge_block_array.size())
		{
			LOG(ERROR) << " addr out-of-range (LoadUnit) (B) : addr = " << addr  ;
		}

		clusterinfo_t clusterinfo = work_block->clusterinfo; //retreive cluster info
		if (clusterinfo.flag == VALID)
		{
			unit_option->module_params.clustered = YES;
			unit_option->module_params.cptr = clusterinfo.data;
		}

		return;
	}
} // namespace graphtinker