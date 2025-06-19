#include <vector>
#include "Disco.h"

class SGBD
{
private:
    std::vector<Disco> Discos;
public:
    SGBD();
    ~SGBD();
    void Create_Disk();
    void Cargar();
    void Select();
    void Select_Discriminado();
    void Select_Discriminado_Archivo();
    void Buscar_reemplazar();
    Disco* BuscarDisco(const std::string &);
    void EliminarRegistro();
    void MostrarEstadoDisco();
    void MostrarSectoresOcupados();
    void MostrarBloquesOcupados();
    void OperarPaginas();
    void MostrarBloque();
    void MostrarPageTable();
    void RequerirPagina();

};
