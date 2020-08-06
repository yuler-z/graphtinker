#include <iostream>
#include "util/util.h"

int main(){

    uint result;
    util::StringToNum("12345678", result);
    
    std::cout << "result: " << result << std::endl;

    return 0;
}