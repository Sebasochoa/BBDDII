#include "Disco.h"
#include <iostream>
#include <filesystem>

std::string obtenerRutaDirectorioPadre()
{
    std::filesystem::path rutaActual = std::filesystem::current_path();

    std::filesystem::path rutaPadre = rutaActual.parent_path();

    return rutaPadre.string();
}

Disco::Disco(const std::string &NDisco)
{
    std::string carpetaPadre = obtenerRutaDirectorioPadre();

    for (int i = 1; i <= 2; ++i)
    {
        std::stringstream ss;
        ss << i;
        std::string numeroComoCadena = ss.str();
        std::string nombreSubcarpeta = carpetaPadre + "/" + NDisco + "/subcarpeta_" + numeroComoCadena;
        std::filesystem::create_directory(nombreSubcarpeta);
    }
}

Disco::Disco(const std::string &NDisco, int NPlates, int NSurfaces, int NTracks, int NSections)
{
    std::filesystem::path carpetaPadre = std::filesystem::current_path();
    std::string dire = carpetaPadre.string();
    for (int i = 0; i < NPlates; i++)
    {
        std::filesystem::create_directory(NDisco);
    }
    
    for (int i = 1; i <= NSurfaces ++i)
    {
        std::stringstream ss;
        ss << i;
        std::string numeroComoCadena = ss.str();
        std::string nombreSubcarpeta = dire + "/" + NDisco + "/subcarpeta_" + numeroComoCadena;
        std::filesystem::create_directory(nombreSubcarpeta);
    }
}