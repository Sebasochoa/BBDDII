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
    int Blocks;
    int CapSection;
    std::string Name;

public:
    Disco(const std::string&);
    Disco(const std::string&, int, int, int, int, int, int);
    int MaxCapacity();
    int FullCapacity(std::string);
    int GetSectorCapacity();
};
#endif
