//
// Created by Seth Parker on 6/17/15.
//

#ifndef FFDROPENC_PRESET_H
#define FFDROPENC_PRESET_H

#include "libconfig.h++"
#include "boost/filesystem.hpp"

namespace ffdropenc {

    class Preset {
    public:
        Preset( boost::filesystem::path path );
    private:
        std::string _name; //Name for display purposes
        std::string _shortname; //For console use
        std::string _description; //Description so the user knows what they're getting into
        libconfig::Config _cfg; //The parsed preset data
    };

} // namespace ffdropenc


#endif //FFDROPENC_PRESET_H
