#include "Disco.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <set>

namespace fs = std::filesystem;

Disco::Disco()
{
    Name = "";
    Plates = Surfaces = Tracks = Sectors = CapSection = 0;
    SectoresPorBloque = 0;
    Blocks.SetBufferManager(&buffer);
}

Disco::Disco(const Disco &other)
{
    Plates = other.Plates;
    Surfaces = other.Surfaces;
    Tracks = other.Tracks;
    Sectors = other.Sectors;
    CapSection = other.CapSection;
    SectoresPorBloque = other.SectoresPorBloque;
    Name = other.Name;
    buffer = other.buffer;
    Blocks = other.Blocks;
    Blocks.SetBufferManager(&buffer);
}

Disco &Disco::operator=(const Disco &other)
{
    if (this != &other)
    {
        Plates = other.Plates;
        Surfaces = other.Surfaces;
        Tracks = other.Tracks;
        Sectors = other.Sectors;
        CapSection = other.CapSection;
        SectoresPorBloque = other.SectoresPorBloque;
        Name = other.Name;
        buffer = other.buffer;
        Blocks = other.Blocks;
        Blocks.SetBufferManager(&buffer);
    }
    return *this;
}

Disco::Disco(Disco &&other) noexcept
{
    Plates = other.Plates;
    Surfaces = other.Surfaces;
    Tracks = other.Tracks;
    Sectors = other.Sectors;
    CapSection = other.CapSection;
    SectoresPorBloque = other.SectoresPorBloque;
    Name = std::move(other.Name);
    buffer = std::move(other.buffer);
    Blocks = std::move(other.Blocks);
    Blocks.SetBufferManager(&buffer);
}

Disco &Disco::operator=(Disco &&other) noexcept
{
    if (this != &other)
    {
        Plates = other.Plates;
        Surfaces = other.Surfaces;
        Tracks = other.Tracks;
        Sectors = other.Sectors;
        CapSection = other.CapSection;
        SectoresPorBloque = other.SectoresPorBloque;
        Name = std::move(other.Name);
        buffer = std::move(other.buffer);
        Blocks = std::move(other.Blocks);
        Blocks.SetBufferManager(&buffer);
    }
    return *this;
}

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
                    std::fstream archivo(nomArchivo, std::ios::out);
                    if (archivo.is_open())
                    {
                        std::vector<std::pair<int, int>> emptySlots;
                        Blocks.EscribirHeaderSlottedPage(archivo, 0, 0, emptySlots);
                        archivo.close();
                    }
                }
            }
        }
    }

    std::ofstream meta(fs::current_path().string() + "/Discos/" + Name + "/metadata.txt");
    meta << "CapSection=" << CapSection << "\n";
    meta << "SectoresPorBloque=" << SectoresPorBloque << "\n";
    meta.close();
    Blocks.SetBufferManager(&buffer);
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
                                if (linea != "SLOTTED_PAGE")
                                {
                                    try
                                    {
                                        CapSection = std::stoi(linea);
                                    }
                                    catch (...)
                                    {
                                    }
                                }
                                archivoCapacidadLeido = true;
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
    Blocks.SetBufferManager(&buffer);
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
                    std::fstream archivo(nomArchivo, std::ios::out);
                    if (archivo.is_open())
                    {
                        std::vector<std::pair<int, int>> emptySlots;
                        Blocks.EscribirHeaderSlottedPage(archivo, 0, 0, emptySlots);
                        archivo.close();
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
    Blocks.SetBufferManager(&buffer);
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

int Disco::FullCapacity()
{
    int totalCapacidadInicial = 0;
    int totalCapacidadDisponible = 0;

    std::string rutaBase = fs::current_path().string() + "/Discos/" + Name;

    for (const auto &plato : fs::directory_iterator(rutaBase))
    {
        if (!plato.is_directory() || plato.path().filename().string().rfind("Plato_", 0) != 0)
            continue;
        for (const auto &superficie : fs::directory_iterator(plato.path()))
        {
            if (!superficie.is_directory())
                continue;
            for (const auto &pista : fs::directory_iterator(superficie.path()))
            {
                if (!pista.is_directory())
                    continue;
                for (const auto &sector : fs::directory_iterator(pista.path()))
                {
                    if (!sector.is_directory())
                        continue;
                    for (const auto &archivo : fs::directory_iterator(sector.path()))
                    {
                        if (archivo.is_regular_file() && archivo.path().extension() == ".txt")
                        {
                            std::fstream in(archivo.path(), std::ios::in);
                            if (!in.is_open())
                                continue;
                            std::string linea;
                            std::getline(in, linea);
                            int capacidadRestante = 0;
                            if (linea == "SLOTTED_PAGE")
                            {
                                in.seekg(0, std::ios::beg);
                                int numSlots, freeOffset;
                                std::vector<std::pair<int, int>> slots;
                                Blocks.LeerHeaderSlottedPage(in, numSlots, freeOffset, slots);
                                capacidadRestante = CapSection - freeOffset;
                            }
                            totalCapacidadDisponible += capacidadRestante;
                            totalCapacidadInicial += CapSection;
                        }
                    }
                }
            }
        }
    }
    int capacidadOcupada = totalCapacidadInicial - totalCapacidadDisponible;
    return capacidadOcupada;
}

void Disco::CargarRegistrosBloquesADiscoSlotted(std::string Name_Table)
{
    fs::path currentPath = fs::current_path();
    std::string dirBloques = (currentPath / "Discos" / ("Bloques_" + Name)).string();

    std::vector<std::string> registros;
    for (int bloqueIndex = 1; bloqueIndex <= Blocks.get_NumBlocks(); ++bloqueIndex)
    {
        auto regs = Blocks.LeerTodosLosRegistrosEnBloque(bloqueIndex);
        for (const auto &r : regs)
        {
            if (!Name_Table.empty())
            {
                size_t p1 = r.find('#');
                if (p1 == std::string::npos)
                    continue;
                size_t p2 = r.find('#', p1 + 1);
                if (p2 == std::string::npos)
                    continue;
                std::string tabla = r.substr(p1 + 1, p2 - p1 - 1);
                if (tabla != Name_Table)
                    continue;
            }
            registros.push_back(r);
        }
    }

    // Llenar los sectores en orden, guardando todo lo que quepa en cada uno
    int registrosCargados = 0, registrosPendientes = 0;
    size_t idxReg = 0;
    for (int sectorId = 0; sectorId < Plates * Surfaces * Tracks * Sectors && idxReg < registros.size(); ++sectorId)
    {
        int plato = sectorId % Plates + 1;
        int superficie = (sectorId / Plates) % Surfaces + 1;
        int pista = (sectorId / (Plates * Surfaces)) % Tracks + 1;
        int sector = (sectorId / (Plates * Surfaces * Tracks)) % Sectors + 1;

        fs::path archivoSector = currentPath / "Discos" / Name /
                                 ("Plato_" + std::to_string(plato)) /
                                 ("Superficie_" + std::to_string(superficie)) /
                                 ("Pista_" + std::to_string(pista)) /
                                 ("Sector_" + std::to_string(sector)) /
                                 (std::to_string(plato) + std::to_string(superficie) + std::to_string(pista) + std::to_string(sector) + ".txt");

        while (idxReg < registros.size())
        {
            const std::string &reg = registros[idxReg];
            if (!Blocks.InsertarRegistroEnArchivo(archivoSector.string(), reg, CapSection))
                break;
            ++idxReg;
            ++registrosCargados;
        }
    }

    registrosPendientes = registros.size() - idxReg;
    std::cout << "Registros cargados al disco (slotted): " << registrosCargados << "\n";
    if (registrosPendientes > 0)
        std::cout << "Registros pendientes por falta de espacio: " << registrosPendientes << "\n";
}

int Disco::RemainCapacity(std::string Archivo)
{
    std::fstream archivo(Archivo, std::ios::in);
    if (!archivo.is_open())
        return 0;

    std::string linea;
    std::getline(archivo, linea);
    if (linea == "SLOTTED_PAGE")
    {
        int numSlots, freeOffset;
        std::vector<std::pair<int, int>> slots;
        Blocks.LeerHeaderSlottedPage(archivo, numSlots, freeOffset, slots);
        archivo.close();
        return CapSection - freeOffset;
    }

    std::string segmento;
    for (size_t i = 0; i < linea.size(); i++)
    {
        if (linea[i] != '-')
            segmento += linea[i];
        else
            break;
    }
    std::istringstream ss(segmento);
    int valor;
    ss >> valor;
    archivo.close();
    return valor;
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
                limpiar << "SLOTTED_PAGE\n";
                limpiar << "NumSlots=0\n";
                limpiar << "FreeSpaceOffset=0\n";
                limpiar << "#DATA\n";
                limpiar.close();
            }
        }
    }
}

void Disco::LlenarBloquesConRegistros()
{
    fs::path currentPath = fs::current_path();
    std::vector<std::string> registros;

    for (int i = 1; i <= Plates; ++i)
    {
        for (int j = 1; j <= Surfaces; ++j)
        {
            for (int k = 1; k <= Tracks; ++k)
            {
                for (int l = 1; l <= Sectors; ++l)
                {
                    fs::path archivoSector = currentPath / "Discos" / Name /
                                             ("Plato_" + std::to_string(i)) /
                                             ("Superficie_" + std::to_string(j)) /
                                             ("Pista_" + std::to_string(k)) /
                                             ("Sector_" + std::to_string(l)) /
                                             (std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt");

                    std::fstream sector(archivoSector, std::ios::in);
                    if (!sector.is_open())
                        continue;

                    std::string linea;
                    std::getline(sector, linea); // saltar capacidad

                    if (linea == "SLOTTED_PAGE")
                    {
                        int numSlots, freeOffset;
                        std::vector<std::pair<int, int>> slots;
                        Blocks.LeerHeaderSlottedPage(sector, numSlots, freeOffset, slots);
                        for (const auto &slot : slots)
                        {
                            sector.seekg(slot.first, std::ios::beg);
                            std::string data(slot.second, '\0');
                            sector.read(&data[0], slot.second);
                            registros.push_back(data);
                        }
                    }
                    else
                    {
                        std::string registro;
                        while (std::getline(sector, registro))
                            registros.push_back(registro);
                    }
                    sector.close();
                }
            }
        }
    }

    // Insertar los registros obtenidos en los bloques usando el nuevo formato slotted page
    for (size_t idxReg = 0; idxReg < registros.size(); ++idxReg)
    {
        bool insertado = false;
        for (int bloqueId = 1; bloqueId <= Blocks.get_NumBlocks(); ++bloqueId)
        {
            if (Blocks.InsertarRegistroEnBloque(registros[idxReg], bloqueId))
            {
                insertado = true;
                break;
            }
        }
        if (!insertado)
            break; // no hay espacio disponible
    }
}

std::string ReemplazarNombreTablaEnRegistro(const std::string &reg, const std::string &nuevoNombre)
{
    // Encuentra las posiciones de los # para ubicar el campo de la tabla
    size_t p1 = reg.find('#');
    if (p1 == std::string::npos)
        return reg;
    size_t p2 = reg.find('#', p1 + 1);
    if (p2 == std::string::npos)
        return reg;
    size_t p3 = reg.find('#', p2 + 1);
    if (p3 == std::string::npos)
        return reg;

    // Reemplaza el campo de la tabla
    std::string nuevoRegistro = reg.substr(0, p2 + 1) + nuevoNombre + reg.substr(p3);
    return nuevoRegistro;
}

void Disco::GuardarRegistrosComoNuevaTabla(const std::vector<std::string> &registros, const std::string &nombreTablaOriginal, const std::string &atributo, const std::string &signo, const std::string &valor)
{
    // 1. Nombre de la nueva tabla/esquema
    std::string nombreTablaNueva = nombreTablaOriginal;
    if (!atributo.empty() && !signo.empty() && !valor.empty())
    {
        nombreTablaNueva += "_" + atributo + signo + valor;
    }
    else
    {
        nombreTablaNueva += "_filtrada";
    }

    // 2. Guardar registros filtrados en el disco con el nuevo nombre de tabla
    fs::path currentPath = fs::current_path();
    int ubicaciones = Plates * Surfaces * Tracks * Sectors;
    int registrosCargados = 0, registrosPendientes = 0;
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

        std::string registroModificado = ReemplazarNombreTablaEnRegistro(registros[idx], nombreTablaNueva);

        int capacidadRestante = RemainCapacity(archivoSector.string());
        if (capacidadRestante >= static_cast<int>(registroModificado.length()) + 1)
        {
            std::ofstream sectorOut(archivoSector, std::ios::app);
            if (sectorOut.is_open())
            {
                sectorOut << registroModificado << "\n";
                sectorOut.close();
                First_Line(archivoSector.string(), std::to_string(capacidadRestante - registroModificado.length() - 1));
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

    // 3. Copiar el esquema original como base para el nuevo esquema y modificar el nombre
    std::string rutaEsquemaOrig = currentPath.string() + "/Discos/" + Name + "/Esquemas/" + nombreTablaOriginal + ".esq";
    std::string rutaEsquemaNuevo = currentPath.string() + "/Discos/" + Name + "/Esquemas/" + nombreTablaNueva + ".esq";

    std::ifstream src(rutaEsquemaOrig);
    std::string lineaEsquema;
    std::getline(src, lineaEsquema);
    src.close();

    // Cambia el nombre de la tabla al inicio del esquema
    size_t posTab = lineaEsquema.find('#');
    if (posTab != std::string::npos)
    {
        lineaEsquema = nombreTablaNueva + lineaEsquema.substr(posTab);
    }
    std::ofstream dst(rutaEsquemaNuevo);
    dst << lineaEsquema << std::endl;
    dst.close();

    std::cout << "Registros guardados en la nueva tabla: " << nombreTablaNueva << " (" << registrosCargados << " registros).\n";
    if (registrosPendientes > 0)
        std::cout << "Registros pendientes por falta de espacio: " << registrosPendientes << "\n";
}

void Disco::ReemplazarSectoresDesdeBloques()
{
    fs::path currentPath = fs::current_path();

    // 1. Obtener todos los registros almacenados en los bloques
    std::vector<std::string> registros;
    for (int bloqueIdx = 1; bloqueIdx <= Blocks.get_NumBlocks(); ++bloqueIdx)
    {
        auto regs = Blocks.LeerTodosLosRegistrosEnBloque(bloqueIdx);
        registros.insert(registros.end(), regs.begin(), regs.end());
    }

    // 2. Reescribir los sectores secuencialmente usando formato slotted page
    size_t idxReg = 0;
    int totalSectores = Plates * Surfaces * Tracks * Sectors;
    for (int sectorId = 0; sectorId < totalSectores; ++sectorId)
    {
        int plato = sectorId % Plates + 1;
        int superficie = (sectorId / Plates) % Surfaces + 1;
        int pista = (sectorId / (Plates * Surfaces)) % Tracks + 1;
        int sector = (sectorId / (Plates * Surfaces * Tracks)) % Sectors + 1;

        fs::path archivoSector = currentPath / "Discos" / Name /
                                 ("Plato_" + std::to_string(plato)) /
                                 ("Superficie_" + std::to_string(superficie)) /
                                 ("Pista_" + std::to_string(pista)) /
                                 ("Sector_" + std::to_string(sector)) /
                                 (std::to_string(plato) + std::to_string(superficie) + std::to_string(pista) + std::to_string(sector) + ".txt");

        // Reiniciar el sector antes de insertar
        {
            std::fstream out(archivoSector, std::ios::out | std::ios::binary | std::ios::trunc);
            if (out.is_open())
            {
                std::vector<std::pair<int, int>> emptySlots;
                Blocks.EscribirHeaderSlottedPage(out, 0, 0, emptySlots);
                out.close();
            }
        }

        // Insertar tantos registros como quepan en el sector
        while (idxReg < registros.size())
        {
            if (!Blocks.InsertarRegistroEnArchivo(archivoSector.string(), registros[idxReg], CapSection))
                break;
            ++idxReg;
        }
    }
}

bool Disco::EliminarRegistro(const std::string &nombreTabla, int id)
{
    bool ok = Blocks.EliminarRegistro(nombreTabla, id);
    if (ok)
    {
        buffer.flushAll();
        ReemplazarSectoresDesdeBloques();
    }
    return ok;
}

void Disco::MostrarResumenCapacidad()
{
    int capacidadTotal = MaxCapacity();
    int capacidadOcupada = FullCapacity();
    int capacidadLibre = capacidadTotal - capacidadOcupada;

    std::cout << "Resumen del Disco: " << Name << std::endl;
    std::cout << "Capacidad total: " << capacidadTotal << " bytes" << std::endl;
    std::cout << "Capacidad ocupada: " << capacidadOcupada << " bytes" << std::endl;
    std::cout << "Capacidad libre: " << capacidadLibre << " bytes" << std::endl;

    // Sectores ocupados/libres
    int sectoresTotales = Plates * Surfaces * Tracks * Sectors;
    int sectoresOcupados = 0;

    std::string rutaBase = fs::current_path().string() + "/Discos/" + Name;
    for (const auto &plato : fs::directory_iterator(rutaBase))
    {
        if (!plato.is_directory() || plato.path().filename().string().rfind("Plato_", 0) != 0)
            continue;
        for (const auto &superficie : fs::directory_iterator(plato.path()))
        {
            if (!superficie.is_directory())
                continue;
            for (const auto &pista : fs::directory_iterator(superficie.path()))
            {
                if (!pista.is_directory())
                    continue;
                for (const auto &sector : fs::directory_iterator(pista.path()))
                {
                    if (!sector.is_directory())
                        continue;
                    for (const auto &archivo : fs::directory_iterator(sector.path()))
                    {
                        if (archivo.is_regular_file() && archivo.path().extension() == ".txt")
                        {
                            std::fstream in(archivo.path(), std::ios::in);
                            if (!in.is_open())
                                continue;
                            std::string linea;
                            std::getline(in, linea);
                            bool ocupado = false;
                            if (linea == "SLOTTED_PAGE")
                            {
                                in.seekg(0, std::ios::beg);
                                int numSlots, freeOffset;
                                std::vector<std::pair<int, int>> slots;
                                Blocks.LeerHeaderSlottedPage(in, numSlots, freeOffset, slots);
                                if (numSlots > 0)
                                    ocupado = true;
                            }
                            else if (in.peek() != EOF)
                            {
                                ocupado = true;
                            }
                            if (ocupado)
                                sectoresOcupados++;
                        }
                    }
                }
            }
        }
    }
    std::cout << "Sectores totales: " << sectoresTotales << std::endl;
    std::cout << "Sectores ocupados: " << sectoresOcupados << std::endl;
    std::cout << "Sectores libres: " << sectoresTotales - sectoresOcupados << std::endl;
}

void Disco::MostrarSectoresOcupados()
{
    std::cout << "Sectores ocupados en Disco: " << Name << std::endl;
    std::string rutaBase = fs::current_path().string() + "/Discos/" + Name;
    for (const auto &plato : fs::directory_iterator(rutaBase))
    {
        if (!plato.is_directory() || plato.path().filename().string().rfind("Plato_", 0) != 0)
            continue;
        std::string platoStr = plato.path().filename().string();
        for (const auto &superficie : fs::directory_iterator(plato.path()))
        {
            if (!superficie.is_directory())
                continue;
            std::string superficieStr = superficie.path().filename().string();
            for (const auto &pista : fs::directory_iterator(superficie.path()))
            {
                if (!pista.is_directory())
                    continue;
                std::string pistaStr = pista.path().filename().string();
                for (const auto &sector : fs::directory_iterator(pista.path()))
                {
                    if (!sector.is_directory())
                        continue;
                    std::string sectorStr = sector.path().filename().string();
                    for (const auto &archivo : fs::directory_iterator(sector.path()))
                    {
                        if (archivo.is_regular_file() && archivo.path().extension() == ".txt")
                        {
                            std::fstream in(archivo.path(), std::ios::in);
                            if (!in.is_open())
                                continue;
                            std::string linea;
                            std::getline(in, linea); // Cabecera
                            int count = 0;
                            if (linea == "SLOTTED_PAGE")
                            {
                                in.seekg(0, std::ios::beg);
                                int numSlots, freeOffset;
                                std::vector<std::pair<int, int>> slots;
                                Blocks.LeerHeaderSlottedPage(in, numSlots, freeOffset, slots);
                                count = numSlots;
                            }
                            if (count > 0)
                            {
                                std::cout << platoStr << " - " << superficieStr << " - " << pistaStr << " - " << sectorStr
                                          << ": " << count << " registro(s)" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Disco::MostrarUbicacionBloques()
{
    std::cout << "Ubicación de Bloques en Disco: " << Name << std::endl;
    int bloqueActual = 1;
    for (int p = 1; p <= Plates; ++p)
    {
        for (int s = 1; s <= Surfaces; ++s)
        {
            for (int t = 1; t <= Tracks; ++t)
            {
                for (int se = 1; se <= Sectors; ++se)
                {
                    std::cout << "Bloque " << bloqueActual
                              << ": Plato " << p
                              << ", Superficie " << s
                              << ", Pista " << t
                              << ", Sector " << se << std::endl;
                    bloqueActual++;
                    // Si tienes menos bloques que sectores, puedes salir aquí
                    if (bloqueActual > Blocks.get_NumBlocks())
                        return;
                }
            }
        }
    }
}

std::string &Disco::RequestPage(int bloqueId, const std::string &ruta, bool write, bool pinned)
{
    return buffer.readBlock(bloqueId, ruta, write, pinned);
}

void Disco::SavePage(int bloqueId)
{
    buffer.writeBlock(bloqueId);
}

void Disco::MarkDirty(int bloqueId)
{
    buffer.markDirty(bloqueId);
}

void Disco::UnpinPage(int bloqueId)
{
    buffer.unpin(bloqueId);
}

void Disco::PrintPageTable() const
{
    buffer.printPageTable();
}

bool Disco::InsertarRegistroEnBloqueYSector(int bloqueId, const std::string &registro)
{
    if (!Blocks.InsertarRegistroEnBloque(registro, bloqueId))
        return false;

    // Buscar un sector con espacio disponible
    namespace fs = std::filesystem;
    fs::path base = fs::current_path();
    int total = Plates * Surfaces * Tracks * Sectors;
    for (int sid = 0; sid < total; ++sid)
    {
        int p = sid % Plates + 1;
        int su = (sid / Plates) % Surfaces + 1;
        int t = (sid / (Plates * Surfaces)) % Tracks + 1;
        int se = (sid / (Plates * Surfaces * Tracks)) % Sectors + 1;
        fs::path sectorFile = base / "Discos" / Name /
                               ("Plato_" + std::to_string(p)) /
                               ("Superficie_" + std::to_string(su)) /
                               ("Pista_" + std::to_string(t)) /
                               ("Sector_" + std::to_string(se)) /
                               (std::to_string(p) + std::to_string(su) + std::to_string(t) + std::to_string(se) + ".txt");
        if (Blocks.InsertarRegistroEnArchivo(sectorFile.string(), registro, CapSection))
        {
            buffer.markDirty(bloqueId);
            buffer.writeBlock(bloqueId);
            return true;
        }
    }
    return false;
}