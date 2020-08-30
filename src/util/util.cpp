#include "util/util.h"

namespace graphtinker{

    bool StringToNum(const std::string& str, uint &result)
    {
        std::istringstream iss(str);

        iss >> result;

        return !( iss >> result).fail();
    }

}
