//
// Created by Seth Parker on 6/14/15.
//

#include "Video.h"

namespace ffdropenc {

    // Constructors
    Video::Video () {
        _inputPath = "";

        _isImgSeq = false;

        _progress = 0.0;

        _transcoded = false;
    }

    Video::Video ( std::string inputPath, Preset* preset, bool isImgSeq ) {

        // set preset first
        _preset = preset;

        // set the source file
        _inputPath = boost::filesystem::canonical(inputPath);

        // set the output file
        _outputDir = _inputPath.parent_path();
        _outputFileName = _inputPath.stem().string();
        _outputSuffix = "_Converted";
        _outputExt = _inputPath.extension().string();
        _appendSuffix = true;
        _overwrite = true;

        // Image sequence stuff
        _isImgSeq = isImgSeq;
        if (_isImgSeq) convertToSeq("30000/1001");
        else _analyze(); // _analyze() needs some work for image sequences

        // set progress
        _progress = 0.0;

        // logs
        _logDir = _outputDir; // Defaults to same as output file
        _cleanupLog = true;

        _transcoded = false;

    };

    // Operators
    bool Video::operator < (const Video& file) const {
        if ( _inputPath == file._inputPath && _isImgSeq == file._isImgSeq )
            return ( _startingIndex < file._startingIndex );
        else
            return ( _inputPath < file._inputPath );
    }

    bool Video::operator == (const Video& file) const {
        return ( _inputPath == file._inputPath );
    }

    // Construct the full output path from all of its requisite parts
    // this.outputPath() == _outputDir + _outputFileName + _outputSuffix + _outputExt
    // To-do: Handle custom names, suffixes, extensions
    boost::filesystem::path Video::outputPath() {
        boost::filesystem::path concatenatedPath = _outputDir;
        std::string outputFullFilename = _outputFileName;
        if (_appendSuffix) outputFullFilename += _outputSuffix;
        outputFullFilename += _outputExt;
        concatenatedPath /= boost::filesystem::path(outputFullFilename);

        return concatenatedPath;
    }

    // Convert this video into an Img Sequence
    int Video::convertToSeq( std::string fps ) {
        _isImgSeq = FF_IS_IMG_SEQ;
        _outputFPS = fps;

        int last_letter_pos = _inputPath.stem().string().find_last_not_of("0123456789");
        std::string fileName = _inputPath.stem().string().substr(0, last_letter_pos + 1);
        std::string seqNumber = _inputPath.stem().string().substr(last_letter_pos + 1);

        _startingIndex = boost::lexical_cast<unsigned long>( seqNumber );
        _outputFileName = fileName;

        // Convert seqNumber to the %nd format
        seqNumber = std::to_string(seqNumber.length());
        if (seqNumber.length() < 2) seqNumber.insert(0, "0");
        seqNumber = "%" + seqNumber + "d";

        fileName += seqNumber + _inputPath.extension().string();

        _inputPath = boost::filesystem::path(_inputPath.parent_path().string() + "/" + fileName);

        return EXIT_SUCCESS;
    }

    // Do the transcoding
    int Video::transcode() {

        // Don't transcode if the output will overwrite the input
        if ( _inputPath.string() == this->outputPath().string() )
            return FF_ERR_INPUT_OVERWRITE;

        std::cout << _command() << std::endl;
        return EXIT_SUCCESS;
    }

    // Analyze
    int Video::_analyze() {
        int fds[2];
        pid_t pid;
        pipe(fds);

        std::string json;

        // To-Do: Make sure the pipe opened
        pid = fork();
        if (pid == (pid_t) 0) {
            // The child process
            close(fds[0]); // Close the read end of the pipe
            dup2 (fds[1], STDOUT_FILENO); // Write to stdout
            // To-Do: Make sure we're using our ffprobe instead of the one in the path
            execlp("ffprobe", "ffprobe", "-loglevel", "quiet", "-of", "json=c=1", "-show_streams", "-i", _inputPath.c_str(), NULL );
            close(fds[1]);
        }
        else {
            // The parent process
            close(fds[1]); // Close the write end of the pipe
            dup2 (fds[0], STDIN_FILENO); // Read from stdin. Note: This is terrible. Use boost stream?
            std::string line;
            while (std::getline(std::cin, line)) {
                json += line; // concat every line we get from cin into one string
                std::cerr << line << std::endl;
            }
            waitpid (pid, NULL, 0); // Wait for the child process to finish
        }

        // Parse the results with picojson
        picojson::value results;
        std::string err = picojson::parse( results, json );
        if (! err.empty()) {
            std::cerr << _inputPath << " " << err << std::endl;
        }

        // ffprobe returns an array of stream objects. We always hope that the first one is the video track.
        // To-Do: check the "codec_type" to determine what we're looking at.
        if ( results.is<picojson::object>() ) {
            picojson::value video_info = results.get("streams").get<picojson::array>()[0];

            // To-Do: Get fps, frame size, codec, # of streams, and anything else we need
            if (video_info.get("duration").is<std::string>())
                _duration = boost::lexical_cast<double>(video_info.get("duration").get<std::string>());
        }

        return EXIT_SUCCESS;
    }

    // Construct the transcoding command
    std::string Video::_command() {

        // The basic transcoder program
        std::string command = FF_TRANSCODER;

        // Settings for img sequences
        if ( _isImgSeq ) command.append( " -r " + _outputFPS + " -start_number " + std::to_string(_startingIndex) );

        // The input file
        command.append( " -i \"" + _inputPath.string() + "\"" );

        // Get the settings for the first output of the selected preset
        for ( int output = 0; output < _preset->numberOfOutputs(); ++output ) {
            command.append(_preset->getSettings(output));

            // Setting to overwrite the output file if it exists
            if (_overwrite) command.append(" -y");

            // Add the suffix
            std::string last_char = &_outputFileName.back();
            if ((last_char != "-") && (last_char != "_") && (last_char != " "))
                _outputSuffix = "-" + _preset->getSuffix(output);
            else
                _outputSuffix = _preset->getSuffix(output);

            // Add the file extension
            _outputExt = _preset->getExtension(output);

            // Add the output path
            command.append(" \"" + this->outputPath().string() + "\"");
            // end this output
        }

        return command;
    }

} // namespace ffdropenc