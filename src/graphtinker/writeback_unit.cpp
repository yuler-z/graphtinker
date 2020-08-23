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
	void Graphtinker::WritebackUnit(
		module_params_t *moduleparams,
		writeback_unit_cmd_t writebackunitcmd,
		edge_t edge,
		work_block_t *work_block,
		vector<work_block_t> &edge_block_array_m_,
		vector<work_block_t> &edge_block_array_o_,
#ifdef EN_CRUMPLE_IN
		vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
#endif
		tracker_t *lvatracker_,
		vertexid_t hvtx_id,
		margin_t first_wblkmargin,
		margin_t sub_block_margin,
		uint geni,
		uint edge_update_cmd
#ifdef EN_CRUMPLE_IN
		,
		uint lastgenworkblockaddr, vector<supervertex_t> &svs
#ifdef EN_CAL
		,
		vector<ll_edgeblock_t> &ll_edge_block_array_
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
			newpageindexpos = add_page(lvatracker_, edge_block_array_o_);

			//update cluster pointer
			moduleparams->clustered = YES;
			moduleparams->cptr = newpageindexpos;

			clusterinfo.flag = VALID;
			clusterinfo.data = newpageindexpos;

			work_block->clusterinfo = clusterinfo;
		}

#ifdef EN_CRUMPLE_IN
		/** once we are in REGULAR UPDATE OPERATION, and a subblock becomes clustered, the child of that subblock MUST point to an entry in the supervertex vector. Whether or not that entry is shared with another (formerly) clustered subblock depends on where the subblock is located 
	CASE 1 : if the sublock is a first child, and the subblock lies in generation 1 => then a new supervertex should be created and should be updated.
	CASE 2 : if the subblock is a first child, and the subblock DOES NOT lie in generation 1 => then a supervertex should have been created by it's founding father. thus it should simply be updated.
	CASE 3 : if the sublock is NOT a first child, and the subblock lies in generation 1 => then a new supervertex should be created and should be updated.
	CASE 4 : if the subblock is NOT a first child, and the subblock DOES NOT lie in generation 1 => then a new supervertex should be created and should be updated.
	NB: this function should be before you write the cluster info to its subblock */
		if (edge_update_cmd != DELETEEDGE)
		{
			if (writebackunitcmd.markasclustered == YES)
			{
				uint subblockid = sub_block_margin.top / sub_block_height_;
				uint subblocksperpage = sub_blocks_per_page_;
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
					LOG(ERROR) << "case 1: founding father doesn't exist. create one : "  ;
					LOG(ERROR) << "svs.size() : " << svs.size() << " (WritebackUnit)"  ;
#endif
				}
				else if ((subblockid == (subblocksperpage - 1)) && (geni > 1))
				{

					// since it is a first child, this subblock should share the same supervertex entry as its parent subblock
					// update cluster info
					///***^ this is under testing ^***//
					uint svs_index = 0;
					if ((geni - 1) == 1)
					{ /// last work_block was in generation 1
						if (lastgenworkblockaddr >= edge_block_array_m_.size())
						{
							LOG(ERROR) << "Graphtinker::WritebackUnit : out-of-range34"  ;
						}
						if (edge_block_array_m_[lastgenworkblockaddr].clusterinfo.flag != VALID)
						{
							LOG(ERROR) << "Graphtinker::WritebackUnit : addr out-of-range8"  ;
						}
						svs_index = edge_block_array_m_[lastgenworkblockaddr].clusterinfo.sv_ptr;
					}
					else
					{
						if (lastgenworkblockaddr >= edge_block_array_o_.size())
						{
							LOG(ERROR) << "Graphtinker::WritebackUnit : out-of-range35"  ;
						}
						if (edge_block_array_o_[lastgenworkblockaddr].clusterinfo.flag != VALID)
						{
							LOG(ERROR) << "Graphtinker::WritebackUnit : addr out-of-range82"  ;
						}
						svs_index = edge_block_array_o_[lastgenworkblockaddr].clusterinfo.sv_ptr;
					}
					clusterinfo.sv_ptr = svs_index;

					// #ifdef EN_OTHERS
					if (svs[svs_index].hvtx_ids.empty())
					{
						LOG(ERROR) << "bug should not be empty5  (WritebackUnit)"  ;
					}
					// #endif

					// append to supervertex's entry's vector
					svs[svs_index].hvtx_ids.push_back(clusterinfo.data);

#ifdef EN_OTHERS
					LOG(ERROR) << "case 2: founding father already exist. lastgenworkblockaddr : " << lastgenworkblockaddr  ;
					LOG(ERROR) << "svs[svs_index].hvtx_ids.size() : " << svs[svs_index].hvtx_ids.size() << " (WritebackUnit)"  ;
					LOG(ERROR) << "svs.size() : " << svs.size() << " (WritebackUnit)"  ;
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
					LOG(ERROR) << "case 3: founding father doesn't exist. create one : "  ;
					LOG(ERROR) << "svs.size() : " << svs.size() << " (WritebackUnit)"  ;
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
					LOG(ERROR) << "case 4: founding father doesn't exist. create one "  ;
					LOG(ERROR) << "svs.size() : " << svs.size() << " (WritebackUnit)"  ;
#endif
				}
				else
				{
					LOG(ERROR) << " should never be seen here (WritebackUnit7) "  ;
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
				if (writebackunitcmd.addr >= edge_block_array_m_.size())
				{
					LOG(ERROR) << " writebackunitcmd.addr out-of-range2 (WritebackUnit)"  ;
				}
			}
			else
			{
				if (writebackunitcmd.addr >= edge_block_array_o_.size())
				{
					LOG(ERROR) << " writebackunitcmd.addr out-of-range3 (WritebackUnit)"  ;
				}
			}
#endif

			work_block->edgeinfo.flag = VALID;
			if (geni == 1)
			{
				edge_block_array_m_[writebackunitcmd.addr] = *work_block;
			}
			else
			{
				edge_block_array_o_[writebackunitcmd.addr] = *work_block;
			}

// update ll_edge_block_array_
#ifdef EN_CRUMPLE_IN
			ll_edge_block_array_[moduleparams->ll_localbaseaddrptr_x].ll_edgeblock[moduleparams->ll_localaddrptr_x].which_gen_is_the_main_copy_located = geni; //***
#endif
		}

		/** write cluster info to all workblocks in given subblock */
		if (writebackunitcmd.markasclustered == YES)
		{
			if (geni == 1)
			{
				uint subblockbaseaddr = get_edgeblock_offset(hvtx_id) + (writebackunitcmd.subblockid * work_blocks_per_subblock_);
				for (uint id = 0; id < work_blocks_per_subblock_; id++)
				{
					edge_block_array_m_[(subblockbaseaddr + id)].clusterinfo = clusterinfo;
				}
			}
			else
			{
				uint subblockbaseaddr = get_edgeblock_offset(hvtx_id) + (writebackunitcmd.subblockid * work_blocks_per_subblock_);
				for (uint id = 0; id < work_blocks_per_subblock_; id++)
				{
					edge_block_array_o_[(subblockbaseaddr + id)].clusterinfo = clusterinfo;
				}
			}
		}

#ifdef EN_CRUMPLE_IN
		if (writebackunitcmd.markasclustered == YES)
		{
			uint index = clusterinfo.data;
			if (index > edgeblock_parentinfo.size())
			{
				LOG(ERROR) << " out of range. WritebackUnit"  ;
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