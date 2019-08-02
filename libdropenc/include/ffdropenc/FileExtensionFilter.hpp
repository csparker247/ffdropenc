#pragma once

#include <regex>
#include <string>
#include <vector>

namespace ffdropenc
{
using ExtensionList = std::vector<std::string>;

inline bool FileExtensionFilter(
    const boost::filesystem::path& path, const ExtensionList& exts)
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
}  // namespace ffdropenc
}  // namespace volcart