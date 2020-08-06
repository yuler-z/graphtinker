#include <string.h>
#include "graphtinker/graphtinker.h"

namespace gt
{
	uint Graphtinker::max(uint A, uint B)
	{
		if (A > B)
		{
			return A;
		}
		else
		{
			return B;
		}

		return A; //should never get here
	}
} // namespace gt