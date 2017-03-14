//
// Created by Seth Parker on 6/14/15.
//

#include "ffdropenc/QueueItem.hpp"

using namespace ffdropenc;
namespace fs = boost::filesystem;

// Constructors
QueueItem::QueueItem(std::string inputPath, Preset* preset, bool isImgSeq)
{

    // set preset first
    _preset = preset;

    // set the source file
    inputPath_ = boost::filesystem::canonical(inputPath);

    // set the output file
    outputDir_ = inputPath_.parent_path();
    outputFileName_ = inputPath_.stem().string();
    _outputSuffix = "_Converted";
    _outputExt = inputPath_.extension().string();
    _appendSuffix = true;
    _overwrite = true;

    // Image sequence stuff
    _isImgSeq = isImgSeq;
    if (_isImgSeq)
        convertToSeq("30000/1001");
    else
        analyze_();  // analyze_() needs some work for image sequences

    // set progress
    _progress = 0.0;

    // logs
    _logDir = outputDir_;  // Defaults to same as output file
    _cleanupLog = true;

    _transcoded = false;
};

// Operators
bool QueueItem::operator<(const QueueItem& file) const
{
    if (inputPath_ == file.inputPath_ && _isImgSeq == file._isImgSeq)
        return (_startingIndex < file._startingIndex);
    else
        return (inputPath_ < file.inputPath_);
}

bool QueueItem::operator==(const QueueItem& file) const
{
    return (inputPath_ == file.inputPath_);
}

// Construct the full output path from all of its requisite parts
// this.outputPath() == outputDir_ + outputFileName_ + _outputSuffix +
// _outputExt
// To-do: Handle custom names, suffixes, extensions
boost::filesystem::path QueueItem::outputPath()
{
    boost::filesystem::path concatenatedPath = outputDir_;
    std::string outputFullFilename = outputFileName_;
    if (_appendSuffix)
        outputFullFilename += _outputSuffix;
    outputFullFilename += _outputExt;
    concatenatedPath /= boost::filesystem::path(outputFullFilename);

    return concatenatedPath;
}

// Convert this video into an Img Sequence
int QueueItem::convertToSeq(std::string fps)
{
    _isImgSeq = FF_IS_IMG_SEQ;
    _outputFPS = fps;

    int last_letter_pos =
        inputPath_.stem().string().find_last_not_of("0123456789");
    std::string fileName =
        inputPath_.stem().string().substr(0, last_letter_pos + 1);
    std::string seqNumber =
        inputPath_.stem().string().substr(last_letter_pos + 1);

    _startingIndex = boost::lexical_cast<unsigned long>(seqNumber);
    outputFileName_ = fileName;

    // Convert seqNumber to the %nd format
    seqNumber = std::to_string(seqNumber.length());
    if (seqNumber.length() < 2)
        seqNumber.insert(0, "0");
    seqNumber = "%" + seqNumber + "d";

    fileName += seqNumber + inputPath_.extension().string();

    inputPath_ = boost::filesystem::path(
        inputPath_.parent_path().string() + "/" + fileName);

    return EXIT_SUCCESS;
}

// Do the transcoding
int QueueItem::transcode()
{

    // Don't transcode if the output will overwrite the input
    if (inputPath_.string() == this->outputPath().string())
        return FF_ERR_INPUT_OVERWRITE;

    std::cout << _command() << std::endl;
    return EXIT_SUCCESS;
}

// Analyze
int QueueItem::analyze_()
{
    int fds[2];
    pid_t pid;
    pipe(fds);

    std::string json;

    // To-Do: Make sure the pipe opened
    pid = fork();
    if (pid == (pid_t)0) {
        // The child process
        close(fds[0]);                // Close the read end of the pipe
        dup2(fds[1], STDOUT_FILENO);  // Write to stdout
        // To-Do: Make sure we're using our ffprobe instead of the one in the
        // path
        execlp(
            "ffprobe", "ffprobe", "-loglevel", "quiet", "-of", "json=c=1",
            "-show_streams", "-i", inputPath_.c_str(), NULL);
        close(fds[1]);
    } else {
        // The parent process
        close(fds[1]);               // Close the write end of the pipe
        dup2(fds[0], STDIN_FILENO);  // Read from stdin. Note: This is terrible.
                                     // Use boost stream?
        std::string line;
        while (std::getline(std::cin, line)) {
            json += line;  // concat every line we get from cin into one string
            std::cerr << line << std::endl;
        }
        waitpid(pid, NULL, 0);  // Wait for the child process to finish
    }

    // Parse the results with picojson
    picojson::value results;
    std::string err = picojson::parse(results, json);
    if (!err.empty()) {
        std::cerr << inputPath_ << " " << err << std::endl;
    }

    // ffprobe returns an array of stream objects. We always hope that the first
    // one is the video track.
    // To-Do: check the "codec_type" to determine what we're looking at.
    if (results.is<picojson::object>()) {
        picojson::value video_info =
            results.get("streams").get<picojson::array>()[0];

        // To-Do: Get fps, frame size, codec, # of streams, and anything else we
        // need
        if (video_info.get("duration").is<std::string>())
            _duration = boost::lexical_cast<double>(
                video_info.get("duration").get<std::string>());
    }

    return EXIT_SUCCESS;
}

// Construct the transcoding command
std::string QueueItem::_command()
{

    // The basic transcoder program
    std::string command = FF_TRANSCODER;

    // Settings for img sequences
    if (_isImgSeq)
        command.append(
            " -r " + _outputFPS + " -start_number " +
            std::to_string(_startingIndex));

    // The input file
    command.append(" -i \"" + inputPath_.string() + "\"");

    // Get the settings for the first output of the selected preset
    for (int output = 0; output < _preset->numberOfOutputs(); ++output) {
        command.append(_preset->getSettings(output));

        // Setting to overwrite the output file if it exists
        if (_overwrite)
            command.append(" -y");

        // Add the suffix
        std::string last_char = &outputFileName_.back();
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

// Check if the file is an approved video format
QueueItem::Type QueueItem::DetermineType(std::string ext)
{

    // Check if video
    for (auto e : FF_VID_EXTENSIONS) {
        if (ext == e) {
            return QueueItem::Type::Video;
        }
    }

    // Check if image sequence
    for (auto e : FF_IMG_EXTENSIONS) {
        if (ext == e) {
            return QueueItem::Type::Sequence;
        }
    }

    // Else we have no idea
    return QueueItem::Type::Undefined;
}