#pragma once

#include <vector>
#include <boost/filesystem.hpp>

namespace ffdropenc
{

bool isVideo(boost::filesystem::path file);

bool isImgSequence(boost::filesystem::path file);

void ExpandDirs(
    std::vector<std::string>& fileList,
    std::vector<std::string>& resolvedFiles);

}  // namespace ffdropenc
