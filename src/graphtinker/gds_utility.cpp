#include <string.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
	uint Graphtinker::getsvtracker(markertracker_t *svtracker)
	{
		uint ret = svtracker->marker;
		svtracker->marker++;
		return ret;
	}
} // namespace gt