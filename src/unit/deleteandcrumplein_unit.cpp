#include "core/graphtinker.h"

#ifdef EN_DCI
namespace graphtinker
{
	/// this function is only used when when an edge is removed from a *clustered region*
	void Graphtinker::CrumpleInUnit(
		writeback_unit_cmd_t writeback_unit_cmd,
		find_report_t find_report,
		edge_t edge,
		vector<work_block_t> &edge_block_array_m_,
		vector<work_block_t> &edge_block_array_o_,
#ifdef EN_CAL
		vector<cal_edgeblock_t> &cal_edgeblock_array_,
#endif
		vector<edgeblock_parentinfo_t> &edgeblock_parentinfo,
		vertexid_t vtx_id,
		margin_t work_block_margin,
		margin_t sub_block_margin,
		uint geni, crumple_in_cmd_t deleteandcrumpleincmd, vector<supervertex_t> &svs, vector<vertexid_t> &freed_edgeblock_list)
	{
		uint work_blocks_per_subblock_ = work_blocks_per_subblock_;
		uint work_blocks_per_page_ = work_blocks_per_page_;

		/// only when an edge is removed from a *clustered region* do we have to pop-out and pop-back-in
		if (deleteandcrumpleincmd.verdict == DCI_CRUMPLEINCMD)
		{
			int retstatus = -1;
			edge_tt edgett;
			vertexid_t ll_localbaseaddrptr = 0;
			vertexid_t ll_localaddrptr = 0;
			int tailhvtx_id = -1;
			uint svs_index = 0;
			uint numclusteredworkblocks = NULLL;

			retstatus = sv_get_next_edge(
				vtx_id,
				svs,
				freed_edgeblock_list,
				&edgett,
				work_block_margin,
				writeback_unit_cmd,
				&tailhvtx_id,
				&svs_index,
				&numclusteredworkblocks,
				geni,
				edge_block_array_m_,
				edge_block_array_o_);

			if (retstatus == 7)
			{
				// retrieved tail edgeblock, and found a valid edge in it
				if (geni == 1)
				{
					if (writeback_unit_cmd.addr >= edge_block_array_m_.size())
					{
						LOG(ERROR) << " addr out-of-range1 (UpdateEdge) "  ;
					}
					if (find_report.local_offset >= WORK_BLOCK_HEIGHT)
					{
						LOG(ERROR) << " addr out-of-range2 (UpdateEdge) "  ;
					}
					edge_block_array_m_[writeback_unit_cmd.addr].edges[find_report.local_offset] = edgett;
				}
				else
				{
					if (writeback_unit_cmd.addr >= edge_block_array_o_.size())
					{
						LOG(ERROR) << " addr out-of-range1 (UpdateEdge) "  ;
					}
					if (find_report.local_offset >= WORK_BLOCK_HEIGHT)
					{
						LOG(ERROR) << " addr out-of-range2 (UpdateEdge) "  ;
					}
					edge_block_array_o_[writeback_unit_cmd.addr].edges[find_report.local_offset] = edgett;
				}

// redirect pointer (ll -> heba) (remember its a 2-way link)
// remember that ll may need to update which edgeblockarray (m or c) it's now pointing to!
#ifdef EN_CAL
				uint work_block_height = WORK_BLOCK_HEIGHT;
				cal_edgeblock_array_[edgett.ll_localbaseaddrptr].ll_edgeblock[edgett.ll_localaddrptr].heba_hvtx_id = vtx_id;
				cal_edgeblock_array_[edgett.ll_localbaseaddrptr].ll_edgeblock[edgett.ll_localaddrptr].heba_workblockid = work_block_margin.top / work_block_height;
				cal_edgeblock_array_[edgett.ll_localbaseaddrptr].ll_edgeblock[edgett.ll_localaddrptr].heba_loffset = find_report.local_offset;
				cal_edgeblock_array_[edgett.ll_localbaseaddrptr].ll_edgeblock[edgett.ll_localaddrptr].which_gen_is_the_main_copy_located = geni;
#endif
			}
			else if (retstatus == 6)
			{
				// retrieved tail edgeblock, but could not find a valid edge in it
				// free what needs to be free
				if (tailhvtx_id > 0 && numclusteredworkblocks == 0)
				{

					// edgeblock is empty. free it. edge_block_array_o_ is always used because tailhvtx_id can never be in edge_block_array_m_
					uint offset = GetEdgeblockOffset(tailhvtx_id);
					for (uint addr = offset; addr < (offset + work_blocks_per_page_); addr++)
					{
						if (addr >= edge_block_array_o_.size())
						{
							LOG(ERROR) << " addr out-of-range3 (UpdateEdge) "  ;
						}
						edge_block_array_o_[addr].clusterinfo.flag = INVALID;
					}

					// detatch it from its father subblock
					uint index = tailhvtx_id;
					if (index >= edgeblock_parentinfo.size())
					{
						LOG(ERROR) << " addr out-of-range4 (UpdateEdge) "  ;
					}
					if (edgeblock_parentinfo[index].flag != VALID)
					{
						LOG(ERROR) << " incorrect (update_edge66)"  ;
					}
					edgeblock_parentinfo_t parentinfo = edgeblock_parentinfo[index];
					uint subblockbaseaddr = GetEdgeblockOffset(parentinfo.vtx_id) + (parentinfo.subblockid * work_blocks_per_subblock_);
					if (parentinfo.gen_of_parent == 1)
					{
						for (uint id = 0; id < work_blocks_per_subblock_; id++)
						{
							edge_block_array_m_[(subblockbaseaddr + id)].clusterinfo.flag = INVALID;
						}
					}
					else
					{
						for (uint id = 0; id < work_blocks_per_subblock_; id++)
						{
							edge_block_array_o_[(subblockbaseaddr + id)].clusterinfo.flag = INVALID;
						}
					}

					// update edgeblock_parentinfo
					if (index >= edgeblock_parentinfo.size())
					{
						LOG(ERROR) << " addr out-of-range4 (UpdateEdge) "  ;
					}
					edgeblock_parentinfo[index].flag = INVALID;

					// pop it out of svs
					if (svs_index >= svs.size())
					{
						LOG(ERROR) << " addr out-of-range5 (UpdateEdge) "  ;
					}
					if (svs[svs_index].hvtx_ids.back() != tailhvtx_id)
					{
						LOG(ERROR) << " incorrect (update_edge67)"  ;
					}
					if (!svs[svs_index].hvtx_ids.empty())
					{
						svs[svs_index].hvtx_ids.pop_back();
					}

					// add to free list
					freed_edgeblock_list.push_back(tailhvtx_id);
				}
			}
			else if (retstatus == 5)
			{
				// could not retrieve tail edgeblock
			}
			else
			{
				LOG(ERROR) << " should never get here2 (UpdateEdge)"  ;
			}
		}
		else if (deleteandcrumpleincmd.verdict == DCI_JUSTQUITCMD)
		{
		}
		else if (deleteandcrumpleincmd.verdict == DCI_NOCMD)
		{
		}
		else
		{
			LOG(ERROR) << " should never get here3 (UpdateEdge)"  ;
		}
	}
} // namespace graphtinker
#endif