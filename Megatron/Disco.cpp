#include "Disco.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

Disco::Disco(const std::string &NDisco)
{
    Name = NDisco;
    Plates = 4;
    Surfaces = 2;
    Tracks = 3;
    Sectors = 4;
    CapSection = 1000;
    
    std::string directorio = fs::current_path().string();
    std::string dirNDisco = directorio + "/" + NDisco;

    fs::create_directory(dirNDisco);

    for (int i = 1; i <= 4; ++i)
    {
        std::string dirPlato = dirNDisco + "/Plato_" + std::to_string(i);
        fs::create_directory(dirPlato);

        for (int j = 1; j <= 2; ++j)
        {
            std::string dirSuperficie = dirPlato + "/Superficie_" + std::to_string(j);
            fs::create_directory(dirSuperficie);

            for (int k = 1; k <= 3; ++k)
            {
                std::string dirPista = dirSuperficie + "/Pista_" + std::to_string(k);
                fs::create_directory(dirPista);

                for (int l = 1; l <= 4; l++)
                {
                    std::string dirSector = dirPista + "/Sector_" + std::to_string(l);
                    std::string nomArchivo = dirSector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";
                    fs::create_directory(dirSector);
                    std::ofstream archivo(nomArchivo);
                }
            }
        }
    }
}

Disco::Disco(const std::string &NDisco, int NPlates, int NSurfaces, int NTracks, int NSections, int Capacity)
{
    Name = NDisco;
    Plates = NPlates;
    Surfaces = NSurfaces;
    Tracks = NTracks;
    Sectors = NSections;
    CapSection = Capacity;

    std::string directorio = fs::current_path().string();
    std::string dirNDisco = directorio + "/" + NDisco;

    fs::create_directory(dirNDisco);

    for (int i = 1; i <= NPlates; ++i)
    {
        std::string dirPlato = dirNDisco + "/Plato_" + std::to_string(i);
        fs::create_directory(dirPlato);

        for (int j = 1; j <= NSurfaces; ++j)
        {
            std::string dirSuperficie = dirPlato + "/Superficie_" + std::to_string(j);
            fs::create_directory(dirSuperficie);

            for (int k = 1; k <= NTracks; ++k)
            {
                std::string dirPista = dirSuperficie + "/Pista_" + std::to_string(k);
                fs::create_directory(dirPista);

                for (int l = 1; l <= NSections; l++)
                {
                    std::string dirSector = dirPista + "/Sector_" + std::to_string(l);

                    std::string nomArchivo = dirSector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";

                    fs::create_directory(dirSector);
                    std::ofstream archivo(nomArchivo);
                    if (archivo.is_open())
                    {
                        archivo << CapSection << std::endl;
                    }
                }
            }
        }
    }
}

int Disco::MaxCapacity()
{
    int Capacity = Plates * Surfaces * Tracks * Sectors * CapSection;
    return Capacity;
}

std::vector<std::string> encontrarDirectoriosConTXT(const std::string &directorioPadre)
{
    std::vector<std::string> directorios;
    for (const auto &plato : fs::directory_iterator(directorioPadre))
    {
        if (plato.is_directory())
        {
            for (const auto &superficie : fs::directory_iterator(plato.path()))
            {
                if (superficie.is_directory())
                {
                    for (const auto &pista : fs::directory_iterator(superficie.path()))
                    {
                        if (pista.is_directory())
                        {
                            for (const auto &sector : fs::directory_iterator(pista.path()))
                            {
                                if (sector.is_directory())
                                {
                                    for (const auto &archivo : fs::directory_iterator(sector.path()))
                                    {
                                        if (archivo.is_regular_file() && archivo.path().extension() == ".txt")
                                        {
                                            directorios.push_back(sector.path().string());
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return directorios;
}

int contarLineasLlenas(const std::string &nombreArchivo)
{
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir el archivo: " << nombreArchivo << std::endl;
        return 0;
    }

    int contador = 0;
    std::string linea;
    while (std::getline(archivo, linea))
    {
        if (!linea.empty())
        {
            contador++;
        }
    }

    archivo.close();
    return contador;
}

int contarLineasLlenasEnDirectorios(std::string NDisco)
{
    int totalLineas = 0;

    std::vector<std::string> directorios = encontrarDirectoriosConTXT(fs::current_path().string() + "\\" + NDisco);

    for (const auto &directorio : directorios)
    {
        for (const auto &archivo : fs::directory_iterator(directorio))
        {
            if (archivo.is_regular_file() && archivo.path().extension() == ".txt")
            {
                int lineasArchivo = contarLineasLlenas(archivo.path().string());
                totalLineas += lineasArchivo;
            }
        }
    }

    return totalLineas;
}

int Disco::FullCapacity(std::string NDisco)
{
    int s = contarLineasLlenasEnDirectorios(NDisco);
    return s * CapSection;
}

int Disco::GetSectorCapacity()
{
    return CapSection;
}
