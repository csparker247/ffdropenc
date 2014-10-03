#ifndef FFCOMMON_H
#define FFCOMMON_H

#include <vector>
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

#endif