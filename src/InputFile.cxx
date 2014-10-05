#include <string>

#include "InputFile.h"
#include "ffcommon.h"

namespace ffdropenc {

  // Initializers/Constructors
  InputFile::InputFile () {}

  // To-Do: basename() is stripping the output path. We need to save it.
  InputFile::InputFile (const std::string p) {
    std::string tempPath;
    std::string tempOutpath;
    std::string tempOutname;
    tempPath = p;
    if (isImage(tempPath)){
      tempPath = makeImageName(tempPath);
      tempOutpath = dirname(tempPath);
      tempOutname = basename(tempPath);
      int sep_Pos = tempOutname.find_last_of("%");
      tempOutname = tempOutname.substr(0, sep_Pos);
      isSeq = true;
    }
    else {
      tempOutpath = dirname(p);
      tempOutname = basename(p);
      isSeq = false;
    }
    path = tempPath;
    outpath = tempOutpath;
    outname = tempOutname;
  }

  // Operators
  bool InputFile::operator < (const InputFile& file) const {
    return (path < file.path);
  }

  bool InputFile::operator == (const InputFile& file) const {
    return (path == file.path);
  }

  // Basic Input
  void InputFile::setIO(const std::string p) {
    std::string tempPath;
    std::string tempOutpath;
    std::string tempOutname;
    tempPath = p;
    if (isImage(tempPath)){
      tempPath = makeImageName(tempPath);
      tempOutpath = dirname(tempPath);
      tempOutname = basename(tempPath);
      int sep_Pos = tempOutname.find_last_of("%");
      tempOutname = tempOutname.substr(0, sep_Pos);
      isSeq = true;
    }
    else {
      tempOutpath = dirname(p);
      tempOutname = basename(p);
      isSeq = false;
    }
    path = tempPath;
    outpath = tempOutpath;
    outname = tempOutname;
  }

  void InputFile::setPath(std::string p) {
    path = p;
  }

  void InputFile::setOutpath(std::string op) {
    outpath = op;
  }

  void InputFile::setOutname(std::string on) {
    outname = on;
  }

  // Basic Output
  std::string InputFile::getPath() const {
    return path;
  }

  std::string InputFile::getOutpath() const {
    return outpath;
  }

  std::string InputFile::getOutname() const {
    return outname;
  }

  // Get and set whether file is part of an Image Sequence
  bool InputFile::isImgSeq() const {
    return isSeq;
  }

  void InputFile::isImgSeq(bool b) {
    isSeq = b;
  }

  // Get and set the InputFile's framerate
  void InputFile::setFps(std::string f) {
    fps = f;
  }

  std::string InputFile::getFps() const {
    return fps;
  }

  // Get and set the InputFile's duration
  void InputFile::setDuration(unsigned long long d) {
    duration = d;
  }

  unsigned long long InputFile::getDuration() const {
    return duration;
  }

  // Build an ffmpeg command given a file path and a preset config
  // To-Do: All of the libconfig loads need error handling
  std::string InputFile::buildCommand(const libconfig::Config& cfg) const {
    // Start the command
    std::string command;
    command = "ffmpeg";
    if (isSeq == true) {
      command.append(" -r " + fps);
    }

    command.append(" -i \"" + path + "\"");

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
            command.append(" -crf " + std::to_string(quality) + " -maxrate:v " + bitrate + " -bufsize:v " + buffer);
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

      // Add the output info
      command.append(" -y \"" + outpath + outname);
      std::string last_char = &outname.back();
      if ((last_char != "-") && (last_char != "_") && (last_char != " ")) {
        command.append("-"); 
      }
      command.append(suffix + "." + extension + "\"");
    }
    return command;
  }
}