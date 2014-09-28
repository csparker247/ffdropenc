#ifndef FFDROPENC_H
#define FFDROPENC_H

#define VERSION "2.0"

#include <dirent.h>
#include <unistd.h>
#include <stdlib.h> 

std::string presetDir = "../presets/";
std::string cfg_extension = "cfg";

// Recursive search algorithm courtesy Paul Rehkugler (https://github.com/paulrehkugler/ExtensionSearch)
void search(std::string directory, std::string extension, std::vector<std::string>& results) {
  DIR* dir_point = opendir(directory.c_str());
  dirent* entry = readdir(dir_point);
  while (entry){                        // if !entry then end of directory
    if (entry->d_type == DT_DIR){       // if entry is a directory
      std::string fname = entry->d_name;
      if (fname != "." && fname != "..")
        search(entry->d_name, extension, results); // search through it
    }
    else if (entry->d_type == DT_REG){    // if entry is a regular file
      std::string fname = entry->d_name;  // filename
      if (fname.find(extension, (fname.length() - extension.length())) != std::string::npos)
        results.push_back(fname);   // add filename to results vector
    }
    entry = readdir(dir_point);
  }
  return;
}

// Get just the filename from a path. From the find_last_of example on cplusplus.com
// To-Do: Modify to remove extension if specified, like bash's basename 
std::string basename (const std::string& str) {
  unsigned found = str.find_last_of("/\\");
  //std::cout << " path: " << str.substr(0,found) << '\n';
  return str.substr(found+1);
}

/* Read the presets directory and fill out presetList and presetFiles vectors.
   presetList contains the console names of the presets, formatted for user interaction.
   presetFiles contains each preset's filename, saved so we don't have to do a directory search later.
   presetList and presetFiles should have matching indices such that presetFiles[1] and presetList[1]
   correspond to the same preset information. There should probably be some sort of struct for this. */
void loadPresets(std::vector<std::string>& presetList, std::vector<std::string>& presetFiles) {
  std::string presetPath;
  libconfig::Config cfg;
  
  // Find cfg files and return them to the presetFiles vector
  search(presetDir, cfg_extension, presetFiles);

  if (presetFiles.size()) {
    // Loop over presetFiles
    std::vector<std::string>::iterator fileIterator = presetFiles.begin();
    while (fileIterator != presetFiles.end()) {
      try {
        // Get the path to the preset and read it
        presetPath = presetDir + *fileIterator;
        cfg.readFile(presetPath.c_str());
        // If not exceptions are caught, lookup its listname and add it to presetList
        std::string presetName = cfg.lookup("listname");
        presetList.push_back(presetName);
        ++fileIterator;
      }
      catch (const libconfig::FileIOException &fioex) {
        // If we can't read the cfg, report an error and remove it from presetFiles
        presetFiles.erase(fileIterator);
        std::cerr << "I/O error while reading file: " << *fileIterator << std::endl;
        std::cerr << "Removed from presets list." << std::endl << std::endl;
      }
      catch (const libconfig::ParseException &pex) {
        // If we can't parse the cfg, report an error and remove it from presetFiles
        presetFiles.erase(fileIterator);
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        std::cerr << "Removed from presets list." << std::endl << std::endl;
      }
    }
  }
  else {
    // If there are no presetFiles, quit the program
    // Note: An entire folder of bad presets WILL cause problems.
    std::cerr << "Fatal Error: No preset files were found." << std::endl << std::endl;
    exit (EXIT_FAILURE);
  }
}

// Build an ffmpeg command given a file path and a preset config
// This command assumes the preset cfg's don't specify incompatible options
// To-Do: All of the libconfig loads need error handling
std::string buildCommand(const std::string& inputFile, const libconfig::Config& cfg) {
  // Start the command
  std::string command;
  command = "ffmpeg -i " + inputFile;

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
        // To-Do: Only set these with certain codecs?
        std::string profile, level, pixfmt;
        stream.lookupValue("profile", profile);
        stream.lookupValue("level", level);
        stream.lookupValue("pixfmt", pixfmt);
        command.append(" -profile " + profile + " -level " + level + " -pix_fmt " + pixfmt);
      } 
      else if (type == "audio") {
        // Audio settings
        // To-Do: Basically everything with audio logic
        command.append(" -c:a " + codec);
        command.append(" -b:a " + bitrate);
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
    std::string outputName = basename(inputFile);
    command.append(" -y " + outputName + "_" + suffix + "." + extension);
  }
  return command;
}

#endif