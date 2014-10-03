#ifndef INPUTFILE_H
#define INPUTFILE_H

#include <libconfig.h++>

namespace ffdropenc {
class InputFile {
  public:
    InputFile ();
    InputFile (std::string);

    bool operator < (const InputFile&) const;
    bool operator == (const InputFile&) const;

    void setIO(std::string);
    void setPath(std::string);
    void setOutname(std::string);
    std::string getPath() const;
    std::string getOutname() const;
    std::string buildCommand(const libconfig::Config& cfg) const;

  private:
    std::string path;
    std::string outname;
};
}

#endif