#include <filesystem>

#include "ffdropenc/Filesystem.hpp"
#include "ffdropenc/Preset.hpp"
#include "ffdropenc/QueueItem.hpp"

namespace ffde = ffdropenc;
namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    fs::path presetDir = argv[1];
    fs::path fileDir = argv[2];

    auto presets = ffde::Preset::LoadPresetDir(presetDir);
    auto files = ffde::FilterFileList({fileDir});

    ffde::Queue queue;
    for (auto& f : files) {
        queue.emplace_back(f, presets[4]);
    }

    for (auto& q : queue) {
        q.transcode();
    }

    return EXIT_SUCCESS;
}