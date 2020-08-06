#include "config/config.h"


int main(){

    gt::Config config("config.ini");
    config.PrintAll();

    return 0;
}