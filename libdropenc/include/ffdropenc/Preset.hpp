#pragma once

#include <boost/filesystem.hpp>

#include "external/json.hpp"

namespace ffdropenc
{

using json = nlohmann::json;

class Preset
{
public:
    Preset(boost::filesystem::path path);
    int numberOfOutputs() { return cfg_["outputs"].size(); }
    std::string getSettings(int outputIndex);
    std::string getSuffix(int outputIndex);
    std::string getExtension(int outputIndex);

    static std::vector<Preset> LoadPresetDir(boost::filesystem::path dir);

private:
    // JSON rep of preset data
    json cfg_;

    std::string ConstructFilterGraph(json filters);
};

}  // namespace ffdropenc