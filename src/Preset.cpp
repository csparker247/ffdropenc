//
// Created by Seth Parker on 6/17/15.
//

#include "Preset.h"

namespace ffdropenc {

    Preset::Preset ( boost::filesystem::path path ) {

        std::string jsonFile = ffdropenc::file_to_string( path.c_str() );

        std::string err = picojson::parse(_cfg, jsonFile);
        if (! err.empty()) {
            std::cerr << err << std::endl;
        }

        std::string _name = _cfg.get( "listname" ).get<std::string>();
        std::string _shortname = _cfg.get( "consolename" ).get<std::string>();

    }

    std::string Preset::getSettings(int outputIndex) {

        // The settings that will appended to the output's transcode command
        std::string settings = "";

        // Get the json info of the requested output file
        picojson::value output = _cfg.get("outputs").get<picojson::array>()[outputIndex];

        // Iterate through all of the stream types
        picojson::array streams = output.get("streams").get<picojson::array>();
        for (picojson::array::iterator stream = streams.begin(); stream != streams.end(); ++stream ) {

            // What sort of stream is this?
            std::string streamType = stream->get("type").get<std::string>();

            // Handle video streams
            if ( streamType == "video" ){
                // Add the codec
                settings.append( " -c:v " + stream->get("codec").get<std::string>() );

                // Skip the rest of this loop if we're copying the input stream
                if ( stream->get("codec").get<std::string>() == "copy" ) continue;

                // Encoding mode //To-Do: Add more modes
                if ( stream->get("mode").get<std::string>() == "crf" ) {
                    int quality = stream->get("quality").get<double>();
                    std::string bitrate = stream->get("bitrate").get<std::string>();
                    std::string buffer = stream->get("buffer").get<std::string>();
                    settings.append(" -crf " + std::to_string(quality) + " -maxrate:v " + bitrate + " -bufsize:v " + buffer);
                }

                // Some codec specific options (mostly H.264 things)
                if ( !stream->get("profile").is<picojson::null>() )
                    settings.append(" -profile " + stream->get("profile").get<std::string>() );
                if ( !stream->get("level").is<picojson::null>() )
                    settings.append(" -level " + stream->get("level").get<std::string>() );
                if ( !stream->get("pixfmt").is<picojson::null>() )
                    settings.append(" -pix_fmt " + stream->get("pixfmt").get<std::string>() );
            } // Video Streams

            // Handle audio streams
            else if ( streamType == "audio" ) {
                // Add the codec
                settings.append( " -c:a " + stream->get("codec").get<std::string>() );

                // Skip the rest of this loop if we're copying the input stream
                if ( stream->get("codec").get<std::string>() == "copy" ) continue;

                // Append the output bitrate if we need it
                if ( stream->get("bitrate").get<std::string>() != "default" ) settings.append( " -b:a " + stream->get("bitrate").get<std::string>() );
            } // Audio Streams

            // Handle filters on the stream
            if ( stream->get("filters").is<picojson::array>() ) {
                picojson::array filters = stream->get("filters").get<picojson::array>();
                if ( streamType == "video" ) settings.append( " -vf " + _filterGraph(filters) );
                if ( streamType == "audio" ) settings.append( " -af " + _filterGraph(filters) );
            }

            // Handle flags on the stream
            if ( stream->get("flags").is<picojson::array>() ) {
                picojson::array flags = stream->get("flags").get<picojson::array>();
                for (picojson::array::iterator flag = flags.begin(); flag != flags.end(); ++flag) {
                    settings.append( " " + flag->get("flag").get<std::string>() );
                }
            }

        } // Stream Iterator

        // Fast start atom
        if ( output.get("faststart").is<bool>() && output.get("faststart").get<bool>() )
            settings.append( " -movflags faststart" );

        return settings;
    }

    std::string Preset::getSuffix( int outputIndex ) {
        return "_" + _cfg.get("outputs").get<picojson::array>()[outputIndex].get("suffix").get<std::string>();
    }

    std::string Preset::getExtension( int outputIndex ) {
        return "." + _cfg.get("outputs").get<picojson::array>()[outputIndex].get("extension").get<std::string>();
    }

    std::string Preset::_filterGraph( picojson::array filters ) {
        std::string filterGraph = ""; // all of the filters concatenated

        for ( picojson::array::iterator filter = filters.begin(); filter != filters.end(); ++filter ) {

            std::string filterCommand = ""; // the command for just this filter

            // Which filter is this?
            if ( !filter->get("filter").is<std::string>() )  // Ignore if there isn't a name
                continue;
            std::string filterName = filter->get("filter").get<std::string>();

            // Handle the scale filter
            if ( filterName == "scale" ) {

                if ( !filter->get("mode").is<double>() )// Ignore if mode doesn't exist
                    continue;

                // Mode #0: Square pixels only, no scaling
                if ( filter->get("mode").get<double>() == 0 ) filterCommand = "scale=iw*sar:ih" ;

                // Mode #1: Square pixels, maintain aspect ratio, limit to width x height
                else if ( filter->get("mode").get<double>() == 1 ) {
                    // Read the settings
                    if ( !filter->get("width").is<double>() || !filter->get("height").is<double>() || !filter->get("dar").is<std::string>() )
                        continue;

                    int width = filter->get("width").get<double>();
                    int height = filter->get("height").get<double>();
                    std::string dar = filter->get("dar").get<std::string>();

                    filterCommand = "scale=iw*sar:ih,";
                    filterCommand.append("scale=");
                    filterCommand.append("\"\'w=if(lt(dar, " + dar + "), trunc(oh*a/2)*2, min(" + std::to_string(width) + ",ceil(iw/2)*2)):");
                    filterCommand.append("h=if(gte(dar, " + dar + "), trunc(ow/a/2)*2, min(" + std::to_string(height) + ",ceil(ih/2)*2))\'\",");
                    filterCommand.append("setsar=1");
                } // Mode #1

            } // Scale filter

            // Handle misc. filters
            else if ( filter->get("data").is<std::string>() ) filterCommand = filter->get("data").get<std::string>();

            // Add this filter to the filter graph
            if( filterCommand != "" && filterGraph.empty() )
                filterGraph.append(filterCommand);
            else if ( filterCommand != "" )
                filterGraph.append( "," + filterCommand );

        } // filter iterator

        return filterGraph;
    }

} //namespace ffdropenc