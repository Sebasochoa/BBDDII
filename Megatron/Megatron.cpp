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
#include <array>

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

bool Megatron::Int(std::string texto)
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

bool Megatron::Float(std::string texto)
{
    std::istringstream ss(texto);
    float valor;
    return ss >> valor && ss.eof();
}

int *Megatron::Info_Disk(std::string Name_Disk)
{
    std::string Directory_Disk = fs::current_path().string() + "/" + Name_Disk;

    int Num_Plates = 0, Num_Surfaces = 0, Num_Tracks = 0, Num_Sectors = 0;

    for (const auto &Plate : fs::directory_iterator(Directory_Disk))
    {
        if (Plate.is_directory())
        {
            Num_Plates++;
        }
    }

    for (const auto &Plate : fs::directory_iterator(Directory_Disk))
    {
        if (Plate.is_directory())
        {
            for (const auto &Surfaces : fs::directory_iterator(Plate.path()))
            {
                if (Surfaces.is_directory())
                {
                    Num_Surfaces++;
                }
            }
        }
    }

    for (const auto &Plate : fs::directory_iterator(Directory_Disk))
    {
        if (Plate.is_directory())
        {
            for (const auto &Surfaces : fs::directory_iterator(Plate.path()))
            {
                if (Surfaces.is_directory())
                {
                    for (const auto &Tracks : fs::directory_iterator(Surfaces.path()))
                    {
                        if (Tracks.is_directory())
                        {
                            Num_Tracks++;
                        }
                    }
                }
            }
        }
    }

    for (const auto &Plate : fs::directory_iterator(Directory_Disk))
    {
        if (Plate.is_directory())
        {
            for (const auto &Surfaces : fs::directory_iterator(Plate.path()))
            {
                if (Surfaces.is_directory())
                {
                    for (const auto &Tracks : fs::directory_iterator(Surfaces.path()))
                    {
                        if (Tracks.is_directory())
                        {
                            for (const auto &Sectors : fs::directory_iterator(Tracks.path()))
                            {
                                if (Sectors.is_directory())
                                {
                                    Num_Sectors++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    int *Directory = new int[4];

    Num_Sectors /= Num_Tracks;
    Num_Tracks /= Num_Surfaces;
    Num_Surfaces /= Num_Plates;

    Directory[0] = Num_Plates;
    Directory[1] = Num_Surfaces;
    Directory[2] = Num_Tracks;
    Directory[3] = Num_Sectors;

    return Directory;
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

void Megatron::First_Line(std::string Directory_File, std::string Replace_Line)
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

bool Megatron::Check_Schemes(std::string Name_Scheme)
{
    std::ifstream File_Schemes(fs::current_path().string() + "/Esquemas.txt");

    std::string Line_Scheme;

    while (getline(File_Schemes, Line_Scheme))
    {
        std::string Segment;
        std::istringstream ss(Line_Scheme);

        while (getline(ss, Segment, '#'))
        {
            if (Segment == Name_Scheme)
            {
                return true;
            }
        }
    }
    return false;
}

void Megatron::Create_Scheme(std::string Name_Scheme)
{
    std::string Scheme = Name_Scheme + "#";

    std::string Name_Attribute_Line, Name_Attribute, Record_Line, Record;

    std::ifstream Upload_File(fs::current_path().string() + "/" + Name_Scheme + ".csv");

    getline(Upload_File, Name_Attribute_Line);
    getline(Upload_File, Record_Line);
    Upload_File.close();
    size_t j = 0;

    for (size_t i = 0; i <= Name_Attribute_Line.length() + 1; i++)
    {
        if (Name_Attribute_Line[i] == ',' || i == Name_Attribute_Line.length())
        {
            for (; j <= Record_Line.length(); j++)
            {
                if (Record_Line[j] == ',' || j == Record_Line.length())
                {
                    if (Int(Record))
                    {
                        Scheme += Name_Attribute + "#int#8#";
                    }
                    else if (Float(Record))
                    {
                        Scheme += Name_Attribute + "#float#10#";
                    }
                    else
                    {
                        if (Record.length() > 20)
                        {
                            Scheme += Name_Attribute + "#str#70#";
                        }
                        else
                        {
                            Scheme += Name_Attribute + "#str#20#";
                        }
                    }
                    break;
                }

                if (Record_Line[j] != ',')
                {
                    Record += Record_Line[j];
                }
            }
            j++;
            Record.clear();
            Name_Attribute.clear();
        }
        if (Name_Attribute_Line[i] != ',')
        {
            Name_Attribute += Name_Attribute_Line[i];
        }
    }

    std::ofstream Scheme_File(fs::current_path().string() + "/Esquemas.txt", std::ios::app);
    Scheme_File << Scheme << std::endl;
    Scheme_File.close();
}

void Megatron::Cargar(std::string Name_Disk)
{
    std::string Upload_File_Name, Name_Scheme;

    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, Upload_File_Name);

    std::ifstream archivo(fs::current_path().string() + "/" + Upload_File_Name + ".csv");

    std::cout << "Ingrese nombre a su Relacion: ";
    getline(std::cin, Name_Scheme);

    if (!Check_Schemes(Name_Scheme))
    {
        Create_Scheme(Name_Scheme);
    }

    std::string Upload_File_Line, Trash;

    int *Info = Info_Disk(Name_Disk);

    std::ifstream Upload_File(fs::current_path().string() + "/" + Upload_File_Name + ".csv");
    getline(Upload_File, Trash);

    int num = NumRegistros(fs::current_path().string() + "/" + Upload_File_Name + ".csv"), contador = 0;

    bool continuarIteraciones = true;

    for (int i = 1; i <= Info[0] && continuarIteraciones; ++i)
    {
        std::string Directory_Plate = fs::current_path().string() + "/" + Name_Disk + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && continuarIteraciones; ++j)
        {
            std::string Directory_Surface = Directory_Plate + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && continuarIteraciones; ++k)
            {

                std::string Directory_Track = Directory_Surface + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && continuarIteraciones; l++)
                {
                    std::string Directory_Sector = Directory_Track + "/Sector_" + std::to_string(l);

                    std::string Directory_File = Directory_Sector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";

                    std::ofstream txt(Directory_File, std::ios::app);
                    if (Upload_File.is_open())
                    {

                        while (getline(Upload_File, Upload_File_Line))
                        {

                            int longitud = static_cast<int>(Upload_File_Line.length());

                            int Vacio = RemainCapacity(Directory_File);

                            if (Vacio > longitud)
                            {
                                contador++;
                                txt << Corregir(Upload_File_Line, Name_Scheme, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;

                                First_Line(Directory_File, std::to_string(Vacio - longitud));
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

    Upload_File.close();
}

void Megatron::Cargar1(std::string Name_Disk)
{
    std::string Upload_File_Name, Name_Scheme;

    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, Upload_File_Name);

    std::ifstream archivo(fs::current_path().string() + "/" + Upload_File_Name + ".csv");

    std::cout << "Ingrese nombre a su Relacion: ";
    getline(std::cin, Name_Scheme);

    if (!Check_Schemes(Name_Scheme))
    {
        Create_Scheme(Name_Scheme);
    }

    std::string Upload_File_Line, Trash;

    int *Info = Info_Disk(Name_Disk);

    std::ifstream Upload_File(fs::current_path().string() + "/" + Upload_File_Name + ".csv");
    getline(Upload_File, Trash);

    int num = 1, contador = 0;

    bool continuarIteraciones = true;

    for (int i = 1; i <= Info[0] && continuarIteraciones; ++i)
    {
        std::string Directory_Plate = fs::current_path().string() + "/" + Name_Disk + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && continuarIteraciones; ++j)
        {
            std::string Directory_Surface = Directory_Plate + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && continuarIteraciones; ++k)
            {

                std::string Directory_Track = Directory_Surface + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && continuarIteraciones; l++)
                {
                    std::string Directory_Sector = Directory_Track + "/Sector_" + std::to_string(l);

                    std::string Directory_File = Directory_Sector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";

                    std::ofstream txt(Directory_File, std::ios::app);
                    if (Upload_File.is_open())
                    {

                        while (getline(Upload_File, Upload_File_Line))
                        {

                            int longitud = static_cast<int>(Upload_File_Line.length());

                            int Vacio = RemainCapacity(Directory_File);

                            if (Vacio > longitud && contador != num)
                            {

                                txt << Corregir(Upload_File_Line, Name_Scheme, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;

                                First_Line(Directory_File, std::to_string(Vacio - longitud));
                                contador++;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    Upload_File.close();
}

void Megatron::Cargarn(std::string Name_Disk)
{
    std::string Upload_File_Name, Name_Scheme;
    int num = 0;
    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, Upload_File_Name);

    std::string arch = fs::current_path().string() + "/" + Upload_File_Name + ".csv";
    std::ifstream archivo(arch);

    std::cout << "Ingrese nombre a su Esquema: ";
    getline(std::cin, Name_Scheme);
    std::cout << "Ingrese la cantidad de registros a cargar: ";
    std::cin >> num;

    if (!Check_Schemes(Name_Scheme))
    {
        Create_Scheme(Name_Scheme);
    }

    std::string Upload_File_Line, Trash;

    int *Info = Info_Disk(Name_Disk);

    std::ifstream Upload_File(fs::current_path().string() + "/" + Upload_File_Name + ".csv");
    getline(Upload_File, Trash);

    int contador = 0;

    bool continuarIteraciones = true;

    for (int i = 1; i <= Info[0] && continuarIteraciones; ++i)
    {
        std::string Directory_Plate = fs::current_path().string() + "/" + Name_Disk + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && continuarIteraciones; ++j)
        {
            std::string Directory_Surface = Directory_Plate + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && continuarIteraciones; ++k)
            {

                std::string Directory_Track = Directory_Surface + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && continuarIteraciones; l++)
                {
                    std::string Directory_Sector = Directory_Track + "/Sector_" + std::to_string(l);

                    std::string Directory_File = Directory_Sector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";

                    std::ofstream txt(Directory_File, std::ios::app);
                    if (Upload_File.is_open())
                    {

                        while (getline(Upload_File, Upload_File_Line))
                        {

                            int longitud = static_cast<int>(Upload_File_Line.length());

                            int Vacio = RemainCapacity(Directory_File);

                            if (Vacio > longitud && contador == num)
                            {
                                contador++;
                                txt << Corregir(Upload_File_Line, Name_Scheme, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l)) << std::endl;

                                First_Line(Directory_File, std::to_string(Vacio - longitud));
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    Upload_File.close();
}

std::string Megatron::Erase_Blanks(std::string cadena)
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

bool Megatron::IsRecord_inTable(std::string linea, std::string NTabla)
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
    int *Info = Info_Disk(NDisco);
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
                            if (IsRecord_inTable(linea, nEsquema))
                            {
                                int offsett = sumatoria + encontrarTercerNumeral(linea);
                                for (size_t i = offsett + 1; i <= offsett + Max(get_Esquema(nEsquema), offset + 1); i++)
                                {
                                    palabra += linea[i];
                                }
                                palabra = Erase_Blanks(palabra);
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


void Megatron::Select_(std::string NDisco)
{
    std::string atributo, signo, nEsquema, esquema, nomatributos, segmento, valor;
    int j = 0;

    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    getline(std::cin, nEsquema);

    int sumatoria = Max(get_Esquema(nEsquema), 1);
    std::string linea, palabra;
    int *Info = Info_Disk(NDisco);
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
                            if (IsRecord_inTable(linea, nEsquema))
                            {
                                std::string select;
                                select += "Plato " + std::to_string(Info[i-1]) + ", Superficie " + std::to_string(Info[j-1]) + ", Pista " + std::to_string(Info[k-1]) + ", Sector " + std::to_string(Info[l-1]) + ":";
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
    int *Info = Info_Disk(NDisco);
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
                            if (IsRecord_inTable(linea, nEsquema))
                            {
                                int offsett = sumatoria + encontrarTercerNumeral(linea);
                                for (size_t i = offsett + 1; i <= offsett + Max(get_Esquema(nEsquema), offset + 1); i++)
                                {
                                    palabrastr += linea[i];
                                }
                                palabrastr = Erase_Blanks(palabrastr);
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

/*
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
}*/
