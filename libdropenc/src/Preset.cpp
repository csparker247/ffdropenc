#include "ffdropenc/Preset.hpp"

#include <istream>

using namespace ffdropenc;
namespace fs = boost::filesystem;

Preset::Preset(fs::path path)
{
    std::ifstream ifs(path.string());
    if (!ifs.good()) {
        throw std::runtime_error("Failed to open preset file");
    }

    ifs >> cfg_;

    ifs.close();
}

std::string Preset::getSettings(int outputIndex)
{

    // The settings that will appended to the output's transcode command
    std::string settings = "";

    // Get the json info of the requested output file
    json output = cfg_["outputs"][outputIndex];

    // Iterate through all of the stream types
    json streams = output["streams"];
    for (json& s : streams) {

        // What sort of stream is this?
        std::string streamType = s["type"].get<std::string>();

        // Handle video streams
        if (streamType == "video") {
            // Add the codec
            settings.append(" -c:v " + s["codec"].get<std::string>());

            // Skip the rest of this loop if we're copying the input stream
            if (s["codec"].get<std::string>() == "copy")
                continue;

            // Encoding mode //To-Do: Add more modes
            if (s["mode"].get<std::string>() == "crf") {
                auto quality = s["quality"].get<int>();
                auto bitrate = s["bitrate"].get<std::string>();
                auto buffer = s["buffer"].get<std::string>();
                settings.append(
                    " -crf " + std::to_string(quality) + " -maxrate:v " +
                    bitrate + " -bufsize:v " + buffer);
            }

            // Some codec specific options (mostly H.264 things)
            if (!s["profile"].is_null())
                settings.append(" -profile " + s["profile"].get<std::string>());
            if (!s["level"].is_null())
                settings.append(" -level " + s["level"].get<std::string>());
            if (!s["pixfmt"].is_null())
                settings.append(" -pix_fmt " + s["pixfmt"].get<std::string>());
        }  // Video Streams

        // Handle audio streams
        else if (streamType == "audio") {
            // Add the codec
            settings.append(" -c:a " + s["codec"].get<std::string>());

            // Skip the rest of this loop if we're copying the input stream
            if (s["codec"].get<std::string>() == "copy")
                continue;

            // Append the output bitrate if we need it
            if (s["bitrate"].get<std::string>() != "default")
                settings.append(" -b:a " + s["bitrate"].get<std::string>());
        }  // Audio Streams

        // Handle filters on the stream
        if (s["filters"].is_array()) {
            json filters = s["filters"];
            if (streamType == "video")
                settings.append(" -vf " + ConstructFilterGraph(filters));
            if (streamType == "audio")
                settings.append(" -af " + ConstructFilterGraph(filters));
        }

        // Handle flags on the stream
        if (s["flags"].is_array()) {
            json flags = s["flags"];
            for (json& flag : flags) {
                settings.append(" " + flag["flag"].get<std::string>());
            }
        }

    }  // Stream Iterator

    // Fast start atom
    if (output["faststart"].get<bool>())
        settings.append(" -movflags faststart");

    return settings;
}

std::string Preset::getSuffix(int outputIndex)
{
    return cfg_["outputs"][outputIndex]["suffix"].get<std::string>();
}

std::string Preset::getExtension(int outputIndex)
{
    return "." + cfg_["outputs"][outputIndex]["extension"].get<std::string>();
}

std::string Preset::ConstructFilterGraph(json filters)
{
    std::string filterGraph = "";  // all of the filters concatenated

    for (picojson::array::iterator filter = filters.begin();
         filter != filters.end(); ++filter) {

        std::string filterCommand = "";  // the command for just this filter

        // Which filter is this?
        if (!filter["filter")
                 .is<std::string>())  // Ignore if there isn't a name
            continue;
        std::string filterName = filter["filter"].get<std::string>();

        // Handle the scale filter
        if (filterName == "scale") {

            if (!filter["mode")
                     .is<double>())  // Ignore if mode doesn't exist
                continue;

            // Mode #0: Square pixels only, no scaling
            if (filter["mode"].get<double>() == 0)
                filterCommand = "scale=iw*sar:ih";

            // Mode #1: Square pixels, maintain aspect ratio, limit to width x
            // height
            else if (filter["mode"].get<double>() == 1) {
                // Read the settings
                if (!filter["width").is<double>() ||
                    !filter["height").is<double>() ||
                    !filter["dar").is<std::string>())
                    continue;

                int width = filter["width"].get<double>();
                int height = filter["height"].get<double>();
                std::string dar = filter["dar"].get<std::string>();

                filterCommand = "scale=iw*sar:ih,";
                filterCommand.append("scale=");
                filterCommand.append(
                    "\"\'w=if(lt(dar, " + dar + "), trunc(oh*a/2)*2, min(" +
                    std::to_string(width) + ",ceil(iw/2)*2)):");
                filterCommand.append(
                    "h=if(gte(dar, " + dar + "), trunc(ow/a/2)*2, min(" +
                    std::to_string(height) + ",ceil(ih/2)*2))\'\",");
                filterCommand.append("setsar=1");
            }  // Mode #1

        }  // Scale filter

        // Handle misc. filters
        else if (filter["data").is<std::string>())
            filterCommand = filter["data"].get<std::string>();

        // Add this filter to the filter graph
        if (filterCommand != "" && filterGraph.empty())
            filterGraph.append(filterCommand);
        else if (filterCommand != "")
            filterGraph.append("," + filterCommand);

    }  // filter iterator

    return filterGraph;
}

// Load any .preset files in the given directory
std::vector<Preset> Preset::LoadPresetDir(fs::path dir)
{

    std::vector<Preset> presets;
    if (fs::exists(dir)) {

        fs::recursive_directory_iterator it(dir);
        fs::recursive_directory_iterator itEnd;

        while (it != itEnd) {
            if (fs::path(*it).extension() == ".preset") {
                presets.emplace_back(*it);
            }
            ++it;
        }
    }

    return presets;
}