#pragma once

#include <exception>
#include <filesystem>
#include <iostream>

#include <QString>
#include <QStringList>
#include <QUrl>

#include "ffdropenc/EncodeSettings.hpp"
#include "ffdropenc/Preset.hpp"

namespace ffdropenc
{

class QueueItemException : public std::exception
{
public:
    explicit QueueItemException(const char* msg) : msg_(msg) {}
    explicit QueueItemException(std::string msg) : msg_(std::move(msg)) {}
    [[nodiscard]] const char* what() const noexcept override
    {
        return msg_.c_str();
    }

protected:
    std::string msg_;
};

class QueueItem
{
public:
    enum class Type { Undefined, Video, Sequence };
    enum class Status { Error = -1, ReadyAnalysis, ReadyEncode, Done };
    using Pointer = std::shared_ptr<QueueItem>;

    QueueItem() = default;
    QueueItem(std::filesystem::path path, EncodeSettings settings);

    static Pointer New();
    static Pointer New(std::filesystem::path path, EncodeSettings settings);

    // Operators
    bool operator<(const QueueItem&) const;
    bool operator==(const QueueItem&) const;

    // Accessors
    std::filesystem::path inputPath() const { return inputPath_; }
    std::filesystem::path outputPath() const;
    QString inputFileName() const { return inputPath_.filename().c_str(); }

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
    QString inputFPS_;
    QString outputFPS_;
    float duration_{-1};

    // image sequence
    static Type determine_type_(const std::filesystem::path& p);
    void convert_to_seq_();
    std::string stemPrefix_;
    std::string stemSeqNum_;
    std::string stemSuffix_;
    size_t determine_starting_index_() const;

    // encoding parameters
    Preset::Pointer preset_;
    bool overwrite_{true};
};

bool ContainsVideos(const std::vector<std::filesystem::path>& files);
bool ContainsImgSequences(const std::vector<std::filesystem::path>& files);

}  // namespace ffdropenc
