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
#include <omp.h>
#include "graphtinker/graphtinker.h"
using namespace std;

namespace gt
{
    uint Graphtinker::get_edgeblock_offset(vertexid_t vid)
    {
        uint offset4rmbase = vid * work_blocks_per_page_;
        return offset4rmbase;
    }
} // namespace gt