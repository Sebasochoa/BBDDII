#ifndef BLOQUES_H
#define BLOQUES_H
#include <string>
#include <vector>
#include <fstream>

class Bloques
{
private:
    std::string NameDisk;
    int Capacity;
    int NumBlocks;

public:
    Bloques();
    void Initialize(int, int, int, std::string);
    void set_NameDisk(std::string);
    int get_NumBlocks();
    void set_NumBlocks(int);
    int get_Capacity();
    void set_Capacity(int);

    bool CargarDesdeArchivo(const std::string &, const std::string &, bool);
    bool CargarRegistros(const std::vector<std::string> &);
    std::vector<std::string> FiltrarRegistros(const std::string &nombreDisco, const std::string &nombreTabla, const std::string &campoFiltro = "", const std::string &operador = "", const std::string &valorFiltro = "");
    void MostrarRegistros(const std::vector<std::string> &registros, const std::string &nombreDisco, const std::string &nombreTabla);
    void MostrarBloquesOcupados();
    void MostrarDetalleBloque(int numBloque);
    bool InsertarRegistroEnBloque(const std::string &registro, int bloqueId);
    bool InsertarRegistroEnArchivo(const std::string &ruta, const std::string &registro, int capacidad);
    bool AgregarRegistroManual(const std::string &nombreTabla, const std::vector<std::string> &valores, bool esFijo);
    int CapacidadMaximaRegistro();
    void EscribirHeaderSlottedPage(std::fstream &bloque, int numSlots, int freeOffset, const std::vector<std::pair<int, int>> &slots);
    void LeerHeaderSlottedPage(std::fstream &bloque, int &numSlots, int &freeOffset, std::vector<std::pair<int, int>> &slots);
     std::vector<std::string> LeerTodosLosRegistrosEnBloque(int bloqueId);
};

#endif