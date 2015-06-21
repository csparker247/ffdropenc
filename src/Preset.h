//
// Created by Seth Parker on 6/17/15.
//

#ifndef FFDROPENC_PRESET_H
#define FFDROPENC_PRESET_H

#include "boost/filesystem.hpp"
#include "picojson.h"

#include "ffio.h"

namespace ffdropenc {

    class Preset {
    public:
        Preset( boost::filesystem::path path );
        int numberOfOutputs() { return _cfg.get("outputs").get<picojson::array>().size(); } ;
        std::string getSettings( int outputIndex );
        std::string getSuffix( int outputIndex );
        std::string getExtension( int outputIndex );

    private:
        std::string _name; //Name for display purposes
        std::string _shortname; //For console use
        std::string _description; //Description so the user knows what they're getting into
        picojson::value _cfg; //The parsed preset data

        std::string _filterGraph( picojson::array filters );
    };

} // namespace ffdropenc


#endif //FFDROPENC_PRESET_H
