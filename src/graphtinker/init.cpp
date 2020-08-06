#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include "graphtinker/graphtinker.h" // Courtesy : Jinja 2.0
using namespace std;
namespace gt
{
	uint Graphtinker::add_page(tracker_t *tracker, vector<edge_nt> &edge_block_array)
	{
		if ((tracker->mark * _work_blocks_per_page) >= edge_block_array.size())
		{ // resize only when filled
			cout << "add_page : resizing _edge_block_array_o..." << endl;
			edge_block_array.resize((edge_block_array.size() + (_eba_o_expansion_addition_height * _work_blocks_per_page)));
		}
		uint pos = tracker->mark;
		tracker->mark += 1;
		return pos;
	}
} // namespace gt