#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	/**
	 * 根据之前的计算，为下个循环做准备，或退出循环。
	 */ 
	void Graphtinker::interval_unit_updateedge(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t *insertparams,
		insert_report_t *insertreport,
		find_params_t *findparams,
		find_report_t *findreport,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t intervalunitcmd,
		margin_t *work_block_margin,
		margin_t *sub_block_margin,
		margin_t *start_wblkmargin,
		margin_t *first_wblkmargin,
		vertexid_t *xvtx_id,
		bucket_t *hadjvtx_id,
		edge_t edge,
		uint edge_update_cmd,
		uint *tripiteration_lcs,
		uint *geni,
		uint *quitstatus,
		uint infiniti
#ifdef EN_CRUMPLE_IN
		,
		uint *lastgenworkblockaddr
#endif
#ifdef EN_CAL
		,
		cal_unit_cmd_t *cal_unit_cmd
#endif
#ifdef EN_CRUMPLE_IN
		,
		crumple_in_cmd_t *heba_deleteandcrumplein_cmd
#endif
	)
	{
		if (intervalunitcmd.verdict == QUIT_TO_NEXT_EDGE)
		{
			// exit
			*quitstatus = 1;
		}
		else if (intervalunitcmd.verdict == CONTINUE_IN_LOWER_GENERATION)
		{

#ifdef EN_CRUMPLE_IN
			// keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
			*lastgenworkblockaddr = get_edgeblock_offset(*xvtx_id) + work_block_margin->top / WORK_BLOCK_HEIGHT;
#endif

			// update/initialize appropriate module globals
			*geni += 1;
			*xvtx_id = moduleparams->cptr;
			*tripiteration_lcs = 0;
			*hadjvtx_id = googlehash(moduleparams->xadjvtx_id, *geni);
			FindWorkBlockMargin(work_block_margin, *hadjvtx_id);
			*start_wblkmargin = *work_block_margin;
			FindSubBlockMargin(sub_block_margin, *hadjvtx_id);

			// initialize appropriate fields of lcs units
			// ..2 funcs don't touch edge fields, to avoid overriding any swapping which may have occurred
			initialize_moduleunit_params2(moduleparams);
			InitLoadUnit(loadunitcmd);
			initialize_insertunit2(insertparams, insertreport);
			initialize_findunit2(findparams, findreport);
			InitWritebackUnit(writebackunitcmd);
		}
		else if (intervalunitcmd.verdict == CONTINUE_IN_CURRENT_GENERATION)
		{

			// update/initialize appropriate module globals
			*tripiteration_lcs += 1;
		}
		else if (intervalunitcmd.verdict == CONTINUE_FROM_FIRST_GENERATION)
		{

			// update/initialize appropriate module globals
			*geni = 1;
			*xvtx_id = edge.xvtx_id;
			*tripiteration_lcs = 0;
			*hadjvtx_id = googlehash(edge.xadjvtx_id, *geni); //calculate hashes
			FindWorkBlockMargin(work_block_margin, *hadjvtx_id);		  //find margins
			*start_wblkmargin = *work_block_margin;
			*first_wblkmargin = *work_block_margin;
			FindSubBlockMargin(sub_block_margin, *hadjvtx_id);

			//set module cmd
			moduleunitcmd->mode = INSERT_MODE;

			// initialize appropriate fields of lcs units
			// everything is initialized here
			InitModuleUnitParams(moduleparams, edge.xadjvtx_id, edge.edgew);
			InitLoadUnit(loadunitcmd);
			InitInsertUnit(insertparams, insertreport, edge.xadjvtx_id, *hadjvtx_id, edge.edgew);
			InitFindUnit(findparams, findreport, edge.xadjvtx_id, *hadjvtx_id, edge.edgew);
			InitWritebackUnit(writebackunitcmd);
#ifdef EN_CAL
			initialize_llebaverdictcmd(cal_unit_cmd);
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
} // namespace gt