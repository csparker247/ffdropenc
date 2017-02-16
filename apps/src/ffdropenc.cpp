//
// Created by Seth Parker on 6/15/15.
//

#include <boost/filesystem.hpp>

#include "ffdropenc/Preset.hpp"

namespace ffde = ffdropenc;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
    fs::path presetDir = argv[1];

    auto presets = ffde::Preset::LoadPresetDir(presetDir);

    for(auto& p : presets) {
        std::cout << p.getSettings(0) << std::endl;
    }

    return 1;
}