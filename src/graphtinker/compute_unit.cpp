#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	void Graphtinker::ComputeUnit(
		module_unit_cmd_t moduleunitcmd,
		module_params_t *moduleparams,
		insert_params_t *insertparams,
		insert_report_t *insertreport,
		find_params_t findparams,
		find_report_t *findreport,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		margin_t start_wblkmargin,
		bucket_t hadjvtx_id,
		work_block_t *work_block,
		edge_t *edge,
		vertexid_t xvtx_id,
		uint rolledover,
		uint edge_update_cmd,
		uint geni)
	{
		if (moduleunitcmd.mode == FIND_MODE)
		{
			find_unit(
				moduleparams,
				findparams,
				findreport,
				work_block_margin,
				sub_block_margin,
				hadjvtx_id,
				work_block,
				edge_update_cmd);
		}
		else if (moduleunitcmd.mode == INSERT_MODE)
		{
			insert_unit(
				moduleparams,
				insertparams,
				insertreport,
				work_block_margin,
				sub_block_margin,
				hadjvtx_id,
				work_block,
				edge,
				xvtx_id,
				rolledover,
				geni);
		}
		else
		{
			LOG(ERROR) << "bug! : should never get here (compute unit) : moduleunitcmd.mode = " << moduleunitcmd.mode  ;
		}
		return;
	}
} // namespace gt