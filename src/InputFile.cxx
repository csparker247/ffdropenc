#include <string>

#include "InputFile.h"
#include "ffcommon.h"

namespace ffdropenc {

  // Initializers/Constructors
  InputFile::InputFile () {}

  InputFile::InputFile (std::string p) {
    path = p;
    outname = basename(p);
  }

  // Operators
  bool InputFile::operator < (const InputFile& file) const {
    return (path < file.path);
  }

  bool InputFile::operator == (const InputFile& file) const {
    return (path == file.path);
  }

  // Basic Input
  void InputFile::setIO(std::string p) {
    path = p;
    outname = basename(p);
  }

  void InputFile::setPath(std::string p) {
    path = p;
  }

  void InputFile::setOutname(std::string o) {
    outname = o;
  }

  // Basic Output
  std::string InputFile::getPath() const {
    return path;
  }

  std::string InputFile::getOutname() const {
    return outname;
  }

  // Build an ffmpeg command given a file path and a preset config
  // To-Do: All of the libconfig loads need error handling
  std::string InputFile::buildCommand(const libconfig::Config& cfg) const {
    // Start the command
    std::string command;
    command = "ffmpeg -i " + path;

    // Find the outputs in the cfg
    const libconfig::Setting& root = cfg.getRoot();
    const libconfig::Setting &outputs = root["outputs"];
    
    // Iterate over every output setting.
    // Configs allow for more than one output file
    for (int i = 0; i < outputs.getLength(); ++i) {
      const libconfig::Setting &output = outputs[i];

      // Iterate over every stream specifier in an output file
      const libconfig::Setting &streams = output.lookup("streams");
      for (int j = 0; j < streams.getLength(); ++j) {
        const libconfig::Setting &stream = streams[j];
        std::string type, codec, mode, bitrate;
        // These variables should exist for every output stream
        stream.lookupValue("type", type);
        stream.lookupValue("codec", codec);
        stream.lookupValue("mode", mode);
        stream.lookupValue("bitrate", bitrate);

        if (type == "video") {
          // Every video stream will start with this
          command.append(" -c:v " + codec);
          // Skip all other settings if we're just copying the stream
          if (codec == "copy") {
            continue;
          } 

          // Encoding mode (crf, 2pass, etc.) changes a lot
          // Only CRF supported right now
          // To-Do: Flesh this out to account for other modes
          if (mode == "crf") {
            std::string buffer;
            int quality;
            stream.lookupValue("buffer", buffer);
            stream.lookupValue("quality", quality);
            command.append(" -crf " + std::to_string(quality) + " -maxrate " + bitrate + " -bufsize " + buffer);
          }
          // Default to a "should always work" bitrate setting
          else {
            command.append(" -b:v " + bitrate );
          }

          // Set some codec specific options.
          // To-Do: Exception checking if one doesn't exist
          std::string profile, level, pixfmt;
          stream.lookupValue("profile", profile);
          stream.lookupValue("level", level);
          stream.lookupValue("pixfmt", pixfmt);
          command.append(" -profile:v " + profile + " -level " + level + " -pix_fmt " + pixfmt);
        } 
        else if (type == "audio") {
          // Audio settings
          // To-Do: Basically everything with audio logic
          command.append(" -c:a " + codec);
          // Skip all other settings if we're just copying the stream
          if (codec == "copy") {
            continue;
          }
          command.append(" -b:a " + bitrate);
        }

        // Parse the streams filter settings
        try {
          const libconfig::Setting &filters = stream.lookup("filters");
          std::string filterGraph = buildFilterGraph(filters);
          if ((type == "video") && (!filterGraph.empty())) {
            command.append(" -vf " + filterGraph);
          }
          else if ((type == "audio") && (!filterGraph.empty())) {
            command.append(" -af " + filterGraph);
          }
        }
        catch(const libconfig::SettingNotFoundException &nfex)
        {
          // Do nothing.
        }
      }

      // Add the final output file options
      std::string extension, suffix;
      bool faststart;
      output.lookupValue("extension", extension);
      output.lookupValue("suffix", suffix);
      output.lookupValue("faststart", faststart);

      // Add the faststart option
      if (faststart == true) {
        command.append(" -movflags faststart");
      }

      // Get the input filename
      // To-Do: Need to set output name in a little more rigorous fashion
      command.append(" -y " + outname + "_" + suffix + "." + extension);
    }
    return command;
  }
}