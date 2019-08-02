#include "ffdropenc/Preset.hpp"

#include <fstream>
#include <iostream>

using namespace ffdropenc;
namespace fs = std::filesystem;

Preset::Preset(const fs::path& path)
{
    std::ifstream ifs(path.string());
    if (!ifs.good()) {
        throw std::runtime_error("Failed to open preset file");
    }

    ifs >> cfg_;

    ifs.close();
}

QStringList Preset::getSettings(size_t index)
{

    // The settings that will appended to the output's transcode command
    QStringList settings;

    // Get the json info of the requested output file
    json output = cfg_["outputs"][index];

    // Iterate through all of the stream types
    json streams = output["streams"];
    for (json& s : streams) {

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
            if (!s["profile"].is_null()) {
                auto profile = s["profile"].get<std::string>();
                settings << "-profile" << QString::fromStdString(profile);
            }
            if (!s["level"].is_null()) {
                auto level = s["level"].get<std::string>();
                settings << "-level" << QString::fromStdString(level);
            }
            if (!s["pixfmt"].is_null()) {
                auto pixfmt = s["pixfmt"].get<std::string>();
                settings << "-pix_fmt" << QString::fromStdString(pixfmt);
            }
        }

        // Handle audio streams
        else if (type == "audio") {
            // Add the codec
            auto codec = s["codec"].get<std::string>();
            settings << "-c:a" << QString::fromStdString(codec);

            // Skip the rest of this loop if we're copying the input stream
            if (codec == "copy") {
                continue;
            }

            // Append the output bitrate if we need it
            if (s["bitrate"].get<std::string>() != "default") {
                auto bitrate = s["bitrate"].get<std::string>();
                settings << "-b:a" << QString::fromStdString(bitrate);
            }
        }

        // Handle filters on the stream
        if (!s["filters"].is_null()) {
            json filters = s["filters"];
            if (type == "video") {
                settings << "-vf";
            } else if (type == "audio") {
                settings << "-af";
            }
            settings << construct_filter_graph_(filters);
        }

        // Handle flags on the stream
        if (!s["flags"].is_null()) {
            json flags = s["flags"];
            for (json& flag : flags) {
                settings << QString::fromStdString(
                    flag["flag"].get<std::string>());
            }
        }

    }  // Stream Iterator

    // Fast start atom
    if (output["faststart"].get<bool>()) {
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

QStringList Preset::construct_filter_graph_(json filters)
{
    // Output graph
    QStringList graph;

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
                    auto dar = filter["dar"].get<std::string>();
                    command = "scale=iw*sar:ih,";
                    command.append("scale=");
                    command.append("\"\'w=if(lt(dar, ");
                    command.append(QString::fromStdString(dar));
                    command.append("), trunc(oh*a/2)*2, min(");
                    command.append(QString::fromStdString(w));
                    command.append(",ceil(iw/2)*2)):");
                    command.append("h=if(gte(dar, ");
                    command.append(QString::fromStdString(dar));
                    command.append("), trunc(ow/a/2)*2, min(");
                    command.append(QString::fromStdString(h));
                    command.append(",ceil(ih/2)*2))\'\",");
                    command.append("setsar=1");
                    break;
            }
        }

        // Handle misc. filters
        else if (filter["data"].is_string()) {
            command = QString::fromStdString(filter["data"].get<std::string>());
        }

        // Add this filter to the filter graph
        if (!command.isEmpty() && graph.empty()) {
            graph.append(command);
        } else if (!command.isEmpty()) {
            graph.append("," + command);
        }
    }

    return graph;
}

// Load any .preset files in the given directory
std::vector<Preset::Pointer> Preset::LoadPresetDir(const fs::path& dir)
{

    std::vector<Preset::Pointer> presets;
    if (fs::exists(dir)) {

        fs::recursive_directory_iterator it(dir);
        fs::recursive_directory_iterator itEnd;

        while (it != itEnd) {
            if (fs::path(*it).extension() == ".preset") {
                presets.emplace_back(std::make_shared<Preset>(*it));
            }
            ++it;
        }
    }

    return presets;
}