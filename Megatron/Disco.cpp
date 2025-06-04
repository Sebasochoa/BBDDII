#include "Disco.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <set>

namespace fs = std::filesystem;

Disco::Disco(const std::string &NDisco, bool usarPorDefecto)
{
    Name = NDisco;
    Plates = 4;
    Surfaces = 2;
    Tracks = 3;
    Sectors = 4;
    CapSection = 1000;
    SectoresPorBloque = 2;

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

    std::ofstream meta(fs::current_path().string() + "/Discos/" + Name + "/metadata.txt");
    meta << "CapSection=" << CapSection << "\n";
    meta << "SectoresPorBloque=" << SectoresPorBloque << "\n";
    meta.close();
}

Disco::Disco(const std::string &NDisco)
{
    Name = NDisco;
    Plates = Surfaces = Tracks = Sectors = 0;
    CapSection = 0;

    std::string rutaBase = fs::current_path().string() + "/Discos/" + NDisco;
    if (!fs::exists(rutaBase))
        return;

    int maxPlato = 0, maxPista = 0, maxSector = 0;
    bool archivoCapacidadLeido = false;

    for (const auto &plato : fs::directory_iterator(rutaBase))
    {
        if (!plato.is_directory() || plato.path().filename().string().rfind("Plato_", 0) != 0)
            continue;

        int p = std::stoi(plato.path().filename().string().substr(6));
        maxPlato = std::max(maxPlato, p);

        const auto &superficiePath = plato.path() / "Superficie_1";
        if (!fs::exists(superficiePath))
            continue;

        for (const auto &pista : fs::directory_iterator(superficiePath))
        {
            if (!pista.is_directory() || pista.path().filename().string().rfind("Pista_", 0) != 0)
                continue;
            int t = std::stoi(pista.path().filename().string().substr(6));
            maxPista = std::max(maxPista, t);

            for (const auto &sector : fs::directory_iterator(pista.path()))
            {
                if (!sector.is_directory() || sector.path().filename().string().rfind("Sector_", 0) != 0)
                    continue;
                int se = std::stoi(sector.path().filename().string().substr(7));
                maxSector = std::max(maxSector, se);

                if (!archivoCapacidadLeido)
                {
                    for (const auto &archivo : fs::directory_iterator(sector.path()))
                    {
                        if (archivo.is_regular_file() && archivo.path().extension() == ".txt")
                        {
                            std::ifstream in(archivo.path());
                            if (in.is_open())
                            {
                                std::string linea;
                                std::getline(in, linea);
                                try
                                {
                                    CapSection = std::stoi(linea);
                                    archivoCapacidadLeido = true;
                                }
                                catch (...)
                                {
                                }
                                in.close();
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    Plates = maxPlato;
    Surfaces = 2; // fijo
    Tracks = maxPista;
    Sectors = maxSector;
    std::ifstream meta(fs::current_path().string() + "/Discos/" + Name + "/metadata.txt");
    std::string linea;
    std::cout << linea;
    while (std::getline(meta, linea))
    {
        if (linea.rfind("CapSection=", 0) == 0)
            CapSection = std::stoi(linea.substr(11));
        else if (linea.rfind("SectoresPorBloque=", 0) == 0)
            SectoresPorBloque = std::stoi(linea.substr(18));
    }
    meta.close();

    // Inicializar bloques con los valores recuperados
    int capacidadBloque = CapSection * SectoresPorBloque;
    Blocks.set_Capacity(capacidadBloque);

    // Contar cantidad de bloques (archivos .txt) en la carpeta de bloques
    std::string rutaBloques = fs::current_path().string() + "/Discos/Bloques_" + Name;
    int numBloques = 0;
    if (fs::exists(rutaBloques))
    {
        for (const auto &archivo : fs::directory_iterator(rutaBloques))
        {
            if (archivo.path().extension() == ".txt")
                ++numBloques;
        }
    }
    Blocks.set_NumBlocks(numBloques);
    Blocks.set_NameDisk(Name);

    std::cout << "Disco '" << Name << "' cargado desde almacenamiento:\n";
    std::cout << "Platos: " << Plates << "  Superficies: " << Surfaces
              << "  Pistas: " << Tracks << "  Sectores: " << Sectors
              << "  Capacidad por sector: " << CapSection << " bytes\n";
}

Disco::Disco(const std::string &NDisco, int NPlates, int NSurfaces, int NTracks, int NSections, int Capacity, int NumSectorxBloque)
{
    Name = NDisco;
    Plates = NPlates;
    Surfaces = NSurfaces;
    Tracks = NTracks;
    Sectors = NSections;
    CapSection = Capacity;
    SectoresPorBloque = NumSectorxBloque;

    std::string directorio = fs::current_path().string();
    std::string dirNDisco = directorio + "/Discos";

    fs::create_directory(dirNDisco);
    dirNDisco = dirNDisco + "/" + NDisco;
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
    std::cout << "Disco " << NDisco << ": \n";
    std::cout << "Capacidad: " << MaxCapacity() << " B\n";
    std::cout << "Capacidad del Bloque: " << NumSectorxBloque * Capacity << " B\n";
    std::cout << "Capacidad del Sector: " << Capacity << " B\n";

    std::ofstream meta(fs::current_path().string() + "/Discos/" + Name + "/metadata.txt");
    meta << "CapSection=" << CapSection << "\n";
    meta << "SectoresPorBloque=" << SectoresPorBloque << "\n";
    meta.close();
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

bool Disco::IsRecord_inTable(std::string linea, std::string NTabla)
{
    size_t pos1 = linea.find('#');
    if (pos1 == std::string::npos)
        return false;
    size_t pos2 = linea.find('#', pos1 + 1);
    if (pos2 == std::string::npos)
        return false;
    size_t pos3 = linea.find('#', pos2 + 1);
    if (pos3 == std::string::npos)
        return false;

    std::string tabla = linea.substr(pos2 + 1, pos3 - pos2 - 1);
    return tabla == NTabla;
}

void Disco::Upload_Blocks(std::string Name_Table)
{
    fs::path currentPath = fs::current_path();
    std::string dirBloques = (currentPath / "Discos" / ("Bloques_" + Name)).string();

    int registrosCargados = 0;
    int registrosPendientes = 0;
    int bloqueIndex = 1;
    std::vector<std::string> registros;

    // Leer todos los registros de todos los bloques
    while (true)
    {
        std::string pathBloque = dirBloques + "/Bloque_" + std::to_string(bloqueIndex) + ".txt";
        std::ifstream bloqueFile(pathBloque);
        if (!bloqueFile.is_open())
            break;

        std::string primeraLinea;
        std::getline(bloqueFile, primeraLinea); // Salta la línea de capacidad del bloque
        std::string registro;
        while (std::getline(bloqueFile, registro))
        {
            registros.push_back(registro);
        }
        bloqueFile.close();
        bloqueIndex++;
    }

    int ubicaciones = Plates * Surfaces * Tracks * Sectors;
    for (int idx = 0; idx < registros.size(); ++idx)
    {
        int pos = idx % ubicaciones;
        int plato = pos % Plates + 1;
        int superficie = (pos / Plates) % Surfaces + 1;
        int pista = (pos / (Plates * Surfaces)) % Tracks + 1;
        int sector = (pos / (Plates * Surfaces * Tracks)) % Sectors + 1;

        fs::path archivoSector = currentPath / "Discos" / Name /
                                 ("Plato_" + std::to_string(plato)) /
                                 ("Superficie_" + std::to_string(superficie)) /
                                 ("Pista_" + std::to_string(pista)) /
                                 ("Sector_" + std::to_string(sector)) /
                                 (std::to_string(plato) + std::to_string(superficie) + std::to_string(pista) + std::to_string(sector) + ".txt");

        int capacidadRestante = RemainCapacity(archivoSector.string());
        if (capacidadRestante >= static_cast<int>(registros[idx].length()) + 1)
        { // +1 por salto de línea
            std::ofstream sectorOut(archivoSector, std::ios::app);
            if (sectorOut.is_open())
            {
                sectorOut << registros[idx] << "\n";
                sectorOut.close();
                First_Line(archivoSector.string(), std::to_string(capacidadRestante - registros[idx].length() - 1));
                registrosCargados++;
            }
            else
            {
                registrosPendientes++;
            }
        }
        else
        {
            registrosPendientes++;
        }
    }

    std::cout << "Registros cargados al disco: " << registrosCargados << "\n";
    if (registrosPendientes > 0)
        std::cout << "Registros pendientes por falta de espacio: " << registrosPendientes << "\n";
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

std::string Disco::CargarEnBloques(bool formatoFijo)
{
    std::string archivo, nombreTabla;
    std::cout << "Ingrese el nombre del archivo a cargar: ";
    std::cin >> archivo;
    std::cout << "Ingrese nombre a su Relacion: ";
    std::cin >> nombreTabla;
    Blocks.CargarDesdeArchivo(archivo + ".csv", nombreTabla, formatoFijo);
    return nombreTabla;
}

void Disco::Clear_Blocks()
{
    int capacidadBloque = CapSection * SectoresPorBloque;
    std::string rutaBloques = fs::current_path().string() + "/Discos/Bloques_" + Name;

    if (!fs::exists(rutaBloques))
        return;

    for (const auto &archivo : fs::directory_iterator(rutaBloques))
    {
        if (archivo.path().extension() == ".txt")
        {
            std::ofstream limpiar(archivo.path(), std::ios::trunc);
            if (limpiar.is_open())
            {
                limpiar << std::to_string(capacidadBloque) << "\n";
                limpiar.close();
            }
        }
    }
}
