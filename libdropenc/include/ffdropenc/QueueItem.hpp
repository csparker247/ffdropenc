#pragma once

#include <filesystem>
#include <iostream>

#include <QString>
#include <QStringList>
#include <QUrl>

#include "ffdropenc/EncodeSettings.hpp"
#include "ffdropenc/Preset.hpp"

namespace ffdropenc
{

class QueueItem
{
public:
    enum class Type { Undefined, Video, Sequence };
    enum class Status { Error = -1, ReadyAnalysis, ReadyEncode, Done };
    using Pointer = std::shared_ptr<QueueItem>;

    QueueItem() = default;
    QueueItem(std::filesystem::path path, Preset::Pointer preset);

    static Pointer New();
    static Pointer New(std::filesystem::path path, Preset::Pointer preset);

    // Operators
    bool operator<(const QueueItem&) const;
    bool operator==(const QueueItem&) const;

    // Accessors
    std::filesystem::path inputPath() const { return inputPath_; }
    std::filesystem::path outputPath() const;

    void setStatus(Status s) { status_ = s; }
    Status status() const { return status_; }

    void setDuration(float d) { duration_ = d; }
    float duration() const { return duration_; }

    // Modifiers
    void setOutputDir(const std::filesystem::path& d) { outputDir_ = d; }
    void setOutputFilename(const std::filesystem::path& f)
    {
        outputFileName_ = f;
    }

    QStringList encodeArguments() const;

    Preset::Pointer preset() const { return preset_; }

private:
    // input location
    std::filesystem::path inputPath_;
    Type type_{Type::Undefined};
    Status status_{Status::ReadyEncode};

    // output directory
    std::filesystem::path outputDir_;
    // output basename
    std::filesystem::path outputFileName_;

    // progress tracking
    float fps_{-1};
    float duration_{-1};

    // image sequence
    static Type determine_type_(const std::filesystem::path& p);
    void convert_to_seq_(const std::string& fps);
    uint64_t startingIndex_{0};
    std::string outputFPS_{"30000/1001"};

    // encoding parameters
    Preset::Pointer preset_;
    bool overwrite_{true};
};
}  // namespace ffdropenc
