#include "ffdropenc/File.hpp"

#include <boost/algorithm/string.hpp>

// Extension filters for image sequences.
static constexpr std::vector<std::string> FF_VID_EXTENSIONS = {
    "AVI", "GIF",  "MOV", "MP4", "M4A", "3GP", "264", "H264", "M4V",
    "MKV", "MPEG", "MPG", "MTS", "MXF", "OGG", "VOB", "WMV"};
static constexpr std::vector<std::string> FF_IMG_EXTENSIONS = {
    "DPX", "JPG", "JPEG", "PNG", "TIF", "TIFF", "TGA"};

static constexpr bool FF_IS_IMG_SEQ = true;
static constexpr bool FF_IS_VIDEO = true;

using namespace ffdropenc;

// Check if the file is an approved video format
bool isVideo(boost::filesystem::path file)
{

    std::string extension(
        boost::to_upper_copy<std::string>(file.extension().string()));

    for (int i = 0; i < FF_VID_EXTENSIONS.size(); ++i) {
        if (extension == "." + FF_VID_EXTENSIONS[i])
            return FF_IS_VIDEO;
    }

    return false;
}

// Check if this is an image file sequence
bool isImgSequence(boost::filesystem::path file)
{

    std::string extension(
        boost::to_upper_copy<std::string>(file.extension().string()));

    for (int i = 0; i < FF_IMG_EXTENSIONS.size(); ++i) {
        if (extension == "." + FF_IMG_EXTENSIONS[i])
            return FF_IS_IMG_SEQ;
    }

    return false;
}

// Removes directory entries from a vector of file paths by recursively
// expanding them
void ExpandDirs(
    std::vector<std::string>& fileList, std::vector<std::string>& resolvedFiles)
{
    std::vector<std::string>::iterator file = fileList.begin();
    while (file != fileList.end()) {

        // Skip if it doesn't exist
        if (!boost::filesystem::exists(*file))
            continue;

        // Handle regular files
        else if (boost::filesystem::is_regular_file(*file))
            resolvedFiles.push_back(*file);

        // Handle directories
        else if (boost::filesystem::is_directory(*file)) {

            boost::filesystem::recursive_directory_iterator dir(*file);
            boost::filesystem::recursive_directory_iterator dir_end;

            while (dir != dir_end) {
                boost::filesystem::path dir_entry(*dir);
                if (is_regular_file(dir_entry))
                    resolvedFiles.push_back(dir_entry.string());
                ++dir;
            }

            file = fileList.erase(file);
            continue;
        }

        ++file;
    }
}