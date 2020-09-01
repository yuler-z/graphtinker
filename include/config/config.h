#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <core/common.h>
#include <glog/logging.h>


using std::string;

namespace graphtinker{

    class Graphtinker;
    class Config {

        private:
            string file_path_;
            std::map<string, string> config_map_;

        private:
            std::map<string, edge_type_t> edge_type_map_;
            std::vector<string> edge_type_array_;
            edge_type_t edge_type_mark_;

        public:
            Config();
            Config(string file_path);
            bool Load();
            bool Load(string file_path);
            bool ConfigGraph(Graphtinker *gt);
            bool PrintAll();

            string get_file_path();
            void set_file_path(string);

            // getter
            const std::vector<string> &edge_type_array() const ;
            const std::map<string, edge_type_t> &edge_type_map() const ;
    };
}
#endif