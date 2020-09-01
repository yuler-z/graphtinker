#include "unit/unit_flow.h"

namespace graphtinker{
    
    UnitFlow::UnitFlow(Graphtinker* gt){
        gt_ = gt;
        unit_option = new UnitOption();
    }

    UnitFlow::~UnitFlow(){
        delete unit_option;
        unit_option = nullptr;
        gt_ = nullptr;
    }
}