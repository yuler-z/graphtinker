#include "unit/unit_flow.h"
;

namespace graphtinker
{
	/**
	 * 根据之前的计算，为下个循环做准备，或退出循环。
	 */ 
	void UnitFlow::IntervalUnit(
		margin_t *work_block_margin,
		margin_t *sub_block_margin,
		margin_t *start_wblkmargin,
		margin_t *first_wblkmargin,
		vertexid_t *vtx_id,
		bucket_t *adjvtx_id_hash,
		edge_t edge,
		uint edge_update_cmd,
		uint *tripiteration_lcs,
		uint *geni,
		uint *quitstatus
#ifdef EN_CAL
		,
		cal_unit_cmd_t *cal_unit_cmd
#endif
#ifdef EN_CRUMPLE_IN
		,
		crumple_in_cmd_t *heba_deleteandcrumplein_cmd
		,
		uint *lastgenworkblockaddr
#endif
	)
	{
		module_params_t &module_params = unit_option->module_params;
		load_unit_cmd_t &load_unit_cmd = unit_option->load_unit_cmd;
		interval_unit_cmd_t &interval_unit_cmd = unit_option->interval_unit_cmd;


		if (interval_unit_cmd.verdict == QUIT_TO_NEXT_EDGE)
		{
			// exit
			*quitstatus = 1;
		}
		else if (interval_unit_cmd.verdict == CONTINUE_IN_LOWER_GENERATION)
		{

#ifdef EN_CRUMPLE_IN
			// keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
			*lastgenworkblockaddr = GetEdgeblockOffset(*vtx_id) + work_block_margin->top / WORK_BLOCK_HEIGHT;
#endif

			// update/initialize appropriate module globals
			*geni += 1;
			*vtx_id = module_params.cptr;
			*tripiteration_lcs = 0;
			*adjvtx_id_hash = GoogleHash(module_params.adjvtx_id, *geni);
			FindWorkBlockMargin(*adjvtx_id_hash,work_block_margin);
			*start_wblkmargin = *work_block_margin;
			FindSubBlockMargin(*adjvtx_id_hash,sub_block_margin);

			// initialize appropriate fields of lcs units
			// ..2 funcs don't touch edge fields, to avoid overriding any swapping which may have occurred
			unit_option->InitModuleUnitParams2();
			unit_option->InitLoadUnit();
			unit_option->InitInsertUnit2();
			unit_option->InitFindUnit2();
			unit_option->InitWritebackUnit();
		}
		else if (interval_unit_cmd.verdict == CONTINUE_IN_CURRENT_GENERATION)
		{

			// update/initialize appropriate module globals
			*tripiteration_lcs += 1;
		}
		else if (interval_unit_cmd.verdict == CONTINUE_FROM_FIRST_GENERATION)
		{

			// update/initialize appropriate module globals
			*geni = 1;
			*vtx_id = edge.vtx_id;
			*tripiteration_lcs = 0;
			*adjvtx_id_hash = GoogleHash(edge.adjvtx_id, *geni); //calculate hashes
			FindWorkBlockMargin(*adjvtx_id_hash,work_block_margin);		  //find margins
			*start_wblkmargin = *work_block_margin;
			*first_wblkmargin = *work_block_margin;
			FindSubBlockMargin(*adjvtx_id_hash, sub_block_margin);

			//set module cmd
			unit_option->module_unit_cmd.mode = INSERT_MODE;

			// initialize appropriate fields of lcs units
			// everything is initialized here
			unit_option->InitModuleUnitParams(edge.adjvtx_id, edge.edgew);
			unit_option->InitLoadUnit();
			unit_option->InitInsertUnit(edge.adjvtx_id, *adjvtx_id_hash, edge.edgew);
			unit_option->InitFindUnit(edge.adjvtx_id, *adjvtx_id_hash, edge.edgew);
			unit_option->InitWritebackUnit();
#ifdef EN_CAL
			unit_option->InitCalUnit();
#endif
#ifdef EN_CRUMPLE_IN
			init_deleteandcrumplein_verdictcmd(heba_deleteandcrumplein_cmd);
#endif
		}
		else
		{
			LOG(ERROR) << " should never get here (IntervalUnit)"  ;
		}
		return;
	}
	
#ifdef EN_CRUMPLE_IN
	void Graphtinker::init_deleteandcrumplein_verdictcmd(crumple_in_cmd_t *heba_deleteandcrumplein_cmd)
	{
		heba_deleteandcrumplein_cmd->verdict = DCI_NOCMD;
		return;
	}

#endif
} // namespace graphtinker