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

/* Read the presets directory and fill out presetList and presetFiles vectors.
   presetList contains the console names of the presets, formatted for user interaction.
   presetFiles contains each preset's filename, saved so we don't have to do a directory search later.
   presetList and presetFiles should have matching indices such that presetFiles[1] and presetList[1]
   correspond to the same preset information. */
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
        std::string presetName = cfg.lookup("preset.listname");
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

#endif