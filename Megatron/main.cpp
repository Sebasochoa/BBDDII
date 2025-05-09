#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>

#include "SGBD.h"

namespace fs = std::filesystem;

void Menu()
{
    std::cout << "1. Crear Disco Default.\n2. Crear Disco Personalizado.\n3. Cargar Archivo.\n4. Select *.\n5.Salir.\nIngrese la opcion a realizar:";
}

int main()
{
    //  g++ -o main main.cpp Megatron.cpp Disco.cpp
    /*Megatron DB;
    Disco A("Disco_1",5,2,3,4,1500,4);
    DB.Cargar("Disco_1");*/
    SGBD A;
    //A.Create_Disk();
    //A.Cargar();
    //A.Cargar();
    A.Select_Discriminado();


    return 0;
}
