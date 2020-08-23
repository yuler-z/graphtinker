#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <glog/logging.h>



namespace gt{

    class Graphtinker;
    class Config {

        private:
            std::string _file_path;
            std::map<std::string, std::string> _config_map;

        public:
            Config();
            Config(std::string file_path);
            bool Load();
            bool ConfigGraph(Graphtinker *gt);
            bool PrintAll();

            std::string get_file_path();
            void set_file_path(std::string);
    };
}
#endif