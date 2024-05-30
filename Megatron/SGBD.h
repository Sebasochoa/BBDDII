#include <vector>
//#include "Megatron.h"
#include "Disco.h"

class SGBD
{
private:
    std::vector<Disco> Discos;

public:
    SGBD();
    ~SGBD();
    void Create_Disk();
};

