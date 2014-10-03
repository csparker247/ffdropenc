#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include <dirent.h>
#include <unistd.h>
#include <libconfig.h++>

#include "ffcommon.h"
#include "InputFile.h"

// Recursive search algorithm courtesy Paul Rehkugler (https://github.com/paulrehkugler/ExtensionSearch)
void search(const std::string directory, const std::string extension, std::vector<std::string>& results) {
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

// Recursively return all file paths in a parent directory
void expandDir(const std::string directory, std::vector<std::string>& results) {
  DIR* dir_point = opendir(directory.c_str());
  dirent* entry = readdir(dir_point);
  while (entry){                        // if !entry then end of directory
    if (entry->d_type == DT_DIR){       // if entry is a directory
      std::string fname = entry->d_name;
      if (fname != "." && fname != "..")
        expandDir(entry->d_name, results); // search through it
    }
    else if (entry->d_type == DT_REG){    // if entry is a regular file
      std::string fname = entry->d_name;  // filename
      if (fname.substr(0, 1) != ".") {
        results.push_back(fname);   // add filename to results vector
      }
    }
    entry = readdir(dir_point);
  }
  return;
}

// Return a file name, removing the path and extension from a string containing a full file path
// Note: This will not work as expected if the file name includes a '.' and no extension, a rare
// possibility, but one worth mentioning.
std::string basename (const std::string& str) {
  std::string newstr;
  unsigned start, end;
  start = str.find_last_of("/\\");
  end = str.find_last_of(".");
  newstr = str.substr(start + 1, end - start - 1);
  return newstr;
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
  search(PRESET_DIR, PRESET_EXT, presetFiles);

  if (presetFiles.size()) {
    // Loop over presetFiles
    std::vector<std::string>::iterator fileIterator = presetFiles.begin();
    while (fileIterator != presetFiles.end()) {
      try {
        // Get the path to the preset and read it
        presetPath = PRESET_DIR + *fileIterator;
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

// Parse a stream's filters from a cfg and return its filtergraph
std::string buildFilterGraph(const libconfig::Setting& filters) {
  std::string filterGraph;
  // Iterate over all of the filters
  for (int k = 0; k < filters.getLength(); ++k) {
    const libconfig::Setting &filter = filters[k];
    std::string filterName;
    std::string filterCommand;
    filter.lookupValue("filter", filterName);
    
    // Do something special for the scale filter
    // To-do: Make this use the cfg's settings
    if (filterName == "scale") {
      filterCommand = "scale=1920:1080";
    }
    // Otherwise just copy the filter's cmdline option
    else {
      std::string filterData;
      filter.lookupValue("data", filterData);
      filterCommand = filterData;
    }
    
    // If there's not anythign in the filter graph, just add it
    if (filterGraph.empty()) {
      filterGraph.append(filterCommand);
    }
    // Otherwise put a filter separator, then the command
    else {
      filterGraph.append("," + filterCommand);
    }
  }
  return filterGraph;
}