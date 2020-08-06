#include <string.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ctime>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	void Graphtinker::writeback_unit(
		module_params_t *moduleparams,
		writeback_unit_cmd_t writebackunitcmd,
		edge_t edge,
		edge_nt *edgeblock,
		vector<edge_nt> &_edge_block_array_m,
		vector<edge_nt> &_edge_block_array_o,
#ifdef EN_CRUMPLEINONDELETE
		vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
#endif
		tracker_t *_lvatracker,
		vertexid_t hvtx_id,
		margin_t first_wblkmargin,
		margin_t subblkmargin,
		uint geni,
		uint edgeupdatecmd
#ifdef EN_CRUMPLEINONDELETE
		,
		uint lastgenworkblockaddr, vector<supervertex_t> &svs
#ifdef EN_LLGDS
		,
		vector<ll_edgeblock_t> &_ll_edge_block_array
#endif
#endif
	)
	{
		clusterinfo_t clusterinfo;
		uint newpageindexpos = 0;

		/** append cluster info (if necessary) */
		if (writebackunitcmd.markasclustered == YES)
		{
			//initialize LVAentity row
			newpageindexpos = add_page(_lvatracker, _edge_block_array_o);

			//update cluster pointer
			moduleparams->clustered = YES;
			moduleparams->cptr = newpageindexpos;

			clusterinfo.flag = VALID;
			clusterinfo.data = newpageindexpos;

			edgeblock->clusterinfo = clusterinfo;
		}

#ifdef EN_CRUMPLEINONDELETE
		/** once we are in REGULAR UPDATE OPERATION, and a subblock becomes clustered, the child of that subblock MUST point to an entry in the supervertex vector. Whether or not that entry is shared with another (formerly) clustered subblock depends on where the subblock is located 
	CASE 1 : if the sublock is a first child, and the subblock lies in generation 1 => then a new supervertex should be created and should be updated.
	CASE 2 : if the subblock is a first child, and the subblock DOES NOT lie in generation 1 => then a supervertex should have been created by it's founding father. thus it should simply be updated.
	CASE 3 : if the sublock is NOT a first child, and the subblock lies in generation 1 => then a new supervertex should be created and should be updated.
	CASE 4 : if the subblock is NOT a first child, and the subblock DOES NOT lie in generation 1 => then a new supervertex should be created and should be updated.
	NB: this function should be before you write the cluster info to its subblock */
		if (edgeupdatecmd != DELETEEDGE)
		{
			if (writebackunitcmd.markasclustered == YES)
			{
				uint subblockid = subblkmargin.top / _sub_block_height;
				uint subblocksperpage = _sub_blocks_per_page;
				if ((subblockid == (subblocksperpage - 1)) && (geni == 1))
				{

					// super vertex doesn't exist. create one
					supervertex_t sv;
					sv.hvtx_ids.push_back(clusterinfo.data);
					sv.geni_ofparentsubblock = geni;
					svs.push_back(sv);

					// update cluster info
					clusterinfo.sv_ptr = (svs.size() - 1);

#ifdef EN_OTHERS
					cout << "case 1: founding father doesn't exist. create one : " << endl;
					cout << "svs.size() : " << svs.size() << " (writeback_unit)" << endl;
#endif
				}
				else if ((subblockid == (subblocksperpage - 1)) && (geni > 1))
				{

					// since it is a first child, this subblock should share the same supervertex entry as its parent subblock
					// update cluster info
					///***^ this is under testing ^***//
					uint svs_index = 0;
					if ((geni - 1) == 1)
					{ /// last edgeblock was in generation 1
						if (lastgenworkblockaddr >= _edge_block_array_m.size())
						{
							cout << "Graphtinker::writeback_unit : out-of-range34" << endl;
						}
						if (_edge_block_array_m[lastgenworkblockaddr].clusterinfo.flag != VALID)
						{
							cout << "Graphtinker::writeback_unit : addr out-of-range8" << endl;
						}
						svs_index = _edge_block_array_m[lastgenworkblockaddr].clusterinfo.sv_ptr;
					}
					else
					{
						if (lastgenworkblockaddr >= _edge_block_array_o.size())
						{
							cout << "Graphtinker::writeback_unit : out-of-range35" << endl;
						}
						if (_edge_block_array_o[lastgenworkblockaddr].clusterinfo.flag != VALID)
						{
							cout << "Graphtinker::writeback_unit : addr out-of-range82" << endl;
						}
						svs_index = _edge_block_array_o[lastgenworkblockaddr].clusterinfo.sv_ptr;
					}
					clusterinfo.sv_ptr = svs_index;

					// #ifdef EN_OTHERS
					if (svs[svs_index].hvtx_ids.empty())
					{
						cout << "bug should not be empty5  (writeback_unit)" << endl;
					}
					// #endif

					// append to supervertex's entry's vector
					svs[svs_index].hvtx_ids.push_back(clusterinfo.data);

#ifdef EN_OTHERS
					cout << "case 2: founding father already exist. lastgenworkblockaddr : " << lastgenworkblockaddr << endl;
					cout << "svs[svs_index].hvtx_ids.size() : " << svs[svs_index].hvtx_ids.size() << " (writeback_unit)" << endl;
					cout << "svs.size() : " << svs.size() << " (writeback_unit)" << endl;
#endif
				}
				else if ((subblockid != (subblocksperpage - 1)) && (geni == 1))
				{

					// super vertex doesn't exist. create one
					supervertex_t sv;
					sv.hvtx_ids.push_back(clusterinfo.data);
					sv.geni_ofparentsubblock = geni;
					svs.push_back(sv);

					// update cluster info
					clusterinfo.sv_ptr = (svs.size() - 1);

#ifdef EN_OTHERS
					cout << "case 3: founding father doesn't exist. create one : " << endl;
					cout << "svs.size() : " << svs.size() << " (writeback_unit)" << endl;
#endif
				}
				else if ((subblockid != (subblocksperpage - 1)) && (geni > 1))
				{

					// super vertex doesn't exist. create one
					supervertex_t sv;
					sv.hvtx_ids.push_back(clusterinfo.data);
					sv.geni_ofparentsubblock = geni;
					svs.push_back(sv);

					// update cluster info
					clusterinfo.sv_ptr = (svs.size() - 1);

#ifdef EN_OTHERS
					cout << "case 4: founding father doesn't exist. create one " << endl;
					cout << "svs.size() : " << svs.size() << " (writeback_unit)" << endl;
#endif
				}
				else
				{
					cout << "bug! should never be seen here (WritebackUnit7) " << endl;
				}
			}
		}
#endif

		/** write to DRAM */
		if (writebackunitcmd.writeback == YES)
		{
#ifdef EN_BUGCHECK
			if (geni == 1)
			{
				if (writebackunitcmd.addr >= _edge_block_array_m.size())
				{
					cout << "bug! : writebackunitcmd.addr out-of-range2 (WritebackUnit)" << endl;
				}
			}
			else
			{
				if (writebackunitcmd.addr >= _edge_block_array_o.size())
				{
					cout << "bug! : writebackunitcmd.addr out-of-range3 (WritebackUnit)" << endl;
				}
			}
#endif

			edgeblock->edgeinfo.flag = VALID;
			if (geni == 1)
			{
				_edge_block_array_m[writebackunitcmd.addr] = *edgeblock;
			}
			else
			{
				_edge_block_array_o[writebackunitcmd.addr] = *edgeblock;
			}

// update _ll_edge_block_array
#ifdef EN_CRUMPLEINONDELETE
			_ll_edge_block_array[moduleparams->ll_localbaseaddrptr_x].ll_edgeblock[moduleparams->ll_localaddrptr_x].which_gen_is_the_main_copy_located = geni; //***
#endif
		}

		/** write cluster info to all workblocks in given subblock */
		if (writebackunitcmd.markasclustered == YES)
		{
			if (geni == 1)
			{
				uint subblockbaseaddr = get_edgeblock_offset(hvtx_id) + (writebackunitcmd.subblockid * _work_blocks_per_subblock);
				for (uint id = 0; id < _work_blocks_per_subblock; id++)
				{
					_edge_block_array_m[(subblockbaseaddr + id)].clusterinfo = clusterinfo;
				}
			}
			else
			{
				uint subblockbaseaddr = get_edgeblock_offset(hvtx_id) + (writebackunitcmd.subblockid * _work_blocks_per_subblock);
				for (uint id = 0; id < _work_blocks_per_subblock; id++)
				{
					_edge_block_array_o[(subblockbaseaddr + id)].clusterinfo = clusterinfo;
				}
			}
		}

#ifdef EN_CRUMPLEINONDELETE
		if (writebackunitcmd.markasclustered == YES)
		{
			uint index = clusterinfo.data;
			if (index > edgeblock_parentinfo.size())
			{
				cout << "bug! out of range. writeback_unit" << endl;
			}
			if (edgeblock_parentinfo[index].flag != VALID)
			{
				edgeblock_parentinfo[index].gen_of_parent = geni;
				edgeblock_parentinfo[index].xvtx_id = hvtx_id; ///***^ [recently added, correct]
				edgeblock_parentinfo[index].subblockid = writebackunitcmd.subblockid;
				edgeblock_parentinfo[index].flag = VALID;
			}
		}
#endif
		return;
	}
} // namespace gt