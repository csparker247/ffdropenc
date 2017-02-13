//
// Created by Seth Parker on 6/19/15.
//

#include "ffdropenc/ffcommon.h"

namespace ffdropenc
{

// Load any .preset files in the given directory
int loadPresets(
    boost::filesystem::path presetDir,
    std::vector<ffdropenc::Preset*>& presetsList)
{

    if (boost::filesystem::exists(presetDir)) {

        boost::filesystem::recursive_directory_iterator presetFileIterator(
            presetDir);
        boost::filesystem::recursive_directory_iterator
            presetIterator_end;  // default construction defaults to
                                 // iterator.end() value somehow;

        while (presetFileIterator != presetIterator_end) {
            boost::filesystem::path current_fs_entry = *presetFileIterator;
            if (current_fs_entry.extension() == ".preset") {
                Preset* newFilePreset = new Preset(*presetFileIterator);
                presetsList.push_back(newFilePreset);
            }

            ++presetFileIterator;
        }
    }

    return EXIT_SUCCESS;
}

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

}  // namespace ffdropenc