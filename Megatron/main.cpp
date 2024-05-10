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

    Megatron DB;
    //  g++ -o main main.cpp Megatron.cpp Disco.cpp
    
    int opc;
    while (opc != 5)
    {
        Menu();
        std::cin >> opc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (opc)
        {
        case 1:
            std::string name;
            std::cout << "Ingrese nombre del disco: ";
            std::cin >> name;
            Disco A(name);
            break;
        case 2:
            std::string name;
            int Plates, Surfaces, Tracks, Section, Blocks, CapSection;
            std::cout << "Ingrese nombre del disco: ";
            std::cin >> name;
            std::cout << "Ingrese Platos del disco: ";
            std::cin >> Plates;
            std::cout << "Ingrese Pistas del disco: ";
            std::cin >> Tracks;
            std::cout << "Ingrese Sectores del disco: ";
            std::cin >> Section;
            std::cout << "Ingrese la capacidad del sector: ";
            std::cin >> CapSection;
            std::cout << "Ingrese numero de bloques: ";
            std::cin >> Blocks;
            Disco B(name, Plates, 2, Tracks, Section, Blocks, CapSection);
            break;
        case 3:
            DB.Cargar();
            break;
        case 4:
            DB.Select_();
            break;
        case 5:
            opc = 5;
            break;
        default:
            std::cout << "Seleccione una opcion: ";
            std::cin >> opc;
            break;
        }
    }

    return 0;
}
