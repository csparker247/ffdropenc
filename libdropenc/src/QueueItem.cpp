#include "ffdropenc/QueueItem.hpp"

#include <cmath>
#include <regex>

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
QueueItem::QueueItem(std::filesystem::path path, EncodeSettings settings)
    : inputPath_{std::move(path)}, preset_{std::move(settings.preset)}
{
    // set the output file
    if (settings.outputDir.empty()) {
        outputDir_ = inputPath_.parent_path();
    } else {
        outputDir_ = settings.outputDir;
    }
    outputFileName_ = inputPath_.stem();

    // Type stuff
    type_ = determine_type_(inputPath_);
    switch (type_) {
        case Type::Video:
            break;
        case Type::Sequence:
            inputFPS_ = settings.inputFPS;
            outputFPS_ = settings.outputFPS;
            convert_to_seq_();
            break;
        case Type::Undefined:
            throw std::runtime_error("Unable to determine type");
    }
}

QueueItem::Pointer QueueItem::New() { return std::make_shared<QueueItem>(); }

QueueItem::Pointer QueueItem::New(
    std::filesystem::path path, EncodeSettings settings)
{
    return std::make_shared<QueueItem>(path, settings);
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
void QueueItem::convert_to_seq_()
{
    // TODO: This only works if numerical pattern is at end of filename
    auto stem = inputPath_.stem().string();
    auto last_letter_pos = stem.find_last_not_of("0123456789");
    auto fileName = stem.substr(0, last_letter_pos + 1);
    auto seqNumber = stem.substr(last_letter_pos + 1);

    // Set the output filename
    outputFileName_ = (fileName.empty())
                          ? inputPath_.parent_path().stem().string()
                          : fileName;

    // Get list of all paths matching prefix in parent path
    std::regex prefix{fileName + "([0-9]+)"};
    std::smatch match;
    startingIndex_ = std::stoull(seqNumber);
    for (const auto& it : fs::directory_iterator(inputPath_.parent_path())) {
        // Skip entries that aren't files
        if (not it.is_regular_file()) {
            continue;
        }

        //
        auto fn = fs::path(it).stem().string();
        if (std::regex_match(fn, match, prefix)) {
            if (match.size() != 2) {
                continue;
            }
            std::cout << match[0] << " " << match[1] << std::endl;
            // Get minimum and maximum sequence number from list
            size_t idx{std::stoull(match[1])};
            startingIndex_ = std::min(startingIndex_, idx);
        }
    }

    // Convert seqNumber to the %nd format
    seqNumber = std::to_string(seqNumber.length());
    if (seqNumber.length() < 2) {
        seqNumber.insert(0, "0");
    }
    seqNumber = "%" + seqNumber + "d";

    // Final file name
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
        args << "-framerate" << inputFPS_;
        args << "-start_number" << QString::number(startingIndex_);
    }

    // The input file
    args << "-i" << QString::fromStdString(inputPath_.string());

    // Output framerate
    if (type_ == Type::Sequence && inputFPS_ != outputFPS_) {
        args << "-r" << outputFPS_;
    }

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