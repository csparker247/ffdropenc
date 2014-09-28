// ffdropenc
#include <iostream>
#include <string>
#include <vector>

#include <libconfig.h++>

#include "ffdropenc.h"

using namespace std;
using namespace libconfig;

int main (int argc, char* argv[]) {
  cout << endl;
  cout << " ------------------------- " << endl;
  cout << "         ffdropenc         " << endl;
  cout << " ------------------------- " << endl;
  cout << endl;
  
// Parse command line options
  int preset;
  vector<string> inputList;

  preset = stoi(argv[1]);
  for (int i = 2; i < argc; ++i) {
    inputList.push_back(argv[i]);  
  };

// Load all the preset files
  vector<string> presetList;
  vector<string> presetFiles;
  loadPresets(presetList, presetFiles);
  
// Sanity check: output the loaded presets and the file list
  cout << "Loaded the following presets:" << endl;
  for (int i = 0; i < presetList.size(); ++i) {
    cout << "  " << i << ": " << presetList[i] << endl;
  }
  cout << endl;

  cout << "File list:" << endl;
  for (int i = 0; i < inputList.size(); ++i) {
    cout << "  " << i << ": " << inputList[i] << endl;
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
  vector<string>::iterator inputIterator = inputList.begin();
  while (inputIterator != inputList.end()) {
    string ffmpegCommand;
    // Build the command for that file
    ffmpegCommand = buildCommand(*inputIterator, cfg);
    cout << ffmpegCommand << endl;
    ++inputIterator;
  }

  return 0;
}