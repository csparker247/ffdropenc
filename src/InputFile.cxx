#include <string>

#include "InputFile.h"
#include "ffcommon.h"

InputFile::InputFile (std::string p) {
  path = p;
  outname = basename(p);
}