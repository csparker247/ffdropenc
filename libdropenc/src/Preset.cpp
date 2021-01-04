#include "ffdropenc/Preset.hpp"

#include <iostream>

#include <QFile>
#include <QTextStream>

using namespace ffdropenc;
namespace fs = std::filesystem;

Preset::Preset(const QString& path)
{
    QFile data(path);
    if (data.open(QFile::ReadOnly)) {
        QTextStream file(&data);
        cfg_ = json::parse(file.readAll().toStdString());
    }
}

Preset::Pointer Preset::New(const QString& path)
{
    return std::make_shared<Preset>(path);
}

QString Preset::getListName() const
{
    return QString::fromStdString(cfg_["listname"].get<std::string>());
}

QString Preset::getConsoleName() const
{
    return QString::fromStdString(cfg_["consolename"].get<std::string>());
}

QStringList Preset::getSettings(size_t index)
{

    // The settings that will appended to the output's transcode command
    QStringList settings;

    // Get the json info of the requested output file
    json output = cfg_["outputs"][index];

    // Iterate through all of the stream types
    json streams = output["streams"];
    for (const json& s : streams) {

        // What sort of stream is this?
        auto type = s["type"].get<std::string>();

        // Handle video streams
        if (type == "video") {
            // Add the codec
            auto codec = s["codec"].get<std::string>();
            settings << "-c:v";
            settings << QString::fromStdString(codec);

            // Skip the rest of this loop if we're copying the input stream
            if (codec == "copy") {
                continue;
            }

            // Encoding mode
            if (s["mode"].get<std::string>() == "crf") {
                auto quality = s["quality"].get<int>();
                auto bitrate = s["bitrate"].get<std::string>();
                auto buffer = s["buffer"].get<std::string>();
                settings << "-crf" << QString::number(quality);
                settings << "-maxrate:v" << QString::fromStdString(bitrate);
                settings << "-bufsize:v" << QString::fromStdString(buffer);
            }

            // Some codec specific options (mostly H.264 things)
            if (s.contains("profile")) {
                auto profile = s["profile"].get<std::string>();
                settings << "-profile:v" << QString::fromStdString(profile);
            }
            if (s.contains("level")) {
                auto level = s["level"].get<std::string>();
                settings << "-level" << QString::fromStdString(level);
            }
            if (s.contains("pixfmt")) {
                auto pixfmt = s["pixfmt"].get<std::string>();
                settings << "-pix_fmt" << QString::fromStdString(pixfmt);
            }
        }

        // Handle audio streams
        else if (type == "audio") {
            // Add the codec
            auto codec = s["codec"].get<std::string>();
            if(codec == "aac") {
                // TODO: Replace with user-selected library
            }
            settings << "-c:a" << QString::fromStdString(codec);

            // Skip the rest of this loop if we're copying the input stream
            if (codec == "copy") {
                continue;
            }

            // Append the output bitrate if we need it
            if (s.contains("bitrate")) {
                auto bitrate = s["bitrate"].get<std::string>();
                settings << "-b:a" << QString::fromStdString(bitrate);
            }

            if(s.contains("quality")) {
                auto quality = s["quality"].get<std::string>();
                settings << "-q:a" << QString::fromStdString(quality);
            }
        }

        // Handle filters on the stream
        if (s.contains("filters")) {
            json filters = s["filters"];
            if (type == "video") {
                settings << "-vf";
            } else if (type == "audio") {
                settings << "-af";
            }
            settings << construct_filter_graph_(filters);
        }

        // Handle flags on the stream
        if (s.contains("flags")) {
            json flags = s["flags"];
            for (json& flag : flags) {
                settings << QString::fromStdString(
                    flag["key"].get<std::string>());
                settings << QString::fromStdString(
                    flag["val"].get<std::string>());
            }
        }

    }  // Stream Iterator

    // Fast start atom
    if (output.contains("faststart") and output["faststart"].get<bool>()) {
        settings << "-movflags"
                 << "faststart";
    }

    return settings;
}

std::string Preset::getSuffix(size_t index)
{
    return cfg_["outputs"][index]["suffix"].get<std::string>();
}

std::string Preset::getExtension(size_t index)
{
    return "." + cfg_["outputs"][index]["extension"].get<std::string>();
}

QString Preset::construct_filter_graph_(json filters)
{
    // Output graph
    QString graph;

    // Iterate over each filter
    for (json& filter : filters) {
        // Command for just this filter
        QString command;

        // Skip if we don't have a name for the filter
        if (filter["filter"].is_null()) {
            continue;
        }

        // Get the filter name
        auto name = filter["filter"].get<std::string>();

        // Handle the scale filter
        if (name == "scale") {
            // Ignore if the scale mode doesn't exist
            auto mode = static_cast<ScaleMode>(filter["mode"].get<int>());

            switch (mode) {
                case ScaleMode::SquarePixel:
                    command = "scale=iw*sar:ih";
                    break;
                case ScaleMode::SizeLimited:
                    auto w = std::to_string(filter["width"].get<int>());
                    auto h = std::to_string(filter["height"].get<int>());
                    command = "scale=iw*sar:ih,";
                    command.append("scale=w=");
                    command.append(QString::fromStdString(w));
                    command.append(":h=");
                    command.append(QString::fromStdString(h));
                    command.append(":force_original_aspect_ratio=decrease");
                    command.append(":force_divisible_by=2,");
                    command.append("setsar=1");
                    break;
            }
        }

        // Handle misc. filters
        else if (filter["data"].is_string()) {
            command = QString::fromStdString(filter["data"].get<std::string>());
        }

        // Add this filter to the filter graph
        if (!command.isEmpty() && graph.isEmpty()) {
            graph.append(command);
        } else if (!command.isEmpty()) {
            graph.append("," + command);
        }
    }

    return graph;
}