#include "unit/unit_flow.h"

#ifdef EN_CAL
	#define	EN_CAL_SET_CAL_CMD(cmd) \
		set_cal_unit_cmd(cmd)
#else
	#define	EN_CAL_SET_CAL_CMD(cmd)
#endif

#ifdef EN_CRUMPLE_IN
	#define EN_CRUMPLE_IN_SET_VERDICT(cmd)	\
		heba_popoutpopin_cmd->verdict = cmd;
#else
	#define EN_CRUMPLE_IN_SET_VERDICT(cmd)
#endif
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
	void UnitFlow::InferenceUnit(
		uint edge_update_cmd,
		margin_t *work_block_margin,
		margin_t sub_block_margin,
		margin_t start_wblkmargin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
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
		if(search_report.is_search_stop) DLOG(INFO) << "search_report.is_search_stop";
		if(search_report.is_search_success) DLOG(INFO) << "search_report.is_search_success";

		bool is_last_work_block = IsLastWorkBlockInSubBlock(*work_block_margin, start_wblkmargin, sub_block_margin);

#ifdef EN_OTHERS
		//LOG(INFO) <<"inference params of tinker = "<< tinkerid <<", batch = "<<batchid<<", chunk = "<<chunkid ;
		LOG(INFO) << "search_report.is_search_stop = " << search_report.is_search_stop;
		LOG(INFO) << "search_report.is_search_success = " << search_report.is_search_success ;
		LOG(INFO) << "insert_report.exittype = " << insert_report.exittype ;
		LOG(INFO) << "module_unit_cmd.mode = " << module_unit_cmd.mode ;
		LOG(INFO) << "is_last_work_block = " << is_last_work_block ;
#endif

		//|Mode|_|Searchstopped|_|searchsuccessfull_|InsertReport.exittype|_|is_last_work_block?|
		//find-only mode
		if (module_unit_cmd.mode == FIND_MODE && search_report.is_search_stop && !search_report.is_search_success)
		{
			if (edge_update_cmd == INSERTEDGE)
			{
				findonlymode_searchstopped_searchunsuccessful_x_x(
					*work_block_margin,
					sub_block_margin,
					vtx_id,
					cal_unit_cmd ,
					heba_popoutpopin_cmd
				);
			}
#ifndef EN_CRUMPLE_IN
			else if (edge_update_cmd == DELETEEDGE)
			{
				findanddeletemode_searchstopped_searchunsuccessful_x_x(
					*work_block_margin,
					sub_block_margin,
					vtx_id ,
					cal_unit_cmd,
					heba_popoutpopin_cmd
				);
			}
#endif
#ifdef EN_CRUMPLE_IN
			else if (edge_update_cmd == DELETEEDGE)
			{
				findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
					
					module_params,
					
					find_params,
					writeback_unit_cmd,
					interval_unit_cmd,
					*work_block_margin,
					sub_block_margin,
					vtx_id,
					cal_unit_cmd,
					heba_popoutpopin_cmd
				);
			}
#endif
			else
			{
				LOG(ERROR) << " should never be seen here (inference_unit34)"  ;
			}
		}
		else if (module_unit_cmd.mode == FIND_MODE && search_report.is_search_stop && search_report.is_search_success)
		{
			if (edge_update_cmd == INSERTEDGE)
			{
				findonlymode_searchstopped_searchsuccessful_x_x(
					*work_block_margin,
					sub_block_margin,
					vtx_id,
					cal_unit_cmd,
					heba_popoutpopin_cmd
				);
			}
#ifndef EN_CRUMPLE_IN
			else if (edge_update_cmd == DELETEEDGE)
			{
				findanddeletemode_searchstopped_searchsuccessful_x_x(
					*work_block_margin,
					sub_block_margin,
					vtx_id,
					cal_unit_cmd,
					heba_popoutpopin_cmd
				);
			}
#endif
#ifdef EN_CRUMPLE_IN
			else if (edge_update_cmd == DELETEEDGE)
			{
				findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
					
					module_params,
					
					find_params,
					find_report,
					writeback_unit_cmd,
					interval_unit_cmd,
					*work_block_margin,
					sub_block_margin,
					vtx_id,
					cal_unit_cmd,
					heba_popoutpopin_cmd
				);
			}
#endif
			else
			{
				LOG(ERROR) << " should never be seen here (inference_unit35)"  ;
			}
		}
		else if (module_unit_cmd.mode == FIND_MODE && !search_report.is_search_stop && search_report.is_search_success == false && !is_last_work_block)
		{
			findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
				work_block_margin,
				sub_block_margin,
				vtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (module_unit_cmd.mode == FIND_MODE && !search_report.is_search_stop && !search_report.is_search_success && is_last_work_block)
		{
			findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
				*work_block_margin,
				sub_block_margin,
				vtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
			//insert-only mode
		}
		else if (module_unit_cmd.mode == INSERT_MODE && insert_report.exittype == PASSEDTHROUGHANDSWAPPEDATSOMEPOINT && !is_last_work_block)
		{
			insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
				work_block_margin,
				sub_block_margin,
				vtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (module_unit_cmd.mode == INSERT_MODE && insert_report.exittype == PASSEDTHROUGHANDSWAPPEDATSOMEPOINT && is_last_work_block)
		{
			insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
				*work_block_margin,
				sub_block_margin,
				vtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (module_unit_cmd.mode == INSERT_MODE && insert_report.exittype == LOADEDINTOEMPTYBUCKET)
		{
			insertonlymode_x_x_loadedintoemptybucket_x(
				*work_block_margin,
				sub_block_margin,
				vtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (module_unit_cmd.mode == INSERT_MODE && insert_report.exittype == FOUNDANDUPDATEDITSELF)
		{
			insertonlymode_x_x_foundandupdateditself_x(
				*work_block_margin,
				sub_block_margin,
				vtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (module_unit_cmd.mode == INSERT_MODE && insert_report.exittype == PASSEDTHROUGH && !is_last_work_block)
		{
			insertonlymode_x_x_passedthrough_notlastworkblock(
				work_block_margin,
				sub_block_margin,
				vtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (module_unit_cmd.mode == INSERT_MODE && insert_report.exittype == PASSEDTHROUGH && is_last_work_block)
		{
			insertonlymode_x_x_passedthrough_lastworkblock(
				*work_block_margin,
				sub_block_margin,
				vtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else
		{
			LOG(ERROR) << " should never get here! (inference unit)"  ;
		}
		return;
	}

	//|Mode|_|Searchstopped|_|searchsuccessfull_|InsertReport.exittype|_|is_last_work_block?|
	//Find-Only Mode
	void UnitFlow::findonlymode_searchstopped_searchunsuccessful_x_x(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		DLOG(INFO) << "set cmd";
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparamsedgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.edge_weight); //***
		set_loadunitcmd_loadnextEB(YES);
		set_writebackunitcmd_writebackcurrentEB(NO, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_continue_from_first_generation();
		EN_CAL_SET_CAL_CMD(INSERTCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void UnitFlow::findonlymode_searchstopped_searchsuccessful_x_x(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(FIND_MODE);																										  //default mode
		set_moduleunitparamsedgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.edge_weight);														  //***
		set_loadunitcmd_loadnextEB(YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		EN_CAL_SET_CAL_CMD(UPDATECMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void UnitFlow::findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
		margin_t *work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparamsedgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.edge_weight);														//***
		set_loadunitcmd_loadnextEB(YES);																										//load next EB
		set_writebackunitcmd_writebackcurrentEB(NO, (GetEdgeblockOffset(vtx_id) + work_block_margin->top / WORK_BLOCK_HEIGHT), sub_block_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(work_block_margin, sub_block_margin);
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void UnitFlow::findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id ,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparamsedgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.edge_weight);														 //***
		set_loadunitcmd_loadnextEB(YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(NO, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		if (unit_option->module_params.clustered != YES)
		{
			setwritebackunitcmd_markasclustered(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)));
		} //***???
		set_intervalunitcmd_continue_in_lower_generation();
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	//Insert-Only Mode
	void UnitFlow::insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
		margin_t *work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparamsedgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.edge_weight);													 //***
		set_loadunitcmd_loadnextEB(YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(YES, (GetEdgeblockOffset(vtx_id) + work_block_margin->top / WORK_BLOCK_HEIGHT), sub_block_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(work_block_margin, sub_block_margin);
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void UnitFlow::insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparamsedgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		if (unit_option->module_params.clustered != YES)
		{
			setwritebackunitcmd_markasclustered(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)));
		}
		set_intervalunitcmd_continue_in_lower_generation();
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void UnitFlow::insertonlymode_x_x_loadedintoemptybucket_x(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparamsedgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		EN_CAL_SET_CAL_CMD(UPDATEEDGEPTRSCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void UnitFlow::insertonlymode_x_x_foundandupdateditself_x(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(FIND_MODE);
		set_moduleunitparamsedgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void UnitFlow::insertonlymode_x_x_passedthrough_notlastworkblock(
		margin_t *work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(INSERT_MODE);
		set_moduleunitparamsedgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.edge_weight);													//***
		set_loadunitcmd_loadnextEB(YES);																										//load next EB
		set_writebackunitcmd_writebackcurrentEB(NO, (GetEdgeblockOffset(vtx_id) + work_block_margin->top / WORK_BLOCK_HEIGHT), sub_block_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation( work_block_margin, sub_block_margin);
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void UnitFlow::insertonlymode_x_x_passedthrough_lastworkblock(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd( INSERT_MODE);
		set_moduleunitparamsedgefields(unit_option->insert_params.adjvtx_id, unit_option->insert_params.edge_weight);													 //***
		set_loadunitcmd_loadnextEB( YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(NO, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		if (unit_option->module_params.clustered != YES)
		{
			setwritebackunitcmd_markasclustered(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)));
		}
		set_intervalunitcmd_continue_in_lower_generation();
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	//Find-And-Delete Mode
	void UnitFlow::findanddeletemode_searchstopped_searchunsuccessful_x_x(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd( FIND_MODE);
		set_moduleunitparamsedgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.edge_weight);														 //***
		set_loadunitcmd_loadnextEB( YES);																										 //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(NO, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
#ifdef EN_OTHERS
		LOG(ERROR) << " find not successfull (inference_unit22)"  ; //***
#endif
		return;
	}

	void UnitFlow::findanddeletemode_searchstopped_searchsuccessful_x_x(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd( FIND_MODE);
		set_moduleunitparamsedgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.edge_weight);														  //***
		set_loadunitcmd_loadnextEB( YES);																										  //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		EN_CAL_SET_CAL_CMD(DELETECMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);

#ifdef EN_OTHERS
		if (find_report.is_deleted == YES)
		{
			LOG(INFO) << "entry founf but deleted (inference unit)"  ;
		}
		else if (find_report.is_deleted == NO)
		{
			LOG(ERROR) << " entry actually not found at all (inference unit)"  ;
		}
		else
		{
			LOG(ERROR) << " should not get here (inference_unit55)"  ;
		}
#endif
		return;
	}

	//Find-And-Delete-And-Crumplein Mode
	void UnitFlow::findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd( FIND_MODE);
		set_moduleunitparamsedgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.edge_weight);														 //***
		set_loadunitcmd_loadnextEB( YES);																										 //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(NO, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge();
		EN_CAL_SET_CAL_CMD(NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_JUSTQUITCMD);

#ifdef EN_OTHERS
		LOG(ERROR) << " find not successfull (inference_unit11)"  ; //***
#endif
		return;
	}

	void UnitFlow::findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t vtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd( FIND_MODE);
		set_moduleunitparamsedgefields(unit_option->find_params.adjvtx_id, unit_option->find_params.edge_weight);
		set_loadunitcmd_loadnextEB( YES);
		set_writebackunitcmd_writebackcurrentEB(YES, (GetEdgeblockOffset(vtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin);
		set_intervalunitcmd_quit_to_next_edge();
		EN_CAL_SET_CAL_CMD(DELETEANDCRUMPLEINCMD);
		if (unit_option->module_params.clustered == YES)
		{
			EN_CRUMPLE_IN_SET_VERDICT(DCI_CRUMPLEINCMD);
		}
		else
		{
			EN_CRUMPLE_IN_SET_VERDICT(DCI_JUSTQUITCMD);
		}

#ifdef EN_OTHERS
		if (find_report.is_deleted == YES)
		{
			LOG(INFO) << "entry founf but deleted (inference unit)"  ;
		}
		else if (find_report.is_deleted == NO)
		{
			LOG(ERROR) << " entry actually not found at all (inference unit)"  ;
		}
		else
		{
			LOG(ERROR) << " should not get here (inference_unit66)"  ;
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

	void UnitFlow::set_intervalunitcmd_continue_in_current_generation(margin_t *work_block_margin, margin_t sub_block_margin)
	{
		unit_option->interval_unit_cmd.verdict = CONTINUE_IN_CURRENT_GENERATION;
		updatemarginandrolloverstatus(work_block_margin, sub_block_margin);
		return;
	}

	void UnitFlow::set_intervalunitcmd_quit_to_next_edge()
	{
		unit_option->interval_unit_cmd.verdict = QUIT_TO_NEXT_EDGE;
		return;
	}

	void UnitFlow::set_loadunitcmd_loadnextEB(uint cmd)
	{
		unit_option->load_unit_cmd.load = cmd;
		return;
	}

	void UnitFlow::set_moduleunitcmd(uint cmd)
	{
		unit_option->module_unit_cmd.mode = cmd;
		return;
	}

	void UnitFlow::set_moduleunitparamsedgefields(vertexid_t adjvtx_id, edgeweight_t edge_weight)
	{
		unit_option->module_params.adjvtx_id = adjvtx_id;
		unit_option->module_params.edge_weight = edge_weight;
		return;
	}

	void UnitFlow::set_writebackunitcmd_writebackcurrentEB(uint cmd, uint addr, margin_t sub_block_margin)
	{
		unit_option->writeback_unit_cmd.writeback = cmd; //writeback EB
		unit_option->writeback_unit_cmd.addr = addr;
		unit_option->writeback_unit_cmd.subblockid = sub_block_margin.top / sub_block_height_;
		return;
	}

	void UnitFlow::setwritebackunitcmd_markasclustered(uint cmd, uint addr)
	{
		unit_option->writeback_unit_cmd.markasclustered = cmd; //mark as clustered
		unit_option->writeback_unit_cmd.addr = addr;
		return;
	}

	void UnitFlow::set_cal_unit_cmd(uint cmd)
	{
		unit_option->cal_unit_cmd.verdict = cmd;
		return;
	}

	//utility functions
	void UnitFlow::updatemarginandrolloverstatus(margin_t *work_block_margin, margin_t sub_block_margin)
	{
		if (work_block_margin->bottom == sub_block_margin.bottom)
		{ //roll over
			DLOG(INFO) << "**** reached end of sub-block, rolling over... ****"  ;
			work_block_margin->top = sub_block_margin.top;
			work_block_margin->bottom = sub_block_margin.top + (WORK_BLOCK_HEIGHT - 1);
			unit_option->module_params.rolledover = YES;
		}
		else
		{ //move forward
			work_block_margin->top += WORK_BLOCK_HEIGHT;
			work_block_margin->bottom += WORK_BLOCK_HEIGHT;
		}
		return;
	}

	bool UnitFlow::IsLastWorkBlockInSubBlock(margin_t work_block_margin, margin_t start_wblkmargin, margin_t sub_block_margin)
	{
		if (unit_option->module_params.rolledover == YES && work_block_margin.top == start_wblkmargin.top)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
} // namespace graphtinker