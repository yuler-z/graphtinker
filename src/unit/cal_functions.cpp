#include "unit/unit_flow.h"

namespace graphtinker
{
#ifdef EN_CAL
	void UnitFlow::CalInsert(
		edge_t edge,
		vector<cal_edgeblock_t> &cal_edgeblock_array_,
		vector<cal_logical_vertex_entity_t> &cal_lva_,
		cal_edgeblock_tracker_t *cal_edgeblock_tracker_,
		uint geni)
	{
		module_params_t &module_params = unit_option->module_params;
		uint tailid = 0;
		uint offsetaddr = 0;
		uint localoffsetaddr = 0;
		uint lvaindex = 0;
		uint ONE = 1;

		/// calculate lvaindex
		lvaindex = edge.vtx_id / LVACOARSENESSWIDTH;

		/// resize cal_lva_ if full
		if (lvaindex >= cal_lva_.size())
		{
			cal_lva_.resize((cal_lva_.size() + cal_lva_expansion_addition_height_));
			LOG(INFO) << "CalInsert : resizing cal_lva_... "  ;
		}
#ifdef EN_BUGCHECK
		if (lvaindex >= cal_lva_.size())
		{
			LOG(INFO) << "Graphtinker::CalInsert : out-of-range 1 lvaindex = " << lvaindex << ", cal_lva_.size() = " << cal_lva_.size()  ;
		}
#endif

		/// retrieve cal_lva_ entity
		cal_logical_vertex_entity_t entity = cal_lva_[lvaindex];

		/// resize cal_edgeblock_array_ if full
		if (((entity.lastlocalbaseaddr) >= cal_edgeblock_array_.size()) || (cal_edgeblock_tracker_->ptraddr >= cal_edgeblock_array_.size()))
		{
			LOG(INFO) << "CalInsert : resizing cal_edgeblock_array_..."  ;
			cal_edgeblock_array_.resize((cal_edgeblock_array_.size() + cal_eba_expansion_addition_height_));
		}

		/// check if we need to initialize first
		if (entity.flag != VALID)
		{
			//never used before, initialize first before use
			// create
			entity.baseaddr = cal_edgeblock_tracker_->ptraddr;
			entity.lastlocalbaseaddr = cal_edgeblock_tracker_->ptraddr;
			entity.lastlocaladdr = 0;
			entity.totaledgecount = 0;
			entity.flag = VALID;

			// insert
			uint currentlocalbaseaddr = cal_edgeblock_tracker_->ptraddr;
			cal_edgeblock_array_[currentlocalbaseaddr].ll_edgeblock[0] = edge;
			cal_edgeblock_array_[currentlocalbaseaddr].metadata.edgecount = 1;
			cal_edgeblock_array_[currentlocalbaseaddr].metadata.nextcptr = NAv;						 // next edgeblock location
			cal_edgeblock_array_[currentlocalbaseaddr].metadata.currcptr = cal_edgeblock_tracker_->ptraddr; // this current edgeblock location
			cal_edgeblock_array_[currentlocalbaseaddr].metadata.prevcptr = NAv;

#ifdef EN_BUGCHECK
			if ((cal_edgeblock_tracker_->ptraddr) >= cal_edgeblock_array_.size())
			{
				LOG(ERROR) << " out-of-range 31 (GraphTinkerLL)"  ;
			}
#endif

			// increment tracker
			cal_edgeblock_tracker_->ptraddr += 1; // update
		}

		/// insert edge
		else if (entity.flag == VALID)
		{
			if ((entity.totaledgecount % LLEDGEBLOCKSIZE) == 0)
			{
				/// full, allocate new edgeblock and insert in it

				/// update
				uint currentlocalbaseaddr = entity.lastlocalbaseaddr;
				if (currentlocalbaseaddr >= cal_edgeblock_array_.size())
				{
					LOG(ERROR) << "Graphtinker::CalInsert : out-of-range 22, entity.lastlocalbaseaddr = " << entity.lastlocalbaseaddr << ", cal_edgeblock_array_.size() = " << cal_edgeblock_array_.size()  ;
				}
				cal_edgeblock_array_[currentlocalbaseaddr].metadata.nextcptr = cal_edgeblock_tracker_->ptraddr; // next edgeblock location

				/// update
				entity.lastlocalbaseaddr = cal_edgeblock_tracker_->ptraddr;
				entity.lastlocaladdr = 0;

				/// insert
				uint nextlocalbaseaddr = cal_edgeblock_tracker_->ptraddr;
#ifdef EN_BUGCHECK
				if ((cal_edgeblock_tracker_->ptraddr) >= cal_edgeblock_array_.size())
				{
					LOG(ERROR) << " out-of-range 3 (GraphTinkerLL)"  ;
				}
#endif
				cal_edgeblock_array_[nextlocalbaseaddr].ll_edgeblock[entity.lastlocaladdr] = edge;
				cal_edgeblock_array_[nextlocalbaseaddr].metadata.edgecount = 1;
				cal_edgeblock_array_[nextlocalbaseaddr].metadata.nextcptr = NAv;					  // unknown next edgeblock location
				cal_edgeblock_array_[nextlocalbaseaddr].metadata.currcptr = cal_edgeblock_tracker_->ptraddr; // current edgeblock location
				cal_edgeblock_array_[nextlocalbaseaddr].metadata.prevcptr = cal_edgeblock_array_[currentlocalbaseaddr].metadata.currcptr;

				/// increment
				cal_edgeblock_tracker_->ptraddr += 1; // update
			}
			else
			{
				/// not full, insert edge
				cal_edgeblock_array_[entity.lastlocalbaseaddr].ll_edgeblock[entity.lastlocaladdr] = edge;
				cal_edgeblock_array_[entity.lastlocalbaseaddr].metadata.edgecount += 1;
			}
		}

/// append entity.lastlocalbaseaddr & entity.lastlocaladdr to insert_params
/// this is how every edge in edge_block_array gets its pointer to cal_edgeblock_array_
#ifdef EN_CAL
		module_params.ll_localbaseaddrptr_x = entity.lastlocalbaseaddr;
		module_params.ll_localaddrptr_x = entity.lastlocaladdr;
#endif

		/// common updates
		entity.totaledgecount += 1;
		entity.lastlocaladdr += 1;

/// submit changes
#ifdef EN_BUGCHECK
		if (lvaindex >= cal_lva_.size())
		{
			LOG(ERROR) << " out-of-range 4 (GraphTinkerLL). lvaindex = " << lvaindex << ", cal_lva_.size() = " << cal_lva_.size()  ;
		}
#endif
		cal_lva_[lvaindex] = entity;
		return;
	}

	void UnitFlow::CalUpdate(
		edge_t edge,
		vector<cal_edgeblock_t> &cal_edgeblock_array_)
	{
		vertexid_t &ll_localaddrptr = unit_option->module_params.ll_localaddrptr_x;
		vertexid_t &ll_localbaseaddrptr = unit_option->module_params.ll_localbaseaddrptr_x;

		cal_edgeblock_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].weight += 1;
		return;
	}

	void UnitFlow::CalDelete(
		edge_t edge,
		vector<cal_edgeblock_t> &cal_edgeblock_array_)
	{
		vertexid_t &ll_localaddrptr = unit_option->module_params.ll_localaddrptr_x;
		vertexid_t &ll_localbaseaddrptr = unit_option->module_params.ll_localbaseaddrptr_x;
		cal_edgeblock_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag = INVALID;
		return;
	}

	void UnitFlow::CalDeleteAndCrumpleIn(
		edge_t edge,
		vector<cal_edgeblock_t> &cal_edgeblock_array_,
		vector<cal_logical_vertex_entity_t> &cal_lva_,
		cal_edgeblock_tracker_t *cal_edgeblock_tracker_,
		vector<work_block_t> &edge_block_array_m_,
		vector<work_block_t> &edge_block_array_o_,
		uint geni)
	{
		vertexid_t &ll_localaddrptr = unit_option->module_params.ll_localaddrptr_x;
		vertexid_t &ll_localbaseaddrptr = unit_option->module_params.ll_localbaseaddrptr_x;
		// find partiton
		uint work_blocks_per_subblock_ = work_blocks_per_subblock_;
		edge_t nedge;

		// delete edge
		cal_edgeblock_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag = INVALID;
		// return;

		// pop-out from end and pop-in to hole where edge was deleted
		uint lvaindex = edge.vtx_id / LVACOARSENESSWIDTH;
		if (lvaindex >= cal_lva_.size())
		{
			LOG(ERROR) << " addr out-of-range11 (gds_ll) "  ;
		}
		cal_logical_vertex_entity_t entity = cal_lva_[lvaindex];
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
				cal_lva_[lvaindex] = entity;

				// get out
				return;
			}
			else
			{

				// update entity <only these fields>
				entity.lastlocaladdr = entity.lastlocaladdr - 1;
				entity.totaledgecount = entity.totaledgecount - 1;

				// pop-out, pop-in
				if (localbaseaddr >= cal_edgeblock_array_.size())
				{
					LOG(ERROR) << " addr out-of-range7 (gds_ll) "  ;
				}
				if (localaddrofvalidedge >= LLEDGEBLOCKSIZE)
				{
					LOG(ERROR) << " addr out-of-range88. entity.lastlocaladdr : " << entity.lastlocaladdr << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
				}
				if (ll_localbaseaddrptr >= cal_edgeblock_array_.size())
				{
					LOG(ERROR) << " addr out-of-range9 (gds_ll) "  ;
				}
				if (ll_localaddrptr >= LLEDGEBLOCKSIZE)
				{
					LOG(ERROR) << " addr out-of-range10. ll_localaddrptr : " << ll_localaddrptr << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
				}

				// pop-out
				nedge = cal_edgeblock_array_[localbaseaddr].ll_edgeblock[localaddrofvalidedge];

				// invalidate //***
				if (nedge.flag != VALID)
				{
					LOG(ERROR) << " something wrong551. localbaseaddr : " << localbaseaddr << ", localaddrofvalidedge : " << localaddrofvalidedge << ", ll_localbaseaddrptr : " << ll_localbaseaddrptr << ", ll_localaddrptr : " << ll_localaddrptr << " (gds_ll) "  ;
				}

				cal_edgeblock_array_[localbaseaddr].ll_edgeblock[localaddrofvalidedge].flag = INVALID;

				// pop in
				if (cal_edgeblock_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag == VALID)
				{
					LOG(ERROR) << " something wrong457. ll_localbaseaddrptr : " << ll_localbaseaddrptr << ", ll_localaddrptr : " << ll_localaddrptr << " (gds_ll) "  ;
				}
				cal_edgeblock_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr] = nedge;

				// redirect pointer (heba -> ll)
				uint workblockaddr = GetEdgeblockOffset(nedge.heba_hvtx_id) + nedge.heba_workblockid;
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
				cal_lva_[lvaindex] = entity;
				return;
			}
		}
		else if (localaddr == 0)
		{
			// empty, free this edgeblock and fetch from its predecessor new edgeblock

			uint localaddrofvalidedge = (LLEDGEBLOCKSIZE - 1);

			if (entity.lastlocalbaseaddr >= cal_edgeblock_array_.size())
			{
				LOG(ERROR) << " addr out-of-range67 (gds_ll) "  ;
			}
			uint prevcptr = cal_edgeblock_array_[entity.lastlocalbaseaddr].metadata.prevcptr; // next edgeblock location
			if (prevcptr != NAv)
			{
				if (prevcptr >= cal_edgeblock_array_.size())
				{
					LOG(ERROR) << " addr out-of-range69. prevcptr : " << prevcptr << ", cal_edgeblock_array_.size() : " << cal_edgeblock_array_.size() << " (gds_ll) "  ;
				}
			}
			uint prevlocalbaseaddr;
			if (prevcptr != NAv)
			{
				prevlocalbaseaddr = cal_edgeblock_array_[prevcptr].metadata.currcptr;
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
					cal_edgeblock_array_[prevcptr].metadata.nextcptr = NAv;
				} //****

				// update
				cal_lva_[lvaindex] = entity;

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
					cal_edgeblock_array_[prevcptr].metadata.nextcptr = NAv;
				} //****

				// pop-out, pop-in
				if (localbaseaddr >= cal_edgeblock_array_.size())
				{
					LOG(ERROR) << " addr out-of-range7 (gds_ll) "  ;
				}
				if (localaddrofvalidedge >= LLEDGEBLOCKSIZE)
				{
					LOG(ERROR) << " addr out-of-range89. entity.lastlocaladdr : " << entity.lastlocaladdr << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
				}
				if (ll_localbaseaddrptr >= cal_edgeblock_array_.size())
				{
					LOG(ERROR) << " addr out-of-range9 (gds_ll) "  ;
				}
				if (ll_localaddrptr >= LLEDGEBLOCKSIZE)
				{
					LOG(ERROR) << " addr out-of-range10. ll_localaddrptr : " << ll_localaddrptr << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
				}

				// pop-out
				nedge = cal_edgeblock_array_[localbaseaddr].ll_edgeblock[localaddrofvalidedge];

				// invalidate //***
				if (nedge.flag != VALID)
				{
					LOG(ERROR) << " something wrong552. localbaseaddr : " << localbaseaddr << ", localaddrofvalidedge : " << localaddrofvalidedge << " (gds_ll) "  ;
				}
				cal_edgeblock_array_[localbaseaddr].ll_edgeblock[localaddrofvalidedge].flag = INVALID;

				// pop in
				if (cal_edgeblock_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag == VALID)
				{
					LOG(ERROR) << " something wrong557. ll_localbaseaddrptr : " << ll_localbaseaddrptr << ", ll_localaddrptr : " << ll_localaddrptr << " (gds_ll) "  ;
				}
				cal_edgeblock_array_[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr] = nedge;

				// redirect pointer (heba -> ll)
				uint workblockaddr = GetEdgeblockOffset(nedge.heba_hvtx_id) + nedge.heba_workblockid;
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
				cal_lva_[lvaindex] = entity;
				return;
			}
		}
		else
		{
			LOG(ERROR) << " should never get here45 (gds_ll)"  ;
		}
		return;
	}

	void UnitFlow::CalUpdateEdgePtrs(
		edge_t edge,
		vector<cal_edgeblock_t> &cal_edgeblock_array_)
	{
		module_params_t &module_params = unit_option->module_params;
		if (edge.heba_hvtx_id < 0)
		{
			LOG(ERROR) << " invalid454. edge.heba_hvtx_id : " << edge.heba_hvtx_id << " (gds_ll) "  ;
		}
		if (module_params.ll_localbaseaddrptr_x >= cal_edgeblock_array_.size())
		{
			LOG(ERROR) << " addr out-of-range47 (gds_ll) "  ;
		}
		if (module_params.ll_localaddrptr_x >= LLEDGEBLOCKSIZE)
		{
			LOG(ERROR) << " addr out-of-range5. ll_localaddrptr : " << module_params.ll_localaddrptr_x << ", LLEDGEBLOCKSIZE : " << LLEDGEBLOCKSIZE << " (gds_ll) "  ;
		}
		cal_edgeblock_array_[module_params.ll_localbaseaddrptr_x].ll_edgeblock[module_params.ll_localaddrptr_x].heba_hvtx_id = edge.heba_hvtx_id;
		cal_edgeblock_array_[module_params.ll_localbaseaddrptr_x].ll_edgeblock[module_params.ll_localaddrptr_x].heba_workblockid = edge.heba_workblockid;
		cal_edgeblock_array_[module_params.ll_localbaseaddrptr_x].ll_edgeblock[module_params.ll_localaddrptr_x].heba_loffset = edge.heba_loffset;
		return;
	}
#endif
} // namespace graphtinker