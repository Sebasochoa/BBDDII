#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include "Disco.h"
#include "Records_Manager.h"
#include <vector>
#include <filesystem>
#include <array>

namespace fs = std::filesystem;

Records_Manager::Records_Manager()
{
}

Records_Manager::~Records_Manager()
{
}


int Records_Manager::get_NumAtributos(std::string esquema)
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

bool Records_Manager::Int(std::string texto)
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

bool Records_Manager::Float(std::string texto)
{
    std::istringstream ss(texto);
    float valor;
    return ss >> valor && ss.eof();
}

int *Records_Manager::Info_Disk(std::string Name_Disk)
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

int Records_Manager::RemainCapacity(std::string Archivo)
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

void Records_Manager::First_Line(std::string Directory_File, std::string Replace_Line)
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

int Records_Manager::NumRegistros(std::string DirArchivo)
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

std::string *Records_Manager::split(std::string &s, char delimiter, int &numTokens)
{
    numTokens = 1;
    bool inQuotes = false;
    for (char c : s)
    {
        if (c == '"')
        {
            inQuotes = !inQuotes;
        }
        else if (c == delimiter && !inQuotes)
        {
            numTokens++;
        }
    }

    std::string *tokens = new std::string[numTokens];
    std::string token;
    int index = 0;
    inQuotes = false;

    for (char c : s)
    {
        if (c == '"')
        {
            inQuotes = !inQuotes;
        }
        else if (c == delimiter && !inQuotes)
        {
            tokens[index++] = token;
            token.clear();
            continue;
        }
        token += c;
    }
    tokens[index] = token;

    return tokens;
}

int *Records_Manager::findLongestEntryLengths(std::string &filename, int &numAttributes)
{
    std::ifstream file(filename);
    std::string line;
    int *maxAttributeLengths = nullptr;
    numAttributes = 0;
    std::getline(file, line);

    while (std::getline(file, line))
    {
        int numTokens = 0;
        std::string *attributes = split(line, ',', numTokens);

        if (maxAttributeLengths == nullptr)
        {
            numAttributes = numTokens;
            maxAttributeLengths = new int[numAttributes];
            for (int i = 0; i < numAttributes; ++i)
            {
                maxAttributeLengths[i] = 0;
            }
        }

        for (int i = 0; i < numTokens; ++i)
        {
            maxAttributeLengths[i] = std::max(maxAttributeLengths[i], static_cast<int>(attributes[i].length()));
        }

        delete[] attributes;
    }

    file.close();
    return maxAttributeLengths;
}

bool Records_Manager::Check_Schemes(std::string Name_Scheme)
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

void Records_Manager::Create_Scheme(std::string Name_Scheme)
{
    std::string Scheme = Name_Scheme + "#";

    std::string Name_Attribute_Line, Name_Attribute, Record_Line, Record;

    std::ifstream Upload_File(fs::current_path().string() + "/" + Name_Scheme + ".csv");

    std::string filename = fs::current_path().string() + "/" + Name_Scheme + ".csv";
    int numAttributes = 0;

    int *Length_Atributes = findLongestEntryLengths(filename, numAttributes);

    getline(Upload_File, Name_Attribute_Line);
    getline(Upload_File, Record_Line);
    Upload_File.close();
    size_t j = 0;
    int k = 0;

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
                        Scheme += Name_Attribute + "#int#" + std::to_string(Length_Atributes[k]) + "#";
                        k++;
                    }
                    else if (Float(Record))
                    {
                        Scheme += Name_Attribute + "#float#" + std::to_string(Length_Atributes[k]) + "#";
                        k++;
                    }
                    else
                    {
                        Scheme += Name_Attribute + "#str#" + std::to_string(Length_Atributes[k]) + "#";
                        k++;
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

void Records_Manager::Cargar(std::string Name_Disk)
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
                                txt << Corregir(Upload_File_Line, Name_Scheme, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l),num) << std::endl;

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

void Records_Manager::Cargar1(std::string Name_Disk)
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
    int numReg = NumRegistros(fs::current_path().string() + "/" + Upload_File_Name + ".csv");

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

                                txt << Corregir(Upload_File_Line, Name_Scheme, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), numReg) << std::endl;

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

void Records_Manager::Cargarn(std::string Name_Disk)
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

    int numReg = NumRegistros(fs::current_path().string() + "/" + Upload_File_Name + ".csv");
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
                                txt << Corregir(Upload_File_Line, Name_Scheme, contador, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l),numReg) << std::endl;

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

std::string Records_Manager::Erase_Blanks(std::string cadena)
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

bool Records_Manager::IsRecord_inTable(std::string linea, std::string NTabla)
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


//Hacerlo con cualquier numeral
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

void Records_Manager::Select_1(std::string NDisco)
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

std::string Records_Manager::Corregir(std::string linea, std::string NTabla, int contador, std::string dir, int num_Registros)
{
    size_t pos = 0;
    std::string res, esq;
    std::ifstream esquemas(fs::current_path().string() + "/Esquemas.txt");
    std::getline(esquemas, esq);
    int mult = 1;
    bool str = false;
    std::string segmento, id_Reg;

    std::stringstream ss;
    ss << std::setw(std::to_string(num_Registros).length()) << std::setfill(' ') << contador;
    id_Reg =  ss.str();

    res += dir + '#' + id_Reg + "#" + NTabla + "#";
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

std::string Records_Manager::LlenarI(std::string linea, int cant)
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

std::string Records_Manager::get_Esquema(std::string tabla)
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

int Records_Manager::Max(std::string esq, int mult)
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

std::string Records_Manager::get_NomAtributos(std::string tabla)
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

void Records_Manager::Select_(std::string NDisco)
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
                                select += "Plato " + std::to_string(Info[i - 1]) + ", Superficie " + std::to_string(Info[j - 1]) + ", Pista " + std::to_string(Info[k - 1]) + ", Sector " + std::to_string(Info[l - 1]) + ":";
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

void Records_Manager::Select_(std::string nEsquema, std::string atributo, std::string signo, int valor, std::string NDisco)
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

