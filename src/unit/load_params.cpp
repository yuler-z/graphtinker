#include "unit/unit_flow.h"

namespace graphtinker
{
	/**
	 * 设置insert unit和find unit的params
	 * 该边是否是所处work_block的start block
	 */
	void UnitFlow::load_params(
		margin_t work_block_margin,
		bucket_t adjvtx_id_hash)
	{
		if (adjvtx_id_hash >= work_block_margin.top && unit_option->module_params.rolledover == NO)
		{
			unit_option->insert_params.is_start_blk = true;
			unit_option->find_params.is_start_blk = true;
		}
		else
		{
			unit_option->insert_params.is_start_blk = false;
			unit_option->find_params.is_start_blk = false;
		}
		return;
	}
} // namespace graphtinker