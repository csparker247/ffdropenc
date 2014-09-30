// ffdropenc

#include "ffdropenc.h"
#include "ffcommon.h"
#include "InputFile.h"

using namespace std;
using namespace libconfig;

// Environment defines
string presetDir = PRESET_DIR;
string cfg_extension = PRESET_EXT;

int main (int argc, char* argv[]) {
  cout << endl;
  cout << " ------------------------- " << endl;
  cout << "         ffdropenc         " << endl;
  cout << " ------------------------- " << endl;
  cout << endl;
  
// Parse command line options
  int preset;
  vector<InputFile> inputList;

  preset = stoi(argv[1]);
  for (int i = 2; i < argc; ++i) {
    InputFile thisFile(argv[i]);
    inputList.push_back(thisFile);  
  };

// Load all the preset files
  vector<string> presetList;
  vector<string> presetFiles;
  loadPresets(presetList, presetFiles);

// To-Do: Recognize image sequences in inputList?

// Remove duplicates from inputList
  vector<InputFile>::iterator duplicateRemover;
  sort(inputList.begin(), inputList.end());
  duplicateRemover = unique(inputList.begin(), inputList.end());
  inputList.resize( distance(inputList.begin(), duplicateRemover) );

// Sanity check: output the loaded presets and the file list
// To-Do: Move this to a test.
  cout << "Loaded the following presets:" << endl;
  for (int i = 0; i < presetList.size(); ++i) {
    cout << "  " << i << ": " << presetList[i] << endl;
  }
  cout << endl;

  cout << "File list:" << endl;
  for (int i = 0; i < inputList.size(); ++i) {
    cout << "  " << i << ": " << inputList[i].path << endl;
  }
  cout << endl;

// Load the user's preset
  Config cfg;
  string presetPath;
  presetPath = presetDir + presetFiles[preset];
  cfg.readFile(presetPath.c_str());
  string name = cfg.lookup("listname");
  cout << "Selected preset: " << name << endl << endl;

// Process each file in inputList
  vector<InputFile>::iterator inputIterator = inputList.begin();
  while (inputIterator != inputList.end()) {
    string ffmpegCommand;
    // Build the command for that file
    ffmpegCommand = buildCommand(*inputIterator, cfg);
    cout << ffmpegCommand << endl;
    ++inputIterator;
  }

  return 0;
}