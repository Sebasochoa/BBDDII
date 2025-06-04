#ifndef BLOQUES_H
#define BLOQUES_H
#include <string>
#include <vector>


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
};

#endif