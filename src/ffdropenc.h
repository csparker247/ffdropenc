#ifndef FFDROPENC_H
#define FFDROPENC_H

#define VERSION "2.0"

#include <dirent.h>
#include <unistd.h>

std::string presetDir = "../presets/";
std::string cfg_extension = "cfg";

// recursive search algorithm
void search(std::string directory, std::string extension, std::vector<std::string>& results) {
  DIR* dir_point = opendir(directory.c_str());
  dirent* entry = readdir(dir_point);
  while (entry){                  // if !entry then end of directory
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

void loadPresets(std::vector<std::string>& presetList, std::vector<std::string>& presetFiles) {
  std::string presetPath;
  libconfig::Config cfg;
  
  search(presetDir, cfg_extension, presetFiles);

  if (presetFiles.size()){
    for (int i = 0; i < presetFiles.size(); ++i) {
      try {
        presetPath = presetDir + presetFiles[i];
        cfg.readFile(presetPath.c_str());
        std::string presetName = cfg.lookup("preset.listname");
        presetList.push_back(presetName);
      }
      catch (const libconfig::FileIOException &fioex) {
        std::cerr << "I/O error while reading file: " << presetFiles[i] << std::endl;
      }
      catch (const libconfig::ParseException &pex) {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
      }
    }
  }
  else{
    std::cerr << "Error: No preset files were found." << std::endl;
  }
}

#endif