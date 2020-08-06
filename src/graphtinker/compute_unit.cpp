#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	void Graphtinker::compute_unit(
		module_unit_cmd_t moduleunitcmd,
		module_params_t *moduleparams,
		insert_params_t *insertparams,
		insert_report_t *insertreport,
		find_params_t findparams,
		find_report_t *findreport,
		margin_t wblkmargin,
		margin_t subblkmargin,
		margin_t start_wblkmargin,
		bucket_t hadjvtx_id,
		edge_nt *edgeblock,
		edge_t *edge,
		vertexid_t xvtx_id,
		uint rolledover,
		uint edgeupdatecmd,
		uint geni)
	{
		if (moduleunitcmd.mode == FINDONLYMODE)
		{
			find_unit(
				moduleparams,
				findparams,
				findreport,
				wblkmargin,
				subblkmargin,
				hadjvtx_id,
				edgeblock,
				edgeupdatecmd);
		}
		else if (moduleunitcmd.mode == INSERTONLYMODE)
		{
			insert_unit(
				moduleparams,
				insertparams,
				insertreport,
				wblkmargin,
				subblkmargin,
				hadjvtx_id,
				edgeblock,
				edge,
				xvtx_id,
				rolledover,
				geni);
		}
		else
		{
			cout << "bug! : should never get here (compute unit) : moduleunitcmd.mode = " << moduleunitcmd.mode << endl;
		}
		return;
	}
} // namespace gt