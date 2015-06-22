//
// Created by Seth Parker on 6/19/15.
//

#ifndef FFDROPENC_FFCOMMON_H
#define FFDROPENC_FFCOMMON_H

#include <array>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "ffdefines.h"
#include "Preset.h"

// Extension filters for image sequences.
// To-Do: Move these to a config so there's no need to recompile
const std::array<std::string, 7> FF_IMG_EXTENSIONS = { "DPX", "JPG", "JPEG", "PNG", "TIF", "TIFF", "TGA" };

namespace ffdropenc {

    void RemoveDirs(std::vector<std::string>& fileList, std::vector<std::string>& resolvedFiles);

    int loadPresets( boost::filesystem::path presetDir, std::vector<ffdropenc::Preset*>& presetsList );

    bool isImgSequence( boost::filesystem::path file );

} // namespace ffdropenc

#endif //FFDROPENC_FFCOMMON_H
