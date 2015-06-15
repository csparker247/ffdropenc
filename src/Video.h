// ffdropenc::Video
// Created by Seth Parker on 6/14/15.
//
// This class keeps track of all input videos/image sequences and things we need to know about them

#ifndef FFDROPENC_VIDEO_H
#define FFDROPENC_VIDEO_H

#include <boost/filesystem.hpp>

namespace ffdropenc {

class Video {
public:
    Video( boost::filesystem::path inputPath, unsigned preset = 0 );

    // Accessors
    boost::filesystem::path inputPath() { return _inputPath; };
    boost::filesystem::path outputPath();
    double progress() { return _progress; };

    // Modifiers
    int setInputPath();
    int setOutputDir();
    int setOutputFilename();
    int setOutputSuffix();
    int setOutputExt();

    //int analyze();

private:
    // input location
    boost::filesystem::path _inputPath; // where the file is

    // output location
    // _outputDir + _outputFileName + _outputSuffix + _outputExt == output file and location
    boost::filesystem::path _outputDir; // where the file is going
    std::string _outputFileName; // what it will be called
    std::string _outputSuffix; // what will be appended to the filename
    std::string _outputExt; // what its extension will be
    bool _appendSuffix; // do we want to append _outputSuffix to the filename?

    // progress tracking
    double _fps; // frames per second
    unsigned long _frames; // number of frames
    double _progress; // progress of encoding between 0.0 and 1.0

    // image sequence
    bool _isImgSeq; // is an image sequence?
    unsigned long _startingIndex; // what's the first number in the seq? (e.g. 000, 050, 121?)

    // encoding parameters
    unsigned _preset; // the index of the preset we want to use for encoding

    // debug log stuff
    boost::filesystem::path _logDir; // where the debug log will be saved
    bool _cleanupLog; // delete the log when encoding is finished?

    bool _transcoded;

}; // Video

} // namespace ffdropenc

#endif //FFDROPENC_VIDEO_H
