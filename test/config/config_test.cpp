#include "config/config.h"


int main(){

    graphtinker::Config config("config.ini");
    config.PrintAll();

    return 0;
}