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
const std::array<std::string, 17 > FF_VID_EXTENSIONS = { "AVI", "GIF", "MOV", "MP4", "M4A", "3GP", "264", "H264", "M4V", "MKV", "MPEG", "MPG", "MTS", "MXF", "OGG", "VOB", "WMV"};
const std::array<std::string,  7 > FF_IMG_EXTENSIONS = { "DPX", "JPG", "JPEG", "PNG", "TIF", "TIFF", "TGA" };

namespace ffdropenc {

    int loadPresets( boost::filesystem::path presetDir, std::vector<ffdropenc::Preset*>& presetsList );

    bool isVideo( boost::filesystem::path file );

    bool isImgSequence( boost::filesystem::path file );

    void ExpandDirs(std::vector<std::string>& fileList, std::vector<std::string>& resolvedFiles);

} // namespace ffdropenc

#endif //FFDROPENC_FFCOMMON_H
