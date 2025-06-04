#include "Bloques.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <queue>

namespace fs = std::filesystem;

Bloques::Bloques()
{
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
            archivo << Capacity << std::endl;
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

std::string FormatearFI(const std::string &linea, const std::string &esquema, const std::string &tabla, int id, const std::string &bloqueID)
{
    std::vector<std::string> valores = SplitCSVLine(linea);
    std::string resultado = bloqueID + "#" + std::to_string(id) + "#" + tabla + "#";

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

std::string FormatearVA(const std::string &linea, const std::string &tabla, int id, const std::string &bloqueID)
{
    std::vector<std::string> campos = SplitCSVLine(linea);
    std::string datos, metadatos;

    for (size_t i = 0; i < campos.size(); ++i)
    {
        std::string campo = Escape(campos[i]);
        datos += campo + "|";
        metadatos += std::to_string(i) + ":" + std::to_string(campos[i].length()) + ";";
    }

    return bloqueID + "#" + std::to_string(id) + "#" + tabla + "#" + datos + "#METADATA:" + metadatos;
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
            registro = FormatearFI(linea, esquema, nombreTabla, id, "BLOQUE");
        else
            registro = FormatearVA(linea, nombreTabla, id, "BLOQUE");

        registrosFormateados.push_back(registro);
        id++;
    }

    archivo.close();
    return CargarRegistros(registrosFormateados);
}

bool Bloques::CargarRegistros(const std::vector<std::string> &registros)
{
    std::queue<std::string> colaRegistros;
    for (const auto &r : registros)
        colaRegistros.push(r);

    std::string rutaBase = std::filesystem::current_path().string() + "/Discos/Bloques_" + NameDisk + "/";

    for (int i = 1; i <= NumBlocks && !colaRegistros.empty(); ++i)
    {
        std::string bloquePath = rutaBase + "Bloque_" + std::to_string(i) + ".txt";

        std::ifstream bloqueIn(bloquePath);
        if (!bloqueIn.is_open())
            continue;

        std::string linea;
        std::getline(bloqueIn, linea);
        int capacidadDisponible = std::stoi(linea);
        std::string contenidoBloque((std::istreambuf_iterator<char>(bloqueIn)), std::istreambuf_iterator<char>());
        bloqueIn.close();

        std::ofstream bloqueOut(bloquePath);
        if (!bloqueOut.is_open())
            continue;

        std::ostringstream nuevoContenido;
        while (!colaRegistros.empty())
        {
            const std::string &registro = colaRegistros.front();
            int tam = static_cast<int>(registro.size());
            if (tam + 1 > capacidadDisponible)
                break;

            nuevoContenido << registro << '\n';
            capacidadDisponible -= (tam + 1); // +1 por el salto de línea
            colaRegistros.pop();
        }

        bloqueOut << capacidadDisponible << "\n"
                  << nuevoContenido.str() << contenidoBloque;
        bloqueOut.close();
    }

    return colaRegistros.empty();
}
