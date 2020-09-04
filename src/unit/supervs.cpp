#include <string.h>
#include <iostream>
#include <string>
#include "core/graphtinker.h"

#ifdef EN_DCI
namespace graphtinker
{
	int Graphtinker::sv_get_next_edge(
		uint vtx_id,
		vector<supervertex_t> &svs,
		vector<vertexid_t> &freed_edgeblock_list,
		edge_tt *edgett,
		margin_t workblock_margin,
		writeback_unit_cmd_t writeback_unit_cmd,
		int *tailhvtx_id,
		uint *svs_index,
		uint *numclusteredworkblocks,
		uint geni,
		vector<workblock_t> &edge_block_array_m_,
		vector<workblock_t> &edge_block_array_o_)
	{
		// return 5;
		/// we got here because the current subblock we're in is clustered, this cluster info has our svs index pointer (sv_ptr)
		uint workblock_height = WORK_BLOCK_HEIGHT;
		uint workblocks_per_subblock_ = workblocks_per_subblock_;
		uint currworkblockaddr = get_edgeblock_offset(vtx_id) + workblock_margin.top / workblock_height;

		// get tail edgeblock
		// uint svs_index=0;
		*tailhvtx_id = sv_get_tail_edgeblock(svs, currworkblockaddr, svs_index, geni, edge_block_array_m_, edge_block_array_o_);
		if (*tailhvtx_id < 0)
		{
			return 5;
		}
		else
		{

			// get offset of edgeblock in edge_block_array
			uint offset = get_edgeblock_offset(*tailhvtx_id);

			// pick any edge from it
			int edgefound = -1;
			edgefound = sv_pick_edge(
				vtx_id,
				offset,
				edgett,
				numclusteredworkblocks,
				edge_block_array_o_);

			if (edgefound < 0)
			{
				return 6;
			}
			else
			{
				return 7;
			}
		}
		LOG(ERROR) << " should not get here (supervs)"  ;
		return NULLL;
	}

	int Graphtinker::sv_get_tail_edgeblock(
		vector<supervertex_t> &svs,
		uint currworkblockaddr,
		uint *svs_index,
		uint geni,
		vector<workblock_t> &edge_block_array_m_,
		vector<workblock_t> &edge_block_array_o_)
	{
		// get where in svs the founding father is pointing to
		if (geni == 1)
		{
			if (currworkblockaddr >= edge_block_array_m_.size())
			{
				LOG(ERROR) << " addr out-of-range4 (supervs) "  ;
			}
			if (edge_block_array_m_[currworkblockaddr].clusterinfo.flag != VALID)
			{
				LOG(ERROR) << " addr out-of-range8 (supervs) "  ;
			}
			*svs_index = edge_block_array_m_[currworkblockaddr].clusterinfo.sv_ptr;
		}
		else
		{
			if (currworkblockaddr >= edge_block_array_o_.size())
			{
				LOG(ERROR) << " addr out-of-range5 (supervs) "  ;
			}
			if (edge_block_array_o_[currworkblockaddr].clusterinfo.flag != VALID)
			{
				LOG(ERROR) << " addr out-of-range6 (supervs) "  ;
			}
			*svs_index = edge_block_array_o_[currworkblockaddr].clusterinfo.sv_ptr;
		}

		// return FAILED if empty
		if (*svs_index >= svs.size())
		{
			return -1;
			LOG(ERROR) << " addr out-of-range5 (supervs). *svs_index : " << *svs_index << ", svs.size() : " << svs.size()  ;
			return -1;
		}
		if (svs[*svs_index].hvtx_ids.empty())
		{
			return -1;
		}

		// return if deleted edge was not in a parent subblock
		if (*svs_index >= svs.size())
		{
			LOG(ERROR) << " addr out-of-range6 (supervs) "  ;
		}
		if (geni >= (svs[*svs_index].geni_ofparentsubblock + svs[*svs_index].hvtx_ids.size()))
		{
			return -1;
		}

		// get the last hvtx_id of that svs entry (pointed to by the founding father)
		vertexid_t hvtx_id = svs[*svs_index].hvtx_ids.back(); //***
		return hvtx_id;
	}

	int Graphtinker::sv_pick_edge(
		uint vtx_id,
		uint offset,
		edge_tt *edgett,
		uint *numclusteredworkblocks,
		vector<workblock_t> &edge_block_array)
	{
		// edge_tt eedge;
		uint workblock_height = WORK_BLOCK_HEIGHT;
		uint workblocks_per_page_ = workblocks_per_page_;
		*numclusteredworkblocks = 0;

		for (int addr = offset; addr < (offset + workblocks_per_page_); addr++)
		{
			if (addr >= edge_block_array.size())
			{
				LOG(ERROR) << " addr out-of-range6 (supervs) "  ;
			}
			if (edge_block_array[addr].clusterinfo.flag == VALID)
			{
				*numclusteredworkblocks += 1;
			}
			for (uint f = 0; f < workblock_height; f++)
			{
				if (f >= workblock_height)
				{
					LOG(ERROR) << " addr out-of-range7 (supervs) "  ;
				}
				if (edge_block_array[addr].edges[f].flag == VALID)
				{
					*edgett = edge_block_array[addr].edges[f]; // edge picked

					edge_block_array[addr].edges[f].flag = DELETED; // mark location as deleted
					return 1;
				}
			}
		}
		// if it reaches here, then the edgeblock is emptied
		return -1;
	}
} // namespace graphtinker
#endif