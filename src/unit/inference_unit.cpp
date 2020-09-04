#include "unit/unit_flow.h"

/**
1. all cmds are set:
	- module_params for this current lcs iteration is made up-to-date
	- module_unit_cmd for next lcs iteration is set 
	- load_unit_cmd for the next lcs iteration is set
	- writeback_unit_cmd for this current lcs iteration is set
	- interval_unit_cmd for this current lcs iteration is set
	after inference unit returns
2. margin information (workblock margin) for the next lcs iteration is set
*/
namespace graphtinker
{

	/**
	 * 给出接下来的决断
	 * 选择退出循环
	 * 还是其他行为
	 */
	void UnitFlow::inference_unit(
		bool is_insert_edge, // insert or delete
		margin_t *workblock_margin,
		margin_t subblock_margin,
		margin_t start_wblkmargin,
		vertexid_t vtx_id)
	{
		search_report_t &search_report = unit_option->search_report;
		module_unit_cmd_t &module_unit_cmd = unit_option->module_unit_cmd;
		find_params_t &find_params = unit_option->find_params;
		find_report_t &find_report = unit_option->find_report;
		insert_report_t &insert_report = unit_option->insert_report;

		search_report.is_search_stop = false;
		search_report.is_search_success = false;
		if (module_unit_cmd.mode == FIND_MODE)
		{
			search_report.is_search_stop = find_report.is_found || find_report.is_empty || find_report.is_reach_max_prob_length;
			search_report.is_search_success = find_report.is_found;
		}
		DLOG(INFO) << "-----Inference Unit-----";
		if (search_report.is_search_stop)
			DLOG(INFO) << "search_report.is_search_stop";
		if (search_report.is_search_success)
			DLOG(INFO) << "search_report.is_search_success";

		bool is_last_workblock = is_last_workblock_in_subblock(*workblock_margin, start_wblkmargin, subblock_margin);

#ifdef EN_OTHERS
		//LOG(INFO) <<"inference params of tinker = "<< tinkerid <<", batch = "<<batchid<<", chunk = "<<chunkid ;
		LOG(INFO) << "search_report.is_search_stop = " << search_report.is_search_stop;
		LOG(INFO) << "search_report.is_search_success = " << search_report.is_search_success;
		LOG(INFO) << "insert_report.exittype = " << insert_report.exittype;
		LOG(INFO) << "module_unit_cmd.mode = " << module_unit_cmd.mode;
		LOG(INFO) << "is_last_workblock = " << is_last_workblock;
#endif

		//|Mode|_|Searchstopped|_|searchsuccessfull_|InsertReport.exittype|_|is_last_workblock?|
		//find-only mode
		if (module_unit_cmd.mode == FIND_MODE)
		{
			// |search stop|search success|is_last_workblock|is_insert_edge|
			// case 1: 0x100
			// case 2: 0x110 0x111
			// case 3: 0x000
			// case 4: 0x001
			
			if (search_report.is_search_stop && !search_report.is_search_success)
			{

				if (is_insert_edge)// insert edge
				{
					find_stopped_notsuccess_insert(
						*workblock_margin,
						subblock_margin,
						vtx_id);
				}
#ifndef EN_DCI
				else if (!is_insert_edge)/*delete_edge*/
				{
					find_stopped_notsuccess_insert_delete(
						*workblock_margin,
						subblock_margin,
						vtx_id);
				}
#else
				else if (!is_insert_edge)/*delete edge*/
				{
					find_stopped_notsuccessful_dci_delete(
						module_params,
						find_params,
						writeback_unit_cmd,
						interval_unit_cmd,
						*workblock_margin,
						subblock_margin,
						vtx_id);
				}
#endif
				else
				{
					LOG(ERROR) << " should never be seen here (inference_unit34)";
				}
			}
			else if (search_report.is_search_stop && search_report.is_search_success)
			{
				if (is_insert_edge)
				{
					find_stopped_success_insert(
						*workblock_margin,
						subblock_margin,
						vtx_id);
				}
#ifndef EN_DCI
				else if (!is_insert_edge)/*delete*/
				{
					find_stopped_success_delete(
						*workblock_margin,
						subblock_margin,
						vtx_id);
				}
#else
				else if (!is_insert_edge)/*delete*/
				{
					find_stopped_success_dci_delete(

						module_params,

						find_params,
						find_report,
						writeback_unit_cmd,
						interval_unit_cmd,
						*workblock_margin,
						subblock_margin,
						vtx_id);
				}
#endif
				else
				{
					LOG(ERROR) << " should never be seen here (inference_unit35)";
				}
			}
			else if (!search_report.is_search_stop && !search_report.is_search_success && !is_last_workblock)
			{
				find_notstopped_notsuccess_notlastworkblock(
					workblock_margin,
					subblock_margin,
					vtx_id);
			}
			else if (!search_report.is_search_stop && !search_report.is_search_success && is_last_workblock)
			{

				find_notstopped_notsuccessful_lastworkblock(
					*workblock_margin,
					subblock_margin,
					vtx_id);
			}
		}
		else if (module_unit_cmd.mode == INSERT_MODE)
		{
			if (insert_report.exittype == PASSEDTHROUGHANDSWAPPEDATSOMEPOINT && !is_last_workblock)
			{

				insert_passedthroughandswappedatsomepoint_notlastworkblock(
					workblock_margin,
					subblock_margin,
					vtx_id);
			}
			else if (insert_report.exittype == PASSEDTHROUGHANDSWAPPEDATSOMEPOINT && is_last_workblock)
			{
				insert_passedthroughandswappedatsomepoint_lastworkblock(
					*workblock_margin,
					subblock_margin,
					vtx_id);
			}
			else if (insert_report.exittype == LOAD_INTO_EMPTY_BUCKET)
			{
				insert_loadedintoemptybucket(
					*workblock_margin,
					subblock_margin,
					vtx_id);
			}
			else if (insert_report.exittype == FOUNDANDUPDATEDITSELF)
			{
				insert_foundandupdateditself(
					*workblock_margin,
					subblock_margin,
					vtx_id);
			}
			else if (insert_report.exittype == PASSEDTHROUGH && !is_last_workblock)
			{
				insert_passedthrough_notlastworkblock(
					workblock_margin,
					subblock_margin,
					vtx_id);
			}
			else if (insert_report.exittype == PASSEDTHROUGH && is_last_workblock)
			{
				insert_passedthrough_lastworkblock(
					*workblock_margin,
					subblock_margin,
					vtx_id);
			}
			else
			{
				LOG(ERROR) << " should never get here! (inference unit)";
			}
			return;
		}
	}

	//|Mode|_|Searchstopped|_|searchsuccessfull_|InsertReport.exittype|_|is_last_workblock?|
	//Find-Only Mode
	void UnitFlow::find_stopped_notsuccess_insert(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		DLOG(INFO) << "set cmd";
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparams_edgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.weight); //***
		set_loadunitcmd_loadnextedgeblock(true);
		set_writebackunitcmd_writebackcurrentedgeblock(NO, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		set_intervalunitcmd_continue_from_first_generation();
		SET_CAL_UNIT_CMD(INSERTCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	void UnitFlow::find_stopped_success_insert(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);																													   //default mode
		set_moduleunitparams_edgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.weight);											   //***
		set_loadunitcmd_loadnextedgeblock(true);																												   //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		SET_CAL_UNIT_CMD(UPDATECMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	void UnitFlow::find_notstopped_notsuccess_notlastworkblock(
		margin_t *workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparams_edgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.weight);											 //***
		set_loadunitcmd_loadnextedgeblock(true);																												 //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(NO, (gt_->get_edgeblock_offset(vtx_id) + workblock_margin->top / WORK_BLOCK_HEIGHT), subblock_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(workblock_margin, subblock_margin);
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	void UnitFlow::find_notstopped_notsuccessful_lastworkblock(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparams_edgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.weight);											  //***
		set_loadunitcmd_loadnextedgeblock(true);																												  //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(NO, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		if (unit_option->module_params.clustered != YES)
		{
			set_writebackunitcmd_markasclustered(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)));
		} //***???
		set_intervalunitcmd_continue_in_lower_generation();
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	//Insert-Only Mode
	void UnitFlow::insert_passedthroughandswappedatsomepoint_notlastworkblock(
		margin_t *workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparams_edgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.weight);										  //***
		set_loadunitcmd_loadnextedgeblock(true);																												  //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(YES, (gt_->get_edgeblock_offset(vtx_id) + workblock_margin->top / WORK_BLOCK_HEIGHT), subblock_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(workblock_margin, subblock_margin);
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	void UnitFlow::insert_passedthroughandswappedatsomepoint_lastworkblock(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparams_edgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.weight);										   //***
		set_loadunitcmd_loadnextedgeblock(true);																												   //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		if (unit_option->module_params.clustered != YES)
		{
			set_writebackunitcmd_markasclustered(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)));
		}
		set_intervalunitcmd_continue_in_lower_generation();
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	void UnitFlow::insert_loadedintoemptybucket(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparams_edgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.weight);										   //***
		set_loadunitcmd_loadnextedgeblock(true);																												   //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		SET_CAL_UNIT_CMD(UPDATEEDGEPTRSCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	void UnitFlow::insert_foundandupdateditself(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparams_edgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.weight);										   //***
		set_loadunitcmd_loadnextedgeblock(true);																												   //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	void UnitFlow::insert_passedthrough_notlastworkblock(
		margin_t *workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparams_edgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.weight);										 //***
		set_loadunitcmd_loadnextedgeblock(true);																												 //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(NO, (gt_->get_edgeblock_offset(vtx_id) + workblock_margin->top / WORK_BLOCK_HEIGHT), subblock_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(workblock_margin, subblock_margin);
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	void UnitFlow::insert_passedthrough_lastworkblock(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparams_edgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.weight);										  //***
		set_loadunitcmd_loadnextedgeblock(true);																												  //load next EB
		set_writebackunitcmd_writebackcurrentedgeblock(NO, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		if (unit_option->module_params.clustered != YES)
		{
			set_writebackunitcmd_markasclustered(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)));
		}
		set_intervalunitcmd_continue_in_lower_generation();
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
		return;
	}

	//Find-And-Delete Mode
	void UnitFlow::find_stopped_notsuccess_insert_delete(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparams_edgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.weight);											  //***
		set_loadunitcmd_loadnextedgeblock(true);																												  //*** NO YES
		set_writebackunitcmd_writebackcurrentedgeblock(NO, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);
#ifdef EN_OTHERS
		LOG(ERROR) << " find not successfull (inference_unit22)"; //***
#endif
		return;
	}

	void UnitFlow::find_stopped_success_delete(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparams_edgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.weight);											   //***
		set_loadunitcmd_loadnextedgeblock(true);																												   //*** NO YES
		set_writebackunitcmd_writebackcurrentedgeblock(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		SET_CAL_UNIT_CMD(DELETECMD);
		SET_DCI_UNIT_CMD(DCI_NOCMD);

#ifdef EN_OTHERS
		if (find_report.is_deleted)
		{
			LOG(INFO) << "entry founf but deleted (inference unit)";
		}
		else if (!find_report.is_deleted == NO)
		{
			LOG(ERROR) << " entry actually not found at all (inference unit)";
		}
		else
		{
			LOG(ERROR) << " should not get here (inference_unit55)";
		}
#endif
		return;
	}

	//Find-And-Delete-And-Crumplein Mode
	void UnitFlow::find_stopped_notsuccessful_dci_delete(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparams_edgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.weight);											  //***
		set_loadunitcmd_loadnextedgeblock(true);																												  //*** NO YES
		set_writebackunitcmd_writebackcurrentedgeblock(NO, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		SET_CAL_UNIT_CMD(NOCMD);
		SET_DCI_UNIT_CMD(DCI_JUSTQUITCMD);

#ifdef EN_OTHERS
		LOG(ERROR) << " find not successfull (inference_unit11)"; //***
#endif
		return;
	}

	void UnitFlow::find_stopped_success_dci_delete(
		margin_t workblock_margin,
		margin_t subblock_margin,
		vertexid_t vtx_id)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparams_edgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.weight);
		set_loadunitcmd_loadnextedgeblock(true);
		set_writebackunitcmd_writebackcurrentedgeblock(YES, (gt_->get_edgeblock_offset(vtx_id) + (workblock_margin.top / WORK_BLOCK_HEIGHT)), subblock_margin);
		set_intervalunitcmd_quit_to_next_edge();
		SET_CAL_UNIT_CMD(DELETEANDCRUMPLEINCMD);
		if (unit_option->module_params.clustered == YES)
		{
			SET_DCI_UNIT_CMD(DCI_CRUMPLEINCMD);
		}
		else
		{
			SET_DCI_UNIT_CMD(DCI_JUSTQUITCMD);
		}

#ifdef EN_OTHERS
		if (find_report.is_deleted)
		{
			LOG(INFO) << "entry founf but deleted (inference unit)";
		}
		else if (!find_report.is_deleted)
		{
			LOG(ERROR) << " entry actually not found at all (inference unit)";
		}
		else
		{
			LOG(ERROR) << " should not get here (inference_unit66)";
		}
#endif
		return;
	}

	//verdict functions
	void UnitFlow::set_intervalunitcmd_continue_from_first_generation()
	{
		unit_option->interval_unit_cmd.verdict = CONTINUE_FROM_FIRST_GENERATION;
		return;
	}

	void UnitFlow::set_intervalunitcmd_continue_in_lower_generation()
	{
		unit_option->interval_unit_cmd.verdict = CONTINUE_IN_LOWER_GENERATION;
		return;
	}

	void UnitFlow::set_intervalunitcmd_continue_in_current_generation(margin_t *workblock_margin, margin_t subblock_margin)
	{
		unit_option->interval_unit_cmd.verdict = CONTINUE_IN_CURRENT_GENERATION;
		update_margin_and_rollover_status(workblock_margin, subblock_margin);
		return;
	}

	void UnitFlow::set_intervalunitcmd_quit_to_next_edge()
	{
		unit_option->interval_unit_cmd.verdict = QUIT_TO_NEXT_EDGE;
		return;
	}

	void UnitFlow::set_loadunitcmd_loadnextedgeblock(uint cmd)
	{
		unit_option->load_unit_cmd.load = cmd;
		return;
	}

	void UnitFlow::set_moduleunitcmd(uint cmd)
	{
		unit_option->module_unit_cmd.mode = cmd;
		return;
	}

	void UnitFlow::set_moduleunitparams_edgefields(vertexid_t adjvtx_id, edge_weight_t weight)
	{
		unit_option->module_params.adjvtx_id = adjvtx_id;
		unit_option->module_params.weight = weight;
		return;
	}

	void UnitFlow::set_writebackunitcmd_writebackcurrentedgeblock(uint cmd, uint addr, margin_t subblock_margin)
	{
		unit_option->writeback_unit_cmd.writeback = cmd; //writeback EB
		unit_option->writeback_unit_cmd.addr = addr;
		unit_option->writeback_unit_cmd.subblockid = subblock_margin.top / gt_->subblock_height_;
		return;
	}

	void UnitFlow::set_writebackunitcmd_markasclustered(uint cmd, uint addr)
	{
		unit_option->writeback_unit_cmd.markasclustered = cmd; //mark as clustered
		unit_option->writeback_unit_cmd.addr = addr;
		return;
	}

#ifdef EN_CAL
	void UnitFlow::set_cal_unit_cmd(uint cmd)
	{
		unit_option->cal_unit_cmd.verdict = cmd;
		return;
	}
#endif

#ifdef EN_DCI
	void UnitFlow::set_dci_unit_cmd(uint cmd)
	{
		unit_option->dci_unit_cmd.verdict = cmd;
		return;
	}
#endif

	//utility functions
	void UnitFlow::update_margin_and_rollover_status(margin_t *workblock_margin, margin_t subblock_margin)
	{
		if (workblock_margin->bottom == subblock_margin.bottom)
		{ //roll over
			DLOG(INFO) << "**** reached end of sub-block, rolling over... ****";
			workblock_margin->top = subblock_margin.top;
			workblock_margin->bottom = subblock_margin.top + (WORK_BLOCK_HEIGHT - 1);
			unit_option->module_params.rolledover = YES;
		}
		else
		{ //move forward
			workblock_margin->top += WORK_BLOCK_HEIGHT;
			workblock_margin->bottom += WORK_BLOCK_HEIGHT;
		}
		return;
	}

	bool UnitFlow::is_last_workblock_in_subblock(margin_t workblock_margin, margin_t start_wblkmargin, margin_t subblock_margin)
	{
		if (unit_option->module_params.rolledover == YES && workblock_margin.top == start_wblkmargin.top)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
} // namespace graphtinker