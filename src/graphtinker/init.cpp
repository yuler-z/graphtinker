#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include "graphtinker/graphtinker.h" // Courtesy : Jinja 2.0
using namespace std;
namespace gt
{
	uint Graphtinker::add_page(tracker_t *tracker, vector<work_block_t> &edge_block_array)
	{
		if ((tracker->mark * work_blocks_per_page_) >= edge_block_array.size())
		{ // resize only when filled
			LOG(INFO) << "add_page : resizing edge_block_array_o_..."  ;
			edge_block_array.resize((edge_block_array.size() + (eba_o_expansion_addition_height_ * work_blocks_per_page_)));
		}
		uint pos = tracker->mark;
		tracker->mark += 1;
		return pos;
	}
} // namespace gt