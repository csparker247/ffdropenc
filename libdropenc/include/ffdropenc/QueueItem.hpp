#pragma once

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "ffdropenc/Filesystem.hpp"
#include "ffdropenc/Preset.hpp"

namespace ffdropenc
{

class QueueItem
{
public:
    enum class Type { Undefined, Video, Sequence };

    QueueItem() = delete;
    QueueItem(boost::filesystem::path path, Preset::Pointer preset);

    // Operators
    bool operator<(const QueueItem&) const;
    bool operator==(const QueueItem&) const;

    // Accessors
    boost::filesystem::path inputPath() { return inputPath_; }
    boost::filesystem::path outputPath();
    double progress() { return progress_; }

    // Modifiers
    void setOutputDir(boost::filesystem::path d) { outputDir_ = d; }
    void setOutputFilename(boost::filesystem::path f) { outputFileName_ = f; }

    void convertToSeq(std::string fps);

    void transcode();

    static Type DetermineType(boost::filesystem::path p);

private:
    // input location
    boost::filesystem::path inputPath_;
    Type type_;

    // output directory
    boost::filesystem::path outputDir_;
    // output basename
    boost::filesystem::path outputFileName_;

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
    boost::filesystem::path logPath_;
    bool logCleanup_;

    bool transcoded_;
};

using Queue = std::vector<QueueItem>;
}
