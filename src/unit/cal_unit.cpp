#include "unit/unit_flow.h"

namespace graphtinker
{
	void UnitFlow::cal_unit(
		edge_t edge,
		vector<work_block_t> &edge_block_array_m_,
		vector<work_block_t> &edge_block_array_o_,
		uint geni
#ifdef EN_CAL
		,
		vector<cal_edgeblock_t> &cal_edgeblock_array_, vector<cal_logical_vertex_entity_t> &cal_lva_, cal_edgeblock_tracker_t *cal_edgeblock_tracker_
#endif
	)
	{
// if searchstopped and unsuccessful
// append to LL, add {lastlocalbaseaddr, lastlocaladdr} to edge
// off we go to insert unit !
// NB: this unit should be before interval unit***
#ifdef EN_CAL
		cal_unit_cmd_t &cal_unit_cmd = unit_option->cal_unit_cmd;
		if (cal_unit_cmd.verdict == INSERTCMD)
		{
			cal_insert(
				edge,
				cal_edgeblock_array_,
				cal_lva_,
				cal_edgeblock_tracker_,
				geni);
		}
		else if (cal_unit_cmd.verdict == UPDATECMD)
		{
			cal_update(
				edge,
				cal_edgeblock_array_);
		}
		else if (cal_unit_cmd.verdict == DELETECMD)
		{
			cal_delete(
				edge,
				cal_edgeblock_array_);
		}
		else if (cal_unit_cmd.verdict == DELETEANDCRUMPLEINCMD)
		{
			cal_delete_and_crumple_in(
				edge,
				cal_edgeblock_array_,
				cal_lva_,
				cal_edgeblock_tracker_,
				edge_block_array_m_,
				edge_block_array_o_,
				geni);
		}
		else if (cal_unit_cmd.verdict == UPDATEEDGEPTRSCMD)
		{
			cal_update_edge_ptrs(
				edge,
				cal_edgeblock_array_);
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
} // namespace graphtinker