#include "ffdropenc/QueueItem.hpp"

#include <nlohmann/json.hpp>

#include "ffdropenc/Filesystem.hpp"

using namespace ffdropenc;
namespace fs = std::filesystem;

// Extension filters for image sequences.
static const ExtensionList FF_VID_EXTENSIONS = {
    "AVI", "GIF",  "MOV", "MP4", "M4A", "3GP", "264", "H264", "M4V",
    "MKV", "MPEG", "MPG", "MTS", "MXF", "OGG", "VOB", "WMV"};
static const ExtensionList FF_IMG_EXTENSIONS = {"DPX", "JPG",  "JPEG", "PNG",
                                                "TIF", "TIFF", "TGA"};

// Constructors
QueueItem::QueueItem(const std::filesystem::path& path, Preset::Pointer preset)
    : inputPath_{path}, preset_{std::move(preset)}
{
    // set the output file
    outputDir_ = inputPath_.parent_path();
    outputFileName_ = inputPath_.stem();

    // Type stuff
    type_ = determine_type_(inputPath_);
    switch (type_) {
        case Type::Video:
            // analyze_();
            break;
        case Type::Sequence:
            convert_to_seq_("30000/1001");
            break;
        case Type::Undefined:
            throw std::runtime_error("Unable to determine type");
    }
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
std::filesystem::path QueueItem::outputPath() const
{
    auto path = outputFileName_;
    path += preset_->getSuffix(0);
    path += preset_->getExtension(0);

    return outputDir_ / path;
}

// Convert this Item into an Img Sequence
void QueueItem::convert_to_seq_(const std::string& fps)
{
    outputFPS_ = fps;

    // to-do: This only works if numerical pattern is at end of filename
    auto stem = inputPath_.stem().string();
    auto last_letter_pos = stem.find_last_not_of("0123456789");
    auto fileName = stem.substr(0, last_letter_pos + 1);
    auto seqNumber = stem.substr(last_letter_pos + 1);

    startingIndex_ = std::stoull(seqNumber);
    outputFileName_ = (fileName.empty())
                          ? inputPath_.parent_path().stem().string()
                          : fileName;

    // Convert seqNumber to the %nd format
    seqNumber = std::to_string(seqNumber.length());
    if (seqNumber.length() < 2) {
        seqNumber.insert(0, "0");
    }
    seqNumber = "%" + seqNumber + "d";

    fileName += seqNumber + inputPath_.extension().string();

    inputPath_ = inputPath_.parent_path() / fileName;
}

// Analyze

// Construct the transcoding command
QStringList QueueItem::encodeArguments() const
{
    QStringList args;

    // Settings for img sequences
    if (type_ == Type::Sequence) {
        args << "-r" << QString::fromStdString(outputFPS_);
        args << "-start_number" << QString::number(startingIndex_);
    }

    // The input file
    args << "-i" << QString::fromStdString(inputPath_.string());

    // Get the settings for the first output of the selected preset
    for (size_t output = 0; output < preset_->numberOfOutputs(); ++output) {
        args << preset_->getSettings(output);

        // Setting to overwrite the output file if it exists
        if (overwrite_) {
            args << "-y";
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
        args << QString::fromStdString(path.string());
    }

    return args;
}

// Check if the file is an approved video format
QueueItem::Type QueueItem::determine_type_(const fs::path& p)
{
    if (FileExtensionFilter(p, FF_VID_EXTENSIONS)) {
        return QueueItem::Type::Video;
    } else if (FileExtensionFilter(p, FF_IMG_EXTENSIONS)) {
        return QueueItem::Type::Sequence;
    } else {
        return QueueItem::Type::Undefined;
    }
}