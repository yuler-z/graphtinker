#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

#ifdef EN_CAL
	#define	EN_CAL_SET_CAL_CMD(cal_unit_cmd, CMD) \
		set_cal_unit_cmd(cal_unit_cmd, CMD)
#else
	#define	EN_CAL_SET_CAL_CMD(cal_unit_cmd, CMD)
#endif

#ifdef EN_CRUMPLE_IN
	#define EN_CRUMPLE_IN_SET_VERDICT(cmd)	\
		heba_popoutpopin_cmd->verdict = cmd;
#else
	#define EN_CRUMPLE_IN_SET_VERDICT(cmd)
#endif
/**
1. all cmds are set:
	- moduleparams for this current lcs iteration is made up-to-date
	- moduleunitcmd for next lcs iteration is set 
	- loadunitcmd for the next lcs iteration is set
	- writebackunitcmd for this current lcs iteration is set
	- intervalunitcmd for this current lcs iteration is set
	after inference unit returns
2. margin information (workblock margin) for the next lcs iteration is set
*/
namespace gt
{


	/**
	 * 给出接下来的决断
	 * 选择退出循环
	 * 还是其他行为
	 */
	void Graphtinker::InferenceUnit(
		uint edge_update_cmd,
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		find_report_t findreport,
		insert_params_t insertparams, //***
		insert_report_t insertreport,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t *work_block_margin,
		margin_t sub_block_margin,
		margin_t start_wblkmargin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		//infer from search report
		search_report_t searchreport;
		searchreport.search_stop = false;
		searchreport.search_success = false;
		if (moduleunitcmd->mode == FIND_MODE)
		{
			searchreport.search_stop = findreport.is_found || findreport.is_empty || findreport.is_reach_max_prob_length;
		}
		if (moduleunitcmd->mode == FIND_MODE)
		{
			searchreport.search_success = findreport.is_found;
		}

		bool is_last_work_block = IsLastWorkBlockInSubBlock(*work_block_margin, start_wblkmargin, sub_block_margin, *moduleparams);

#ifdef EN_OTHERS
		//LOG(INFO) <<"inference params of tinker = "<< tinkerid <<", batch = "<<batchid<<", chunk = "<<chunkid ;
		LOG(INFO) << "searchreport.search_stop = " << searchreport.search_stop;
		LOG(INFO) << "searchreport.search_success = " << searchreport.search_success ;
		LOG(INFO) << "insertreport.exittype = " << insertreport.exittype ;
		LOG(INFO) << "moduleunitcmd->mode = " << moduleunitcmd->mode ;
		LOG(INFO) << "is_last_work_block = " << is_last_work_block ;
#endif

		//|Mode|_|Searchstopped|_|searchsuccessfull_|InsertReport.exittype|_|is_last_work_block?|
		//find-only mode
		if (moduleunitcmd->mode == FIND_MODE && searchreport.search_stop && !searchreport.search_success)
		{
			if (edge_update_cmd == INSERTEDGE)
			{
				findonlymode_searchstopped_searchunsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					writebackunitcmd,
					intervalunitcmd,
					*work_block_margin,
					sub_block_margin,
					xvtx_id,
					cal_unit_cmd ,
					heba_popoutpopin_cmd
				);
			}
#ifndef EN_CRUMPLE_IN
			else if (edge_update_cmd == DELETEEDGE)
			{
				findanddeletemode_searchstopped_searchunsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					writebackunitcmd,
					intervalunitcmd,
					*work_block_margin,
					sub_block_margin,
					xvtx_id ,
					cal_unit_cmd,
					heba_popoutpopin_cmd
				);
			}
#endif
#ifdef EN_CRUMPLE_IN
			else if (edge_update_cmd == DELETEEDGE)
			{
				findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					writebackunitcmd,
					intervalunitcmd,
					*work_block_margin,
					sub_block_margin,
					xvtx_id,
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
		else if (moduleunitcmd->mode == FIND_MODE && searchreport.search_stop && searchreport.search_success)
		{
			if (edge_update_cmd == INSERTEDGE)
			{
				findonlymode_searchstopped_searchsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					writebackunitcmd,
					intervalunitcmd,
					*work_block_margin,
					sub_block_margin,
					xvtx_id,
					cal_unit_cmd,
					heba_popoutpopin_cmd
				);
			}
#ifndef EN_CRUMPLE_IN
			else if (edge_update_cmd == DELETEEDGE)
			{
				findanddeletemode_searchstopped_searchsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					findreport,
					writebackunitcmd,
					intervalunitcmd,
					*work_block_margin,
					sub_block_margin,
					xvtx_id,
					cal_unit_cmd,
					heba_popoutpopin_cmd
				);
			}
#endif
#ifdef EN_CRUMPLE_IN
			else if (edge_update_cmd == DELETEEDGE)
			{
				findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					findreport,
					writebackunitcmd,
					intervalunitcmd,
					*work_block_margin,
					sub_block_margin,
					xvtx_id,
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
		else if (moduleunitcmd->mode == FIND_MODE && !searchreport.search_stop && searchreport.search_success == false && !is_last_work_block)
		{
			findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				findparams,
				writebackunitcmd,
				intervalunitcmd,
				work_block_margin,
				sub_block_margin,
				xvtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (moduleunitcmd->mode == FIND_MODE && !searchreport.search_stop && !searchreport.search_success && is_last_work_block)
		{
			findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				findparams,
				writebackunitcmd,
				intervalunitcmd,
				*work_block_margin,
				sub_block_margin,
				xvtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
			//insert-only mode
		}
		else if (moduleunitcmd->mode == INSERT_MODE && insertreport.exittype == PASSEDTHROUGHANDSWAPPEDATSOMEPOINT && !is_last_work_block)
		{
			insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				work_block_margin,
				sub_block_margin,
				xvtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (moduleunitcmd->mode == INSERT_MODE && insertreport.exittype == PASSEDTHROUGHANDSWAPPEDATSOMEPOINT && is_last_work_block)
		{
			insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				*work_block_margin,
				sub_block_margin,
				xvtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (moduleunitcmd->mode == INSERT_MODE && insertreport.exittype == LOADEDINTOEMPTYBUCKET)
		{
			insertonlymode_x_x_loadedintoemptybucket_x(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				*work_block_margin,
				sub_block_margin,
				xvtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (moduleunitcmd->mode == INSERT_MODE && insertreport.exittype == FOUNDANDUPDATEDITSELF)
		{
			insertonlymode_x_x_foundandupdateditself_x(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				*work_block_margin,
				sub_block_margin,
				xvtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (moduleunitcmd->mode == INSERT_MODE && insertreport.exittype == PASSEDTHROUGH && !is_last_work_block)
		{
			insertonlymode_x_x_passedthrough_notlastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				work_block_margin,
				sub_block_margin,
				xvtx_id,
				cal_unit_cmd,
				heba_popoutpopin_cmd
			);
		}
		else if (moduleunitcmd->mode == INSERT_MODE && insertreport.exittype == PASSEDTHROUGH && is_last_work_block)
		{
			insertonlymode_x_x_passedthrough_lastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				*work_block_margin,
				sub_block_margin,
				xvtx_id,
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
	void Graphtinker::findonlymode_searchstopped_searchunsuccessful_x_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERT_MODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight); //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_continue_from_first_generation(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, INSERTCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void Graphtinker::findonlymode_searchstopped_searchsuccessful_x_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams, //***
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);																										  //default mode
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, UPDATECMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void Graphtinker::findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t *work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														//***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										//load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + work_block_margin->top / WORK_BLOCK_HEIGHT), sub_block_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(
			intervalunitcmd,
			work_block_margin,
			sub_block_margin,
			moduleparams);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void Graphtinker::findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams, //***
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id ,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		if (moduleparams->clustered != YES)
		{
			setwritebackunitcmd_markasclustered(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)));
		} //***???
		set_intervalunitcmd_continue_in_lower_generation(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	//Insert-Only Mode
	void Graphtinker::insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams, //***
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t *work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERT_MODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + work_block_margin->top / WORK_BLOCK_HEIGHT), sub_block_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(
			intervalunitcmd,
			work_block_margin,
			sub_block_margin,
			moduleparams);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void Graphtinker::insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERT_MODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		if (moduleparams->clustered != YES)
		{
			setwritebackunitcmd_markasclustered(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)));
		}
		set_intervalunitcmd_continue_in_lower_generation(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void Graphtinker::insertonlymode_x_x_loadedintoemptybucket_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, UPDATEEDGEPTRSCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void Graphtinker::insertonlymode_x_x_foundandupdateditself_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void Graphtinker::insertonlymode_x_x_passedthrough_notlastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t *work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERT_MODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													//***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										//load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + work_block_margin->top / WORK_BLOCK_HEIGHT), sub_block_margin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(
			intervalunitcmd,
			work_block_margin,
			sub_block_margin,
			moduleparams);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	void Graphtinker::insertonlymode_x_x_passedthrough_lastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERT_MODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		if (moduleparams->clustered != YES)
		{
			setwritebackunitcmd_markasclustered(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)));
		}
		set_intervalunitcmd_continue_in_lower_generation(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
		return;
	}

	//Find-And-Delete Mode
	void Graphtinker::findanddeletemode_searchstopped_searchunsuccessful_x_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);
#ifdef EN_OTHERS
		LOG(ERROR) << " find not successfull (inference_unit22)"  ; //***
#endif
		return;
	}

	void Graphtinker::findanddeletemode_searchstopped_searchsuccessful_x_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		find_report_t findreport,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, DELETECMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_NOCMD);

#ifdef EN_OTHERS
		if (findreport.is_deleted == YES)
		{
			LOG(INFO) << "entry founf but deleted (inference unit)"  ;
		}
		else if (findreport.is_deleted == NO)
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
	void Graphtinker::findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, NOCMD);
		EN_CRUMPLE_IN_SET_VERDICT(DCI_JUSTQUITCMD);

#ifdef EN_OTHERS
		LOG(ERROR) << " find not successfull (inference_unit11)"  ; //***
#endif
		return;
	}

	void Graphtinker::findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		find_report_t findreport,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		vertexid_t xvtx_id,
		cal_unit_cmd_t *cal_unit_cmd,
		crumple_in_cmd_t *heba_popoutpopin_cmd
	)
	{
		set_moduleunitcmd(moduleunitcmd, FIND_MODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (work_block_margin.top / WORK_BLOCK_HEIGHT)), sub_block_margin);
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
		EN_CAL_SET_CAL_CMD(cal_unit_cmd, DELETEANDCRUMPLEINCMD);
		if (moduleparams->clustered == YES)
		{
			EN_CRUMPLE_IN_SET_VERDICT(DCI_CRUMPLEINCMD);
		}
		else
		{
			EN_CRUMPLE_IN_SET_VERDICT(DCI_JUSTQUITCMD);
		}

#ifdef EN_OTHERS
		if (findreport.is_deleted == YES)
		{
			LOG(INFO) << "entry founf but deleted (inference unit)"  ;
		}
		else if (findreport.is_deleted == NO)
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
	void Graphtinker::set_intervalunitcmd_continue_from_first_generation(interval_unit_cmd_t *intervalunitcmd)
	{
		intervalunitcmd->verdict = CONTINUE_FROM_FIRST_GENERATION;
		return;
	}

	void Graphtinker::set_intervalunitcmd_continue_in_lower_generation(interval_unit_cmd_t *intervalunitcmd)
	{
		intervalunitcmd->verdict = CONTINUE_IN_LOWER_GENERATION;
		return;
	}

	void Graphtinker::set_intervalunitcmd_continue_in_current_generation(interval_unit_cmd_t *intervalunitcmd, margin_t *work_block_margin, margin_t sub_block_margin, module_params_t *moduleparams)
	{
		intervalunitcmd->verdict = CONTINUE_IN_CURRENT_GENERATION;
		updatemarginandrolloverstatus(work_block_margin, sub_block_margin, moduleparams);
		return;
	}

	void Graphtinker::set_intervalunitcmd_quit_to_next_edge(interval_unit_cmd_t *intervalunitcmd)
	{
		intervalunitcmd->verdict = QUIT_TO_NEXT_EDGE;
		return;
	}

	void Graphtinker::set_loadunitcmd_loadnextEB(load_unit_cmd_t *loadunitcmd, uint cmd)
	{
		loadunitcmd->load = cmd;
		return;
	}

	void Graphtinker::set_moduleunitcmd(module_unit_cmd_t *moduleunitcmd, uint cmd)
	{
		moduleunitcmd->mode = cmd;
		return;
	}

	void Graphtinker::set_moduleunitparamsedgefields(module_params_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight)
	{
		moduleparams->xadjvtx_id = xadjvtx_id;
		moduleparams->edge_weight = edge_weight;
		return;
	}

	void Graphtinker::set_writebackunitcmd_writebackcurrentEB(writeback_unit_cmd_t *writebackunitcmd, uint cmd, uint addr, margin_t sub_block_margin)
	{
		writebackunitcmd->writeback = cmd; //writeback EB
		writebackunitcmd->addr = addr;
		writebackunitcmd->subblockid = sub_block_margin.top / sub_block_height_;
		return;
	}

	void Graphtinker::setwritebackunitcmd_markasclustered(writeback_unit_cmd_t *writebackunitcmd, uint cmd, uint addr)
	{
		writebackunitcmd->markasclustered = cmd; //mark as clustered
		writebackunitcmd->addr = addr;
		return;
	}

	void Graphtinker::set_cal_unit_cmd(cal_unit_cmd_t *cal_unit_cmd, uint cmd)
	{
		cal_unit_cmd->verdict = cmd;
		return;
	}

	//utility functions
	void Graphtinker::updatemarginandrolloverstatus(margin_t *work_block_margin, margin_t sub_block_margin, module_params_t *moduleparams)
	{
		if (work_block_margin->bottom == sub_block_margin.bottom)
		{ //roll over
			DLOG(INFO) << "**** reached end of sub-block, rolling over... ****"  ;
			work_block_margin->top = sub_block_margin.top;
			work_block_margin->bottom = sub_block_margin.top + (WORK_BLOCK_HEIGHT - 1);
			moduleparams->rolledover = YES;
		}
		else
		{ //move forward
			work_block_margin->top += WORK_BLOCK_HEIGHT;
			work_block_margin->bottom += WORK_BLOCK_HEIGHT;
		}
		return;
	}

	bool Graphtinker::IsLastWorkBlockInSubBlock(margin_t work_block_margin, margin_t start_wblkmargin, margin_t sub_block_margin, module_params_t moduleparams)
	{
		/* 
		bucket_t nexttopmargin = 0;
		bucket_t nextbottommargin = 0;
		uint nextrolloverstatus = 0;

		if (work_block_margin.bottom == sub_block_margin.bottom)
		{ //roll over
			nexttopmargin = sub_block_margin.top;
			nextbottommargin = sub_block_margin.top + (WORK_BLOCK_HEIGHT - 1); //this is correct!
			nextrolloverstatus = YES;
		}
		else
		{ //move forward
			nexttopmargin = work_block_margin.top + WORK_BLOCK_HEIGHT;
			nextbottommargin = work_block_margin.bottom + WORK_BLOCK_HEIGHT;
			nextrolloverstatus = moduleparams.rolledover;
		}
		*/

		if (moduleparams.rolledover == YES && work_block_margin.top == start_wblkmargin.top)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
} // namespace gt