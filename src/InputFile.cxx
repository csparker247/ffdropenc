#include <string>

#include "InputFile.h"
#include "ffcommon.h"

// Initializers
InputFile::InputFile () {}

InputFile::InputFile (std::string p) {
  path = p;
  outname = basename(p);
}

// Operators
bool InputFile::operator < (const InputFile& file) const {
  return (path < file.path);
}

bool InputFile::operator == (const InputFile& file) const {
  return (path == file.path);
}

// Functions
void InputFile::setIO(std::string p) {
  path = p;
  outname = basename(p);
}

void InputFile::setIO(std::string p, std::string o) {
  path = p;
  outname = o;
}

std::string InputFile::getPath() const {
  return path;
}

std::string InputFile::getOutname() const {
  return outname;
}