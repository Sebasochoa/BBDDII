#ifndef DISCO_H
#define DISCO_H
#include "Bloques.h"
class Disco
{
private:
    int Plates;
    int Surfaces;
    int Tracks;
    int Sectors;
    int CapSection;
    std::string Name;
    Bloques Blocks;

public:
    Disco(const std::string &);
    Disco(const std::string &, int, int, int, int, int, int);
    std::string Get_Name();
    int MaxCapacity();
    int FullCapacity(std::string);
    //int GetSectorCapacity();
    void Upload_Blocks();
    void Clear_Blocks();
    int RemainCapacity(std::string);
    std::string Corregir(std::string, int ,std::string);
    void First_Line(std::string, std::string);
};
#endif
