#include <string>

#include "InputFile.h"
#include "ffcommon.h"

InputFile::InputFile () {}

InputFile::InputFile (std::string p) {
  path = p;
  outname = basename(p);
}

bool InputFile::operator < (const InputFile& str) const {
	return (path < str.path);
}

bool InputFile::operator == (const InputFile& str) const {
	return (path == str.path);
}