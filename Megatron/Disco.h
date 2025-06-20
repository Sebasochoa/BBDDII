#ifndef DISCO_H
#define DISCO_H
#include "Bloques.h"
#include "BufferManager.h"
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
    BufferManager buffer;

public:
    Bloques Blocks;
    Disco();
    Disco(const Disco &other);
    Disco &operator=(const Disco &other);
    Disco(Disco &&other) noexcept;
    Disco &operator=(Disco &&other) noexcept;
    Disco(const std::string &nombre);
    Disco(const std::string &nombre, bool usarPorDefecto);
    Disco(const std::string &nombre, int platos, int superficies, int pistas, int sectores, int capSector, int sectoresPorBloque);

    std::string Get_Name();
    int MaxCapacity();
    int FullCapacity();
    void CargarRegistrosBloquesADiscoSlotted(std::string);
    void Clear_Blocks();
    int RemainCapacity(std::string);
    void First_Line(std::string, std::string);

    std::string CargarEnBloques(bool);
    void LlenarBloquesConRegistros();
    void GuardarRegistrosComoNuevaTabla(const std::vector<std::string> &registros, const std::string &nombreTablaOriginal, const std::string &atributo, const std::string &signo, const std::string &valor);
    bool EliminarRegistro(const std::string &nombreTabla, int id);
    void ReemplazarSectoresDesdeBloques();
    void MostrarResumenCapacidad();
    void MostrarSectoresOcupados();

    // Buffer related helpers
    std::string &RequestPage(int bloqueId, const std::string &ruta, bool write = false, bool pinned = false);
    void UnpinPage(int bloqueId);
    void PrintPageTable() const;

    bool InsertarRegistroEnBloqueYSector(int bloqueId, const std::string &registro);
    void ImprimirBloque(int);
};
#endif
