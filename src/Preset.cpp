//
// Created by Seth Parker on 6/17/15.
//

#include "Preset.h"

namespace ffdropenc {

    Preset::Preset ( boost::filesystem::path path ) {

        _cfg.readFile( path.c_str() );
        std::string _name = _cfg.lookup("listname");
        std::string _shortname = _cfg.lookup("consolename");

    }

} //namespace ffdropenc