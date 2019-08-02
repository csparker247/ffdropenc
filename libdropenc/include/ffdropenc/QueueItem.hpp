#pragma once

#include <filesystem>
#include <iostream>

#include <QString>
#include <QStringList>
#include <QUrl>

#include "ffdropenc/Preset.hpp"

namespace ffdropenc
{

class QueueItem
{
public:
    enum class Type { Undefined, Video, Sequence };

    QueueItem() = default;
    QueueItem(const std::filesystem::path& path, Preset::Pointer preset);

    // Operators
    bool operator<(const QueueItem&) const;
    bool operator==(const QueueItem&) const;

    // Accessors
    std::filesystem::path inputPath() const { return inputPath_; }
    std::filesystem::path outputPath() const;

    // Modifiers
    void setOutputDir(const std::filesystem::path& d) { outputDir_ = d; }
    void setOutputFilename(const std::filesystem::path& f)
    {
        outputFileName_ = f;
    }

    QStringList encodeArguments() const;

private:
    // input location
    std::filesystem::path inputPath_;
    Type type_;

    // output directory
    std::filesystem::path outputDir_;
    // output basename
    std::filesystem::path outputFileName_;

    // progress tracking
    double fps_{-1};
    unsigned long frames_{-1};
    double duration_{-1};

    // image sequence
    static Type determine_type_(const std::filesystem::path& p);
    void convert_to_seq_(const std::string& fps);
    uint64_t startingIndex_{0};
    std::string outputFPS_{"30000/1001"};

    // encoding parameters
    Preset::Pointer preset_;
    bool overwrite_{true};

    bool transcoded_{false};
};
}
