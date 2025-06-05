#include <vector>
#include "Records_Manager.h"
#include "Disco.h"

class SGBD
{
private:
    std::vector<Disco> Discos;
    Records_Manager Records;
public:
    SGBD();
    ~SGBD();
    void Create_Disk();
    void Cargar();
    void Select();
    void Select_Discriminado();
    void Select_Discriminado_Archivo();
    void Buscar_reemplazar();
    Disco BuscarDisco(std::string);
    void MostrarEstadoDisco();
    void MostrarSectoresOcupados();
    void MostrarBloquesOcupados();
};
