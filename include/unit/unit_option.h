#ifndef UNIT_OPTION_H
#define UNIT_OPTION_H

#include "core/common.h"

namespace graphtinker{
    class UnitOption{
        public:
            module_params_t module_params;
            module_unit_cmd_t module_unit_cmd;            

            load_unit_cmd_t load_unit_cmd;

            insert_params_t insert_params;
            insert_report_t insert_report;

            find_params_t find_params;
            find_report_t find_report;

            search_report_t search_report;

            writeback_unit_cmd_t writeback_unit_cmd;

            interval_unit_cmd_t interval_unit_cmd;

            cal_unit_cmd_t cal_unit_cmd;

        
        public:
            UnitOption();
            ~UnitOption();

        public:

            // Init Unit Option of the State Machine
            void InitModuleUnitParams(vertexid_t adjvtx_id, edgeweight_t edge_weight);
            void InitLoadUnit();
            void InitInsertUnit(vertexid_t adjvtx_id, bucket_t adjvtx_id_hash, edgeweight_t edge_weight);
            void InitFindUnit(vertexid_t adjvtx_id, bucket_t adjvtx_id_hash, edgeweight_t edge_weight);
            void InitWritebackUnit();

            void InitModuleUnitParams2();
            void InitInsertUnit2();
            void InitFindUnit2();

            void InitCalUnit();
            void ClearCalAdr();









            //getter
            /*
            module_params_t &module_params();
            module_unit_cmd_t &module_unit_cmd();
            load_unit_cmd_t &load_unit_cmd();
            insert_params_t &insert_params();
            insert_report_t &insert_report();
            find_params_t &find_params();
            find_report_t &find_report();
            search_report_t &search_report();
            writeback_unit_cmd_t &writeback_unit_cmd();
            interval_unit_cmd_t &interval_unit_cmd();
            */



            
            




    };


}

#endif