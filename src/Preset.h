//
// Created by Seth Parker on 6/17/15.
//

#ifndef FFDROPENC_PRESET_H
#define FFDROPENC_PRESET_H

#include "libconfig.h++"

namespace ffdropenc {

    class Preset {
    public:

    private:
        std::string _name; //Name for display purposes
        std::string _description; //Description so the user knows what they're getting into
        libconfig::Config _cfg; //The parsed preset data
    };

}


#endif //FFDROPENC_PRESET_H
