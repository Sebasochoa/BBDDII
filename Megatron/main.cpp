#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>

#include "SGBD.h"

namespace fs = std::filesystem;

void Menu()
{
    std::cout << "1. Crear Disco Personalizado.\n2. Cargar Archivo.\n3. Select *\n4. Select <\n5. Mostrar Estado del Disco.\n6. Mostrar Sectores Ocupados.\n7. Mostrar Bloques Ocupados.\n8. Eliminar Registro.\n9. Operar Paginas.\n10. Salir.\nIngrese la opcion a realizar: ";
}

int main()
{
    // g++ -o main main.cpp SGBD.cpp Disco.cpp Bloques.cpp
    SGBD A;
    int opc = 0;
    while (opc != -1)
    {
        Menu();
        std::cin >> opc;
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
            A.Select_Discriminado();
            break;
        case 5:
            A.MostrarEstadoDisco();
            break;
        case 6:
            A.MostrarSectoresOcupados();
            break;
        case 7:
            A.MostrarBloquesOcupados();
            break;
        case 8:
            A.EliminarRegistro();
            break;
        case 9:
            A.OperarPaginas();
            break;
        case 10:
            opc = -1;
        default:
            break;
        }
    }

    return 0;
}
