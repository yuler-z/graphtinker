#include "unit/unit_flow.h"
;

namespace graphtinker
{
	/**
	 * 根据之前的计算，为下个循环做准备，或退出循环。
	 */ 
	void UnitFlow::interval_unit(
		edge_t edge,
		margin_t *workblock_margin,
		margin_t *subblock_margin,
		margin_t *start_wblkmargin,
		vertexid_t *vtx_id,
		bucket_t *adjvtx_id_hash,
		uint *tripiteration_lcs,
		uint *geni,
		uint *quitstatus
#ifdef EN_DCI
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

#ifdef EN_DCI
			// keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
			*lastgenworkblockaddr = get_edgeblock_offset(*vtx_id) + workblock_margin->top / WORK_BLOCK_HEIGHT;
#endif

			// 1. update/initialize appropriate module globals
			*geni += 1;
			*vtx_id = module_params.cptr;
			*tripiteration_lcs = 0;
			*adjvtx_id_hash = gt_->google_hash(module_params.adjvtx_id,module_params.type, *geni);
			gt_->find_workblock_margin(*adjvtx_id_hash, workblock_margin);
			gt_->find_subblock_margin(*adjvtx_id_hash,subblock_margin);
			*start_wblkmargin = *workblock_margin;

			// 2. funcs don't touch edge fields, to avoid overriding any swapping which may have occurred
			unit_option->InitModuleUnitParams2();
			unit_option->InitLoadUnit();
			unit_option->InitFindUnit2();
			unit_option->InitInsertUnit2();
			unit_option->InitWritebackUnit();
		}
		else if (interval_unit_cmd.verdict == CONTINUE_IN_CURRENT_GENERATION)
		{

			*tripiteration_lcs += 1;
		}
		else if (interval_unit_cmd.verdict == CONTINUE_FROM_FIRST_GENERATION)
		{

			//1. update/initialize appropriate module globals
			*geni = 1;
			*vtx_id = edge.vtx_id;
			*tripiteration_lcs = 0;
			*adjvtx_id_hash = gt_->google_hash(edge.adjvtx_id,edge.type,*geni);
			gt_->find_workblock_margin(*adjvtx_id_hash,workblock_margin);
			*start_wblkmargin = *workblock_margin;
			gt_->find_subblock_margin(*adjvtx_id_hash, subblock_margin);

			//2. set module cmd
			unit_option->module_unit_cmd.mode = INSERT_MODE;

			//3. initialize appropriate fields of lcs units. everything is initialized here
			unit_option->InitModuleUnitParams(edge);
			unit_option->InitLoadUnit();
			unit_option->InitFindUnit(edge, *adjvtx_id_hash);
			unit_option->InitInsertUnit(edge, *adjvtx_id_hash);
			unit_option->InitWritebackUnit();
#ifdef EN_CAL
			unit_option->InitCalUnit();
#endif
#ifdef EN_DCI
			init_dci_unit_cmd(heba_deleteandcrumplein_cmd);
#endif
		}
		else
		{
			LOG(ERROR) << " should never get here (interval_unit)"  ;
		}
		return;
	}
	
#ifdef EN_DCI
	void Graphtinker::init_dci_unit_cmd()
	{
		unit_option->dci_cmd.verdict = DCI_NOCMD;
		return;
	}

#endif
} // namespace graphtinker