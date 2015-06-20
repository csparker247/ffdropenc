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

        // The settings that will appended to the outout's transcode command
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

        } // Stream Iterator

        return settings;
    }

} //namespace ffdropenc