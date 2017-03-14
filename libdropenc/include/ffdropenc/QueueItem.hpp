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

    QueueItem() : type_(Type::Undefined), _progress(0.0), _transcoded(false) {}
    QueueItem(std::string inputPath, Preset* preset, bool isImgSeq = false);

    // Operators
    bool operator<(const QueueItem&) const;
    bool operator==(const QueueItem&) const;

    // Accessors
    boost::filesystem::path inputPath() { return inputPath_; };
    boost::filesystem::path outputPath();
    double progress() { return _progress; };

    // Modifiers
    int setInputPath();
    int setOutputDir();
    int setOutputFilename();
    int setOutputSuffix();
    int setOutputExt();

    int convertToSeq(std::string fps);

    int transcode();

    static Type DetermineType(std::string ext);

private:
    // input location
    boost::filesystem::path inputPath_;  // where the file is
    Type type_;

    // output location
    boost::filesystem::path outputDir_;       // where the file is going
    boost::filesystem::path outputFileName_;  // what it will be called
    std::string _outputSuffix;  // what will be appended to the filename
    std::string _outputExt;     // what its extension will be
    bool _appendSuffix;  // do we want to append _outputSuffix to the filename?

    // metadata
    int analyze_();  // fills out the metadata

    // progress tracking
    double _fps;            // frames per second
    unsigned long _frames;  // number of frames
    double _duration;       // length in seconds
    double _progress;       // progress of encoding between 0.0 and 1.0

    // image sequence
    bool _isImgSeq;                // is an image sequence?
    unsigned long _startingIndex;  // what's the first number in the seq? (e.g.
                                   // 0, 50, 121?)
    std::string _outputFPS;        // the desired frame rate for the output file

    // encoding parameters
    Preset* _preset;         // A pointer to the preset we're using
    std::string _command();  // returns the transcoding command that is run by
                             // this.transcode();
    bool _overwrite;         // overwrite the output file?

    // debug log stuff
    boost::filesystem::path _logDir;  // where the debug log will be saved
    bool _cleanupLog;  // delete the log when encoding is finished?

    bool _transcoded;

};  // Video

}  // namespace ffdropenc
