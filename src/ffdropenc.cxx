// ffdropenc

#include "ffdropenc.h"
#include "ffcommon.h"
#include "InputFile.h"

// Environment defines
const std::string presetDir = PRESET_DIR;
const std::string cfg_extension = PRESET_EXT;

int main (int argc, char* argv[]) {
  std::cout << std::endl;
  std::cout << " ------------------------- " << std::endl;
  std::cout << "         ffdropenc         " << std::endl;
  std::cout << " ------------------------- " << std::endl;
  std::cout << std::endl;
  
// Parse preset selection from command line
  int preset;
  preset = std::stoi(argv[1]);

// Load all the preset files
  std::vector<std::string> presetList;
  std::vector<std::string> presetFiles;
  loadPresets(presetList, presetFiles);

// Make a vector of InputFiles
// This step filters out all non-files and expands directories
  std::vector<ffdropenc::InputFile> inputList;  
  struct stat buffer;
  std::vector<std::string> childPaths;
  for (int i = 2; i < argc; ++i) {
    std::string tempPath = argv[i];
    lstat(tempPath.c_str(), &buffer);
    if (S_ISDIR(buffer.st_mode)) {
      expandDir(tempPath.c_str(), childPaths);
    }
    else if (S_ISREG(buffer.st_mode)) {
      ffdropenc::InputFile newFile;
      // If this is an image, convert its path to one ffmpeg can accept
      if (isImage(tempPath)){
        tempPath = makeImageName(tempPath);
        newFile.isImgSeq(true);
      }
      else { 
        newFile.isImgSeq(false);
      }
      newFile.setPath(tempPath);
      inputList.push_back(newFile);
    } 
  };

// Add all of the regular files we found in directories to the inputList
// There's another Image Sequence filtering step here
  std::vector<std::string>::iterator addPaths = childPaths.begin();
  while (addPaths != childPaths.end()) {
    ffdropenc::InputFile newFile;
    std::string tempPath = *addPaths;
    if (isImage(tempPath)){
      tempPath = makeImageName(tempPath);
      newFile.isImgSeq(true);
    }
    else { 
      newFile.isImgSeq(false);
    }
    newFile.setPath(tempPath);
    inputList.push_back(newFile);
    ++addPaths;
  }

// Remove duplicates from inputList
  std::vector<ffdropenc::InputFile>::iterator duplicateRemover;
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


///// START THE REAL PROCESSING /////

// Load the user's preset
  libconfig::Config cfg;
  std::string presetPath;
  presetPath = presetDir + presetFiles[preset];
  cfg.readFile(presetPath.c_str());
  std::string name = cfg.lookup("listname");
  std::cout << "Selected preset: " << name << std::endl << std::endl;

// Process each file in inputList
  std::vector<ffdropenc::InputFile>::iterator inputIterator = inputList.begin();
  while (inputIterator != inputList.end()) {
    std::string ffmpegCommand;
    // Build the command for that file
    ffmpegCommand = inputIterator->buildCommand(cfg);
    std::cout << ffmpegCommand.c_str() << std::endl;
    ++inputIterator;
  }

  return 0;
}