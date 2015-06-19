//
// Created by Seth Parker on 6/17/15.
//

#include "Preset.h"

namespace ffdropenc {

    Preset::Preset ( boost::filesystem::path path ) {

        std::string jsonFile = ffdropenc::file_to_string( path.c_str() );

        std::string err = picojson::parse(_cfg, jsonFile);
        if (! err.empty()) {
            std::cerr << err << std::endl;
        }

        std::string _name = _cfg.get( "listname" ).get<std::string>();
        std::string _shortname = _cfg.get( "consolename" ).get<std::string>();

    }

} //namespace ffdropenc