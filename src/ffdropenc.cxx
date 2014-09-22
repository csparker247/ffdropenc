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
  string preset;
  vector<string> inputlist;

  preset = argv[1];
  for (int i = 2; i < argc; i++) {
    inputlist.push_back(argv[i]);  
  };

  Config cfg;

  // Read the file. If there is an error, report it and exit.
  try
  {
    cfg.readFile("../presets/appletv.cfg");
  }
  catch(const FileIOException &fioex)
  {
    std::cerr << "I/O error while reading file." << std::endl;
    return 1;
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return 1;
  }

  string name = cfg.lookup("preset.listname");
  cout << "Preset name: " << name << endl << endl;

  return 0;
}