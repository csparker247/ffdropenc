#pragma once

#include <filesystem>

#include <QString>

#include "ffdropenc/Preset.hpp"

namespace ffdropenc
{
struct EncodeSettings {
    Preset::Pointer preset;
    QString inputFPS{"30"};
    QString outputFPS{"30"};
    std::filesystem::path outputDir;
};
}  // namespace ffdropenc
