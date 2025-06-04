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
    int SectoresPorBloque;
    std::string Name;
    Bloques Blocks;

public:
    Disco(const std::string &nombre);                                                                                              
    Disco(const std::string &nombre, bool usarPorDefecto);                                                                         
    Disco(const std::string &nombre, int platos, int superficies, int pistas, int sectores, int capSector, int sectoresPorBloque); 

    std::string Get_Name();
    int MaxCapacity();
    int FullCapacity(std::string);
    // int GetSectorCapacity();
    bool IsRecord_inTable(std::string, std::string);
    void Upload_Blocks(std::string);
    void Clear_Blocks();
    int RemainCapacity(std::string);
    std::string Corregir(std::string, int, std::string);
    void First_Line(std::string, std::string);

    std::string CargarEnBloques(bool);
};
#endif
