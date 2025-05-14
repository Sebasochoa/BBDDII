#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <filesystem>
#include <array>
#include <iomanip>
#include "Disco.h"
#include "Records_Manager.h"

namespace fs = std::filesystem;

// =============================================
// Constructores y destructores
// =============================================

Records_Manager::Records_Manager() {}
Records_Manager::~Records_Manager() {}

// =============================================
// Funciones de utilidad
// =============================================

bool Records_Manager::Int(const std::string &texto)
{
    if (texto.empty())
        return false;
    std::istringstream ss(texto);
    int valor;
    return ss >> valor && ss.eof();
}

bool Records_Manager::Float(const std::string &texto)
{
    std::istringstream ss(texto);
    float valor;
    return ss >> valor && ss.eof();
}

std::string Records_Manager::Erase_Blanks(const std::string &cadena)
{
    std::string resultado;
    for (char caracter : cadena)
    {
        if (!isspace(caracter) && caracter != ' ')
        {
            resultado += caracter;
        }
    }
    return resultado;
}

std::string Records_Manager::LlenarI(const std::string &linea, int cant)
{
    std::string res;
    size_t resto = cant - linea.length();
    res.append(resto, ' ');
    res += linea;
    return res;
}

// =============================================
// Manejo de esquemas
// =============================================

bool Records_Manager::Check_Schemes(const std::string &Name_Scheme)
{
    std::ifstream File_Schemes(fs::current_path().string() + "/Esquemas.txt");
    std::string Line_Scheme;

    while (getline(File_Schemes, Line_Scheme))
    {
        std::istringstream ss(Line_Scheme);
        std::string Segment;
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

void Records_Manager::Create_Scheme(const std::string &Name_Scheme)
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

int Records_Manager::get_NumAtributos(const std::string &esquema)
{
    std::string linea;
    std::ifstream archivo(fs::current_path().string() + "/Esquemas.txt");

    if (!archivo.is_open())
        return 0;

    while (std::getline(archivo, linea))
    {
        if (linea.find(esquema) != std::string::npos)
        {
            archivo.close();
            return (std::count(linea.begin(), linea.end(), '#') - 1) / 3;
        }
    }
    archivo.close();
    return 0;
}

std::string Records_Manager::get_Esquema(const std::string &tabla)
{
    std::string linea;
    std::ifstream archivo(fs::current_path().string() + "/Esquemas.txt");

    if (archivo.is_open())
    {
        while (std::getline(archivo, linea))
        {
            if (linea.find(tabla) != std::string::npos)
            {
                archivo.close();
                return linea;
            }
        }
        archivo.close();
    }
    return "";
}

std::string Records_Manager::get_NomAtributos(const std::string &tabla)
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

int Records_Manager::Max(const std::string &esq, int mult)
{
    size_t pos = 0;
    int contador = 0;
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
        pos++;
    }
    return numero_str.empty() ? 0 : std::stoi(numero_str);
}

// =============================================
// Manejo de disco y archivos
// =============================================

std::array<int, 4> Records_Manager::Info_Disk(const std::string &Name_Disk)
{
    namespace fs = std::filesystem;
    std::string Directory_Disk = fs::current_path().string() + "/" + Name_Disk;

    // Inicializar contadores
    int Num_Plates = 0, Num_Surfaces = 0, Num_Tracks = 0, Num_Sectors = 0;

    // Contar platos (directorios en el directorio raíz)
    for (const auto &entry : fs::directory_iterator(Directory_Disk))
    {
        if (entry.is_directory())
            Num_Plates++;
    }

    // Contar superficies (por cada plato)
    for (const auto &plate : fs::directory_iterator(Directory_Disk))
    {
        if (plate.is_directory())
        {
            for (const auto &surface : fs::directory_iterator(plate.path()))
            {
                if (surface.is_directory())
                    Num_Surfaces++;
            }
        }
    }

    // Contar pistas (por cada superficie)
    for (const auto &plate : fs::directory_iterator(Directory_Disk))
    {
        if (plate.is_directory())
        {
            for (const auto &surface : fs::directory_iterator(plate.path()))
            {
                if (surface.is_directory())
                {
                    for (const auto &track : fs::directory_iterator(surface.path()))
                    {
                        if (track.is_directory())
                            Num_Tracks++;
                    }
                }
            }
        }
    }

    // Contar sectores (por cada pista)
    for (const auto &plate : fs::directory_iterator(Directory_Disk))
    {
        if (plate.is_directory())
        {
            for (const auto &surface : fs::directory_iterator(plate.path()))
            {
                if (surface.is_directory())
                {
                    for (const auto &track : fs::directory_iterator(surface.path()))
                    {
                        if (track.is_directory())
                        {
                            for (const auto &sector : fs::directory_iterator(track.path()))
                            {
                                if (sector.is_directory())
                                    Num_Sectors++;
                            }
                        }
                    }
                }
            }
        }
    }

    // Calcular promedios y devolver resultados
    return {
        Num_Plates,
        Num_Surfaces / std::max(1, Num_Plates), // Evitar división por cero
        Num_Tracks / std::max(1, Num_Surfaces),
        Num_Sectors / std::max(1, Num_Tracks)};
}

int Records_Manager::RemainCapacity(const std::string &Archivo)
{
    std::ifstream archivo(Archivo);
    if (!archivo.is_open())
    {
        return -1; // Error: no se pudo abrir el archivo
    }

    std::string primeraLinea;
    if (!getline(archivo, primeraLinea))
    {
        return -2; // Error: archivo vacío
    }

    // Buscar cualquier número en la primera línea
    size_t inicioNum = primeraLinea.find_first_of("0123456789");
    if (inicioNum == std::string::npos)
    {
        return -3; // Error: no se encontraron dígitos
    }

    size_t finNum = primeraLinea.find_first_not_of("0123456789", inicioNum);
    if (finNum == std::string::npos)
    {
        finNum = primeraLinea.length();
    }

    try
    {
        return std::stoi(primeraLinea.substr(inicioNum, finNum - inicioNum));
    }
    catch (const std::exception &e)
    {
        return -4; // Error: conversión fallida
    }
}

void Records_Manager::First_Line(const std::string &Directory_File, const std::string &Replace_Line)
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

int Records_Manager::NumRegistros(const std::string &DirArchivo)
{
    std::ifstream archivo(DirArchivo);
    std::string linea;
    int Num = 0;

    while (getline(archivo, linea))
        Num++;
    return Num - 1;
}

// =============================================
// Manejo de registros
// =============================================

std::string *Records_Manager::split(std::string &s, char delimiter, int &numTokens)
{
    // Contar tokens
    numTokens = 1;
    bool inQuotes = false;
    for (char c : s)
    {
        if (c == '"')
            inQuotes = !inQuotes;
        else if (c == delimiter && !inQuotes)
            numTokens++;
    }

    // Dividir string
    std::string *tokens = new std::string[numTokens];
    std::string token;
    int index = 0;
    inQuotes = false;

    for (char c : s)
    {
        if (c == '"')
            inQuotes = !inQuotes;
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

int *Records_Manager::findLongestEntryLengths(const std::string &filename, int &numAttributes)
{
    std::ifstream file(filename);
    std::string line;
    int *maxAttributeLengths = nullptr;
    numAttributes = 0;

    std::getline(file, line); // Saltar encabezado

    while (std::getline(file, line))
    {
        int numTokens = 0;
        std::string *attributes = split(line, ',', numTokens);

        if (maxAttributeLengths == nullptr)
        {
            numAttributes = numTokens;
            maxAttributeLengths = new int[numAttributes]();
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

bool Records_Manager::IsRecord_inTable(const std::string &linea, const std::string &NTabla)
{
    size_t posInicio = linea.find_first_of('#', linea.find_first_of('#') + 1);
    if (posInicio == std::string::npos)
        return false;

    size_t posFinal = linea.find_first_of('#', posInicio + 1);
    if (posFinal == std::string::npos)
        return false;

    return linea.substr(posInicio + 1, posFinal - posInicio - 1) == NTabla;
}

std::string Records_Manager::Corregir(const std::string &linea, const std::string &NTabla,
                                      int contador, const std::string &dir, int num_Registros)
{
    std::string res, segmento;
    bool str = false;

    std::stringstream ss;
    ss << std::setw(std::to_string(num_Registros).length()) << std::setfill('0') << contador;
    std::string id_Reg = ss.str();

    res += dir + '#' + id_Reg + "#" + NTabla + "#" + NTabla + "#";

    int mult = 1;
    for (size_t i = 0; i <= linea.length(); i++)
    {
        if (i == linea.length() || (linea[i] == ',' && !str))
        {
            res += LlenarI(segmento, Max(get_Esquema(NTabla), mult++));
            segmento.clear();
            if (i == linea.length())
                break;
        }
        else if (linea[i] == '"')
        {
            str = !str;
        }
        else
        {
            segmento += linea[i];
        }
    }

    return res;
}

std::string Records_Manager::Cargar(const std::string &Name_Disk)
{
    std::string Upload_File_Name, Name_Scheme;
    std::cin.ignore();
    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, Upload_File_Name);

    std::cout << "Ingrese nombre a su Relacion: ";
    getline(std::cin, Name_Scheme);

    if (!Check_Schemes(Name_Scheme))
    {
        Create_Scheme(Name_Scheme);
    }

    std::string Upload_File_Line, Trash;
    std::array<int, 4> Info = Info_Disk(Name_Disk);

    std::ifstream Upload_File(fs::current_path().string() + "/" + Upload_File_Name + ".csv");
    getline(Upload_File, Trash); // Saltar encabezado

    int num = NumRegistros(fs::current_path().string() + "/" + Upload_File_Name + ".csv");
    int contador = 0;
    bool registroPendiente = false;
    std::string registroNoGuardado;

    for (int i = 1; i <= Info[0] && contador < num; ++i)
    {
        std::string Directory_Plate = fs::current_path().string() + "/" + Name_Disk + "/Plato_" + std::to_string(i);

        for (int j = 1; j <= Info[1] && contador < num; ++j)
        {
            std::string Directory_Surface = Directory_Plate + "/Superficie_" + std::to_string(j);

            for (int k = 1; k <= Info[2] && contador < num; ++k)
            {
                std::string Directory_Track = Directory_Surface + "/Pista_" + std::to_string(k);

                for (int l = 1; l <= Info[3] && contador < num; ++l)
                {
                    std::string Directory_Sector = Directory_Track + "/Sector_" + std::to_string(l);
                    std::string Directory_File = Directory_Sector + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";

                    std::ofstream txt(Directory_File, std::ios::app);
                    if (!txt.is_open()) continue;

                    // Procesar registro pendiente primero
                    if (registroPendiente)
                    {
                        int longitud = static_cast<int>(registroNoGuardado.length());
                        int Vacio = RemainCapacity(Directory_File);

                        if (Vacio >= longitud)
                        {
                            txt << Corregir(registroNoGuardado, Name_Scheme, ++contador, 
                                          std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), 
                                          num) << std::endl;
                            First_Line(Directory_File, std::to_string(Vacio - longitud));
                            registroPendiente = false;
                        }
                        else
                        {
                            continue; 
                        }
                    }

                    while (Upload_File.is_open() && getline(Upload_File, Upload_File_Line) && contador < num)
                    {
                        int longitud = static_cast<int>(Upload_File_Line.length());
                        int Vacio = RemainCapacity(Directory_File);

                        if (Vacio >= longitud)
                        {
                            txt << Corregir(Upload_File_Line, Name_Scheme, ++contador, 
                                          std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), 
                                          num) << std::endl;
                            First_Line(Directory_File, std::to_string(Vacio - longitud));
                        }
                        else
                        {
                            registroNoGuardado = Upload_File_Line;
                            registroPendiente = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    Upload_File.close();
    
    if (registroPendiente)
    {
        std::cerr << "Advertencia: No se pudo cargar el registro " << (contador + 1) 
                  << " por falta de espacio en el disco." << std::endl;
    }

    return Name_Scheme;
}

// =============================================
// Operaciones SELECT
// =============================================

void Records_Manager::Select_all(const std::string &NDisco)
{
    std::string nEsquema;
    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    std::getline(std::cin, nEsquema);

    std::string esquema = get_Esquema(nEsquema);
    RecordsManagerHelpers::Print_Name_Atribute(esquema);

    std::array<int, 4> Info = Info_Disk(NDisco);
    for (int i = 1; i <= Info[0]; ++i)
    {
        for (int j = 1; j <= Info[1]; ++j)
        {
            for (int k = 1; k <= Info[2]; ++k)
            {
                for (int l = 1; l <= Info[3]; ++l)
                {
                    std::string nomArchivo = fs::current_path().string() + "/" + NDisco +
                                             "/Plato_" + std::to_string(i) +
                                             "/Superficie_" + std::to_string(j) +
                                             "/Pista_" + std::to_string(k) +
                                             "/Sector_" + std::to_string(l) + "/" +
                                             std::to_string(i) + std::to_string(j) +
                                             std::to_string(k) + std::to_string(l) + ".txt";

                    std::ifstream Select(nomArchivo);
                    std::string linea;
                    while (getline(Select, linea))
                    {
                        if (RecordsManagerHelpers::obtenerEntreHash(linea, 3) == nEsquema)
                        {
                            RecordsManagerHelpers::imprimirDatosFormateados(esquema, linea);
                        }
                    }
                }
            }
        }
    }
}

void Records_Manager::Select_(const std::string &nEsquema, const std::string &atributo,
                              const std::string &signo, int valor, const std::string &NDisco)
{
    std::string esquema = get_Esquema(nEsquema);
    std::string nomatributos = get_NomAtributos(esquema);

    // Calcular desplazamiento
    std::istringstream ss(nomatributos);
    std::string segmento;
    int offset = 0, sumatoria = 0;

    while (std::getline(ss, segmento, ','))
    {
        if (segmento == atributo)
            break;
        sumatoria += Max(esquema, ++offset);
    }

    // Buscar en el disco
    std::array<int, 4> Info = Info_Disk(NDisco);
    for (int i = 1; i <= Info[0]; ++i)
    {
        for (int j = 1; j <= Info[1]; ++j)
        {
            for (int k = 1; k <= Info[2]; ++k)
            {
                for (int l = 1; l <= Info[3]; ++l)
                {
                    std::string nomArchivo = fs::current_path().string() + "/" + NDisco +
                                             "/Plato_" + std::to_string(i) +
                                             "/Superficie_" + std::to_string(j) +
                                             "/Pista_" + std::to_string(k) +
                                             "/Sector_" + std::to_string(l) + "/" +
                                             std::to_string(i) + std::to_string(j) +
                                             std::to_string(k) + std::to_string(l) + ".txt";

                    std::ifstream archivo(nomArchivo);
                    std::string linea;
                    while (getline(archivo, linea))
                    {
                        if (!IsRecord_inTable(linea, nEsquema))
                            continue;

                        size_t posCuartoHash = linea.find('#', linea.find('#', linea.find('#', linea.find('#') + 1) + 1) + 1);
                        std::string valstr = Erase_Blanks(linea.substr(
                            sumatoria + posCuartoHash,
                            Max(esquema, offset + 1)));

                        if (valstr.empty() || !std::all_of(valstr.begin(), valstr.end(), ::isdigit))
                            continue;

                        int val = std::stoi(valstr);
                        if ((signo == "<" && val < valor) || (signo == ">" && val > valor) ||
                            (signo == "<=" && val <= valor) || (signo == ">=" && val >= valor) ||
                            (signo == "==" && val == valor))
                        {
                            std::cout << linea << "\n";
                        }
                    }
                }
            }
        }
    }
}
