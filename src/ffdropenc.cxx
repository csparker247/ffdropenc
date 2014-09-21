// ffdropenc
#include <iostream>
#include <iomanip>

#include "ffdropenc.h"

#include "cereal/cereal.hpp"

using namespace std;

int main (int argc, char* argv[]) {
  cout << endl;
  cout << " ------------------------- " << endl;
  cout << "         ffdropenc         " << endl;
  cout << " ------------------------- " << endl;
  cout << endl;
  
// Parse command line options
  string preset = argv[1];
  vector<string> inputfiles(argv + 2, argv + argc);

  return 0;
}