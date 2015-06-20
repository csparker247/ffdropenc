//
// Created by Seth Parker on 6/14/15.
//

#include "Video.h"
#include "Preset.h"

namespace ffdropenc {

    // Constructors
    Video::Video ( boost::filesystem::path inputPath, Preset* preset ) {

        // set preset first
        _preset = preset;

        // set the source file
        _inputPath = boost::filesystem::canonical(inputPath);

        // set the output file
        _outputDir = inputPath.parent_path();
        _outputFileName = inputPath.stem().string();
        _outputSuffix = "_Converted";
        _outputExt = inputPath.extension().string();
        _appendSuffix = true;

        // To-Do: analyze video
        // _analyze();

        // set progress
        _progress = 0.0;

        // logs
        _logDir = _outputDir; // Defaults to same as output file
        _cleanupLog = true;

        _transcoded = false;

    };

    // Construct the full output path from all of its requisite parts
    // this.outputPath() == _outputDir + _outputFileName + _outputSuffix + _outputExt
    boost::filesystem::path Video::outputPath() {
        boost::filesystem::path concatenatedPath = _outputDir;
        std::string outputFullFilename = _outputFileName;
        if (_appendSuffix) outputFullFilename += _outputSuffix;
        outputFullFilename += _outputExt;
        concatenatedPath /= boost::filesystem::path(outputFullFilename);

        return concatenatedPath;
    }

    int Video::transcode() {
        std::cout << _command() << std::endl;
        return EXIT_SUCCESS;
    }

    // Construct the transcoding command
    std::string Video::_command() {
        std::string command = "ffmpeg";

        if (_isImgSeq) command.append( " -r " + _outputFPS );

        command.append( " -i \"" + _inputPath.string() + "\"" );

        command.append( _preset->getSettings(0) );

        //To-Do: Overwrite output file?

        command.append( " " + this->outputPath().string() );

        return command;
    }

} // namespace ffdropenc