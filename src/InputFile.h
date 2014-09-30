#ifndef INPUTFILE_H
#define INPUTFILE_H

class InputFile {
  public:
    InputFile ();
    InputFile (std::string);

    bool operator < (const InputFile&) const;
    bool operator == (const InputFile&) const;

    void setIO(std::string);
    void setIO(std::string, std::string);
    std::string getPath() const;
    std::string getOutname() const;

  private:
    std::string path;
    std::string outname;
};

#endif