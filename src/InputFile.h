#ifndef INPUTFILE_H
#define INPUTFILE_H

class InputFile {
  public:
    InputFile ();
    InputFile (std::string);
    bool operator < (const InputFile& str) const;
    bool operator == (const InputFile& str) const;
    std::string path;
    std::string outname;
  private:
};

#endif