#include "SGBD.h"
#include <iostream>

SGBD::SGBD()
{
}

SGBD::~SGBD()
{
}

void SGBD::Create_Disk()
{
    std::string Name_Disk;
    int NPlates, NTracks, NSections, Capacity, NumSectorxBloque;
    std::cout << "Ingrese nombre del Disco: ";
    std::cin >> Name_Disk;
    std::cout << "Ingrese Cantidad de Platos: ";
    std::cin >> NPlates;
    std::cout << "Ingrese Cantidad de Pistas: ";
    std::cin >> NTracks;
    std::cout << "Ingrese Cantidad de Sectores: ";
    std::cin >> NSections;
    std::cout << "Ingrese Capacidad del Sector: ";
    std::cin >> Capacity;
    std::cout << "Ingrese Cantidad de Sectores x Bloque: ";
    std::cin >> NumSectorxBloque;

    Discos.push_back(Disco(Name_Disk,NPlates,2,NTracks,NSections,Capacity,NumSectorxBloque));
    std::cout << Discos[0].Get_Name() << std::endl;
}
