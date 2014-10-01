// ffdropenc

#include "ffdropenc.h"
#include "ffcommon.h"
#include "InputFile.h"

// Environment defines
std::string presetDir = PRESET_DIR;
std::string cfg_extension = PRESET_EXT;

int main (int argc, char* argv[]) {
  std::cout << std::endl;
  std::cout << " ------------------------- " << std::endl;
  std::cout << "         ffdropenc         " << std::endl;
  std::cout << " ------------------------- " << std::endl;
  std::cout << std::endl;
  
// Parse command line options
  int preset;
  std::vector<InputFile> inputList;

  // These will be replaced with Qt option/file pickers
  preset = std::stoi(argv[1]);
  for (int i = 2; i < argc; ++i) {
    InputFile thisFile(argv[i]);
    inputList.push_back(thisFile);  
  };

// Load all the preset files
  std::vector<std::string> presetList;
  std::vector<std::string> presetFiles;
  loadPresets(presetList, presetFiles);

// To-Do: Filter by file extension and expand directories (dirent.h)

// To-Do: Convert image sequence paths

// Remove duplicates from inputList
  std::vector<InputFile>::iterator duplicateRemover;
  sort(inputList.begin(), inputList.end());
  duplicateRemover = unique(inputList.begin(), inputList.end());
  inputList.resize( distance(inputList.begin(), duplicateRemover) );

// Sanity check: output the loaded presets and the file list
// To-Do: Move this to a test.
  std::cout << "Loaded the following presets:" << std::endl;
  for (int i = 0; i < presetList.size(); ++i) {
    std::cout << "  " << i << ": " << presetList[i] << std::endl;
  }
  std::cout << std::endl;

  std::cout << "File list:" << std::endl;
  for (int i = 0; i < inputList.size(); ++i) {
    std::cout << "  " << i << ": " << inputList[i].getPath() << std::endl;
  }
  std::cout << std::endl;

// Load the user's preset
  libconfig::Config cfg;
  std::string presetPath;
  presetPath = presetDir + presetFiles[preset];
  cfg.readFile(presetPath.c_str());
  std::string name = cfg.lookup("listname");
  std::cout << "Selected preset: " << name << std::endl << std::endl;

// Process each file in inputList
  std::vector<InputFile>::iterator inputIterator = inputList.begin();
  while (inputIterator != inputList.end()) {
    std::string ffmpegCommand;
    // Build the command for that file
    ffmpegCommand = buildCommand(*inputIterator, cfg);
    std::cout << ffmpegCommand.c_str() << std::endl;
    ++inputIterator;
  }

  return 0;
}