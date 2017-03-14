#pragma once

#include <vector>
#include <boost/filesystem.hpp>

#include "ffdropenc/QueueItem.hpp"

namespace ffdropenc
{

namespace filesystem
{

std::vector<boost::filesystem::path> FilterFileList(
    const std::vector<boost::filesystem::path>& fileList);
}
}
