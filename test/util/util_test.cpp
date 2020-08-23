#include <iostream>
#include "util/util.h"
#include <glog/logging.h>

int main(){

    uint result;
    util::StringToNum("12345678", result);
    
    LOG(INFO) << "result: " << result << std::endl;

    return 0;
}