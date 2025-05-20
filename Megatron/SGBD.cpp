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

    Discos.push_back(Disco(Name_Disk, NPlates, 2, NTracks, NSections, Capacity, NumSectorxBloque));
}

void SGBD::Cargar()
{
    std::string name_disk;
    std::cout << "Ingrese el Disco al que quiere Cargar el documento: ";
    std::cin >> name_disk;
    std::string Table = Records.Cargar(name_disk);
    int j = 0;
    for (size_t i = 0; i < Discos.size(); i++)
    {
        if (name_disk == Discos[i].Get_Name())
        {
            j = i;
        }
    }
    Discos[j].Upload_Blocks(Table);
}

void SGBD::Select()
{
    std::string name_disk;
    std::cout << "Ingrese el Disco de donde quiere seleccionar la Tabla: ";
    std::cin >> name_disk;
    Records.Select_all(name_disk);
}

void SGBD::Select_Discriminado()
{
    std::string name_disk, name_atribute, sign, name_squeme;
    int valor;
    std::cout << "Ingrese el Disco de donde quiere seleccionar la Tabla: ";
    std::cin >> name_disk;
    std::cout << "Ingrese la Tabla de donde quiere seleccionar el Atributo: ";
    std::cin >> name_squeme;
    std::cout << "Ingrese el Atributo a seleccionar: ";
    std::cin >> name_atribute;
    std::cout << "Ingrese signo con el que seleccionara el atributo ( >, <, =) ";
    std::cin >> sign;
    std::cout << "Ingrese el valor que desea seleccionar ";
    std::cin >> valor;
    Records.Select_(name_squeme, name_atribute, sign, valor, name_disk);
}

void SGBD::Select_Discriminado_Archivo()
{
    std::string name_disk, name_atribute, sign, name_squeme;
    int valor;
    std::cout << "Ingrese el Disco de donde quiere seleccionar la Tabla: ";
    std::cin >> name_disk;
    std::cout << "Ingrese la Tabla de donde quiere seleccionar el Atributo: ";
    std::cin >> name_squeme;
    std::cout << "Ingrese el Atributo a seleccionar: ";
    std::cin >> name_atribute;
    std::cout << "Ingrese signo con el que seleccionara el atributo ( >, <, =) ";
    std::cin >> sign;
    std::cout << "Ingrese el valor que desea seleccionar ";
    std::cin >> valor;
    Records.Select_(name_squeme, name_atribute, sign, valor, name_disk);
}

void SGBD::Buscar_reemplazar()
{
    std::string name_disk, name_squeme;

    std::cout << "Ingrese el Disco de donde quiere buscar la Tabla: ";
    std::cin >> name_disk;
    std::cout << "Ingrese la Tabla de donde quiere seleccionar el Atributo: ";
    std::cin >> name_squeme;
    Records.ReemplazarRegistro(name_disk, name_squeme);
}