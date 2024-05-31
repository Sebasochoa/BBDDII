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
}

void SGBD::Cargar()
{
    std::string name_disk;
    std::cout << "Ingrese el Disco al que quiere Cargar el documento: ";
    std::cin >>name_disk;
    Records.Cargar(name_disk);
    int j = 0;
    for (size_t i = 0; i < Discos.size(); i++)
    {
        if (name_disk == Discos[i].Get_Name())
        {
            j = i;
        }
    }
    Discos[j].Upload_Blocks();
    
}
