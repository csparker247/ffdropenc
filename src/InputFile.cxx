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

void InputFile::setPath(std::string p) {
  path = p;
}

void InputFile::setOutname(std::string o) {
  outname = o;
}

std::string InputFile::getPath() const {
  return path;
}

std::string InputFile::getOutname() const {
  return outname;
}