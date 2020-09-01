#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
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
#include <gflags/gflags.h>
#include "core/graphtinker.h"
#include "core/vertices.h"
;
using namespace graphtinker;

int main(int argc, char* argv[])
{
	// 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 初始化日志库
	FLAGS_logtostderr = true;
	FLAGS_colorlogtostderr = true;
	// FLAGS_log_dir = "/home/yuler/graphtinker/log";
    google::InitGoogleLogging(argv[0]);
	LOG(INFO) <<"started  (updates_test)" ;	

	std::string config_file_path = "config.ini";
	graphtinker::Graphtinker tinkerA(config_file_path);
	tinkerA.batch_insert_edge("RMAT_500K_8M", BATCH_SIZE);
	tinkerA.batch_delete_edge("RMAT_500K_8M", BATCH_SIZE);
	return 0;
}