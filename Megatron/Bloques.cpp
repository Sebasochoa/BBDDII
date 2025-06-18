#include "Bloques.h"
#include "BufferManager.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <queue>
#include <iomanip>

namespace fs = std::filesystem;

static void ParseHeaderFromString(const std::string &data, int &numSlots, int &freeOffset,
                                  std::vector<std::pair<int, int>> &slots)
{
    std::stringstream ss(data);
    std::string line;
    std::getline(ss, line); // SLOTTED_PAGE
    if (line != "SLOTTED_PAGE")
        return;
    std::getline(ss, line); // NumSlots
    numSlots = std::stoi(line.substr(line.find('=') + 1));
    std::getline(ss, line); // FreeSpaceOffset
    freeOffset = std::stoi(line.substr(line.find('=') + 1));
    slots.clear();
    for (int i = 0; i < numSlots; ++i)
    {
        std::getline(ss, line);
        size_t eq = line.find('=');
        size_t comma = line.find(',', eq + 1);
        int offset = std::stoi(line.substr(eq + 1, comma - eq - 1));
        int size = std::stoi(line.substr(comma + 1));
        slots.push_back({offset, size});
    }
    std::getline(ss, line); // #DATA
}

static std::string BuildHeaderString(int numSlots, int &freeOff,
                                     std::vector<std::pair<int, int>> &slots)
{
    int headerSize = 0;
    std::string header;
    while (true)
    {
        int offset = headerSize;
        for (int i = 0; i < numSlots; ++i)
        {
            const_cast<std::pair<int, int> &>(slots[i]).first = offset;
            offset += slots[i].second;
        }
        freeOff = offset;
        std::ostringstream oss;
        oss << "SLOTTED_PAGE\n";
        oss << "NumSlots=" << numSlots << "\n";
        oss << "FreeSpaceOffset=" << freeOff << "\n";
        for (int i = 0; i < numSlots; ++i)
            oss << "Slot" << i << "=" << slots[i].first << "," << slots[i].second << "\n";
        oss << "#DATA\n";
        header = oss.str();
        if ((int)header.size() == headerSize)
            break;
        headerSize = (int)header.size();
    }
    return header;
}

Bloques::Bloques()
{
    bufferManager = nullptr;
}

void Bloques::SetBufferManager(BufferManager *bm)
{
    bufferManager = bm;
}

void Bloques::Initialize(int capacity, int numBlocksx, int maxCapacity, std::string nameDisk)
{
    Capacity = capacity * numBlocksx;
    NumBlocks = maxCapacity / (capacity * numBlocksx);
    NameDisk = nameDisk;
    std::string dirDisco = fs::current_path().string() + "/Discos/Bloques_" + nameDisk;
    fs::create_directory(dirDisco);
    for (size_t i = 0; i < NumBlocks; i++)
    {
        std::string dirBloques = dirDisco + "/Bloque_" + std::to_string(i + 1) + ".txt";

        std::ofstream archivo(dirBloques);
        if (archivo.is_open())
        {
            archivo << "SLOTTED_PAGE\n";
            archivo << "NumSlots=0\n";
            archivo << "FreeSpaceOffset=0\n";
            archivo << "#DATA\n";
            archivo.close();
        }
    }
}

void Bloques::set_NameDisk(std::string N_Disco)
{
    NameDisk = N_Disco;
}

int Bloques::get_NumBlocks()
{
    return NumBlocks;
}

void Bloques::set_NumBlocks(int numBlocks)
{
    NumBlocks = numBlocks;
}

int Bloques::get_Capacity()
{
    return Capacity;
}

void Bloques::set_Capacity(int capacity)
{
    Capacity = capacity;
}

#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <algorithm>

bool EsEntero(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool EsDecimal(const std::string &s)
{
    std::istringstream iss(s);
    float f;
    return (iss >> f) && iss.eof();
}

std::vector<std::string> SplitCSVLine(const std::string &linea)
{
    std::vector<std::string> resultado;
    std::string campo;
    bool dentroDeComillas = false;

    for (size_t i = 0; i < linea.length(); ++i)
    {
        char c = linea[i];
        if (c == '"')
        {
            dentroDeComillas = !dentroDeComillas;
        }
        else if (c == ',' && !dentroDeComillas)
        {
            resultado.push_back(campo);
            campo.clear();
        }
        else
        {
            campo += c;
        }
    }
    resultado.push_back(campo);
    return resultado;
}

std::string GenerarEsquema(const std::string &nombreArchivoCSV, const std::string &nombreTabla, bool esFijo, const std::string &nombreDisco)
{
    std::ifstream file(nombreArchivoCSV);
    std::string lineaCabecera;
    if (!std::getline(file, lineaCabecera))
    {
        return "";
    }

    std::vector<std::string> nombresAtributos = SplitCSVLine(lineaCabecera);
    std::vector<std::string> tipos(nombresAtributos.size(), "str");
    std::vector<int> longitudes(nombresAtributos.size(), 0);

    std::string linea;
    while (std::getline(file, linea))
    {
        std::vector<std::string> valores = SplitCSVLine(linea);
        for (size_t i = 0; i < valores.size() && i < nombresAtributos.size(); ++i)
        {
            if (EsEntero(valores[i]))
            {
                if (tipos[i] == "str")
                    tipos[i] = "int";
            }
            else if (EsDecimal(valores[i]))
            {
                if (tipos[i] == "str" || tipos[i] == "int")
                    tipos[i] = "float";
            }
            else
            {
                tipos[i] = "str";
            }
            if (esFijo)
            {
                int longitud = static_cast<int>(valores[i].length());
                if (longitud > longitudes[i])
                    longitudes[i] = longitud;
            }
        }
    }

    std::string esquema = nombreTabla + "#";
    for (size_t i = 0; i < nombresAtributos.size(); ++i)
    {
        int longitudFinal = esFijo ? std::max(10, longitudes[i]) : 0;
        esquema += nombresAtributos[i] + "#" + tipos[i] + "#" + std::to_string(longitudFinal) + "#";
    }
    esquema += esFijo ? "FI" : "VA";

    // Guardar el esquema
    std::string rutaEsquema = fs::current_path().string() + "/Discos/" + nombreDisco + "/Esquemas";
    fs::create_directories(rutaEsquema);
    std::ofstream esquemaOut(rutaEsquema + "/" + nombreTabla + ".esq");
    esquemaOut << esquema << std::endl;
    esquemaOut.close();

    return esquema;
}

std::string FormatearFI(const std::string &linea, const std::string &esquema, const std::string &tabla, int id)
{
    std::vector<std::string> valores = SplitCSVLine(linea);
    std::string resultado = std::to_string(id) + "#" + tabla + "#";

    size_t pos = esquema.find('#');
    std::vector<int> longitudes;

    int campo = 0;
    while ((pos = esquema.find('#', pos + 1)) != std::string::npos)
    {
        size_t tipoPos = esquema.find('#', pos + 1);
        size_t lenPos = esquema.find('#', tipoPos + 1);
        if (lenPos == std::string::npos)
            break;

        std::string longitudStr = esquema.substr(tipoPos + 1, lenPos - tipoPos - 1);
        int longitud = std::stoi(longitudStr);
        longitudes.push_back(longitud);
        pos = lenPos;
        campo++;
    }

    for (size_t i = 0; i < valores.size(); ++i)
    {
        std::string campo = valores[i];
        int longitud = (i < longitudes.size()) ? longitudes[i] : 10;
        if (campo.length() < longitud)
            resultado += std::string(longitud - campo.length(), ' ') + campo;
        else
            resultado += campo.substr(0, longitud);
    }
    return resultado;
}

std::string Escape(const std::string &campo)
{
    std::string r;
    for (char c : campo)
    {
        if (c == '|' || c == '#')
            r += '\\';
        r += c;
    }
    return r;
}

std::string FormatearVA(const std::string &linea, const std::string &tabla, int id)
{
    std::vector<std::string> campos = SplitCSVLine(linea);
    std::string datos, metadatos;

    for (size_t i = 0; i < campos.size(); ++i)
    {
        std::string campo = Escape(campos[i]);
        datos += campo + "|";
        metadatos += std::to_string(i) + ":" + std::to_string(campos[i].length()) + ";";
    }

    return std::to_string(id) + "#" + tabla + "#" + datos + "#METADATA:" + metadatos;
}

bool Bloques::CargarDesdeArchivo(const std::string &nombreArchivoCSV, const std::string &nombreTabla, bool esFormatoFijo)
{
    std::ifstream archivo(nombreArchivoCSV);
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir el archivo: " << nombreArchivoCSV << std::endl;
        return false;
    }

    // Generar y guardar esquema
    std::string esquema = GenerarEsquema(nombreArchivoCSV, nombreTabla, esFormatoFijo, NameDisk);
    if (esquema.empty())
    {
        std::cerr << "Error al generar esquema." << std::endl;
        return false;
    }

    std::string linea;
    std::getline(archivo, linea); // saltar encabezado

    std::vector<std::string> registrosFormateados;
    int id = 1;
    while (std::getline(archivo, linea))
    {
        std::string registro;
        if (esFormatoFijo)
            registro = FormatearFI(linea, esquema, nombreTabla, id);
        else
            registro = FormatearVA(linea, nombreTabla, id);

        registrosFormateados.push_back(registro);
        id++;
    }

    archivo.close();
    return CargarRegistros(registrosFormateados);
}

bool Bloques::CargarRegistros(const std::vector<std::string> &registros)
{
    size_t idxReg = 0;
    for (; idxReg < registros.size(); ++idxReg)
    {
        bool insertado = false;
        // Prueba en cada bloque hasta insertarlo
        for (int bloqueId = 1; bloqueId <= NumBlocks; ++bloqueId)
        {
            if (InsertarRegistroEnBloque(registros[idxReg], bloqueId))
            {
                insertado = true;
                break;
            }
        }
        if (!insertado)
            break; // Ya no hay espacio en ningún bloque
    }
    return idxReg == registros.size(); // True si todos fueron insertados
}

// Asume estructura simple: campo#tipo#longitud#...#FI/VA
struct Campo
{
    std::string nombre;
    std::string tipo;
    int longitud;
};

// Extrae el esquema de un archivo
std::vector<Campo> LeerEsquema(const std::string &nombreDisco, const std::string &nombreTabla, std::string &formato)
{
    std::vector<Campo> esquema;
    std::string ruta = std::filesystem::current_path().string() + "/Discos/" + nombreDisco + "/Esquemas/" + nombreTabla + ".esq";
    std::ifstream archivo(ruta);
    std::string linea;
    if (!archivo.is_open())
        return esquema;
    std::getline(archivo, linea);
    archivo.close();

    std::vector<std::string> partes;
    std::stringstream ss(linea);
    std::string temp;
    while (std::getline(ss, temp, '#'))
        partes.push_back(temp);

    // partes: [tabla, campo1, tipo1, long1, campo2, tipo2, long2, ..., formato]
    for (size_t i = 1; i + 2 < partes.size() - 1; i += 3)
    {
        esquema.push_back({partes[i], partes[i + 1], std::stoi(partes[i + 2])});
    }
    formato = partes.back();
    return esquema;
}

// Extrae los campos de un registro en formato FI
std::vector<std::string> ParsearRegistroFI(const std::string &reg, const std::vector<Campo> &esquema)
{
    // Salta los campos de sistema: bloqueID, id, nombreTabla
    std::vector<std::string> campos;
    size_t ini = 0;
    int saltar = 2;
    for (int i = 0; i < saltar; ++i)
    {
        size_t p = reg.find('#', ini);
        if (p == std::string::npos)
            return campos;
        ini = p + 1;
    }
    // Extraer cada campo por longitud fija
    for (const auto &c : esquema)
    {
        campos.push_back(reg.substr(ini, c.longitud));
        ini += c.longitud;
    }
    return campos;
}

// Extrae los campos de un registro en formato VA (usa | como separador)
std::vector<std::string> ParsearRegistroVA(const std::string &reg, const std::vector<Campo> &esquema)
{
    std::vector<std::string> campos;
    size_t ini = 0;
    int saltar = 2;
    for (int i = 0; i < saltar; ++i)
    {
        size_t p = reg.find('#', ini);
        if (p == std::string::npos)
            return campos;
        ini = p + 1;
    }
    size_t fin = reg.find('#', ini);
    std::string data = reg.substr(ini, fin - ini);
    std::stringstream ss(data);
    std::string temp;
    while (std::getline(ss, temp, '|'))
    {
        if (!temp.empty())
            campos.push_back(temp);
    }
    return campos;
}

std::string Trim(const std::string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Compara dos strings según tipo y operador
bool Comparar(const std::string &campo, const std::string &tipo, const std::string &op, const std::string &valor)
{
    std::string campoLimpio = Trim(campo);
    if (campoLimpio == "")
    {
        return false;
    }

    if (tipo == "int")
    {
        int a = std::stoi(campoLimpio);
        int b = std::stoi(valor);
        if (op == "=")
            return a == b;
        if (op == ">")
            return a > b;
        if (op == "<")
            return a < b;
    }
    else if (tipo == "float")
    {
        float a = std::stof(campoLimpio);
        float b = std::stof(valor);
        if (op == "=")
            return a == b;
        if (op == ">")
            return a > b;
        if (op == "<")
            return a < b;
    }
    else
    {
        if (op == "=")
            return campoLimpio == valor;
    }
    return false;
}

std::vector<std::string> Bloques::FiltrarRegistros(const std::string &nombreDisco, const std::string &nombreTabla, const std::string &campoFiltro, const std::string &operador, const std::string &valorFiltro)
{
    std::vector<std::string> resultado;
    std::string formato;
    auto esquema = LeerEsquema(nombreDisco, nombreTabla, formato);

    // Identificar el índice y tipo del campo filtro
    int idxFiltro = -1;
    std::string tipoFiltro = "";
    if (!campoFiltro.empty())
    {
        for (int i = 0; i < esquema.size(); ++i)
        {
            if (esquema[i].nombre == campoFiltro)
            {
                idxFiltro = i;
                tipoFiltro = esquema[i].tipo;
                break;
            }
        }
        if (idxFiltro == -1)
        {
            std::cerr << "Campo '" << campoFiltro << "' no encontrado en la tabla.\n";
            return resultado;
        }
    }

    // Recorre todos los bloques
    std::string rutaBloques = std::filesystem::current_path().string() + "/Discos/Bloques_" + nombreDisco + "/";
    for (int i = 1;; ++i)
    {
        std::string ruta = rutaBloques + "Bloque_" + std::to_string(i) + ".txt";
        if (!std::filesystem::exists(ruta))
            break;

        std::string data;
        if (!bufferManager)
        {
            std::fstream bloque(ruta, std::ios::in);
            if (!bloque.is_open())
                break;
            std::stringstream ss;
            ss << bloque.rdbuf();
            data = ss.str();
            bloque.close();
        }
        else
        {
            data = bufferManager->readBlock(i, ruta);
        }

        if (data.rfind("SLOTTED_PAGE", 0) == 0)
        {
            int numSlots, freeOffset;
            std::vector<std::pair<int, int>> slots;
            ParseHeaderFromString(data, numSlots, freeOffset, slots);
            for (const auto &slot : slots)
            {
                std::string reg = data.substr(slot.first, slot.second);
                size_t p1 = reg.find('#');
                size_t p2 = reg.find('#', p1 + 1);
                if (p1 == std::string::npos || p2 == std::string::npos)
                    continue;
                std::string tablaEnRegistro = reg.substr(p1 + 1, p2 - p1 - 1);
                if (tablaEnRegistro != nombreTabla)
                    continue; // Solo si coincide con la tabla

                std::vector<std::string> campos;
                if (formato == "FI")
                    campos = ParsearRegistroFI(reg, esquema);
                else
                    campos = ParsearRegistroVA(reg, esquema);

                if (campos.empty())
                    continue;

                if (campoFiltro.empty())
                {
                    resultado.push_back(reg);
                }
                else if (Comparar(campos[idxFiltro], tipoFiltro, operador, valorFiltro))
                {
                    resultado.push_back(reg);
                }
            }
        }
    }
    return resultado;
}

void Bloques::MostrarRegistros(const std::vector<std::string> &registros, const std::string &nombreDisco, const std::string &nombreTabla)
{
    // Leer el esquema y formato
    std::string formato;
    auto esquema = LeerEsquema(nombreDisco, nombreTabla, formato);
    if (esquema.empty())
    {
        std::cout << "No se pudo cargar el esquema de la tabla.\n";
        return;
    }

    // Títulos de columna
    std::cout << "+";
    for (const auto &campo : esquema)
    {
        std::cout << std::setw(std::max(10, (int)campo.nombre.size() + 2)) << std::setfill('-') << "" << "+";
    }
    std::cout << "\n|";
    for (const auto &campo : esquema)
    {
        std::cout << std::setw(std::max(10, (int)campo.nombre.size() + 2)) << std::setfill(' ') << campo.nombre << "|";
    }
    std::cout << "\n+";
    for (const auto &campo : esquema)
    {
        std::cout << std::setw(std::max(10, (int)campo.nombre.size() + 2)) << std::setfill('-') << "" << "+";
    }
    std::cout << "\n";

    // Imprime cada registro con los campos separados
    for (const auto &reg : registros)
    {
        std::vector<std::string> campos;
        if (formato == "FI")
            campos = ParsearRegistroFI(reg, esquema);
        else
            campos = ParsearRegistroVA(reg, esquema);

        std::cout << "|";
        for (size_t i = 0; i < esquema.size(); ++i)
        {
            std::string valor = (i < campos.size()) ? campos[i] : "";
            // Para FI, elimina los espacios de relleno
            if (formato == "FI")
            {
                valor.erase(valor.find_last_not_of(' ') + 1);
            }
            std::cout << std::setw(std::max(10, (int)esquema[i].nombre.size() + 2)) << std::setfill(' ') << valor << "|";
        }
        std::cout << "\n";
    }
    // Línea final
    std::cout << "+";
    for (const auto &campo : esquema)
    {
        std::cout << std::setw(std::max(10, (int)campo.nombre.size() + 2)) << std::setfill('-') << "" << "+";
    }
    std::cout << "\n";
}

void Bloques::MostrarBloquesOcupados()
{
    std::string rutaBase = std::filesystem::current_path().string() + "/Discos/Bloques_" + NameDisk + "/";
    std::cout << "Bloques en disco '" << NameDisk << "':" << std::endl;

    for (int i = 1; i <= NumBlocks; ++i)
    {
        std::string bloquePath = rutaBase + "Bloque_" + std::to_string(i) + ".txt";
        std::string data;
        if (!bufferManager)
        {
            std::fstream bloque(bloquePath, std::ios::in);
            if (!bloque.is_open())
                continue;
            std::stringstream ss;
            ss << bloque.rdbuf();
            data = ss.str();
            bloque.close();
        }
        else
        {
            data = bufferManager->readBlock(i, bloquePath);
        }

        std::stringstream bloque(data);
        std::string linea;
        std::getline(bloque, linea);
        int registros = 0;
        int capacidadOcupada = 0;
        int capacidadDisponible = 0;
        int capacidadTotal = Capacity;

        if (linea == "SLOTTED_PAGE")
        {
            bloque.seekg(0, std::ios::beg);
            int numSlots, freeOffset;
            std::vector<std::pair<int, int>> slots;
            ParseHeaderFromString(data, numSlots, freeOffset, slots);
            registros = numSlots;
            capacidadOcupada = freeOffset;
            capacidadDisponible = capacidadTotal - capacidadOcupada;
        }
        std::cout << "Bloque " << i << ": "
                  << (registros > 0 ? std::to_string(registros) + " registros, " : "vacio, ")
                  << capacidadOcupada << "B/" << capacidadTotal << "B (" << (capacidadTotal ? capacidadOcupada * 100 / capacidadTotal : 0) << "% ocupado)" << std::endl;
    }
}

void Bloques::MostrarDetalleBloque(int numBloque)
{
    std::string rutaBase = std::filesystem::current_path().string() + "/Discos/Bloques_" + NameDisk + "/";
    std::string bloquePath = rutaBase + "Bloque_" + std::to_string(numBloque) + ".txt";
    std::ifstream bloqueIn(bloquePath);
    std::string data = bufferManager->readBlock(numBloque, bloquePath);

    std::stringstream bloqueIn(data);
    std::string linea;
    std::getline(bloqueIn, linea);
    int capacidadDisponible = std::stoi(linea);
    int capacidadTotal = Capacity;
    int capacidadOcupada = capacidadTotal - capacidadDisponible;
    std::cout << "Bloque " << numBloque << ":\n";
    std::cout << "  Capacidad total: " << capacidadTotal << " B\n";
    std::cout << "  Capacidad ocupada: " << capacidadOcupada << " B\n";
    std::cout << "  Capacidad libre: " << capacidadDisponible << " B\n";
    std::cout << "  Registros:\n";
    int id = 1;
    while (std::getline(bloqueIn, linea))
    {
        if (!linea.empty())
            std::cout << "    " << id++ << ": " << linea << std::endl;
    }
}

bool Bloques::AgregarRegistroManual(const std::string &nombreTabla, const std::vector<std::string> &valores, bool esFijo)
{
    // Leer esquema
    std::string formato;
    auto esquema = LeerEsquema(NameDisk, nombreTabla, formato);
    if (esquema.empty())
    {
        std::cout << "No se encontro el esquema para la tabla.\n";
        return false;
    }

    // Formatear registro
    std::string registro;
    if (esFijo)
    {
        // Formatear en FI
        registro = "BLOQUE#0#" + nombreTabla + "#";
        for (size_t i = 0; i < valores.size(); ++i)
        {
            std::string campo = valores[i];
            int longitud = (i < esquema.size()) ? esquema[i].longitud : 10;
            if (campo.length() < longitud)
                registro += std::string(longitud - campo.length(), ' ') + campo;
            else
                registro += campo.substr(0, longitud);
        }
    }
    else
    {
        // Formato VA
        std::string datos, metadatos;
        for (size_t i = 0; i < valores.size(); ++i)
        {
            std::string campo = Escape(valores[i]);
            datos += campo + "|";
            metadatos += std::to_string(i) + ":" + std::to_string(valores[i].length()) + ";";
        }
        registro = "BLOQUE#0#" + nombreTabla + "#" + datos + "#METADATA:" + metadatos;
    }

    return false;
}

int Bloques::CapacidadMaximaRegistro()
{
    std::string rutaBase = std::filesystem::current_path().string() + "/Discos/Bloques_" + NameDisk + "/";
    int maxCap = 0;
    for (int i = 1; i <= NumBlocks; ++i)
    {
        std::string bloquePath = rutaBase + "Bloque_" + std::to_string(i) + ".txt";
        std::string data;
        if (!bufferManager)
        {
            std::ifstream bloqueIn(bloquePath);
            if (!bloqueIn.is_open())
                continue;
            std::getline(bloqueIn, data);
            bloqueIn.close();
        }
        else
        {
            data = bufferManager->readBlock(i, bloquePath);
            std::stringstream ss(data);
            std::getline(ss, data);
        }
        int capacidadDisponible = std::stoi(data);
        if (capacidadDisponible > maxCap)
            maxCap = capacidadDisponible;
    }
    std::cout << "La capacidad máxima disponible para insertar un registro es: " << maxCap << " bytes.\n";
    return maxCap;
}

void Bloques::EscribirHeaderSlottedPage(std::fstream &bloque, int numSlots, int freeOffset, const std::vector<std::pair<int, int>> &slots)
{
    bloque.seekp(0, std::ios::beg);
    bloque << "SLOTTED_PAGE\n";
    bloque << "NumSlots=" << numSlots << "\n";
    bloque << "FreeSpaceOffset=" << freeOffset << "\n";
    for (int i = 0; i < numSlots; ++i)
    {
        bloque << "Slot" << i << "=" << slots[i].first << "," << slots[i].second << "\n";
    }
    bloque << "#DATA\n";
}

void Bloques::LeerHeaderSlottedPage(std::fstream &bloque, int &numSlots, int &freeOffset, std::vector<std::pair<int, int>> &slots)
{
    bloque.seekg(0, std::ios::beg);
    std::string linea;
    std::getline(bloque, linea); // SLOTTED_PAGE
    std::getline(bloque, linea); // NumSlots
    numSlots = std::stoi(linea.substr(linea.find('=') + 1));
    std::getline(bloque, linea); // FreeSpaceOffset
    freeOffset = std::stoi(linea.substr(linea.find('=') + 1));
    slots.clear();
    for (int i = 0; i < numSlots; ++i)
    {
        std::getline(bloque, linea); // SlotX
        size_t eq = linea.find('=');
        size_t comma = linea.find(',', eq + 1);
        int offset = std::stoi(linea.substr(eq + 1, comma - eq - 1));
        int size = std::stoi(linea.substr(comma + 1));
        slots.push_back({offset, size});
    }
    std::getline(bloque, linea); // #DATA
}

bool Bloques::InsertarRegistroEnBloque(const std::string &registro, int bloqueId)
{
    std::string rutaBloque = std::filesystem::current_path().string() + "/Discos/Bloques_" + NameDisk + "/Bloque_" + std::to_string(bloqueId) + ".txt";
    if (!bufferManager)
        return InsertarRegistroEnArchivo(rutaBloque, registro, Capacity);

    std::string &data = bufferManager->readBlock(bloqueId, rutaBloque);
    int numSlots, freeOffset;
    std::vector<std::pair<int, int>> slots;
    ParseHeaderFromString(data, numSlots, freeOffset, slots);

    std::vector<std::string> datos;
    for (const auto &s : slots)
        datos.push_back(data.substr(s.first, s.second));

    datos.push_back(registro);
    slots.push_back({0, (int)registro.size()});
    numSlots++;

    int freeOff = freeOffset;
    std::string header = BuildHeaderString(numSlots, freeOff, slots);
    if (freeOff > Capacity)
        return false;

    std::string result = header;
    for (const auto &d : datos)
        result += d;
    data = result;
    bufferManager->markDirty(bloqueId);
    return true;
}

std::vector<std::string> Bloques::LeerTodosLosRegistrosEnBloque(int bloqueId)
{
    std::vector<std::string> registros;
    std::string rutaBloque = std::filesystem::current_path().string() + "/Discos/Bloques_" + NameDisk + "/Bloque_" + std::to_string(bloqueId) + ".txt";
    std::string &data = bufferManager->readBlock(bloqueId, rutaBloque);
    int numSlots, freeOffset;
    std::vector<std::pair<int, int>> slots;
    ParseHeaderFromString(data, numSlots, freeOffset, slots);
    for (const auto &slot : slots)
    {
        registros.push_back(data.substr(slot.first, slot.second));
    }

    return registros;
}

bool Bloques::InsertarRegistroEnArchivo(const std::string &ruta, const std::string &registro, int capacidad)
{
    std::fstream archivo(ruta, std::ios::in);
    if (!archivo.is_open())
        return false;

    int numSlots, freeOffset;
    std::vector<std::pair<int, int>> slots;
    LeerHeaderSlottedPage(archivo, numSlots, freeOffset, slots);

    std::vector<std::string> datos;
    for (const auto &s : slots)
    {
        archivo.seekg(s.first, std::ios::beg);
        std::string d(s.second, '\0');
        archivo.read(&d[0], s.second);
        datos.push_back(d);
    }
    archivo.close();

    datos.push_back(registro);
    slots.push_back({0, (int)registro.size()});
    numSlots++;

    int headerSize = 0;
    std::string header;
    int freeOff = 0;
    while (true)
    {
        int offset = headerSize;
        for (int i = 0; i < numSlots; ++i)
        {
            slots[i].first = offset;
            offset += slots[i].second;
        }
        freeOff = offset;

        std::ostringstream oss;
        oss << "SLOTTED_PAGE\n";
        oss << "NumSlots=" << numSlots << "\n";
        oss << "FreeSpaceOffset=" << freeOff << "\n";
        for (int i = 0; i < numSlots; ++i)
            oss << "Slot" << i << "=" << slots[i].first << "," << slots[i].second << "\n";
        oss << "#DATA\n";

        header = oss.str();
        if ((int)header.size() == headerSize)
            break;
        headerSize = (int)header.size();
    }

    if (freeOff > capacidad)
        return false;

    std::ofstream out(ruta, std::ios::binary | std::ios::trunc);
    if (!out.is_open())
        return false;
    out.write(header.c_str(), header.size());
    for (const auto &d : datos)
        out.write(d.c_str(), d.size());
    out.close();
    return true;
}