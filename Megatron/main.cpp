#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>
#include "Disco.h"
#include "Megatron.h"

namespace fs = std::filesystem;

void Menu()
{
    std::cout << "1. Crear Disco Default.\n2. Crear Disco Personalizado.\n3. Cargar Archivo.\n4. Select *.\n5.Salir.\nIngrese la opcion a realizar:";
}

int main()
{
    //  g++ -o main main.cpp Megatron.cpp Disco.cpp
    Megatron DB;
    Disco A("Disco_1");
    Disco B("Disco_",4,2,4,8,1000);
    DB.Cargar("Disco_1");
    DB.Select_("Disco_1");
    B.FullCapacity("Disco_1");
    B.MaxCapacity();
    std::cout << B.GetSectorCapacity();



    return 0;
}
