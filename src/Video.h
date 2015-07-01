// ffdropenc::Video
// Created by Seth Parker on 6/14/15.
//
// This class keeps track of all input videos/image sequences and things we need to know about them

#ifndef FFDROPENC_VIDEO_H
#define FFDROPENC_VIDEO_H

#include <string>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "ffdefines.h"
#include "Preset.h"

namespace ffdropenc {

    class Video {
    public:
        // Constructors
        Video();
        Video( std::string inputPath, Preset* preset, bool isImgSeq = false );

        // Operators
        bool operator < (const Video&) const;
        bool operator == (const Video&) const;

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

        int convertToSeq( std::string fps );

        int transcode();

    private:
        // input location
        boost::filesystem::path _inputPath; // where the file is

        // output location
        boost::filesystem::path _outputDir; // where the file is going
        std::string _outputFileName; // what it will be called
        std::string _outputSuffix; // what will be appended to the filename
        std::string _outputExt; // what its extension will be
        bool _appendSuffix; // do we want to append _outputSuffix to the filename?

        // metadata
        int _analyze(); // fills out the metadata

        // progress tracking
        double _fps; // frames per second
        unsigned long _frames; // number of frames
        unsigned long _duration; // length in ms
        double _progress; // progress of encoding between 0.0 and 1.0

        // image sequence
        bool _isImgSeq; // is an image sequence?
        unsigned long _startingIndex; // what's the first number in the seq? (e.g. 0, 50, 121?)
        std::string _outputFPS; // the desired frame rate for the output file

        // encoding parameters
        Preset* _preset; // A pointer to the preset we're using
        std::string _command(); // returns the transcoding command that is run by this.transcode();
        bool _overwrite; // overwrite the output file?

        // debug log stuff
        boost::filesystem::path _logDir; // where the debug log will be saved
        bool _cleanupLog; // delete the log when encoding is finished?

        bool _transcoded;

    }; // Video

} // namespace ffdropenc

#endif //FFDROPENC_VIDEO_H
