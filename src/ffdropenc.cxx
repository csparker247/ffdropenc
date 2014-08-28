// ffdropenc
#include <iostream>
#include <iomanip>

#include "ffdropenc.h"

#include "boost/program_options.hpp"

using namespace std;
namespace po = boost::program_options;

#define VERSION "2.0"

int main (int argc, char* argv[]) {
  cout << "ffdropenc " << VERSION << setprecision(5) << endl;
  
// Parse command line options
  string preset;
  vector<string> inputfiles;

  po::options_description desc("Generic options");
  desc.add_options()
    ("help,h", "show help")
    ("preset,p", po::value(&preset), "select output preset")
    ("input,i", po::value(&inputfiles), "input file(s)")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }

  if (vm.count("preset")) {
    cout << "You selected the " << preset << " preset." << endl;
  }

  return 0;
}