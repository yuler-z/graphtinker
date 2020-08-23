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
#include "graphtinker/graphtinker.h"
using namespace std;
namespace gt
{
#ifdef EN_CAL
	void Graphtinker::ll_insert(
		edge_t edge,
		module_params_t *moduleparams,
		insert_params_t *insertparams,
		vector<ll_edgeblock_t> &ll_edge_block_array_,
		vector<ll_logicalvertexentity_t> &ll_lva_,
		ll_eba_tracker_t *ll_eba_tracker_,
		uint geni)
	{
		uint tailid = 0;
		uint offsetaddr = 0;
		uint localoffsetaddr = 0;
		uint lvaindex = 0;
		uint ONE = 1;

		/// calculate lvaindex
		lvaindex = edge.xvtx_id / LVACOARSENESSWIDTH;

		/// resize ll_lva_ if full
		if (lvaindex >= ll_lva_.size())
		{
			ll_lva_.resize((ll_lva_.size() + ll_lva_expansion_addition_height_));
			LOG(INFO) << "ll_insert : resizing ll_lva_... "  ;
		}
#ifdef EN_BUGCHECK
		if (lvaindex >= ll_lva_.size())
		{
			LOG(INFO) << "Graphtinker::ll_insert : out-of-range 1 lvaindex = " << lvaindex << ", ll_lva_.size() = " << ll_lva_.size()  ;
		}
#endif

		/// retrieve ll_lva_ entity
		ll_logicalvertexentity_t entity = ll_lva_[lvaindex];

		/// resize ll_edge_block_array_ if full
		if (((entity.lastlocalbaseaddr) >= ll_edge_block_array_.size()) || (ll_eba_tracker_->ptraddr >= ll_edge_block_array_.size()))
		{
			LOG(INFO) << "ll_insert : resizing ll_edge_block_array_..."  ;
			ll_edge_block_array_.resize((ll_edge_block_array_.size() + ll_eba_expansion_addition_height_));
		}

		/// check if we need to initialize first
		if (entity.flag != VALID)
		{
			//never used before, initialize first before use
			// create
			entity.baseaddr = ll_eba_tracker_->ptraddr;
			entity.lastlocalbaseaddr = ll_eba_tracker_->ptraddr;
			entity.lastlocaladdr = 0;
			entity.totaledgecount = 0;
			entity.flag = VALID;

			// insert
			uint currentlocalbaseaddr = ll_eba_tracker_->ptraddr;
			ll_edge_block_array_[currentlocalbaseaddr].ll_edgeblock[0] = edge;
			ll_edge_block_array_[currentlocalbaseaddr].metadata.edgecount = 1;
			ll_edge_block_array_[currentlocalbaseaddr].metadata.nextcptr = NAv;						 // next edgeblock location
			ll_edge_block_array_[currentlocalbaseaddr].metadata.currcptr = ll_eba_tracker_->ptraddr; // this current edgeblock location
			ll_edge_block_array_[currentlocalbaseaddr].metadata.prevcptr = NAv;

#ifdef EN_BUGCHECK
			if ((ll_eba_tracker_->ptraddr) >= ll_edge_block_array_.size())
			{
				LOG(ERROR) << " out-of-range 31 (GraphTinkerLL)"  ;
			}
#endif

			// increment tracker
			ll_eba_tracker_->ptraddr += 1; // update
		}

		/// insert edge
		else if (entity.flag == VALID)
		{
			if ((entity.totaledgecount % LLEDGEBLOCKSIZE) == 0)
			{
				/// full, allocate new edgeblock and insert in it

				/// update
				uint currentlocalbaseaddr = entity.lastlocalbaseaddr;
				if (currentlocalbaseaddr >= ll_edge_block_array_.size())
				{
					LOG(ERROR) << "Graphtinker::ll_insert : out-of-range 22, entity.lastlocalbaseaddr = " << entity.lastlocalbaseaddr << ", ll_edge_block_array_.size() = " << ll_edge_block_array_.size()  ;
				}
				ll_edge_block_array_[currentlocalbaseaddr].metadata.nextcptr = ll_eba_tracker_->ptraddr; // next edgeblock location

				/// update
				entity.lastlocalbaseaddr = ll_eba_tracker_->ptraddr;
				entity.lastlocaladdr = 0;

				/// insert
				uint nextlocalbaseaddr = ll_eba_tracker_->ptraddr;
#ifdef EN_BUGCHECK
				if ((ll_eba_tracker_->ptraddr) >= ll_edge_block_array_.size())
				{
					LOG(ERROR) << " out-of-range 3 (GraphTinkerLL)"  ;
				}
#endif
				ll_edge_block_array_[nextlocalbaseaddr].ll_edgeblock[entity.lastlocaladdr] = edge;
				ll_edge_block_array_[nextlocalbaseaddr].metadata.edgecount = 1;
				ll_edge_block_array_[nextlocalbaseaddr].metadata.nextcptr = NAv;					  // unknown next edgeblock location
				ll_edge_block_array_[nextlocalbaseaddr].metadata.currcptr = ll_eba_tracker_->ptraddr; // current edgeblock location
				ll_edge_block_array_[nextlocalbaseaddr].metadata.prevcptr = ll_edge_block_array_[currentlocalbaseaddr].metadata.currcptr;

				/// increment
				ll_eba_tracker_->ptraddr += 1; // update
			}
			else
			{
				/// not full, insert edge
				ll_edge_block_array_[entity.lastlocalbaseaddr].ll_edgeblock[entity.lastlocaladdr] = edge;
				ll_edge_block_array_[entity.lastlocalbaseaddr].metadata.edgecount += 1;
			}
		}

/// append entity.lastlocalbaseaddr & entity.lastlocaladdr to insertparams
/// this is how every edge in edge_block_array gets its pointer to ll_edge_block_array_
#ifdef EN_CAL
		moduleparams->ll_localbaseaddrptr_x = entity.lastlocalbaseaddr;
		moduleparams->ll_localaddrptr_x = entity.lastlocaladdr;
#endif

		/// common updates
		entity.totaledgecount += 1;
		entity.lastlocaladdr += 1;

/// submit changes
#ifdef EN_BUGCHECK
		if (lvaindex >= ll_lva_.size())
		{
			LOG(ERROR) << " out-of-range 4 (GraphTinkerLL). lvaindex = " << lvaindex << ", ll_lva_.size() = " << ll_lva_.size()  ;
		}
#endif
		ll_lva_[lvaindex] = entity;
		return;
	}

	void Graphtinker::ll_update(
		edge_t edge,
		vertexid_t ll_localbaseaddrptr,
		vertexid_t ll_localaddrptr,
		vector<ll_edgeblock_t> &ll_edge_block_array_)
	{
		ll_edge_block_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].edgew += 1;
		return;
	}

	void Graphtinker::ll_delete(
		edge_t edge,
		vertexid_t ll_localbaseaddrptr,
		vertexid_t ll_localaddrptr,
		vector<ll_edgeblock_t> &ll_edge_block_array_)
	{
		ll_edge_block_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag = INVALID;
		return;
	}

	void Graphtinker::ll_deleteandcrumplein(
		edge_t edge,
		vertexid_t ll_localbaseaddrptr,
		vertexid_t ll_localaddrptr,
		vector<ll_edgeblock_t> &ll_edge_block_array_,
		vector<ll_logicalvertexentity_t> &ll_lva_,
		ll_eba_tracker_t *ll_eba_tracker_,
		vector<work_block_t> &edge_block_array_m_,
		vector<work_block_t> &edge_block_array_o_,
		uint geni)
	{
		// find partiton
		uint work_blocks_per_subblock_ = work_blocks_per_subblock_;
		edge_t nedge;

		// delete edge
		ll_edge_block_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag = INVALID;
		// return;

		// pop-out from end and pop-in to hole where edge was deleted
		uint lvaindex = edge.xvtx_id / LVACOARSENESSWIDTH;
		if (lvaindex >= ll_lva_.size())
		{
			LOG(ERROR) << " addr out-of-range11 (gds_ll) "  ;
		}
		ll_logicalvertexentity_t entity = ll_lva_[lvaindex];
		if (entity.flag != VALID)
		{
			LOG(ERROR) << " something wrong4 (gds_ll) "  ;
		}
		if (entity.lastlocalbaseaddr == NAv)
		{
			return;
		} //****

		uint localbaseaddr = entity.lastlocalbaseaddr;
		uint localaddr = entity.lastlocaladdr;
		// return;

		if (localaddr > 0)
		{
			// not empty, fetch from edgeblock

			uint localaddrofvalidedge = localaddr - 1;

			if ((localbaseaddr == ll_localbaseaddrptr) && (localaddrofvalidedge == ll_localaddrptr))
			{
				// the last was the hole

				// update entity <only these fields>
				entity.lastlocaladdr = entity.lastlocaladdr - 1;
				entity.totaledgecount = entity.totaledgecount - 1;

				// update
				ll_lva_[lvaindex] = entity;

				// get out
				return;
			}
			else
			{

				// update entity <only these fields>
				entity.lastlocaladdr = entity.lastlocaladdr - 1;
				entity.totaledgecount = entity.totaledgecount - 1;

				// pop-out, pop-in
				if (localbaseaddr >= ll_edge_block_array_.size())
				{
					LOG(ERROR) << " addr out-of-range7 (gds_ll) "  ;
				}
				if (localaddrofvalidedge >= LLEDGEBLOCKSIZE)
				{
					LOG(ERROR) << " addr out-of-range88. entity.lastlocaladdr : " << entity.lastlocaladdr << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
				}
				if (ll_localbaseaddrptr >= ll_edge_block_array_.size())
				{
					LOG(ERROR) << " addr out-of-range9 (gds_ll) "  ;
				}
				if (ll_localaddrptr >= LLEDGEBLOCKSIZE)
				{
					LOG(ERROR) << " addr out-of-range10. ll_localaddrptr : " << ll_localaddrptr << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
				}

				// pop-out
				nedge = ll_edge_block_array_[localbaseaddr].ll_edgeblock[localaddrofvalidedge];

				// invalidate //***
				if (nedge.flag != VALID)
				{
					LOG(ERROR) << " something wrong551. localbaseaddr : " << localbaseaddr << ", localaddrofvalidedge : " << localaddrofvalidedge << ", ll_localbaseaddrptr : " << ll_localbaseaddrptr << ", ll_localaddrptr : " << ll_localaddrptr << " (gds_ll) "  ;
				}

				ll_edge_block_array_[localbaseaddr].ll_edgeblock[localaddrofvalidedge].flag = INVALID;

				// pop in
				if (ll_edge_block_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag == VALID)
				{
					LOG(ERROR) << " something wrong457. ll_localbaseaddrptr : " << ll_localbaseaddrptr << ", ll_localaddrptr : " << ll_localaddrptr << " (gds_ll) "  ;
				}
				ll_edge_block_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr] = nedge;

				// redirect pointer (heba -> ll)
				uint workblockaddr = get_edgeblock_offset(nedge.heba_hvtx_id) + nedge.heba_workblockid;
				if (nedge.which_gen_is_the_main_copy_located == 1)
				{
					if (workblockaddr >= edge_block_array_m_.size())
					{
						LOG(ERROR) << "Graphtinker::ll_functions : addr out-of-range6 : nedge.which_gen_is_the_main_copy_located : " << nedge.which_gen_is_the_main_copy_located << ", workblockaddr : " << workblockaddr  ;
					}
					edge_block_array_m_[workblockaddr].edges[nedge.heba_loffset].ll_localbaseaddrptr = ll_localbaseaddrptr;
					edge_block_array_m_[workblockaddr].edges[nedge.heba_loffset].ll_localaddrptr = ll_localaddrptr;
				}
				else
				{
					if (workblockaddr >= edge_block_array_o_.size())
					{
						LOG(ERROR) << "Graphtinker::ll_functions : addr out-of-range7 : nedge.which_gen_is_the_main_copy_located : " << nedge.which_gen_is_the_main_copy_located << ", workblockaddr : " << workblockaddr << ", nedge.heba_hvtx_id : " << nedge.heba_hvtx_id << ", edge_block_array_o_.size() : " << edge_block_array_o_.size()  ;
					}
					edge_block_array_o_[workblockaddr].edges[nedge.heba_loffset].ll_localbaseaddrptr = ll_localbaseaddrptr;
					edge_block_array_o_[workblockaddr].edges[nedge.heba_loffset].ll_localaddrptr = ll_localaddrptr;
				}

				// update
				ll_lva_[lvaindex] = entity;
				return;
			}
		}
		else if (localaddr == 0)
		{
			// empty, free this edgeblock and fetch from its predecessor new edgeblock

			uint localaddrofvalidedge = (LLEDGEBLOCKSIZE - 1);

			if (entity.lastlocalbaseaddr >= ll_edge_block_array_.size())
			{
				LOG(ERROR) << " addr out-of-range67 (gds_ll) "  ;
			}
			uint prevcptr = ll_edge_block_array_[entity.lastlocalbaseaddr].metadata.prevcptr; // next edgeblock location
			if (prevcptr != NAv)
			{
				if (prevcptr >= ll_edge_block_array_.size())
				{
					LOG(ERROR) << " addr out-of-range69. prevcptr : " << prevcptr << ", ll_edge_block_array_.size() : " << ll_edge_block_array_.size() << " (gds_ll) "  ;
				}
			}
			uint prevlocalbaseaddr;
			if (prevcptr != NAv)
			{
				prevlocalbaseaddr = ll_edge_block_array_[prevcptr].metadata.currcptr;
			} //****
			if (prevcptr != NAv)
			{
				if (prevlocalbaseaddr != prevcptr)
				{
					LOG(ERROR) << " incorrect value. prevcptr : " << prevcptr << ", prevlocalbaseaddr : " << prevlocalbaseaddr << " (gds_ll) "  ;
				}
			}

			localbaseaddr = prevcptr;

			if (((localbaseaddr == ll_localbaseaddrptr) && (localaddrofvalidedge == ll_localaddrptr)) || (prevcptr == NAv))
			{
				// the last was the hole

				// update entity
				entity.lastlocalbaseaddr = prevcptr; //if(prevcptr != NAv){ entity.lastlocalbaseaddr = prevcptr; } else { entity.lastlocalbaseaddr = NAv; } //****
				entity.lastlocaladdr = (LLEDGEBLOCKSIZE - 1);
				entity.totaledgecount = entity.totaledgecount - 1; //***![check later]

				if (prevcptr != NAv)
				{
					ll_edge_block_array_[prevcptr].metadata.nextcptr = NAv;
				} //****

				// update
				ll_lva_[lvaindex] = entity;

				// get out
				return;
			}
			else
			{

				// update entity
				entity.lastlocalbaseaddr = prevcptr; //****
				entity.lastlocaladdr = (LLEDGEBLOCKSIZE - 1);
				entity.totaledgecount = entity.totaledgecount - 1; //***![check later]

				if (prevcptr != NAv)
				{
					ll_edge_block_array_[prevcptr].metadata.nextcptr = NAv;
				} //****

				// pop-out, pop-in
				if (localbaseaddr >= ll_edge_block_array_.size())
				{
					LOG(ERROR) << " addr out-of-range7 (gds_ll) "  ;
				}
				if (localaddrofvalidedge >= LLEDGEBLOCKSIZE)
				{
					LOG(ERROR) << " addr out-of-range89. entity.lastlocaladdr : " << entity.lastlocaladdr << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
				}
				if (ll_localbaseaddrptr >= ll_edge_block_array_.size())
				{
					LOG(ERROR) << " addr out-of-range9 (gds_ll) "  ;
				}
				if (ll_localaddrptr >= LLEDGEBLOCKSIZE)
				{
					LOG(ERROR) << " addr out-of-range10. ll_localaddrptr : " << ll_localaddrptr << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
				}

				// pop-out
				nedge = ll_edge_block_array_[localbaseaddr].ll_edgeblock[localaddrofvalidedge];

				// invalidate //***
				if (nedge.flag != VALID)
				{
					LOG(ERROR) << " something wrong552. localbaseaddr : " << localbaseaddr << ", localaddrofvalidedge : " << localaddrofvalidedge << " (gds_ll) "  ;
				}
				ll_edge_block_array_[localbaseaddr].ll_edgeblock[localaddrofvalidedge].flag = INVALID;

				// pop in
				if (ll_edge_block_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag == VALID)
				{
					LOG(ERROR) << " something wrong557. ll_localbaseaddrptr : " << ll_localbaseaddrptr << ", ll_localaddrptr : " << ll_localaddrptr << " (gds_ll) "  ;
				}
				ll_edge_block_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr] = nedge;

				// redirect pointer (heba -> ll)
				uint workblockaddr = get_edgeblock_offset(nedge.heba_hvtx_id) + nedge.heba_workblockid;
				if (nedge.which_gen_is_the_main_copy_located == 1)
				{
					if (workblockaddr >= edge_block_array_m_.size())
					{
						LOG(ERROR) << "Graphtinker::ll_functions : addr out-of-range 96 (gds_ll) "  ;
					}
					edge_block_array_m_[workblockaddr].edges[nedge.heba_loffset].ll_localbaseaddrptr = ll_localbaseaddrptr;
					edge_block_array_m_[workblockaddr].edges[nedge.heba_loffset].ll_localaddrptr = ll_localaddrptr;
				}
				else
				{
					if (workblockaddr >= edge_block_array_o_.size())
					{
						LOG(ERROR) << "Graphtinker::ll_functions : addr out-of-range 97 (gds_ll) "  ;
					}
					edge_block_array_o_[workblockaddr].edges[nedge.heba_loffset].ll_localbaseaddrptr = ll_localbaseaddrptr;
					edge_block_array_o_[workblockaddr].edges[nedge.heba_loffset].ll_localaddrptr = ll_localaddrptr;
				}

				// update
				ll_lva_[lvaindex] = entity;
				return;
			}
		}
		else
		{
			LOG(ERROR) << " should never get here45 (gds_ll)"  ;
		}
		return;
	}

	void Graphtinker::ll_updateedgeptrs(
		edge_t edge,
		module_params_t moduleparams,
		vector<ll_edgeblock_t> &ll_edge_block_array_)
	{
		if (edge.heba_hvtx_id < 0)
		{
			LOG(ERROR) << " invalid454. edge.heba_hvtx_id : " << edge.heba_hvtx_id << " (gds_ll) "  ;
		}
		if (moduleparams.ll_localbaseaddrptr_x >= ll_edge_block_array_.size())
		{
			LOG(ERROR) << " addr out-of-range47 (gds_ll) "  ;
		}
		if (moduleparams.ll_localaddrptr_x >= LLEDGEBLOCKSIZE)
		{
			LOG(ERROR) << " addr out-of-range5. ll_localaddrptr : " << moduleparams.ll_localaddrptr_x << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
		}
		ll_edge_block_array_[moduleparams.ll_localbaseaddrptr_x].ll_edgeblock[moduleparams.ll_localaddrptr_x].heba_hvtx_id = edge.heba_hvtx_id;
		ll_edge_block_array_[moduleparams.ll_localbaseaddrptr_x].ll_edgeblock[moduleparams.ll_localaddrptr_x].heba_workblockid = edge.heba_workblockid;
		ll_edge_block_array_[moduleparams.ll_localbaseaddrptr_x].ll_edgeblock[moduleparams.ll_localaddrptr_x].heba_loffset = edge.heba_loffset;
		return;
	}
#endif
} // namespace gt