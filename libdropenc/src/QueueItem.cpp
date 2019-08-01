#include "ffdropenc/QueueItem.hpp"

#include "ffdropenc/Filesystem.hpp"
#include "ffdropenc/json.hpp"

using namespace ffdropenc;
namespace fs = std::filesystem;

// Extension filters for image sequences.
static const ExtensionList FF_VID_EXTENSIONS = {
    "AVI", "GIF",  "MOV", "MP4", "M4A", "3GP", "264", "H264", "M4V",
    "MKV", "MPEG", "MPG", "MTS", "MXF", "OGG", "VOB", "WMV"};
static const ExtensionList FF_IMG_EXTENSIONS = {"DPX", "JPG",  "JPEG", "PNG",
                                                "TIF", "TIFF", "TGA"};

// Constructors
QueueItem::QueueItem(fs::path path, Preset::Pointer preset)
    : progress_(0.0)
    , preset_(preset)
    , overwrite_(true)
    , logCleanup_(true)
    , transcoded_(false)
{
    // set the source file
    inputPath_ = std::filesystem::canonical(path);

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
}

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
std::filesystem::path QueueItem::outputPath()
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

    startingIndex_ = std::stoull(seqNumber);
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
        if (last_char != '-' && last_char != '_' && last_char != ' ') {
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
QueueItem::Type QueueItem::DetermineType(const fs::path& p)
{
    if (FileExtensionFilter(p, FF_VID_EXTENSIONS)) {
        return QueueItem::Type::Video;
    } else if (FileExtensionFilter(p, FF_IMG_EXTENSIONS)) {
        return QueueItem::Type::Sequence;
    } else {
        return QueueItem::Type::Undefined;
    }
}