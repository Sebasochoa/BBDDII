#ifndef DISCO_H
#define DISCO_H
#include <string>
class Disco
{
private:
    int Plates;
    int Surfaces;
    int Tracks;
    int Section;

public:
    Disco(const std::string&);
    Disco(const std::string&, int, int, int, int);
};
#endif
