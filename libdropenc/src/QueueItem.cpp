#include "ffdropenc/QueueItem.hpp"

#include <boost/algorithm/string.hpp>

#include "external/json.hpp"

// Extension filters for image sequences.
static const std::vector<std::string> FF_VID_EXTENSIONS = {
    "AVI", "GIF",  "MOV", "MP4", "M4A", "3GP", "264", "H264", "M4V",
    "MKV", "MPEG", "MPG", "MTS", "MXF", "OGG", "VOB", "WMV"};
static const std::vector<std::string> FF_IMG_EXTENSIONS = {
    "DPX", "JPG", "JPEG", "PNG", "TIF", "TIFF", "TGA"};

using namespace ffdropenc;
namespace fs = boost::filesystem;

using json = nlohmann::json;

// Constructors
QueueItem::QueueItem(fs::path path, Preset::Pointer preset)
    : progress_(0.0)
    , preset_(preset)
    , overwrite_(true)
    , logCleanup_(true)
    , transcoded_(false)
{
    // set the source file
    inputPath_ = boost::filesystem::canonical(path);

    // set the output file
    outputDir_ = inputPath_.parent_path();
    outputFileName_ = inputPath_.stem();

    // Type stuff
    type_ = DetermineType(path.extension());
    switch (type_) {
        case Type::Video:
            // analyze_();
            break;
        case Type::Sequence:
            convertToSeq("30000/1001");
            break;
        case Type::Undefined:
            throw std::runtime_error("Unable to determine type");
    }

    // logs
    logPath_ = outputDir_;  // Defaults to same as output file
};

// Operators
bool QueueItem::operator<(const QueueItem& file) const
{
    if (inputPath_ == file.inputPath_ && type_ == file.type_)
        return (startingIndex_ < file.startingIndex_);
    else
        return (inputPath_ < file.inputPath_);
}

bool QueueItem::operator==(const QueueItem& file) const
{
    return (inputPath_ == file.inputPath_);
}

// Construct the full output path from all of its requisite parts
boost::filesystem::path QueueItem::outputPath()
{
    auto path = outputFileName_;
    path += preset_->getSuffix(0);
    path += preset_->getExtension(0);

    return outputDir_ / path;
}

// Convert this video into an Img Sequence
void QueueItem::convertToSeq(std::string fps)
{
    outputFPS_ = fps;

    auto stem = inputPath_.stem().string();
    auto last_letter_pos = stem.find_last_not_of("0123456789");
    auto fileName = stem.substr(0, last_letter_pos + 1);
    auto seqNumber = stem.substr(last_letter_pos + 1);

    startingIndex_ = boost::lexical_cast<uint64_t>(seqNumber);
    outputFileName_ = fileName;

    // Convert seqNumber to the %nd format
    seqNumber = std::to_string(seqNumber.length());
    if (seqNumber.length() < 2) {
        seqNumber.insert(0, "0");
    }
    seqNumber = "%" + seqNumber + "d";

    fileName += seqNumber + inputPath_.extension().string();

    inputPath_ = inputPath_.parent_path() / fileName;
}

// Do the transcoding
void QueueItem::transcode() { std::cout << command_() << std::endl; }

// Analyze
void QueueItem::analyze_()
{
    int fds[2];
    pid_t pid;
    pipe(fds);

    json results;

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
        results << std::cin;
        waitpid(pid, NULL, 0);  // Wait for the child process to finish
    }

    // ffprobe returns an array of stream objects. We always hope that the first
    // one is the video track.
    // To-Do: check the "codec_type" to determine what we're looking at.
    if (results.empty()) {
        throw std::runtime_error("Failed to read from ffprobe");
    }

    json info = results["streams"][0];

    // To-Do: Get fps, frame size, codec, # of streams, and anything else we
    // need
    if (info["duration"].is_number_float()) {
        _duration = info["duration"].get<double>();
    }
}

// Construct the transcoding command
std::string QueueItem::command_()
{

    // The basic transcoder program
    std::string command = "ffmpeg";

    // Settings for img sequences
    if (type_ == Type::Sequence) {
        command.append(
            " -r " + outputFPS_ + " -start_number " +
            std::to_string(startingIndex_));
    }

    // The input file
    command.append(" -i \"" + inputPath_.string() + "\"");

    // Get the settings for the first output of the selected preset
    for (size_t output = 0; output < preset_->numberOfOutputs(); ++output) {
        command.append(preset_->getSettings(output));

        // Setting to overwrite the output file if it exists
        if (overwrite_) {
            command.append(" -y");
        }

        auto path = outputDir_ / outputFileName_;

        // Add the suffix
        auto last_char = outputFileName_.string().back();
        if (last_char != '-' || last_char != '_' || last_char != ' ') {
            path += "-";
        }
        path += preset_->getSuffix(output);

        // Add the file extension
        path += preset_->getExtension(output);

        // Add the output path
        command.append(" \"" + path.string() + "\"");
    }

    return command;
}

// Check if the file is an approved video format
QueueItem::Type QueueItem::DetermineType(fs::path p)
{
    auto ext = boost::to_upper_copy<std::string>(p.extension().string());

    // Check if video
    for (auto e : FF_VID_EXTENSIONS) {
        if (ext == "." + e) {
            return QueueItem::Type::Video;
        }
    }

    // Check if image sequence
    for (auto e : FF_IMG_EXTENSIONS) {
        if (ext == "." + e) {
            return QueueItem::Type::Sequence;
        }
    }

    // Else we have no idea
    return QueueItem::Type::Undefined;
}