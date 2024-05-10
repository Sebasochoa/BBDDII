#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include "Disco.h"
#include "Megatron.h"
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

Megatron::Megatron()
{
    if (!Existe("../Esquemas.txt"))
    {
        std::ofstream archivo("../Esquemas.txt");
    }
}

Megatron::~Megatron()
{
}

bool Megatron::Existe(const char *nombreArchivo)
{
    std::ifstream archivo(nombreArchivo);
    return archivo.good();
}

int Megatron::get_NumAtributos(std::string esquema)
{
    std::string linea;
    std::ifstream archivo(fs::current_path().string() + "/Esquemas.txt");
    size_t pos = 0;
    int contador = 0;
    bool encontro = false;
    if (archivo.is_open())
    {
        while (std::getline(archivo, linea))
        {

            if (linea.find(esquema) != std::string::npos)
            {
                encontro = true;
                break;
            }
        }
        archivo.close();

        if (!encontro)
        {
            return 0;
        }
    }
    while ((pos = linea.find('#', pos)) != std::string::npos)
    {
        ++contador;
        ++pos;
    }
    contador = (contador - 1) / 3;
    return contador;
}

bool esEntero(const std::string &texto)
{
    if (!texto.empty())
    {
        std::istringstream ss(texto);
        int valor;
        return ss >> valor && ss.eof();
    }
    else
    {
        return false;
    }
}

bool esFlotante(const std::string &texto)
{
    std::istringstream ss(texto);
    float valor;
    return ss >> valor && ss.eof();
}

std::vector<int> Info_Disco(std::string NDisco)
{
    std::vector<int> directorios;
    std::string directorioPadre = fs::current_path().string() + "\\" + NDisco;
    int NPlates = 0, NSuperficies = 0, NPistas = 0, NSectores = 0;
    for (const auto &plato : fs::directory_iterator(directorioPadre))
    {
        if (plato.is_directory())
        {
            NPlates++;
        }
    }

    for (const auto &plato : fs::directory_iterator(directorioPadre))
    {
        if (plato.is_directory())
        {
            for (const auto &superficie : fs::directory_iterator(plato.path()))
            {
                if (superficie.is_directory())
                {
                    NSuperficies++;
                }
            }
        }
    }

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
                            NPistas++;
                        }
                    }
                }
            }
        }
    }

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
                                    NSectores++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    NSectores /= NPistas;
    NPistas /= NSuperficies;
    NSuperficies /= NPlates;
    directorios.push_back(NPlates);
    directorios.push_back(NSuperficies);
    directorios.push_back(NPistas);
    directorios.push_back(NSectores);
    return directorios;
}

int RemainCapacity(std::string Archivo)
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

void modificarPrimeraLinea(const std::string &nombreArchivo, const std::string &nuevaPrimeraLinea)
{
    // Abrir el archivo en modo de lectura y escritura
    std::ifstream archivoEntrada(nombreArchivo);
    if (!archivoEntrada.is_open())
    {
        std::cerr << "No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return;
    }

    // Leer el contenido del archivo línea por línea
    std::string contenido;
    std::string linea;
    while (std::getline(archivoEntrada, linea))
    {
        contenido += linea + "\n"; // Agregar cada línea al contenido
    }
    archivoEntrada.close(); // Cerrar el archivo

    // Modificar la primera línea en el contenido
    size_t posFinPrimeraLinea = contenido.find('\n'); // Encontrar el primer salto de línea
    if (posFinPrimeraLinea != std::string::npos)
    {
        contenido.replace(0, posFinPrimeraLinea, nuevaPrimeraLinea); // Reemplazar la primera línea
    }
    else
    {
        std::cerr << "El archivo está vacío o no tiene saltos de línea." << std::endl;
        return;
    }

    // Abrir el archivo en modo de escritura (esto sobrescribirá el archivo existente)
    std::ofstream archivoSalida(nombreArchivo);
    if (!archivoSalida.is_open())
    {
        std::cerr << "No se pudo abrir el archivo " << nombreArchivo << " para escritura." << std::endl;
        return;
    }

    // Escribir el contenido modificado en el archivo
    archivoSalida << contenido;
    archivoSalida.close(); // Cerrar el archivo
}

int NumRegistros(std::string DirArchivo)
{
    std::ifstream archivo(DirArchivo);
    std::string linea;
    int Num = 0;
    while (getline(archivo, linea))
    {
        Num++;
    }
    return Num - 1;
}

void Megatron::Cargar(std::string NDisco)
{
    Tuplas.clear();
    std::string nArchivo, nEsquema, ComEsquema, Segmento;
    bool ExisEsquema = false;
    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, nArchivo);

    std::string arch = fs::current_path().string() + "/" + nArchivo + ".csv";
    std::ifstream archivo(arch);

    std::cout << "Ingrese nombre a su Esquema: ";
    getline(std::cin, nEsquema);

    std::ifstream Comprobar(fs::current_path().string() + "/Esquemas.txt");

    while (getline(Comprobar, ComEsquema))
    {
        std::string Segmento;
        std::istringstream ss(ComEsquema);

        while (getline(ss, Segmento, '#'))
        {
            if (Segmento == nEsquema)
            {
                ExisEsquema = true;
                break;
            }
        }

        if (ExisEsquema)
        {
            break;
        }
    }

    if (!ExisEsquema)
    {
        Tuplas += nEsquema + "#";

        std::string esq, atributo, tipe, tipo;

        getline(archivo, esq);
        getline(archivo, tipe);
        archivo.close();
        size_t j = 0;
        bool comillas = false;

        for (size_t i = 0; i <= esq.length() + 1; i++)
        {
            if (esq[i] == ',' || i == esq.length())
            {
                for (; j <= tipe.length(); j++)
                {
                    if ((tipe[j] == ',' && !comillas) || j == tipe.length())
                    {
                        if (esEntero(tipo))
                        {
                            Tuplas += atributo + "#int#8#";
                        }
                        else if (esFlotante(tipo))
                        {
                            Tuplas += atributo + "#float#10#";
                        }
                        else
                        {
                            if (tipo.length() > 20)
                            {
                                Tuplas += atributo + "#str#70#";
                            }
                            else
                            {
                                Tuplas += atributo + "#str#20#";
                            }
                        }
                        break;
                    }
                    if (tipe[j] == '"')
                    {
                        comillas = !comillas;
                    }

                    if (tipe[j] != ',')
                    {
                        tipo += tipe[j];
                    }
                }
                j++;
                tipo.clear();
                atributo.clear();
            }
            if (esq[i] != ',')
            {
                atributo += esq[i];
            }
        }

        std::ofstream Esquem(fs::current_path().string() + "/Esquemas.txt", std::ios::app);
        Esquem << Tuplas << std::endl;
        Esquem.close();
    }
    std::string esq;
    std::vector<int> Info = Info_Disco(NDisco);
    std::string directorio = fs::current_path().string();
    std::string dirNDisco = directorio + "/" + NDisco;
    std::ifstream Cargar(arch);
    getline(Cargar, esq);
    std::string linea;
    int num = NumRegistros(arch), contador = 0;
    bool conti = true;
    bool continuarIteraciones = true;

    for (int i = 1; i <= Info[0] && continuarIteraciones; ++i)
    {
        std::string dirPlato = dirNDisco + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && continuarIteraciones; ++j)
        {
            std::string dirSuperficie = dirPlato + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && continuarIteraciones; ++k)
            {
                std::string dirPista = dirSuperficie + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && continuarIteraciones; l++)
                {
                    std::string dirSector = dirPista + "/Sector_" + std::to_string(l);
                    std::string nomArchivo = dirSector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";
                    std::ofstream txt(nomArchivo, std::ios::app);
                    if (Cargar.is_open())
                    {
                        if (!linea.empty())
                        {
                            txt << Corregir(linea, nEsquema, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;
                            contador++;
                        }
                        while (getline(Cargar, linea))
                        {
                            int longitud = static_cast<int>(linea.length());
                            int Vacio = RemainCapacity(nomArchivo);
                            if (Vacio > longitud)
                            {
                                txt << Corregir(linea, nEsquema, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;

                                modificarPrimeraLinea(nomArchivo, std::to_string(Vacio - longitud));
                                contador++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (contador == num)
                        {
                            continuarIteraciones = false;
                        }
                    }
                }
            }
        }
    }

    Cargar.close();
}

void Megatron::Cargar1(std::string NDisco)
{
    Tuplas.clear();
    std::string nArchivo, nEsquema, ComEsquema, Segmento;
    bool ExisEsquema = false;
    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, nArchivo);

    std::string arch = fs::current_path().string() + "/" + nArchivo + ".csv";
    std::ifstream archivo(arch);

    std::cout << "Ingrese nombre a su Esquema: ";
    getline(std::cin, nEsquema);

    std::ifstream Comprobar(fs::current_path().string() + "/Esquemas.txt");

    while (getline(Comprobar, ComEsquema))
    {
        std::string Segmento;
        std::istringstream ss(ComEsquema);

        while (getline(ss, Segmento, '#'))
        {
            if (Segmento == nEsquema)
            {
                ExisEsquema = true;
                break;
            }
        }

        if (ExisEsquema)
        {
            break;
        }
    }

    if (!ExisEsquema)
    {
        Tuplas += nEsquema + "#";

        std::string esq, atributo, tipe, tipo;

        getline(archivo, esq);
        getline(archivo, tipe);
        archivo.close();
        size_t j = 0;
        bool comillas = false;

        for (size_t i = 0; i <= esq.length() + 1; i++)
        {
            if (esq[i] == ',' || i == esq.length())
            {
                for (; j <= tipe.length(); j++)
                {
                    if ((tipe[j] == ',' && !comillas) || j == tipe.length())
                    {
                        if (esEntero(tipo))
                        {
                            Tuplas += atributo + "#int#8#";
                        }
                        else if (esFlotante(tipo))
                        {
                            Tuplas += atributo + "#float#10#";
                        }
                        else
                        {
                            if (tipo.length() > 20)
                            {
                                Tuplas += atributo + "#str#70#";
                            }
                            else
                            {
                                Tuplas += atributo + "#str#20#";
                            }
                        }
                        break;
                    }
                    if (tipe[j] == '"')
                    {
                        comillas = !comillas;
                    }

                    if (tipe[j] != ',')
                    {
                        tipo += tipe[j];
                    }
                }
                j++;
                tipo.clear();
                atributo.clear();
            }
            if (esq[i] != ',')
            {
                atributo += esq[i];
            }
        }

        std::ofstream Esquem(fs::current_path().string() + "/Esquemas.txt", std::ios::app);
        Esquem << Tuplas << std::endl;
        Esquem.close();
    }
    std::string esq;
    std::vector<int> Info = Info_Disco(NDisco);
    std::string directorio = fs::current_path().string();
    std::string dirNDisco = directorio + "/" + NDisco;
    std::ifstream Cargar(arch);
    getline(Cargar, esq);
    std::string linea;
    int num = 1, contador = 0;
    bool conti = true;
    bool continuarIteraciones = true;

    for (int i = 1; i <= Info[0] && continuarIteraciones; ++i)
    {
        std::string dirPlato = dirNDisco + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && continuarIteraciones; ++j)
        {
            std::string dirSuperficie = dirPlato + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && continuarIteraciones; ++k)
            {
                std::string dirPista = dirSuperficie + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && continuarIteraciones; l++)
                {
                    std::string dirSector = dirPista + "/Sector_" + std::to_string(l);
                    std::string nomArchivo = dirSector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";
                    std::ofstream txt(nomArchivo, std::ios::app);
                    if (Cargar.is_open())
                    {
                        if (!linea.empty())
                        {
                            txt << Corregir(linea, nEsquema, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;
                            contador++;
                        }
                        while (getline(Cargar, linea))
                        {
                            int longitud = static_cast<int>(linea.length());
                            int Vacio = RemainCapacity(nomArchivo);
                            if (Vacio > longitud)
                            {
                                txt << Corregir(linea, nEsquema, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;

                                modificarPrimeraLinea(nomArchivo, std::to_string(Vacio - longitud));
                                contador++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (contador == num)
                        {
                            continuarIteraciones = false;
                        }
                    }
                }
            }
        }
    }

    Cargar.close();
}

void Megatron::Cargarn(std::string NDisco)
{
    Tuplas.clear();
    std::string nArchivo, nEsquema, ComEsquema, Segmento;
    bool ExisEsquema = false;
    int num = 0;
    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, nArchivo);

    std::string arch = fs::current_path().string() + "/" + nArchivo + ".csv";
    std::ifstream archivo(arch);

    std::cout << "Ingrese nombre a su Esquema: ";
    getline(std::cin, nEsquema);
    std::cout << "Ingrese la cantidad de registros a cargar: ";
    std::cin >> num;

    std::ifstream Comprobar(fs::current_path().string() + "/Esquemas.txt");

    while (getline(Comprobar, ComEsquema))
    {
        std::string Segmento;
        std::istringstream ss(ComEsquema);

        while (getline(ss, Segmento, '#'))
        {
            if (Segmento == nEsquema)
            {
                ExisEsquema = true;
                break;
            }
        }

        if (ExisEsquema)
        {
            break;
        }
    }

    if (!ExisEsquema)
    {
        Tuplas += nEsquema + "#";

        std::string esq, atributo, tipe, tipo;

        getline(archivo, esq);
        getline(archivo, tipe);
        archivo.close();
        size_t j = 0;
        bool comillas = false;

        for (size_t i = 0; i <= esq.length() + 1; i++)
        {
            if (esq[i] == ',' || i == esq.length())
            {
                for (; j <= tipe.length(); j++)
                {
                    if ((tipe[j] == ',' && !comillas) || j == tipe.length())
                    {
                        if (esEntero(tipo))
                        {
                            Tuplas += atributo + "#int#8#";
                        }
                        else if (esFlotante(tipo))
                        {
                            Tuplas += atributo + "#float#10#";
                        }
                        else
                        {
                            if (tipo.length() > 20)
                            {
                                Tuplas += atributo + "#str#70#";
                            }
                            else
                            {
                                Tuplas += atributo + "#str#20#";
                            }
                        }
                        break;
                    }
                    if (tipe[j] == '"')
                    {
                        comillas = !comillas;
                    }

                    if (tipe[j] != ',')
                    {
                        tipo += tipe[j];
                    }
                }
                j++;
                tipo.clear();
                atributo.clear();
            }
            if (esq[i] != ',')
            {
                atributo += esq[i];
            }
        }

        std::ofstream Esquem(fs::current_path().string() + "/Esquemas.txt", std::ios::app);
        Esquem << Tuplas << std::endl;
        Esquem.close();
    }
    std::string esq;
    std::vector<int> Info = Info_Disco(NDisco);
    std::string directorio = fs::current_path().string();
    std::string dirNDisco = directorio + "/" + NDisco;
    std::ifstream Cargar(arch);
    getline(Cargar, esq);
    std::string linea;
    int contador = 0;
    bool conti = true;
    bool continuarIteraciones = true;

    for (int i = 1; i <= Info[0] && continuarIteraciones; ++i)
    {
        std::string dirPlato = dirNDisco + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && continuarIteraciones; ++j)
        {
            std::string dirSuperficie = dirPlato + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && continuarIteraciones; ++k)
            {
                std::string dirPista = dirSuperficie + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && continuarIteraciones; l++)
                {
                    std::string dirSector = dirPista + "/Sector_" + std::to_string(l);
                    std::string nomArchivo = dirSector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";
                    std::ofstream txt(nomArchivo, std::ios::app);
                    if (Cargar.is_open())
                    {
                        if (!linea.empty())
                        {
                            txt << Corregir(linea, nEsquema, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;
                            contador++;
                        }
                        while (getline(Cargar, linea))
                        {
                            int longitud = static_cast<int>(linea.length());
                            int Vacio = RemainCapacity(nomArchivo);
                            if (Vacio > longitud)
                            {
                                txt << Corregir(linea, nEsquema, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;

                                modificarPrimeraLinea(nomArchivo, std::to_string(Vacio - longitud));
                                contador++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (contador == num)
                        {
                            continuarIteraciones = false;
                        }
                    }
                }
            }
        }
    }

    Cargar.close();
}

std::string eliminarEspaciosBlancos(const std::string &cadena)
{
    std::string resultado;

    for (char caracter : cadena)
    {
        if (!isspace(caracter) || caracter != ' ')
        {
            resultado += caracter;
        }
    }

    return resultado;
}

bool Registro_Tabla(std::string linea, std::string NTabla)
{

    size_t posInicio = linea.find_first_of('#', linea.find_first_of('#') + 1);
    if (posInicio == std::string::npos)
    {
        return false;
    }

    size_t posFinal = linea.find_first_of('#', posInicio + 1);
    if (posFinal == std::string::npos)
    {
        return false;
    }

    std::string segmento = linea.substr(posInicio + 1, posFinal - posInicio - 1);

    return segmento == NTabla;
}

int encontrarTercerNumeral(const std::string &cadena)
{
    size_t posPrimero = cadena.find_first_of('#');
    if (posPrimero == std::string::npos)
    {
        return -1;
    }

    size_t posSegundo = cadena.find_first_of('#', posPrimero + 1);
    if (posSegundo == std::string::npos)
    {
        return -1;
    }

    size_t posTercero = cadena.find_first_of('#', posSegundo + 1);
    if (posTercero == std::string::npos)
    {
        return -1;
    }

    return static_cast<int>(posTercero);
}

void Megatron::Select_1(std::string NDisco)
{

    std::string atributo, signo, nEsquema, esquema, nomatributos, segmento, valor;
    int j = 0;

    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    getline(std::cin, nEsquema);

    int numatributos = get_NumAtributos(nEsquema);
    std::string *enteros = new std::string[numatributos];

    esquema = get_Esquema(nEsquema);
    nomatributos = get_NomAtributos(esquema);

    for (size_t i = 0; i < nomatributos.length(); i++)
    {

        if (nomatributos[i] == ',')
        {
            if (j < numatributos)
            {
                enteros[j] = segmento;
                segmento.clear();
                j++;
            }
        }
        else
        {
            segmento += nomatributos[i];
        }
    }
    int opc = 999999;
    std::cout << "Ingrese el atributo a seleccionar: \n";
    for (int i = 0; i < numatributos; i++)
    {

        std::cout << i + 1 << ". " << enteros[i] << std::endl;
    }

    while (opc > numatributos + 1)
    {
        std::cout << "Ingrese una opcion valida: ";
        std::cin >> opc;
    }
    atributo = enteros[opc - 1];
    opc = 0;
    signo = '=';
    std::cout << "Ingrese la comparacion: ";
    std::cin >> valor;

    std::istringstream ss(nomatributos);
    int offset = 0, sumatoria = 0;

    while (std::getline(ss, segmento, ','))
    {
        if (segmento == atributo)
        {
            break;
        }
        offset++;
    }

    for (int i = 0; i < offset; i++)
    {
        sumatoria += Max(get_Esquema(nEsquema), i + 1);
    }

    std::string linea, palabra;
    std::vector<int> Info = Info_Disco(NDisco);
    bool continuarIteraciones = true;
    for (int i = 1; i <= Info[0] && continuarIteraciones; ++i)
    {
        std::string dirPlato = fs::current_path().string() + "/" + NDisco + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && continuarIteraciones; ++j)
        {
            std::string dirSuperficie = dirPlato + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && continuarIteraciones; ++k)
            {
                std::string dirPista = dirSuperficie + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && continuarIteraciones; l++)
                {
                    std::string dirSector = dirPista + "/Sector_" + std::to_string(l);
                    std::string nomArchivo = dirSector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";
                    std::ifstream Select(nomArchivo);
                    while (getline(Select, linea))
                    {

                        if (linea.length() > sumatoria)
                        {
                            if (Registro_Tabla(linea, nEsquema))
                            {
                                int offsett = sumatoria + encontrarTercerNumeral(linea);
                                for (size_t i = offsett + 1; i <= offsett + Max(get_Esquema(nEsquema), offset + 1); i++)
                                {
                                    palabra += linea[i];
                                }
                                palabra = eliminarEspaciosBlancos(palabra);
                                if (palabra == valor)
                                {
                                    std::string select;
                                    select += "Plato " + std::to_string(Info[i]) + ", Superficie " + std::to_string(Info[j]) + ", Pista " + std::to_string(Info[k]) + ", Sector " + std::to_string(Info[l]) + ":";
                                    for (size_t i = offsett + 1; i < linea.length(); i++)
                                    {
                                        select += linea[i];
                                    }

                                    std::cout << select << std::endl;
                                    continuarIteraciones = false;
                                }

                                palabra.clear();
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }
        }
    }
}

std::string Megatron::Corregir(std::string linea, std::string NTabla, int contador, std::string dir)
{
    size_t pos = 0;
    std::string res, esq;
    std::ifstream esquemas(fs::current_path().string() + "/Esquemas.txt");
    std::getline(esquemas, esq);
    int mult = 1;
    bool str = false;
    std::string segmento;
    res += dir + '#' + std::to_string(contador) + "#" + NTabla + "#";
    for (size_t i = 0; i < linea.length(); i++)
    {
        if (linea[i] == '"')
        {
            str = !str;
        }
        else if (linea[i] == ',' && !str)
        {
            res += LlenarI(segmento, Max(get_Esquema(NTabla), mult));
            segmento.clear();
            mult++;
        }
        else
        {
            segmento += linea[i];
        }
    }
    res += LlenarI(segmento, Max(get_Esquema(NTabla), mult));
    return res;
}

std::string Megatron::LlenarI(std::string linea, int cant)
{
    std::string res;
    size_t resto = cant - linea.length();
    for (size_t i = 0; i < resto; i++)
    {
        res += " ";
    }
    res += linea;
    return res;
}

std::string Megatron::LlenarD(std::string linea, int cant)
{
    std::string res;
    size_t resto = cant - linea.length();
    res += linea;
    for (size_t i = 0; i < resto; i++)
    {
        res += " ";
    }

    return res;
}

std::string Megatron::Vaciar(std::string linea, int *max, std::string esquema)
{
    std::string resultado = linea;
    size_t offset = 0;
    int natri = get_NumAtributos(esquema);
    for (int i = 0; i < natri; ++i)
    {
        size_t pos = max[i];
        if (pos + offset < resultado.size())
        {
            resultado.insert(pos + offset, "#");
            offset++;
            offset += max[i];
        }
    }
    resultado.erase(std::remove_if(resultado.begin(), resultado.end(), [](unsigned char c)
                                   { return std::isspace(c); }),
                    resultado.end());
    std::string res, segmento;
    int mult = 1, enca = 0, maxi = 0;
    std::string esq = get_Esquema(esquema);
    for (size_t i = 0; i < resultado.length(); i++)
    {

        if (resultado[i] == '#')
        {
            enca = Encabezados(esq, mult);
            maxi = Max(esq, mult);
            if (enca < maxi)
            {
                res += LlenarD(segmento, maxi + 1);
            }
            else
            {
                res += LlenarD(segmento, enca + 1);
            }
            segmento.clear();
            mult++;
        }
        else
        {
            segmento += resultado[i];
        }
    }
    enca = Encabezados(esq, mult);
    maxi = Max(esq, mult);
    if (enca < maxi)
    {
        res += LlenarD(segmento, maxi);
    }
    else
    {
        res += LlenarD(segmento, enca);
    }
    return res;
}

std::string Megatron::get_Esquema(std::string tabla)
{
    std::string linea;
    std::ifstream archivo(fs::current_path().string() + "/Esquemas.txt");

    bool encontro = false;
    if (archivo.is_open())
    {
        while (std::getline(archivo, linea))
        {

            if (linea.find(tabla) != std::string::npos)
            {
                encontro = true;
                break;
            }
        }
        archivo.close();

        if (!encontro)
        {
            return 0;
        }
    }
    return linea;
}

int Megatron::Max(std::string esq, int mult)
{
    size_t pos = 0;
    int contador = 0, max = 0;
    std::string numero_str;

    while ((pos = esq.find('#', pos)) != std::string::npos)
    {
        ++contador;

        if (contador == 3 * mult)
        {
            pos++;
            while (pos < esq.length() && esq[pos] != '#')
            {
                numero_str += esq[pos];
                pos++;
            }
            break;
        }
        else
        {
            pos++;
        }
    }

    if (!numero_str.empty())
    {
        max = std::stoi(numero_str);
    }
    return max;
}

int Megatron::Encabezados(std::string esquema, int mult)
{
    size_t pos = 0;
    int contador = 2, max = 0;

    while ((pos = esquema.find('#', pos)) != std::string::npos)
    {
        ++contador;
        if (contador == 3 * mult)
        {
            pos++;
            while (pos < esquema.length() && esquema[pos] != '#')
            {
                max++;
                pos++;
            }
        }
        else
        {
            pos++;
        }
    }
    return max;
}

std::string Megatron::get_NomAtributos(std::string tabla)
{

    size_t pos = 0;
    int contador = 2;
    std::string nombres;

    while ((pos = tabla.find('#', pos)) != std::string::npos)
    {
        ++contador;
        if (contador == 3)
        {
            pos++;
            while (pos < tabla.length() && tabla[pos] != '#')
            {
                nombres += tabla[pos];
                pos++;
            }
            nombres += ',';
            contador = 0;
        }
        else
        {
            pos++;
        }
    }
    return nombres;
}

int *Megatron::Max(std::string esq)
{
    size_t pos = 0;
    int contador = 0;
    std::string str;
    while ((pos = esq.find('#', pos)) != std::string::npos)
    {
        ++contador;
        ++pos;
    }
    contador = (contador - 1) / 3;
    int *numero_str = new int[contador];
    for (int i = 0; i < contador; i++)
    {
        numero_str[i] = Max(esq, i + 1);
    }

    return numero_str;
}

void Megatron::Select_(std::string NDisco)
{
    std::string atributo, signo, nEsquema, esquema, nomatributos, segmento, valor;
    int j = 0;

    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    getline(std::cin, nEsquema);

    int sumatoria = Max(get_Esquema(nEsquema), 1);
    std::string linea, palabra;
    std::vector<int> Info = Info_Disco(NDisco);
    bool continuarIteraciones = true;
    for (int i = 1; i <= Info[0] && continuarIteraciones; ++i)
    {
        std::string dirPlato = fs::current_path().string() + "/" + NDisco + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && continuarIteraciones; ++j)
        {
            std::string dirSuperficie = dirPlato + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && continuarIteraciones; ++k)
            {
                std::string dirPista = dirSuperficie + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && continuarIteraciones; l++)
                {
                    std::string dirSector = dirPista + "/Sector_" + std::to_string(l);
                    std::string nomArchivo = dirSector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";
                    std::ifstream Select(nomArchivo);
                    while (getline(Select, linea))
                    {

                        if (linea.length() > sumatoria)
                        {
                            if (Registro_Tabla(linea, nEsquema))
                            {
                                std::string select;
                                select += "Plato " + std::to_string(Info[i]) + ", Superficie " + std::to_string(Info[j]) + ", Pista " + std::to_string(Info[k]) + ", Sector " + std::to_string(Info[l]) + ":";
                                int offsett = encontrarTercerNumeral(linea);
                                for (size_t i = offsett + 1; i < linea.length(); i++)
                                {
                                    select += linea[i];
                                }

                                std::cout << select << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Megatron::Select_(std::string nEsquema, std::string atributo, std::string signo, int valor, std::string NDisco)
{
    std::string esquema, nomatributos, segmento;
    int j = 0;

    int numatributos = get_NumAtributos(nEsquema);
    std::string *enteros = new std::string[numatributos];

    esquema = get_Esquema(nEsquema);
    nomatributos = get_NomAtributos(esquema);

    std::istringstream ss(nomatributos);
    int offset = 0, sumatoria = 0;

    while (std::getline(ss, segmento, ','))
    {
        if (segmento == atributo)
        {
            break;
        }
        offset++;
    }

    for (int i = 0; i < offset; i++)
    {
        sumatoria += Max(get_Esquema(nEsquema), i + 1);
    }

    std::string linea, palabrastr;
    int palabra;
    std::vector<int> Info = Info_Disco(NDisco);
    for (int i = 1; i <= Info[0]; ++i)
    {
        std::string dirPlato = fs::current_path().string() + "/" + NDisco + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1]; ++j)
        {
            std::string dirSuperficie = dirPlato + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2]; ++k)
            {
                std::string dirPista = dirSuperficie + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3]; l++)
                {
                    std::string dirSector = dirPista + "/Sector_" + std::to_string(l);
                    std::string nomArchivo = dirSector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";
                    std::ifstream Select(nomArchivo);
                    while (getline(Select, linea))
                    {

                        if (linea.length() > sumatoria)
                        {
                            if (Registro_Tabla(linea, nEsquema))
                            {
                                int offsett = sumatoria + encontrarTercerNumeral(linea);
                                for (size_t i = offsett + 1; i <= offsett + Max(get_Esquema(nEsquema), offset + 1); i++)
                                {
                                    palabrastr += linea[i];
                                }
                                palabrastr = eliminarEspaciosBlancos(palabrastr);
                                palabra = std::stoi(palabrastr);
                                if (signo == "<")
                                {
                                    if (palabra == valor)
                                    {
                                        std::cout << linea << std::endl;
                                    }
                                }
                                else if (signo == ">")
                                {
                                    if (palabra > valor)
                                    {
                                        std::cout << linea << std::endl;
                                    }
                                }
                                else if (signo == "<=")
                                {
                                    if (palabra <= valor)
                                    {
                                        std::cout << linea << std::endl;
                                    }
                                }
                                else if (signo == ">=")
                                {
                                    if (palabra >= valor)
                                    {
                                        std::cout << linea << std::endl;
                                    }
                                }

                                palabrastr.clear();
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }
        }
    }
}

bool Megatron::Entero(const std::string &cadena1, const std::string &cadena2, const std::string &palabra)
{
    std::istringstream ss1(cadena1);
    std::istringstream ss2(cadena2);
    std::string token1, token2;

    while (std::getline(ss1, token1, ','))
    {
        if (token1 == palabra)
        {
            while (std::getline(ss2, token2, '#'))
            {
                if (token2 == palabra)
                {
                    if (std::getline(ss2, token2, '#'))
                    {
                        return (token2 == "int");
                    }
                }
            }
        }
    }
    return false;
}

void buscarYEliminar(const std::string &directorioPadre, char caracterABuscar)
{
    // Iterar sobre todos los archivos y carpetas dentro del directorio padre
    for (const auto &entrada : fs::directory_iterator(directorioPadre))
    {
        // Verificar si la entrada es un archivo
        if (fs::is_regular_file(entrada))
        {
            // Obtener el nombre del archivo
            std::string nombreArchivo = entrada.path().filename().string();

            // Verificar si el archivo es un archivo de texto (.txt)
            if (nombreArchivo.find(".txt") != std::string::npos)
            {
                // Abrir el archivo para lectura y escritura
                std::ifstream archivoEntrada(entrada.path());
                std::ofstream archivoSalida(entrada.path().string() + ".temp");

                // Procesar el archivo línea por línea
                std::string linea;
                while (std::getline(archivoEntrada, linea))
                {
                    // Buscar y eliminar el caracter en la línea
                    size_t pos = linea.find(caracterABuscar);
                    if (pos != std::string::npos)
                    {
                        // Eliminar el caracter
                        linea.erase(pos, 1);
                    }

                    // Escribir la línea modificada en el archivo de salida
                    archivoSalida << linea << std::endl;
                }

                // Cerrar los archivos
                archivoEntrada.close();
                archivoSalida.close();

                // Reemplazar el archivo original con el archivo temporal
                fs::rename(entrada.path().string() + ".temp", entrada.path());
            }
        }
        else if (fs::is_directory(entrada))
        {
            // Si la entrada es un directorio, llamar recursivamente a la función
            buscarYEliminar(entrada.path().string(), caracterABuscar);
        }
    }
}

void Megatron::Select_I(std::string NDisco)
{
    std::string atributo, signo, nEsquema, esquema, nomatributos, segmento;
    int valor = 0, j = 0;

    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    getline(std::cin, nEsquema);

    int numatributos = get_NumAtributos(nEsquema);
    std::string *enteros = new std::string[numatributos];

    esquema = get_Esquema(nEsquema);
    nomatributos = get_NomAtributos(esquema);

    for (size_t i = 0; i < nomatributos.length(); i++)
    {

        if (nomatributos[i] == ',')
        {
            if (Entero(nomatributos, esquema, segmento))
            {

                if (j < numatributos)
                {
                    enteros[j] = segmento;
                }
                j++;
            }
            segmento.clear();
        }
        else
        {
            segmento += nomatributos[i];
        }
    }
    int longitud = sizeof(enteros[0]) / sizeof(enteros), opc = 100000;
    std::cout << "Ingrese el atributo a seleccionar: \n";
    for (int i = 0; i < longitud; i++)
    {

        std::cout << i + 1 << ". " << enteros[i] << std::endl;
    }
    while (opc > longitud + 1)
    {
        std::cout << "Ingrese una opcion valida: ";
        std::cin >> opc;
    }
    atributo = enteros[opc - 1];
    opc = 0;
    while (opc < 1 || opc > 4)
    {
        std::cout << "Signo de comparacion: \n1. <\n2. >\n3. <=\n4. >=\nIngrese una opcion valida: ";
        std::cin >> opc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (opc)
        {
        case 1:
            signo = "<";
            break;
        case 2:
            signo = ">";
            break;
        case 3:
            signo = "<=";
            break;
        case 4:
            signo = ">=";
            break;
        default:
            std::cout << "Seleccione una opcion: ";
            std::cin >> opc;
            break;
        }
    }
    std::cout << "Ingrese cantidad a comparar: ";
    std::cin >> valor;

    Select_(nEsquema, atributo, signo, valor, NDisco);
}
