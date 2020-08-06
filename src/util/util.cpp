#include "util/util.h"
namespace util{

    bool StringToNum(const std::string& str, uint &result)
    {
        std::istringstream iss(str);

        iss >> result;

        return !( iss >> result).fail();
    }

}
