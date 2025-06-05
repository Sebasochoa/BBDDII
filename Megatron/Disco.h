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

public:
    Bloques Blocks;
    Disco();
    Disco(const std::string &nombre);
    Disco(const std::string &nombre, bool usarPorDefecto);
    Disco(const std::string &nombre, int platos, int superficies, int pistas, int sectores, int capSector, int sectoresPorBloque);

    std::string Get_Name();
    int MaxCapacity();
    int FullCapacity();
    void Upload_Blocks(std::string);
    void Clear_Blocks();
    int RemainCapacity(std::string);
    void First_Line(std::string, std::string);

    std::string CargarEnBloques(bool);
    void LlenarBloquesConRegistros();
    void GuardarRegistrosComoNuevaTabla(const std::vector<std::string> &registros, const std::string &nombreTablaOriginal, const std::string &atributo, const std::string &signo, const std::string &valor);
    void MostrarResumenCapacidad();
    void MostrarSectoresOcupados();
    void MostrarUbicacionBloques();
};
#endif
