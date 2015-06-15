//
// Created by Seth Parker on 6/14/15.
//

#include "Video.h"

namespace ffdropenc {

// Constructors
Video::Video ( boost::filesystem::path inputPath, unsigned preset ) {

    // set preset first
    _preset = preset; // defaults to first in Presets vector

    // set the source file
    _inputPath = inputPath;

    // set the output file
    _outputDir = inputPath.parent_path();
    _outputFileName = inputPath.stem().string();
    _outputSuffix = "_Converted";
    _outputExt = inputPath.extension().string();
    _appendSuffix = true;

    /* To-Do: analyze video
     * _fps =
     * _frames =
     * _isImgSeq =
     * _startingIndex =
    */

    // set progress
    _progress = 0.0;

    // logs
    _logDir = _outputDir; // Defaults to same as output file
    _cleanupLog = true;

    _transcoded = false;

};

} // namespace ffdropenc