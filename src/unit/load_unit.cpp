#include "unit/unit_flow.h"

;


namespace graphtinker
{

	void UnitFlow::load_unit(
		margin_t workblock_margin,
		vertexid_t vtx_id,
		vector<workblock_t> &edge_block_array,
		workblock_t *workblock)
	{

		uint trueoffset4rmbase = gt_->get_edgeblock_offset(vtx_id);
		uint addr = trueoffset4rmbase + workblock_margin.top / WORK_BLOCK_HEIGHT;

		DLOG(INFO) << "----load Unit----";
		DLOG(INFO) << "trueoffset4rmbase = " << trueoffset4rmbase;
		DLOG(INFO) << "addr of workblock = " << addr;

		// prevLoadAddr accelerating
		static uint preLoadAddr = NAv;
		if (unit_option->load_unit_cmd.load && preLoadAddr != addr )
		{
			if (addr >= edge_block_array.size())
			{
				LOG(ERROR) << " addr out-of-range (load_unit) : vtx_id : " << vtx_id << ", workblock_margin.top/WORK_BLOCK_HEIGHT : " << workblock_margin.top / WORK_BLOCK_HEIGHT << " addr : " << addr << ", edge_block_array.size() : " << edge_block_array.size(); // << ", geni : " << geni  ;
				return;
			}

			*workblock = edge_block_array[addr];
			preLoadAddr = NAv; //reset
		}

		 	preLoadAddr = addr; //assign

		if (addr >= edge_block_array.size())
		{
			LOG(ERROR) << " addr out-of-range (load_unit) (B) : addr = " << addr  ;
		}

		clusterinfo_t clusterinfo = workblock->clusterinfo; //retreive cluster info
		if (clusterinfo.flag == VALID)
		{
			unit_option->module_params.clustered = YES;
			unit_option->module_params.cptr = clusterinfo.data;
		}

		return;
	}
} // namespace graphtinker