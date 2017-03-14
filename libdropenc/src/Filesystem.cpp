#include "ffdropenc/Filesystem.hpp"

#include <boost/algorithm/string.hpp>

// Extension filters for image sequences.
static const std::vector<std::string> FF_VID_EXTENSIONS = {
    "AVI", "GIF",  "MOV", "MP4", "M4A", "3GP", "264", "H264", "M4V",
    "MKV", "MPEG", "MPG", "MTS", "MXF", "OGG", "VOB", "WMV"};
static const std::vector<std::string> FF_IMG_EXTENSIONS = {
    "DPX", "JPG", "JPEG", "PNG", "TIF", "TIFF", "TGA"};

using namespace ffdropenc;
namespace fs = boost::filesystem;

// Removes directory entries from a vector of file paths by recursively
// expanding them
std::vector<fs::path> filesystem::FilterFileList(
    const std::vector<fs::path>& fileList)
{
    std::vector<fs::path> resolvedFiles;

    for (auto file : fileList) {

        // Skip if it doesn't exist
        if (!boost::filesystem::exists(file)) {
            continue;
        }

        // Handle regular files
        else if (boost::filesystem::is_regular_file(file)) {
            resolvedFiles.emplace_back(file);
        }

        // Handle directories
        else if (boost::filesystem::is_directory(file)) {

            boost::filesystem::recursive_directory_iterator dir(file);
            boost::filesystem::recursive_directory_iterator dir_end;

            while (dir != dir_end) {
                boost::filesystem::path dir_entry(*dir);
                if (is_regular_file(dir_entry)) {
                    resolvedFiles.emplace_back(dir_entry);
                }
                ++dir;
            }
        }
    }

    return resolvedFiles;
}