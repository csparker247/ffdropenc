#pragma once

#include <filesystem>

#include "ffdropenc/json.hpp"

namespace ffdropenc
{

using json = nlohmann::json;

class Preset
{
public:
    using Pointer = std::shared_ptr<Preset>;

    explicit Preset(std::filesystem::path path);
    size_t numberOfOutputs() { return cfg_["outputs"].size(); }
    std::string getSettings(size_t index);
    std::string getSuffix(size_t index);
    std::string getExtension(size_t index);

    static std::vector<Preset::Pointer> LoadPresetDir(
        std::filesystem::path dir);

private:
    enum class ScaleMode { SquarePixel = 0, SizeLimited };

    // JSON rep of preset data
    json cfg_;

    std::string construct_filter_graph_(json filters);
};

}  // namespace ffdropenc