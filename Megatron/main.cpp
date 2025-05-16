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
    // g++ -o main main.cpp SGBD.cpp Records_Manager.cpp Disco.cpp Bloques.cpp
    SGBD A;
    //A.Create_Disk();
    //A.Cargar();
    A.Buscar_reemplazar();
    return 0;
}
