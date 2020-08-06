#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

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
	void Graphtinker::inference_unit(
		uint edgeupdatecmd,
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		find_report_t findreport,
		insert_params_t insertparams, //***
		insert_report_t insertreport,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t *wblkmargin,
		margin_t subblkmargin,
		margin_t start_wblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		//infer from search report
		searchreport_t searchreport;
		searchreport.searchstop = 0;
		searchreport.searchsuccessful = 0;
		if (moduleunitcmd->mode == FINDONLYMODE)
		{
			searchreport.searchstop = (findreport.entryfound == YES || findreport.foundemptybkt == YES || findreport.maxprobelengthreached == YES) ? 1 : 0;
		}
		if (moduleunitcmd->mode == FINDONLYMODE)
		{
			searchreport.searchsuccessful = (findreport.entryfound == YES) ? 1 : 0;
		}

		uint ONE = 1;
		uint islastworkblock = isthelastworkblock(*wblkmargin, start_wblkmargin, subblkmargin, *moduleparams);

#ifdef EN_OTHERS
		//cout<<"inference params of tinker = "<< tinkerid <<", batch = "<<batchid<<", chunk = "<<chunkid<<endl;
		cout << "searchreport.searchstop = " << searchreport.searchstop << endl;
		cout << "searchreport.searchsuccessful = " << searchreport.searchsuccessful << endl;
		cout << "insertreport.exittype = " << insertreport.exittype << endl;
		cout << "moduleunitcmd->mode = " << moduleunitcmd->mode << endl;
		cout << "islastworkblock = " << islastworkblock << endl
			 << endl;
#endif

		//|Mode|_|Searchstopped|_|searchsuccessfull_|InsertReport.exittype|_|islastworkblock?|
		//find-only mode
		if (moduleunitcmd->mode == FINDONLYMODE && searchreport.searchstop == YES && searchreport.searchsuccessful == NO && ONE == 1 && ONE == 1)
		{
			if (edgeupdatecmd == INSERTEDGE)
			{
				findonlymode_searchstopped_searchunsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					writebackunitcmd,
					intervalunitcmd,
					*wblkmargin,
					subblkmargin,
					xvtx_id
#ifdef EN_LLGDS
					,
					llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
					,
					heba_popoutpopin_cmd
#endif
				);
			}
#ifndef EN_CRUMPLEINONDELETE
			else if (edgeupdatecmd == DELETEEDGE)
			{
				findanddeletemode_searchstopped_searchunsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					writebackunitcmd,
					intervalunitcmd,
					*wblkmargin,
					subblkmargin,
					xvtx_id
#ifdef EN_LLGDS
					,
					llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
					,
					heba_popoutpopin_cmd
#endif
				);
			}
#endif
#ifdef EN_CRUMPLEINONDELETE
			else if (edgeupdatecmd == DELETEEDGE)
			{
				findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					writebackunitcmd,
					intervalunitcmd,
					*wblkmargin,
					subblkmargin,
					xvtx_id
#ifdef EN_LLGDS
					,
					llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
					,
					heba_popoutpopin_cmd
#endif
				);
			}
#endif
			else
			{
				cout << "bug! : should never be seen here (inference_unit34)" << endl;
			}
		}
		else if (moduleunitcmd->mode == FINDONLYMODE && searchreport.searchstop == YES && searchreport.searchsuccessful == YES && ONE == 1 && ONE == 1)
		{
			if (edgeupdatecmd == INSERTEDGE)
			{
				findonlymode_searchstopped_searchsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					writebackunitcmd,
					intervalunitcmd,
					*wblkmargin,
					subblkmargin,
					xvtx_id
#ifdef EN_LLGDS
					,
					llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
					,
					heba_popoutpopin_cmd
#endif
				);
			}
#ifndef EN_CRUMPLEINONDELETE
			else if (edgeupdatecmd == DELETEEDGE)
			{
				findanddeletemode_searchstopped_searchsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					findreport,
					writebackunitcmd,
					intervalunitcmd,
					*wblkmargin,
					subblkmargin,
					xvtx_id
#ifdef EN_LLGDS
					,
					llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
					,
					heba_popoutpopin_cmd
#endif
				);
			}
#endif
#ifdef EN_CRUMPLEINONDELETE
			else if (edgeupdatecmd == DELETEEDGE)
			{
				findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
					moduleunitcmd,
					moduleparams,
					loadunitcmd,
					findparams,
					findreport,
					writebackunitcmd,
					intervalunitcmd,
					*wblkmargin,
					subblkmargin,
					xvtx_id
#ifdef EN_LLGDS
					,
					llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
					,
					heba_popoutpopin_cmd
#endif
				);
			}
#endif
			else
			{
				cout << "bug! : should never be seen here (inference_unit35)" << endl;
			}
		}
		else if (moduleunitcmd->mode == FINDONLYMODE && searchreport.searchstop == NO && searchreport.searchsuccessful == NO && ONE == 1 && islastworkblock == NO)
		{
			findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				findparams,
				writebackunitcmd,
				intervalunitcmd,
				wblkmargin,
				subblkmargin,
				xvtx_id
#ifdef EN_LLGDS
				,
				llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
				,
				heba_popoutpopin_cmd
#endif
			);
		}
		else if (moduleunitcmd->mode == FINDONLYMODE && searchreport.searchstop == NO && searchreport.searchsuccessful == NO && ONE == 1 && islastworkblock == YES)
		{
			findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				findparams,
				writebackunitcmd,
				intervalunitcmd,
				*wblkmargin,
				subblkmargin,
				xvtx_id
#ifdef EN_LLGDS
				,
				llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
				,
				heba_popoutpopin_cmd
#endif
			);
			//insert-only mode
		}
		else if (moduleunitcmd->mode == INSERTONLYMODE && ONE == 1 && ONE == 1 && insertreport.exittype == PASSEDTHROUGHANDSWAPPEDATSOMEPOINT && islastworkblock == NO)
		{
			insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				wblkmargin,
				subblkmargin,
				xvtx_id
#ifdef EN_LLGDS
				,
				llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
				,
				heba_popoutpopin_cmd
#endif
			);
		}
		else if (moduleunitcmd->mode == INSERTONLYMODE && ONE == 1 && ONE == 1 && insertreport.exittype == PASSEDTHROUGHANDSWAPPEDATSOMEPOINT && islastworkblock == YES)
		{
			insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				*wblkmargin,
				subblkmargin,
				xvtx_id
#ifdef EN_LLGDS
				,
				llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
				,
				heba_popoutpopin_cmd
#endif
			);
		}
		else if (moduleunitcmd->mode == INSERTONLYMODE && ONE == 1 && ONE == 1 && insertreport.exittype == LOADEDINTOEMPTYBUCKET && ONE == 1)
		{
			insertonlymode_x_x_loadedintoemptybucket_x(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				*wblkmargin,
				subblkmargin,
				xvtx_id
#ifdef EN_LLGDS
				,
				llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
				,
				heba_popoutpopin_cmd
#endif
			);
		}
		else if (moduleunitcmd->mode == INSERTONLYMODE && ONE == 1 && ONE == 1 && insertreport.exittype == FOUNDANDUPDATEDITSELF && ONE == 1)
		{
			insertonlymode_x_x_foundandupdateditself_x(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				*wblkmargin,
				subblkmargin,
				xvtx_id
#ifdef EN_LLGDS
				,
				llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
				,
				heba_popoutpopin_cmd
#endif
			);
		}
		else if (moduleunitcmd->mode == INSERTONLYMODE && ONE == 1 && ONE == 1 && insertreport.exittype == PASSEDTHROUGH && islastworkblock == NO)
		{
			insertonlymode_x_x_passedthrough_notlastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				wblkmargin,
				subblkmargin,
				xvtx_id
#ifdef EN_LLGDS
				,
				llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
				,
				heba_popoutpopin_cmd
#endif
			);
		}
		else if (moduleunitcmd->mode == INSERTONLYMODE && ONE == 1 && ONE == 1 && insertreport.exittype == PASSEDTHROUGH && islastworkblock == YES)
		{
			insertonlymode_x_x_passedthrough_lastworkblock(
				moduleunitcmd,
				moduleparams,
				loadunitcmd,
				insertparams,
				writebackunitcmd,
				intervalunitcmd,
				*wblkmargin,
				subblkmargin,
				xvtx_id
#ifdef EN_LLGDS
				,
				llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
				,
				heba_popoutpopin_cmd
#endif
			);
		}
		else
		{
			cout << "bug! : should never get here! (inference unit)" << endl;
		}
		return;
	}

	//|Mode|_|Searchstopped|_|searchsuccessfull_|InsertReport.exittype|_|islastworkblock?|
	//Find-Only Mode
	void Graphtinker::findonlymode_searchstopped_searchunsuccessful_x_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERTONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight); //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		set_intervalunitcmd_continue_from_first_generation(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, INSERTCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
		return;
	}

	void Graphtinker::findonlymode_searchstopped_searchsuccessful_x_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams, //***
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);																										  //default mode
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, UPDATECMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
		return;
	}

	void Graphtinker::findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t *wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														//***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										//load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + wblkmargin->top / WORK_BLOCK_HEIGHT), subblkmargin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(
			intervalunitcmd,
			wblkmargin,
			subblkmargin,
			moduleparams);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
		return;
	}

	void Graphtinker::findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams, //***
		load_unit_cmd_t *loadunitcmd,
		find_params_t findparams,
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		if (moduleparams->clustered != YES)
		{
			setwritebackunitcmd_markasclustered(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)));
		} //***???
		set_intervalunitcmd_continue_in_lower_generation(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
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
		margin_t *wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERTONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + wblkmargin->top / WORK_BLOCK_HEIGHT), subblkmargin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(
			intervalunitcmd,
			wblkmargin,
			subblkmargin,
			moduleparams);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
		return;
	}

	void Graphtinker::insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERTONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		if (moduleparams->clustered != YES)
		{
			setwritebackunitcmd_markasclustered(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)));
		}
		set_intervalunitcmd_continue_in_lower_generation(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
		return;
	}

	void Graphtinker::insertonlymode_x_x_loadedintoemptybucket_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, UPDATEEDGEPTRSCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
		return;
	}

	void Graphtinker::insertonlymode_x_x_foundandupdateditself_x(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
		return;
	}

	void Graphtinker::insertonlymode_x_x_passedthrough_notlastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t *wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERTONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													//***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										//load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + wblkmargin->top / WORK_BLOCK_HEIGHT), subblkmargin); //writeback EB
		set_intervalunitcmd_continue_in_current_generation(
			intervalunitcmd,
			wblkmargin,
			subblkmargin,
			moduleparams);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
		return;
	}

	void Graphtinker::insertonlymode_x_x_passedthrough_lastworkblock(
		module_unit_cmd_t *moduleunitcmd,
		module_params_t *moduleparams,
		load_unit_cmd_t *loadunitcmd,
		insert_params_t insertparams, //***
		writeback_unit_cmd_t *writebackunitcmd,
		interval_unit_cmd_t *intervalunitcmd,
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, INSERTONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, insertparams.xadjvtx_id, insertparams.edge_weight);													 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //load next EB
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		if (moduleparams->clustered != YES)
		{
			setwritebackunitcmd_markasclustered(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)));
		}
		set_intervalunitcmd_continue_in_lower_generation(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif
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
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif

#ifdef EN_OTHERS
		cout << "bug! find not successfull (inference_unit22)" << endl; //***
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
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														  //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										  //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, DELETECMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_NOCMD;
#endif

#ifdef EN_OTHERS
		if (findreport.entrydeleted == YES)
		{
			cout << "entry founf but deleted (inference unit)" << endl;
		}
		else if (findreport.entrydeleted == NO)
		{
			cout << "bug! entry actually not found at all (inference unit)" << endl;
		}
		else
		{
			cout << "bug! should not get here (inference_unit55)" << endl;
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
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);														 //***
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);																										 //*** NO YES
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, NO, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin); //writeback EB
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, NOCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		heba_popoutpopin_cmd->verdict = DCI_JUSTQUITCMD;
#endif

#ifdef EN_OTHERS
		cout << "bug! find not successfull (inference_unit11)" << endl; //***
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
		margin_t wblkmargin,
		margin_t subblkmargin,
		vertexid_t xvtx_id
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_popoutpopin_cmd
#endif
	)
	{
		set_moduleunitcmd(moduleunitcmd, FINDONLYMODE);
		set_moduleunitparamsedgefields(moduleparams, findparams.xadjvtx_id, findparams.edge_weight);
		set_loadunitcmd_loadnextEB(loadunitcmd, YES);
		set_writebackunitcmd_writebackcurrentEB(writebackunitcmd, YES, (get_edgeblock_offset(xvtx_id) + (wblkmargin.top / WORK_BLOCK_HEIGHT)), subblkmargin);
		set_intervalunitcmd_quit_to_next_edge(intervalunitcmd);
#ifdef EN_LLGDS
		set_llgdsunitcmd(llgdsunitcmd, DELETEANDCRUMPLEINCMD);
#endif
#ifdef EN_CRUMPLEINONDELETE
		if (moduleparams->clustered == YES)
		{
			heba_popoutpopin_cmd->verdict = DCI_CRUMPLEINCMD;
		}
		else
		{
			heba_popoutpopin_cmd->verdict = DCI_JUSTQUITCMD;
		}
#endif

#ifdef EN_OTHERS
		if (findreport.entrydeleted == YES)
		{
			cout << "entry founf but deleted (inference unit)" << endl;
		}
		else if (findreport.entrydeleted == NO)
		{
			cout << "bug! entry actually not found at all (inference unit)" << endl;
		}
		else
		{
			cout << "bug! should not get here (inference_unit66)" << endl;
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

	void Graphtinker::set_intervalunitcmd_continue_in_current_generation(interval_unit_cmd_t *intervalunitcmd, margin_t *wblkmargin, margin_t subblkmargin, module_params_t *moduleparams)
	{
		intervalunitcmd->verdict = CONTINUE_IN_CURRENT_GENERATION;
		updatemarginandrolloverstatus(wblkmargin, subblkmargin, moduleparams);
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

	void Graphtinker::set_writebackunitcmd_writebackcurrentEB(writeback_unit_cmd_t *writebackunitcmd, uint cmd, uint addr, margin_t subblkmargin)
	{
		writebackunitcmd->writeback = cmd; //writeback EB
		writebackunitcmd->addr = addr;
		writebackunitcmd->subblockid = subblkmargin.top / _sub_block_height;
		return;
	}

	void Graphtinker::setwritebackunitcmd_markasclustered(writeback_unit_cmd_t *writebackunitcmd, uint cmd, uint addr)
	{
		writebackunitcmd->markasclustered = cmd; //mark as clustered
		writebackunitcmd->addr = addr;
		return;
	}

	void Graphtinker::set_llgdsunitcmd(llgds_unit_cmd_t *llgdsunitcmd, uint cmd)
	{
		llgdsunitcmd->verdict = cmd;
		return;
	}

	//utility functions
	void Graphtinker::updatemarginandrolloverstatus(margin_t *wblkmargin, margin_t subblkmargin, module_params_t *moduleparams)
	{
		if (wblkmargin->bottom == subblkmargin.bottom)
		{ //roll over
#ifdef cpuem_l1
			cout << "**** reached end of sub-block, rolling over... ****" << endl;
#endif
			wblkmargin->top = subblkmargin.top;
			wblkmargin->bottom = subblkmargin.top + (WORK_BLOCK_HEIGHT - 1);
			moduleparams->rolledover = YES;
		}
		else
		{ //move forward
			wblkmargin->top += WORK_BLOCK_HEIGHT;
			wblkmargin->bottom += WORK_BLOCK_HEIGHT;
		}
		return;
	}

	uint Graphtinker::isthelastworkblock(margin_t wblkmargin, margin_t start_wblkmargin, margin_t subblkmargin, module_params_t moduleparams)
	{
		bucket_t nexttopmargin = 0;
		bucket_t nextbottommargin = 0;
		uint nextrolloverstatus = 0;

		if (wblkmargin.bottom == subblkmargin.bottom)
		{ //roll over
			nexttopmargin = subblkmargin.top;
			nextbottommargin = subblkmargin.top + (WORK_BLOCK_HEIGHT - 1); //this is correct!
			nextrolloverstatus = YES;
		}
		else
		{ //move forward
			nexttopmargin = wblkmargin.top + WORK_BLOCK_HEIGHT;
			nextbottommargin = wblkmargin.bottom + WORK_BLOCK_HEIGHT;
			nextrolloverstatus = moduleparams.rolledover;
		}

		if (moduleparams.rolledover == YES && wblkmargin.top == start_wblkmargin.top)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
} // namespace gt