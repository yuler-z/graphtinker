#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
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
		margin_t *wblkmargin,
		margin_t *subblkmargin,
		margin_t *start_wblkmargin,
		margin_t *first_wblkmargin,
		vertexid_t *xvtx_id,
		bucket_t *hadjvtx_id,
		edge_t edge,
		uint edgeupdatecmd,
		uint *tripiteration_lcs,
		uint *geni,
		uint *quitstatus,
		uint infiniti
#ifdef EN_CRUMPLEINONDELETE
		,
		uint *lastgenworkblockaddr
#endif
#ifdef EN_LLGDS
		,
		llgds_unit_cmd_t *llgdsunitcmd
#endif
#ifdef EN_CRUMPLEINONDELETE
		,
		delete_and_crumple_in_cmd_t *heba_deleteandcrumplein_cmd
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

#ifdef EN_CRUMPLEINONDELETE
			// keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
			*lastgenworkblockaddr = get_edgeblock_offset(*xvtx_id) + wblkmargin->top / WORK_BLOCK_HEIGHT;
#endif

			// update/initialize appropriate module globals
			*geni += 1;
			*xvtx_id = moduleparams->cptr;
			*tripiteration_lcs = 0;
			*hadjvtx_id = googlehash(moduleparams->xadjvtx_id, *geni);
			findwblkmargin(wblkmargin, *hadjvtx_id);
			*start_wblkmargin = *wblkmargin;
			findsubblkmargin(subblkmargin, *hadjvtx_id);

			// initialize appropriate fields of lcs units
			// ..2 funcs don't touch edge fields, to avoid overriding any swapping which may have occurred
			initialize_moduleunit_params2(moduleparams);
			initialize_loadunit(loadunitcmd);
			initialize_insertunit2(insertparams, insertreport);
			initialize_findunit2(findparams, findreport);
			initialize_writebackunit(writebackunitcmd);
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
			findwblkmargin(wblkmargin, *hadjvtx_id);		  //find margins
			*start_wblkmargin = *wblkmargin;
			*first_wblkmargin = *wblkmargin;
			findsubblkmargin(subblkmargin, *hadjvtx_id);

			//set module cmd
			moduleunitcmd->mode = INSERTONLYMODE;

			// initialize appropriate fields of lcs units
			// everything is initialized here
			initialize_moduleunit_params(moduleparams, edge.xadjvtx_id, edge.edgew);
			initialize_loadunit(loadunitcmd);
			initialize_insertunit(insertparams, insertreport, edge.xadjvtx_id, *hadjvtx_id, edge.edgew);
			initialize_findunit(findparams, findreport, edge.xadjvtx_id, *hadjvtx_id, edge.edgew);
			initialize_writebackunit(writebackunitcmd);
#ifdef EN_LLGDS
			initialize_llebaverdictcmd(llgdsunitcmd);
#endif
#ifdef EN_CRUMPLEINONDELETE
			init_deleteandcrumplein_verdictcmd(heba_deleteandcrumplein_cmd);
#endif
		}
		else
		{
			cout << "bug! : should never get here (IntervalUnit)" << endl;
		}
		return;
	}
} // namespace gt