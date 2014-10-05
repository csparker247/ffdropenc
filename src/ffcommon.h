#ifndef FFCOMMON_H
#define FFCOMMON_H

#include <vector>
#include <array>
#include <libconfig.h++>
#include "InputFile.h"

#define VERSION "2.0"
#define PRESET_DIR "../presets/"
#define PRESET_EXT "cfg"

// Extension filters for image sequences.
// To-Do: Move these to a config so there's no need to recompile
const std::array<std::string, 7> img_extension = { "DPX", "JPG", "JPEG", "PNG", "TIF", "TIFF", "TGA" };

// Helper function declarations
void search(const std::string directory, const std::string extension, std::vector<std::string>& results);
void expandDir(const std::string directory, std::vector<std::string>& results);
std::string basename (const std::string str);
std::string dirname (const std::string str);

bool isImage (const std::string str);
std::string makeImageName (const std::string str);

// ffdropenc specific function declarations
void loadPresets(std::vector<std::string>& presetList, std::vector<std::string>& presetFiles);
std::string buildFilterGraph(const libconfig::Setting& filters);

#endif