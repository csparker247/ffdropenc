#pragma once

#include <filesystem>
#include <vector>

#include "ffdropenc/QueueItem.hpp"

namespace ffdropenc
{

using ExtensionList = std::vector<std::string>;

bool FileExtensionFilter(
    const std::filesystem::path& path, const ExtensionList& exts);

std::vector<std::filesystem::path> FilterFileList(
    const std::vector<std::filesystem::path>& fileList);
}
