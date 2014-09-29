#ifndef FFCOMMON_H
#define FFCOMMON_H

#include <stdlib.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include <dirent.h>
#include <unistd.h>
#include <libconfig.h++>

#include "InputFile.h"

#define VERSION "2.0"
#define PRESET_DIR "../presets/"
#define PRESET_EXT "cfg"

// Helper function declarations
void search(std::string directory, std::string extension, std::vector<std::string>& results);
std::string basename (const std::string& str);

// ffdropenc specific function declarations
void loadPresets(std::vector<std::string>& presetList, std::vector<std::string>& presetFiles);
std::string buildFilterGraph(const libconfig::Setting& filters);
std::string buildCommand(const InputFile& inputFile, const libconfig::Config& cfg);

#endif