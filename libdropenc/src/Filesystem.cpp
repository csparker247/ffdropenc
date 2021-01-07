#include <regex>

#include "ffdropenc/Filesystem.hpp"

namespace fs = std::filesystem;

// Removes directory entries from a vector of file paths by recursively
// expanding them
std::vector<fs::path> ffdropenc::FilterFileList(
    const std::vector<fs::path>& fileList)
{
    std::vector<fs::path> resolvedFiles;

    for (const auto& file : fileList) {

        // Skip if it doesn't exist
        if (!fs::exists(file)) {
            continue;
        }

        // Handle regular files
        else if (fs::is_regular_file(file)) {
            resolvedFiles.emplace_back(file);
        }

        // Handle directories
        else if (fs::is_directory(file)) {

            fs::recursive_directory_iterator dir(file);
            fs::recursive_directory_iterator dir_end;

            while (dir != dir_end) {
                fs::path dir_entry(*dir);
                if (is_regular_file(dir_entry)) {
                    resolvedFiles.emplace_back(dir_entry);
                }
                ++dir;
            }
        }
    }

    return resolvedFiles;
}

bool ffdropenc::FileExtensionFilter(
    const fs::path& path, const ExtensionList& exts)
{
    std::string regexExpression = ".*\\.(";
    size_t count = 0;
    for (const auto& e : exts) {
        regexExpression.append(e);
        if (++count < exts.size()) {
            regexExpression.append("|");
        }
    }
    regexExpression.append(")$");

    std::regex extensions{regexExpression, std::regex::icase};
    return std::regex_match(path.extension().string(), extensions);
}