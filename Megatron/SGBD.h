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
};

