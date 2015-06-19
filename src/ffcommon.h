//
// Created by Seth Parker on 6/19/15.
//

#ifndef FFDROPENC_FFCOMMON_H
#define FFDROPENC_FFCOMMON_H

#include <boost/filesystem.hpp>
#include "Preset.h"

namespace ffdropenc {

    int loadPresets( boost::filesystem::path presetDir, std::vector<ffdropenc::Preset*>& presetsList );

} // namespace ffdropenc

#endif //FFDROPENC_FFCOMMON_H
