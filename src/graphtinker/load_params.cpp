#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	void Graphtinker::load_params(
		module_params_t moduleparams,
		insert_params_t *insertparams,
		find_params_t *findparams,
		margin_t wblkmargin,
		bucket_t hadjvtx_id)
	{
		if (hadjvtx_id >= wblkmargin.top && moduleparams.rolledover == NO)
		{
			insertparams->isstartblk = 1;
			findparams->isstartblk = 1;
		}
		else
		{
			insertparams->isstartblk = 0;
			findparams->isstartblk = 0;
		}
		return;
	}
} // namespace gt