#include <string.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	int Graphtinker::sv_get_next_edge(
		uint xvtx_id,
		vector<supervertex_t> &svs,
		vector<vertexid_t> &freed_edgeblock_list,
		edge_tt *edgett,
		margin_t wblkmargin,
		writeback_unit_cmd_t writebackunitcmd,
		int *tailhvtx_id,
		uint *svs_index,
		uint *numclusteredworkblocks,
		uint geni,
		vector<edge_nt> &_edge_block_array_m,
		vector<edge_nt> &_edge_block_array_o)
	{
		// return 5;
		/// we got here because the current subblock we're in is clustered, this cluster info has our svs index pointer (sv_ptr)
		uint work_block_height = WORK_BLOCK_HEIGHT;
		uint _work_blocks_per_subblock = _work_blocks_per_subblock;
		uint currworkblockaddr = get_edgeblock_offset(xvtx_id) + wblkmargin.top / work_block_height;

		// get tail edgeblock
		// uint svs_index=0;
		*tailhvtx_id = sv_get_tail_edgeblock(svs, currworkblockaddr, svs_index, geni, _edge_block_array_m, _edge_block_array_o);
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
				xvtx_id,
				offset,
				edgett,
				numclusteredworkblocks,
				_edge_block_array_o);

			if (edgefound < 0)
			{
				return 6;
			}
			else
			{
				return 7;
			}
		}
		cout << "bug! : should not get here (supervs)" << endl;
		return NULLL;
	}

	int Graphtinker::sv_get_tail_edgeblock(
		vector<supervertex_t> &svs,
		uint currworkblockaddr,
		uint *svs_index,
		uint geni,
		vector<edge_nt> &_edge_block_array_m,
		vector<edge_nt> &_edge_block_array_o)
	{
		// get where in svs the founding father is pointing to
		if (geni == 1)
		{
			if (currworkblockaddr >= _edge_block_array_m.size())
			{
				cout << "bug! : addr out-of-range4 (supervs) " << endl;
			}
			if (_edge_block_array_m[currworkblockaddr].clusterinfo.flag != VALID)
			{
				cout << "bug! : addr out-of-range8 (supervs) " << endl;
			}
			*svs_index = _edge_block_array_m[currworkblockaddr].clusterinfo.sv_ptr;
		}
		else
		{
			if (currworkblockaddr >= _edge_block_array_o.size())
			{
				cout << "bug! : addr out-of-range5 (supervs) " << endl;
			}
			if (_edge_block_array_o[currworkblockaddr].clusterinfo.flag != VALID)
			{
				cout << "bug! : addr out-of-range6 (supervs) " << endl;
			}
			*svs_index = _edge_block_array_o[currworkblockaddr].clusterinfo.sv_ptr;
		}

		// return FAILED if empty
		if (*svs_index >= svs.size())
		{
			return -1;
			cout << "bug! : addr out-of-range5 (supervs). *svs_index : " << *svs_index << ", svs.size() : " << svs.size() << endl;
			return -1;
		}
		if (svs[*svs_index].hvtx_ids.empty())
		{
			return -1;
		}

		// return if deleted edge was not in a parent subblock
		if (*svs_index >= svs.size())
		{
			cout << "bug! : addr out-of-range6 (supervs) " << endl;
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
		uint xvtx_id,
		uint offset,
		edge_tt *edgett,
		uint *numclusteredworkblocks,
		vector<edge_nt> &edge_block_array)
	{
		// edge_tt eedge;
		uint work_block_height = WORK_BLOCK_HEIGHT;
		uint _work_blocks_per_page = _work_blocks_per_page;
		*numclusteredworkblocks = 0;

		for (int addr = offset; addr < (offset + _work_blocks_per_page); addr++)
		{
			if (addr >= edge_block_array.size())
			{
				cout << "bug! : addr out-of-range6 (supervs) " << endl;
			}
			if (edge_block_array[addr].clusterinfo.flag == VALID)
			{
				*numclusteredworkblocks += 1;
			}
			for (uint f = 0; f < work_block_height; f++)
			{
				if (f >= work_block_height)
				{
					cout << "bug! : addr out-of-range7 (supervs) " << endl;
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
} // namespace gt