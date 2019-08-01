#pragma once

#include <iostream>

#include <filesystem>

#include "ffdropenc/Preset.hpp"

namespace ffdropenc
{

class QueueItem
{
public:
    enum class Type { Undefined, Video, Sequence };

    QueueItem() = delete;
    QueueItem(std::filesystem::path path, Preset::Pointer preset);

    // Operators
    bool operator<(const QueueItem&) const;
    bool operator==(const QueueItem&) const;

    // Accessors
    std::filesystem::path inputPath() { return inputPath_; }
    std::filesystem::path outputPath();
    double progress() { return progress_; }

    // Modifiers
    void setOutputDir(std::filesystem::path d) { outputDir_ = d; }
    void setOutputFilename(std::filesystem::path f) { outputFileName_ = f; }

    void convertToSeq(std::string fps);

    void transcode();

    static Type DetermineType(const std::filesystem::path& p);

private:
    // input location
    std::filesystem::path inputPath_;
    Type type_;

    // output directory
    std::filesystem::path outputDir_;
    // output basename
    std::filesystem::path outputFileName_;

    // metadata
    void analyze_();

    // progress tracking
    double _fps;            // frames per second
    unsigned long _frames;  // number of frames
    double _duration;       // length in seconds

    // progress of encoding between 0.0 and 1.0
    double progress_;

    // image sequence
    uint64_t startingIndex_;  // what's the first number in the seq? (e.g.
                              // 0, 50, 121?)
    std::string outputFPS_;   // the desired frame rate for the output file

    // encoding parameters
    Preset::Pointer preset_;
    std::string command_();
    bool overwrite_;

    // debug log stuff
    std::filesystem::path logPath_;
    bool logCleanup_;

    bool transcoded_;
};

using Queue = std::vector<QueueItem>;
}
