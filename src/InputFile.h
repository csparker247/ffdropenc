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
    void setOutpath(std::string);
    void setFps(std::string);
    void setDuration(unsigned long long);
    std::string getPath() const;
    std::string getOutname() const;
    std::string getOutpath() const;
    std::string getFps() const;
    unsigned long long getDuration() const;
    std::string buildCommand(const libconfig::Config& cfg) const;
    bool isImgSeq() const;
    void isImgSeq(bool);

  private:
    std::string path;
    std::string outpath;
    std::string outname;
    std::string fps;
    unsigned long long duration;
    bool isSeq;
};
}

#endif