#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>

#include "SGBD.h"

namespace fs = std::filesystem;

void Menu()
{
    std::cout << "1. Crear Disco Personalizado.\n2. Cargar Archivo.\n3. Select *.\n4. Select <\n5. Salir.\nIngrese la opcion a realizar:";
}

int main()
{
    // g++ -o main main.cpp SGBD.cpp Records_Manager.cpp Disco.cpp Bloques.cpp
    SGBD A;
    int opc = 0;
    while (opc!= -1)
    {
        Menu();
        std::cin>>opc;
        switch (opc)
        {
        case 1:
            A.Create_Disk();
            break;
        case 2:
            A.Cargar();
            break;
        case 3:
            A.Select();
            break;
        case 4:
            A.Select_Discriminado_Archivo();
            break;
        case 5:
            opc = -1;
        
        default:
            break;
        }
    }
    
    
    return 0;
}
