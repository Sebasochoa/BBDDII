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

Disco::Disco(const std::string &NDisco, int NPlates, int NSurfaces, int NTracks, int NSections, int Capacity, int NumSectorxBloque)
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
    Blocks.Initialize(Capacity, NumSectorxBloque, MaxCapacity(), NDisco);
}

std::string Disco::Get_Name()
{
    return Name;
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

void Disco::Upload_Blocks()
{

    fs::path currentPath = fs::current_path();

    std::string dirDisck = (currentPath / Name / "Bloques" / "Bloque_").string();
    int m = 1;
    std::ifstream Upload_File;
    bool moreLines = true;
    std::string Upload_File_Line;
    int contador = 1;

    auto open_next_block = [&]() {
        // Abrir el siguiente archivo de bloque
        Upload_File.open(dirDisck + std::to_string(m) + ".txt");
        if (Upload_File) {
            // Saltar la primera línea (el encabezado)
            if (!std::getline(Upload_File, Upload_File_Line)) {
                moreLines = false;
            }
        } else {
            moreLines = false;
        }
    };

    // Abrir el primer archivo de bloque
    open_next_block();

    for (int i = 1; i <= Plates && moreLines; ++i) {
        fs::path Directory_Plate = currentPath / Name / ("Plato_" + std::to_string(i));

        for (int j = 1; j <= Surfaces && moreLines; ++j) {
            fs::path Directory_Surface = Directory_Plate / ("Superficie_" + std::to_string(j));

            for (int k = 1; k <= Tracks && moreLines; ++k) {
                fs::path Directory_Track = Directory_Surface / ("Pista_" + std::to_string(k));

                for (int l = 1; l <= Sectors && moreLines; ++l) {
                    fs::path Directory_Sector = Directory_Track / ("Sector_" + std::to_string(l));
                    fs::path Directory_File = Directory_Sector / (std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt");

                    std::ofstream txt(Directory_File, std::ios::app);
                    std::cout << "Escribiendo en: " << Directory_File << std::endl;

                    while (Upload_File.is_open() && std::getline(Upload_File, Upload_File_Line)) {
                        int longitud = static_cast<int>(Upload_File_Line.length());
                        int Vacio = RemainCapacity(Directory_File.string());

                        if (Vacio > longitud) {
                            txt << Corregir(Upload_File_Line, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;
                            First_Line(Directory_File.string(), std::to_string(Vacio - longitud));
                            ++contador;
                        } else {
                            break; // Si el sector está lleno, pasa al siguiente sector
                        }
                    }

                    // Si llegamos al final del archivo de bloque actual, abre el siguiente
                    if (Upload_File.eof()) {
                        Upload_File.close();
                        ++m;
                        open_next_block();
                    }
                }
            }
        }
    }
}

int Disco::RemainCapacity(std::string Archivo)
{
    std::ifstream archivo(Archivo);
    std::string linea, segmento;
    getline(archivo, linea);
    for (size_t i = 0; i < linea.size(); i++)
    {
        if (linea[i] != '-')
        {
            segmento += linea[i];
        }
        else
        {
            break;
        }
    }
    std::istringstream ss(segmento);
    int valor;
    ss >> valor;
    return valor;
}

std::string Disco::Corregir(std::string linea, int contador, std::string dir)
{
    std::string res;
    res += dir + '#' + std::to_string(contador) + "#" + linea;
    return res;
}

void Disco::First_Line(std::string Directory_File, std::string Replace_Line)
{
    std::ifstream Source_File(Directory_File);

    std::string Content, Source_File_Line;
    while (std::getline(Source_File, Source_File_Line))
    {
        Content += Source_File_Line + "\n";
    }
    Source_File.close();

    size_t Start_Position = Content.find('\n');

    Content.replace(0, Start_Position, Replace_Line);

    std::ofstream Output_File(Directory_File);

    Output_File << Content;
    Output_File.close();
}
/*
int Disco::GetSectorCapacity()
{
    std::string Directory = fs::current_path().string() + "/" + Name;

    for (size_t i = 0; i < Plates; i++)
    {
        std::string Directory_Plate = Directory + "/Plato_" + std::to_string(i);

        for (size_t j = 0; j < Surfaces; j++)
        {
            std::string Directory_Surface = Directory_Plate + "/Superficie_" + std::to_string(j);

            for (size_t k = 0; k < Tracks; k++)
            {

                std::string Directory_Track = Directory_Surface + "/Pista_" + std::to_string(k);
                for (size_t l = 0; l < Sectors; l++)
                {
                    std::string Directory_Sector = Directory_Track + "/Sector_" + std::to_string(l);
                    std::ifstream Directory_File(Directory_Sector);
                    std::string File_Line;
                    getline(Directory_File, File_Line);
                    std::cout << "Capacidad del Sector: " << File_Line << std::endl;
                    continue;

                }
            }
        }
    }
}*/
