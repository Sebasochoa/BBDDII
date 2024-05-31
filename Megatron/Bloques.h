#ifndef BLOQUES_H
#define BLOQUES_H
#include <string>
class Bloques
{
private:
    std::string NameDisk;
    int Capacity;
    int NumBlocks;

public:
    Bloques();
    void Initialize(int, int, int, std::string);
    int get_NumBlocks();
    void set_NumBlocks(int);
    int get_Capacity();
    void set_Capacity(int);
};

#endif