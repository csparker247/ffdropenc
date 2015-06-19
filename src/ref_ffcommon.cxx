#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <locale>

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
        fname.insert(0, directory + "/");
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
std::string basename (const std::string str) {
  std::string newstr;
  unsigned start, end;
  start = str.find_last_of("/\\");
  end = str.find_last_of(".");
  newstr = str.substr(start + 1, end - start - 1);
  return newstr;
}

std::string dirname (const std::string str) {
  std::string newstr;
  unsigned end;
  end = str.find_last_of("/\\");
  newstr = str.substr(0, end + 1);
  return newstr;
}

// Returns true if the string has an extension that matches one of the approved formats
bool isImage (const std::string str) {
  unsigned start, end;
  std::locale loc;
  std::string str_ext;

  // Get extension
  start = str.find_last_of(".");
  end = str.back();
  str_ext = str.substr(start + 1, end - start - 1);

  // Convert case
  for (std::string::size_type j = 0; j < str_ext.length(); ++j) {
    str_ext[j] = std::toupper(str_ext[j],loc);
  }

  // Compare to the approved extensions
  for (int i = 0; i < img_extension.size(); ++i) {
    std::string approvedExtension;
    approvedExtension = img_extension[i];

    if (str_ext == approvedExtension){
      return true;
    }
  }

  return false;
}

// Converts a path of an image sequence (Name ###.jpg) to a formatted path for ffmpeg (Name %03d.jpg)
std::string makeImageName (const std::string str) {
  std::string prefix;
  std::string number;
  std::string ext;
  std::string result;

  // Save the file extension
  int dot_pos = str.find_last_of(".");
  ext = str.substr(dot_pos);
  number = str.substr(0, dot_pos);

  // Save everything but the number
  int last_char = number.find_last_not_of("0123456789");
  prefix = number.substr(0, last_char + 1);
  number = number.substr(last_char + 1);

  // Convert the number to the %nd format
  number = std::to_string(number.length());
  if (number.length() < 2){
    number.insert(0, "0");
  }
  number = "%" + number + "d";

  // Recombine the path
  result = prefix + number + ext;
  
  return result;
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
    if (filterName == "scale") {
      unsigned int filterMode, filterWidth, filterHeight;
      std::string filterdar;
      filter.lookupValue("mode", filterMode);
      filter.lookupValue("width", filterWidth);
      filter.lookupValue("height", filterHeight);
      filter.lookupValue("dar", filterdar);
      if (filterMode == 0) {
        filterCommand = "scale=iw*sar:ih";
      }
      else if (filterMode == 1) {
        filterCommand = "scale=iw*sar:ih,";
        filterCommand.append("scale=");
        filterCommand.append("\"\'w=if(lt(dar, " + filterdar + "), trunc(oh*a/2)*2, min(" + std::to_string(filterWidth) + ",ceil(iw/2)*2)):");
        filterCommand.append("h=if(gte(dar, " + filterdar + "), trunc(ow/a/2)*2, min(" + std::to_string(filterHeight) + ",ceil(ih/2)*2))\'\",");
        filterCommand.append("setsar=1");
      }
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