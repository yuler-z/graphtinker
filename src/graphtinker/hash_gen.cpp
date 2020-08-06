#include <string.h>
#include "graphtinker/graphtinker.h"

#define KMULTFACTOR 1

namespace gt
{
	bucket_t Graphtinker::googlehash(vertexid_t vid, uint geni)
	{
		bucket_t hashval = 0;
		bucket_t exthashval = 0;
		uint page_blockheight = _page_block_height;

		// get extended edgeblock range
		uint ext_edgeblock_range = page_blockheight + (KMULTFACTOR * (geni - 1));

		// do basicHash on extended edgeblock range
		exthashval = vid % ext_edgeblock_range;

		// now hash to edgeblock range
		hashval = exthashval % page_blockheight;
		return hashval;
	}
} // namespace gt