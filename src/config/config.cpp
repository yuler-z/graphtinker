
#include <iostream>

#include "util/util.h"
#include "config/config.h"
#include "config/INIReader.h"
#include "graphtinker/common.h"
#include "graphtinker/graphtinker.h"

namespace gt {

    Config::Config() {

    };

    Config::Config(std::string file_path) {
        _file_path = file_path;

        Load();
    }

    bool Config::Load() {
        std::cout << "config::Load : start load configuration file" << std::endl;
        INIReader reader(_file_path);

        if (reader.ParseError() != 0) {
            std::cout << "can't load config file" << endl;
            return false;
        }


        _config_map.insert(std::make_pair("sgh_for_vtxid", reader.Get("graphtinker", "sgh_for_xvtxid", "SELF")));
        _config_map.insert(std::make_pair("sgh_for_xadjvtxid", reader.Get("graphtinker", "sgh_for_xadjvtxid", "SELF")));
        _config_map.insert(std::make_pair("updatev", reader.Get("graphtinker", "updatev", "SELF")));
        _config_map.insert(std::make_pair("min_vertex", reader.Get("graphtinker", "min_vertex", "0")));
        _config_map.insert(std::make_pair("max_vertex", reader.Get("graphtinker", "max_vertex", "524288")));
        _config_map.insert(std::make_pair("num_vertices", reader.Get("graphtinker", "num_vertices", "524288")));
        _config_map.insert(std::make_pair("num_edges", reader.Get("graphtinker", "num_edges", "8380000")));
        _config_map.insert(
                std::make_pair("graphdirectiontype", reader.Get("graphtinker", "graphdirectiontype", "DIRECTED")));
        _config_map.insert(std::make_pair("sub_block_height", reader.Get("graphtinker", "sub_block_height", "8")));
        _config_map.insert(std::make_pair("page_block_height", reader.Get("graphtinker", "page_block_height", "64")));
        _config_map.insert(std::make_pair("eba_m_expansion_addition_height",
                                          reader.Get("graphtinker", "eba_m_expansion_addition_height", "100000")));
        _config_map.insert(std::make_pair("eba_o_expansion_addition_height",
                                          reader.Get("graphtinker", "eba_o_expansion_addition_height", "100000")));
        _config_map.insert(std::make_pair("ll_eba_expansion_addition_height",
                                          reader.Get("graphtinker", "ll_eba_expansion_addition_height", "1000")));
        _config_map.insert(std::make_pair("ll_lva_expansion_addition_height",
                                          reader.Get("graphtinker", "ll_lva_expansion_addition_height", "100")));
        _config_map.insert(
                std::make_pair("eba_expansion_padding", reader.Get("graphtinker", "eba_expansion_padding", "10000")));

        return true;
    }

    bool Config::PrintAll() {

        if (_config_map.empty()) {
            std::cout << "No configuration file or file is not loaded!" << endl;
            return false;
        }

        map<string, string>::iterator it;
        std::cout << "Print configuration file loaded" << endl;

        for (it = _config_map.begin(); it != _config_map.end(); it++) {
            std::cout << it->first << " = " << it->second << endl;
        }

        return true;
    }

    bool Config::ConfigGraph(Graphtinker *gt) {
        std::cout << "config::ConfigGraph : start config" << std::endl;
        gt->_sgh_for_xvtxid = _config_map["sgh_for_xvtxid"].compare("SELF") == 0 ? SELF : OTHER;
        std::cout << "1" << std::endl;
        gt->_sgh_for_xadjvtxid = _config_map["sgh_for_xadjvtxid"].compare("SELF") == 0 ? SELF : OTHER;
        gt->_updatev = _config_map["updatev"].compare("SELF") == 0 ? SELF : OTHER;

        uint min_vertex, max_vertex;
        util::StringToNum(_config_map["min_vertex"], min_vertex);
        util::StringToNum(_config_map["max_vertex"], max_vertex);
        gt->_vertex_range = max_vertex - min_vertex;

        util::StringToNum(_config_map["num_vertices"], gt->_num_vertices);
        util::StringToNum(_config_map["num_edges"], gt->_num_edges);

        gt->_graphdirectiontype = _config_map.at("graphdirectiontype").compare("DIRECTEDGRAPH") == 0?DIRECTEDGRAPH:UNDIRECTEDGRAPH;

        util::StringToNum(_config_map["sub_block_height"], gt->_sub_block_height);
        util::StringToNum(_config_map["page_block_height"], gt->_page_block_height);
        util::StringToNum(_config_map["eba_m_expansion_addition_height"], gt->_eba_m_expansion_addition_height);
        util::StringToNum(_config_map["eba_o_expansion_addition_height"], gt->_eba_o_expansion_addition_height);
        util::StringToNum(_config_map["ll_eba_expansion_addition_height"], gt->_ll_eba_expansion_addition_height);
        util::StringToNum(_config_map["ll_lva_expansion_addition_height"], gt->_ll_lva_expansion_addition_height);
        util::StringToNum(_config_map["eba_expansion_padding"], gt->_eba_expansion_padding);

        std::cout << "end config" << std::endl;

        return true;
    }


    std::string Config::file_path() {
        return _file_path;
    }

    void Config::set_file_path(std::string file_path) {
        _file_path = file_path;
    }
}
