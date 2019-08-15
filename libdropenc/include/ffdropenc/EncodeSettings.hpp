#pragma once

#include <filesystem>

#include "ffdropenc/Preset.hpp"

namespace ffdropenc
{
struct EncodeSettings {
    Preset::Pointer preset;
    float inputFPS{30};
    float outputFPS{30};
    std::filesystem::path outputDir;
};
}
