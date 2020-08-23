#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	/**
	 * 设置insert unit和find unit的params
	 * 该边是否是所处work_block的start
	 */
	void Graphtinker::LoadParams(
		module_params_t moduleparams,
		insert_params_t *insertparams,
		find_params_t *findparams,
		margin_t work_block_margin,
		bucket_t hadjvtx_id)
	{
		if (hadjvtx_id >= work_block_margin.top && moduleparams.rolledover == NO)
		{
			insertparams->isstartblk = 1;
			findparams->isstartblk = 1;
		}
		else
		{
			insertparams->isstartblk = 0;
			findparams->isstartblk = 0;
		}
		return;
	}
} // namespace gt