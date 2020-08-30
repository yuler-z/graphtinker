#include <iostream>
#include "util/util.h"
#include <glog/logging.h>

using namespace graphtinker;

int main(){

    uint result;
    StringToNum("12345678", result);
    
    LOG(INFO) << "result: " << result << std::endl;

    return 0;
}