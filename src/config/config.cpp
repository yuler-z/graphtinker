
#include <iostream>
#include "config/config.h"
#include "config/INIReader.h"
#include "core/common.h"
#include "util/util.h"
#include "core/graphtinker.h"

using std::map;
namespace graphtinker {

    Config::Config() {

    };

    Config::Config(string file_path) {

        file_path_ = file_path;

        string edge_types[] = {"type0", "type1", "type2", "type3"};
        edge_type_array_ = vector<string>(std::begin(edge_types), std::end(edge_types));
        edge_type_mark_ = 0;
        for (auto &&i : edge_type_array_)
        {
            edge_type_map_.insert(std::make_pair(i,edge_type_mark_++));
        }

        Load("default_config.ini");
        Load(file_path_);
    }

    bool Config::Load(string file_path) {
        DLOG(INFO) <<  "config::Load : start load configuration file";
        INIReader reader(file_path);

        if (reader.ParseError() != 0) {
            LOG(ERROR) << "can't load config file"  ;
            return false;
        }

        config_map_.insert(std::make_pair("sgh_for_vtx_id", reader.Get("graphtinker", "sgh_for_vtx_id", "true")));
        config_map_.insert(std::make_pair("sgh_for_adjvtx_id", reader.Get("graphtinker", "sgh_for_adjvtx_id","true")));
        config_map_.insert(std::make_pair("updatev", reader.Get("graphtinker", "updatev", "true")));
        config_map_.insert(std::make_pair("num_vertices", reader.Get("graphtinker", "num_vertices", "524288")));
        config_map_.insert(std::make_pair("num_edges", reader.Get("graphtinker", "num_edges", "8380000")));
        config_map_.insert(
                std::make_pair("is_directed", reader.Get("graphtinker", "is_directed", "true")));
        config_map_.insert(std::make_pair("sub_block_height", reader.Get("graphtinker", "sub_block_height", "8")));
        config_map_.insert(std::make_pair("page_block_height", reader.Get("graphtinker", "page_block_height", "64")));
        config_map_.insert(std::make_pair("eba_m_expansion_addition_height",
                                          reader.Get("graphtinker", "eba_m_expansion_addition_height", "100000")));
        config_map_.insert(std::make_pair("eba_o_expansion_addition_height",
                                          reader.Get("graphtinker", "eba_o_expansion_addition_height", "100000")));
        config_map_.insert(std::make_pair("cal_eba_expansion_addition_height",
                                          reader.Get("graphtinker", "cal_eba_expansion_addition_height", "1000")));
        config_map_.insert(std::make_pair("cal_lva_expansion_addition_height",
                                          reader.Get("graphtinker", "cal_lva_expansion_addition_height", "100")));
        config_map_.insert(
                std::make_pair("eba_expansion_padding", reader.Get("graphtinker", "eba_expansion_padding", "10000")));

        return true;
    }

    bool Config::PrintAll() {

        if (config_map_.empty()) {
            LOG(ERROR) << "No configuration file or file is not loaded!"  ;
            return false;
        }

        LOG(INFO) << "----------Configuration-------------"  ;

        for (auto it = config_map_.begin(); it != config_map_.end(); it++) {
            LOG(INFO) << it->first << " = " << it->second  ;
        }

        return true;
    }

    bool Config::ConfigGraph(Graphtinker *gt) {
        DLOG(INFO) << "config::ConfigGraph : start config";
        gt->sgh_for_vtx_id_ = config_map_["sgh_for_vtx_id"].compare("true") == 0;
        gt->sgh_for_adjvtx_id_ = config_map_["sgh_for_adjvtx_id"].compare("true") == 0;
        gt->updatev_ = config_map_["updatev"].compare("true") == 0;


        StringToNum(config_map_["num_vertices"], gt->num_vertices_);
        StringToNum(config_map_["num_edges"], gt->num_edges_);
        gt->vertex_range_ = gt->num_edges_;

        gt->is_directed_ = config_map_.at("is_directed").compare("true") == 0;

        StringToNum(config_map_["sub_block_height"], gt->sub_block_height_);
        StringToNum(config_map_["page_block_height"], gt->page_block_height_);
        StringToNum(config_map_["eba_m_expansion_addition_height"], gt->eba_m_expansion_addition_height_);
        StringToNum(config_map_["eba_o_expansion_addition_height"], gt->eba_o_expansion_addition_height_);
        StringToNum(config_map_["cal_eba_expansion_addition_height"], gt->cal_eba_expansion_addition_height_);
        StringToNum(config_map_["cal_lva_expansion_addition_height"], gt->cal_lva_expansion_addition_height_);
        StringToNum(config_map_["eba_expansion_padding"], gt->eba_expansion_padding_);

        DLOG(INFO) << "end config";

        return true;
    }


    string Config::get_file_path() {
        return file_path_;
    }

    void Config::set_file_path(string file_path) {
        file_path_ = file_path;
    }

    // getter
    const std::vector<string> &Config::edge_type_array() const{return edge_type_array_;}
    const std::map<string, edge_type_t> &Config::edge_type_map() const{return edge_type_map_;}
}
