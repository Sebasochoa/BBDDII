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

#include <vector>
#include <map>

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

std::string Records_Manager::EscapeCampoVariable(const std::string &campo)
{
    std::string resultado;
    for (char c : campo)
    {
        if (c == '|' || c == '#')
        {
            resultado += '\\';
        }
        resultado += c;
    }
    return resultado;
}

std::string Records_Manager::GenerarMetadatos(const std::string &linea)
{
    std::string metadata;
    int campo_num = 0;
    bool entre_comillas = false;
    size_t inicio_campo = 0;

    for (size_t pos = 0; pos <= linea.length(); ++pos)
    {
        // Fin de línea o coma fuera de comillas
        if (pos == linea.length() || (!entre_comillas && linea[pos] == ','))
        {
            // Calcular longitud real del campo (sin comillas externas)
            size_t longitud = pos - inicio_campo;
            bool tiene_comillas = (linea[inicio_campo] == '"' && linea[pos - 1] == '"');

            if (tiene_comillas)
            {
                longitud -= 2; // Restar las comillas de inicio y fin
            }

            metadata += std::to_string(campo_num++) + ":" + std::to_string(longitud) + ";";
            inicio_campo = pos + 1; // Saltar la coma
        }
        // Detectar comillas
        else if (linea[pos] == '"')
        {
            entre_comillas = !entre_comillas;
        }
    }

    return metadata;
}

std::string Records_Manager::LlenarI(const std::string &texto, int longitud)
{
    if (texto.length() >= longitud)
    {
        return texto.substr(0, longitud);
    }
    return std::string(longitud - texto.length(), ' ') + texto;
}

// =============================================
// Manejo de esquemas
// =============================================

bool Records_Manager::Check_Schemes(const std::string &Name_Scheme, const std::string &Name_Disk)
{
    std::ifstream File_Schemes(fs::current_path().string() + "/Discos/" + Name_Disk + "/Plato_1/Superficie_1/Pista_1/Sector_1/1111.txt");
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

void Records_Manager::Create_Scheme(const std::string &Name_Scheme, const std::string &Name_Disk, bool formato)
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

    Scheme = formato ? Scheme + "FI" : Scheme + "VA";
    
    std::ofstream Scheme_File(fs::current_path().string() + "/Discos/" + Name_Disk + "/Plato_1/Superficie_1/Pista_1/Sector_1/1111.txt", std::ios::app);
    Scheme_File << Scheme << std::endl;
    Scheme_File.close();
    
}

int Records_Manager::get_NumAtributos(const std::string &esquema, const std::string &Name_Disk)
{
    std::string linea;
    std::ifstream archivo(fs::current_path().string() + "/Discos/" + Name_Disk + "/Plato_1/Superficie_1/Pista_1/Sector_1/1111.txt");

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

std::string Records_Manager::get_Esquema(const std::string &tabla, const std::string &Name_Disk)
{
    std::string linea;
    std::ifstream archivo(fs::current_path().string() + "/Discos/" + Name_Disk + "/Plato_1/Superficie_1/Pista_1/Sector_1/1111.txt");

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
    std::string nombres;
    bool primer_atributo = true;

    pos = tabla.find('#', pos);
    if (pos == std::string::npos)
        return nombres;

    pos++;

    while (pos < tabla.length())
    {
        size_t inicio_atributo = pos;
        pos = tabla.find('#', pos);
        if (pos == std::string::npos)
            break;

        std::string atributo = tabla.substr(inicio_atributo, pos - inicio_atributo);

        if (!primer_atributo)
        {
            nombres += ",";
        }
        else
        {
            primer_atributo = false;
        }
        nombres += atributo;

        pos = tabla.find('#', pos + 1);
        if (pos == std::string::npos)
            break;
        pos = tabla.find('#', pos + 1);
        if (pos == std::string::npos)
            break;
        pos++;
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
    std::string Directory_Disk = fs::current_path().string() + "/Discos/" + Name_Disk;

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
        return -1;
    }

    std::string primeraLinea;
    if (!getline(archivo, primeraLinea))
    {
        return -2;
    }

    size_t inicioNum = primeraLinea.find_first_of("0123456789");
    if (inicioNum == std::string::npos)
    {
        return -3;
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
        return -4;
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

std::string Records_Manager::REstatico(const std::string &linea, const std::string &NTabla, int contador, const std::string &dir, int num_Registros, const std::string &NDisco)
{
    std::string registro = dir + '#' + std::to_string(contador) + '#' + NTabla + '#';

    size_t pos = 0;
    for (int i = 0; i < get_NumAtributos(NTabla, NDisco); ++i)
    {
        int longitud = Max(get_Esquema(NTabla, NDisco), i + 1);

        std::string campo;
        bool entre_comillas = false;
        while (pos < linea.length())
        {
            if (linea[pos] == '"')
            {
                entre_comillas = !entre_comillas;
                pos++;
                continue;
            }

            if (!entre_comillas && linea[pos] == ',')
            {
                pos++;
                break;
            }

            campo += linea[pos];
            pos++;
        }

        registro += LlenarI(campo, longitud);
    }

    return registro;
}

std::string Records_Manager::RVariable(const std::string &linea, const std::string &NTabla, int contador, const std::string &dir, int num_Registros, const std::string &NDisco)
{
    std::string registro = dir + '#' + std::to_string(contador) + '#' + NTabla + '#';

    // 2. Procesar campos variables
    size_t pos = 0;
    bool entre_comillas = false;
    std::string campo_actual;

    while (pos < linea.length())
    {
        if (linea[pos] == '"')
        {
            entre_comillas = !entre_comillas;
            pos++;
            continue;
        }

        if (!entre_comillas && linea[pos] == ',')
        {
            // Final del campo - agregar al registro
            registro += EscapeCampoVariable(campo_actual) + '|';
            campo_actual.clear();
            pos++;
            continue;
        }

        // Caracter normal - agregar al campo actual
        campo_actual += linea[pos];
        pos++;
    }

    // Agregar el último campo si existe
    if (!campo_actual.empty())
    {
        registro += EscapeCampoVariable(campo_actual) + '|';
    }

    // 3. Agregar metadatos
    registro += "#METADATA:" + GenerarMetadatos(linea);

    return registro;
}

std::string Records_Manager::Cargar(const std::string &Name_Disk, const bool &formato)
{
    std::string Upload_File_Name, Name_Scheme;
    std::cin.ignore();
    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, Upload_File_Name);

    std::cout << "Ingrese nombre a su Relacion: ";
    getline(std::cin, Name_Scheme);

    if (!Check_Schemes(Name_Scheme, Name_Disk))
    {
        Create_Scheme(Name_Scheme, Name_Disk, formato);
    }

    std::array<int, 4> disk_info = Info_Disk(Name_Disk);

    std::cout << std::endl;
    const int total_platos = disk_info[0];
    const int sectores_por_plato = disk_info[1] * disk_info[2] * disk_info[3];

    int registros_cargados = 0;
    int registros_procesados = 0;
    bool registro_pendiente = false;
    std::string registro_no_guardado;
    std::string Upload_File_Line;

    std::ifstream count_file(fs::current_path().string() + "/" + Upload_File_Name + ".csv");
    int total_registros = std::count(std::istreambuf_iterator<char>(count_file),
                                     std::istreambuf_iterator<char>(), '\n') -
                          1;
    count_file.close();

    std::ifstream Upload_File(fs::current_path().string() + "/" + Upload_File_Name + ".csv");
    getline(Upload_File, Upload_File_Line);

    for (int sector_global = 0; sector_global < sectores_por_plato && registros_cargados < total_registros; ++sector_global)
    {

        int j = (sector_global / (disk_info[2] * disk_info[3])) % disk_info[1] + 1;
        int k = (sector_global / disk_info[3]) % disk_info[2] + 1;
        int l = sector_global % disk_info[3] + 1;

        for (int i = 1; i <= total_platos && registros_cargados < total_registros; ++i)
        {
            std::string file_path = fs::current_path().string() + "/Discos/" + Name_Disk +
                                    "/Plato_" + std::to_string(i) +
                                    "/Superficie_" + std::to_string(j) +
                                    "/Pista_" + std::to_string(k) +
                                    "/Sector_" + std::to_string(l) + "/" +
                                    std::to_string(i) + std::to_string(j) +
                                    std::to_string(k) + std::to_string(l) + ".txt";

            int capacidad = RemainCapacity(file_path);
            std::ofstream sector_file(file_path, std::ios::app);

            if (!sector_file.is_open())
                continue;

            if (registro_pendiente)
            {
                int longitud = registro_no_guardado.length();
                if (capacidad >= longitud)
                {
                    std::string registro_corregido;
                    if (formato)
                    {
                        registro_corregido = REstatico(Upload_File_Line, Name_Scheme, ++registros_cargados, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), total_registros, Name_Disk);
                    }
                    else
                    {
                        registro_corregido = RVariable(Upload_File_Line, Name_Scheme, ++registros_cargados, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), total_registros, Name_Disk);
                    }

                    sector_file << registro_corregido << std::endl;
                    First_Line(file_path, std::to_string(capacidad - longitud));
                    registro_pendiente = false;
                }
                else
                {
                    continue;
                }
            }

            while (!registro_pendiente && getline(Upload_File, Upload_File_Line))
            {
                int longitud = Upload_File_Line.length();
                capacidad = RemainCapacity(file_path);

                if (capacidad >= longitud)
                {
                    std::string registro_corregido;
                    if (formato)
                    {
                        registro_corregido = REstatico(Upload_File_Line, Name_Scheme, ++registros_cargados, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), total_registros, Name_Disk);
                    }
                    else
                    {
                        registro_corregido = RVariable(Upload_File_Line, Name_Scheme, ++registros_cargados, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), total_registros, Name_Disk);
                    }

                    sector_file << registro_corregido << std::endl;
                    First_Line(file_path, std::to_string(capacidad - longitud));
                    registros_procesados++;
                }
                else
                {
                    registro_no_guardado = Upload_File_Line;
                    registro_pendiente = true;
                    break;
                }

                if (registros_cargados >= total_registros)
                    break;
            }
        }
    }

    Upload_File.close();

    if (registro_pendiente)
    {
        std::cerr << "Advertencia: " << (total_registros - registros_cargados)
                  << " registros no pudieron ser cargados por falta de espacio." << std::endl;
    }
    else if (registros_cargados < total_registros)
    {
        std::cerr << "Advertencia: Solo se cargaron " << registros_cargados
                  << " de " << total_registros << " registros." << std::endl;
    }

    return Name_Scheme;
}

std::string Records_Manager::Cargar(const std::string &Name_Disk, const bool &formato, const std::string &Name_File)
{


    std::array<int, 4> disk_info = Info_Disk(Name_Disk);

    std::cout << std::endl;
    const int total_platos = disk_info[0];
    const int sectores_por_plato = disk_info[1] * disk_info[2] * disk_info[3];

    int registros_cargados = 0;
    int registros_procesados = 0;
    bool registro_pendiente = false;
    std::string registro_no_guardado;
    std::string Upload_File_Line;

    std::ifstream count_file(fs::current_path().string() + "/" + Name_File + ".csv");
    int total_registros = std::count(std::istreambuf_iterator<char>(count_file),
                                     std::istreambuf_iterator<char>(), '\n') -
                          1;
    count_file.close();

    std::ifstream Upload_File(fs::current_path().string() + "/" + Name_File + ".csv");
    getline(Upload_File, Upload_File_Line);

    for (int sector_global = 0; sector_global < sectores_por_plato && registros_cargados < total_registros; ++sector_global)
    {

        int j = (sector_global / (disk_info[2] * disk_info[3])) % disk_info[1] + 1;
        int k = (sector_global / disk_info[3]) % disk_info[2] + 1;
        int l = sector_global % disk_info[3] + 1;

        for (int i = 1; i <= total_platos && registros_cargados < total_registros; ++i)
        {
            std::string file_path = fs::current_path().string() + "/Discos/" + Name_Disk +
                                    "/Plato_" + std::to_string(i) +
                                    "/Superficie_" + std::to_string(j) +
                                    "/Pista_" + std::to_string(k) +
                                    "/Sector_" + std::to_string(l) + "/" +
                                    std::to_string(i) + std::to_string(j) +
                                    std::to_string(k) + std::to_string(l) + ".txt";

            int capacidad = RemainCapacity(file_path);
            std::ofstream sector_file(file_path, std::ios::app);

            if (!sector_file.is_open())
                continue;
            std::cout << "Funciona\n";
            if (registro_pendiente)
            {
                int longitud = registro_no_guardado.length();
                if (capacidad >= longitud)
                {
                    std::string registro_corregido;
                    if (formato)
                    {
                        registro_corregido = REstatico(Upload_File_Line, Name_File, ++registros_cargados, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), total_registros, Name_Disk);
                    }
                    else
                    {
                        registro_corregido = RVariable(Upload_File_Line, Name_File, ++registros_cargados, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), total_registros, Name_Disk);
                    }

                    sector_file << registro_corregido << std::endl;
                    First_Line(file_path, std::to_string(capacidad - longitud));
                    registro_pendiente = false;
                }
                else
                {
                    continue;
                }
            }

            while (!registro_pendiente && getline(Upload_File, Upload_File_Line))
            {
                int longitud = Upload_File_Line.length();
                capacidad = RemainCapacity(file_path);

                if (capacidad >= longitud)
                {
                    std::string registro_corregido;
                    if (formato)
                    {
                        registro_corregido = REstatico(Upload_File_Line, Name_File, ++registros_cargados, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), total_registros, Name_Disk);
                    }
                    else
                    {
                        registro_corregido = RVariable(Upload_File_Line, Name_File, ++registros_cargados, std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l), total_registros, Name_Disk);
                    }

                    sector_file << registro_corregido << std::endl;
                    First_Line(file_path, std::to_string(capacidad - longitud));
                    registros_procesados++;
                }
                else
                {
                    registro_no_guardado = Upload_File_Line;
                    registro_pendiente = true;
                    break;
                }

                if (registros_cargados >= total_registros)
                    break;
            }
        }
    }

    Upload_File.close();

    if (registro_pendiente)
    {
        std::cerr << "Advertencia: " << (total_registros - registros_cargados)
                  << " registros no pudieron ser cargados por falta de espacio." << std::endl;
    }
    else if (registros_cargados < total_registros)
    {
        std::cerr << "Advertencia: Solo se cargaron " << registros_cargados
                  << " de " << total_registros << " registros." << std::endl;
    }

    return Name_File;
}

// =============================================
// Operaciones SELECT actualizadas para manejar formatos fijos y variables
// =============================================

void Records_Manager::Select_all(const std::string &NDisco)
{
    std::string nEsquema;
    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    std::cin.ignore();
    std::getline(std::cin, nEsquema);

    if (nEsquema.empty())
    {
        std::cerr << "Error: Nombre de tabla no puede estar vacío" << std::endl;
        return;
    }

    std::string esquema = get_Esquema(nEsquema, NDisco);
    if (esquema.empty())
    {
        std::cerr << "Error: Tabla '" << nEsquema << "' no encontrada" << std::endl;
        return;
    }

    // Determinar si el esquema es de formato fijo (FI) o variable (VA)
    bool formatoFijo = (esquema.find("#FI") != std::string::npos);

    // Imprimir nombres de atributos
    RecordsManagerHelpers::Print_Name_Atribute(esquema);

    std::array<int, 4> Info = Info_Disk(NDisco);
    if (Info[0] == 0 || Info[1] == 0 || Info[2] == 0 || Info[3] == 0)
    {
        std::cerr << "Error: Estructura de disco inválida" << std::endl;
        return;
    }

    for (int i = 1; i <= Info[0]; ++i)
    {
        for (int j = 1; j <= Info[1]; ++j)
        {
            for (int k = 1; k <= Info[2]; ++k)
            {
                for (int l = 1; l <= Info[3]; ++l)
                {
                    std::string nomArchivo = fs::current_path().string() + "/Discos/" + NDisco + "/Plato_" + std::to_string(i) + "/Superficie_" + std::to_string(j) + "/Pista_" + std::to_string(k) + "/Sector_" + std::to_string(l) + "/" + std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";

                    std::ifstream Select(nomArchivo);
                    if (!Select.is_open())
                        continue;

                    std::string linea;
                    while (getline(Select, linea))
                    {
                        try
                        {
                            std::string tabla_registro = RecordsManagerHelpers::obtenerEntreHash(linea, 2);
                            // std::cout << tabla_registro << std::endl;
                            if (tabla_registro == nEsquema)
                            {
                                if (formatoFijo)
                                {
                                    RecordsManagerHelpers::imprimirDatosFormateados(esquema, linea);
                                }
                                else
                                {
                                    // Procesamiento para formato variable
                                    size_t posDatos = linea.find('#', linea.find('#', linea.find('#') + 1) + 1) + 1;
                                    size_t posMetadata = linea.find("#METADATA:");

                                    if (posMetadata == std::string::npos)
                                    {
                                        std::cerr << "Error: Formato de registro variable inválido" << std::endl;
                                        continue;
                                    }

                                    std::string datos = linea.substr(posDatos, posMetadata - posDatos);
                                    std::string metadata = linea.substr(posMetadata + 10);

                                    // Parsear metadatos
                                    std::vector<std::pair<int, int>> campos;
                                    std::istringstream metaStream(metadata);
                                    std::string campoMeta;
                                    while (std::getline(metaStream, campoMeta, ';'))
                                    {
                                        if (campoMeta.empty())
                                            continue;
                                        size_t sep = campoMeta.find(':');
                                        int numCampo = std::stoi(campoMeta.substr(0, sep));
                                        int longitud = std::stoi(campoMeta.substr(sep + 1));
                                        campos.emplace_back(numCampo, longitud);
                                    }

                                    // Extraer campos según metadatos
                                    std::istringstream datosStream(datos);
                                    std::string campo;
                                    int campoActual = 0;
                                    std::vector<std::string> valores;

                                    while (std::getline(datosStream, campo, '|'))
                                    {
                                        // Eliminar escapes si existen
                                        size_t escapePos;
                                        while ((escapePos = campo.find("\\|")) != std::string::npos)
                                        {
                                            campo.replace(escapePos, 2, "|");
                                        }
                                        while ((escapePos = campo.find("\\#")) != std::string::npos)
                                        {
                                            campo.replace(escapePos, 2, "#");
                                        }

                                        if (campoActual < campos.size())
                                        {
                                            valores.push_back(campo);
                                        }
                                        campoActual++;
                                    }

                                    // Imprimir valores
                                    for (const auto &valor : valores)
                                    {
                                        std::cout << std::left << std::setw(20) << valor << " ";
                                    }
                                    std::cout << std::endl;
                                }
                            }
                        }
                        catch (const std::exception &e)
                        {
                            std::cerr << "Error procesando registro: " << e.what() << std::endl;
                            continue;
                        }
                    }
                }
            }
        }
    }
}

void Records_Manager::Select_(const std::string &nEsquema, const std::string &atributo, const std::string &signo, int valor, const std::string &NDisco)
{
    std::string esquema = get_Esquema(nEsquema, NDisco);
    if (esquema.empty())
    {
        std::cerr << "Error: Esquema no encontrado" << std::endl;
        return;
    }

    bool formatoFijo = (esquema.find("#FI") != std::string::npos);
    std::string nomatributos = get_NomAtributos(esquema);

    // Encontrar el índice y tipo del atributo buscado
    int atributoIndex = -1;
    std::string tipoAtributo;
    std::istringstream ss(nomatributos);
    std::string segmento;
    int currentIndex = 0;

    while (std::getline(ss, segmento, ','))
    {
        if (segmento == atributo)
        {
            atributoIndex = currentIndex;
            tipoAtributo = GetTipoAtributo(esquema, currentIndex);
            break;
        }
        currentIndex++;
    }

    if (atributoIndex == -1)
    {
        std::cerr << "Error: Atributo '" << atributo << "' no encontrado" << std::endl;
        return;
    }

    std::array<int, 4> Info = Info_Disk(NDisco);
    for (int i = 1; i <= Info[0]; ++i)
    {
        for (int j = 1; j <= Info[1]; ++j)
        {
            for (int k = 1; k <= Info[2]; ++k)
            {
                for (int l = 1; l <= Info[3]; ++l)
                {
                    std::string nomArchivo = fs::current_path().string() + "/Discos/" + NDisco +
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

                        try
                        {
                            std::string valorCampo;
                            if (formatoFijo)
                            {
                                // Procesamiento para formato fijo
                                size_t posDatos = linea.find('#', linea.find('#', linea.find('#') + 1) + 1) + 1;
                                int offset = 0;

                                // Calcular la posición del atributo
                                for (int idx = 0; idx < atributoIndex; ++idx)
                                {
                                    offset += Max(esquema, idx + 1);
                                }

                                int longitud = Max(esquema, atributoIndex + 1);
                                valorCampo = Erase_Blanks(linea.substr(posDatos + offset, longitud));
                            }
                            else
                            {
                                // Procesamiento para formato variable
                                size_t posMetadata = linea.find("#METADATA:");
                                if (posMetadata == std::string::npos)
                                    continue;

                                std::string metadata = linea.substr(posMetadata + 10);
                                std::istringstream metaStream(metadata);
                                std::string campoMeta;
                                int campoActual = 0;
                                size_t inicioCampo = 0;

                                // Encontrar la posición y longitud del atributo en los metadatos
                                while (std::getline(metaStream, campoMeta, ';'))
                                {
                                    if (campoMeta.empty())
                                        continue;
                                    size_t sep = campoMeta.find(':');
                                    int numCampo = std::stoi(campoMeta.substr(0, sep));
                                    int longitud = std::stoi(campoMeta.substr(sep + 1));

                                    if (numCampo == atributoIndex)
                                    {
                                        // Extraer el campo de los datos
                                        size_t posDatos = linea.find('#', linea.find('#', linea.find('#') + 1) + 1) + 1;
                                        std::string datos = linea.substr(posDatos, posMetadata - posDatos);

                                        std::istringstream datosStream(datos);
                                        std::string campo;
                                        int campoIdx = 0;

                                        while (std::getline(datosStream, campo, '|'))
                                        {
                                            if (campoIdx == numCampo)
                                            {
                                                // Eliminar escapes
                                                size_t escapePos;
                                                while ((escapePos = campo.find("\\|")) != std::string::npos)
                                                {
                                                    campo.replace(escapePos, 2, "|");
                                                }
                                                while ((escapePos = campo.find("\\#")) != std::string::npos)
                                                {
                                                    campo.replace(escapePos, 2, "#");
                                                }
                                                valorCampo = campo;
                                                break;
                                            }
                                            campoIdx++;
                                        }
                                        break;
                                    }
                                }
                            }

                            // Validar y comparar el valor
                            if (valorCampo.empty())
                                continue;

                            bool cumpleCondicion = false;
                            if (tipoAtributo == "int" || tipoAtributo == "float")
                            {
                                try
                                {
                                    double valorNumerico = std::stod(valorCampo);
                                    double valorComparar = valor;

                                    if (signo == "<")
                                        cumpleCondicion = (valorNumerico < valorComparar);
                                    else if (signo == ">")
                                        cumpleCondicion = (valorNumerico > valorComparar);
                                    else if (signo == "<=")
                                        cumpleCondicion = (valorNumerico <= valorComparar);
                                    else if (signo == ">=")
                                        cumpleCondicion = (valorNumerico >= valorComparar);
                                    else if (signo == "==")
                                        cumpleCondicion = (valorNumerico == valorComparar);
                                }
                                catch (...)
                                {
                                    continue;
                                }
                            }
                            else // Tipo string
                            {
                                if (signo == "==")
                                    cumpleCondicion = (valorCampo == std::to_string(valor));
                            }

                            if (cumpleCondicion)
                            {
                                if (formatoFijo)
                                {
                                    RecordsManagerHelpers::imprimirDatosFormateados(esquema, linea);
                                }
                                else
                                {
                                    std::cout << linea << std::endl;
                                }
                            }
                        }
                        catch (const std::exception &e)
                        {
                            std::cerr << "Error procesando registro: " << e.what() << std::endl;
                            continue;
                        }
                    }
                }
            }
        }
    }
}

void Records_Manager::SelectArchivo(const std::string &nEsquema, const std::string &atributo, const std::string &signo, int valor, const std::string &NDisco)
{
    std::string esquema = get_Esquema(nEsquema, NDisco);
    if (esquema.empty())
    {
        std::cerr << "Error: Esquema no encontrado" << std::endl;
        return;
    }

    bool formatoFijo = (esquema.find("#FI") != std::string::npos);
    std::string nomatributos = get_NomAtributos(esquema);

    // Crear nombre para la nueva relación
    std::string nueva_relacion = nEsquema + "_filtrado_" + atributo + signo + std::to_string(valor);
    std::string temp_csv = fs::current_path().string() + "/" + nueva_relacion + ".csv";
    std::ofstream csv_file(temp_csv);

    // Escribir encabezado
    csv_file << nomatributos << "\n";

    // Encontrar índice y tipo del atributo
    int atributoIndex = -1;
    std::string tipoAtributo;
    std::istringstream ss(nomatributos);
    std::string segmento;
    int currentIndex = 0;

    while (std::getline(ss, segmento, ','))
    {
        if (segmento == atributo)
        {
            atributoIndex = currentIndex;
            tipoAtributo = GetTipoAtributo(esquema, currentIndex);
            break;
        }
        currentIndex++;
    }

    if (atributoIndex == -1)
    {
        std::cerr << "Error: Atributo '" << atributo << "' no encontrado" << std::endl;
        csv_file.close();
        return;
    }

    std::array<int, 4> Info = Info_Disk(NDisco);
    int registros_encontrados = 0;

    for (int i = 1; i <= Info[0]; ++i)
    {
        for (int j = 1; j <= Info[1]; ++j)
        {
            for (int k = 1; k <= Info[2]; ++k)
            {
                for (int l = 1; l <= Info[3]; ++l)
                {
                    std::string nomArchivo = fs::current_path().string() + "/Discos/" + NDisco +
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

                        try
                        {
                            std::string valorCampo;
                            std::vector<std::string> valoresCampos;

                            if (formatoFijo)
                            {
                                // Procesamiento para formato fijo
                                size_t posDatos = linea.find('#', linea.find('#', linea.find('#') + 1) + 1) + 1;
                                int offset = 0;

                                for (int idx = 0; idx < get_NumAtributos(nEsquema, NDisco); ++idx)
                                {
                                    int longitud = Max(esquema, idx + 1);
                                    std::string campo = Erase_Blanks(linea.substr(posDatos + offset, longitud));
                                    valoresCampos.push_back(campo);
                                    offset += longitud;

                                    if (idx == atributoIndex)
                                    {
                                        valorCampo = campo;
                                    }
                                }
                            }
                            else
                            {
                                size_t posDatos = linea.find('#', linea.find('#', linea.find('#') + 1) + 1) + 1;
                                size_t posMetadata = linea.find("#METADATA:");

                                if (posMetadata == std::string::npos)
                                    continue;

                                std::string datos = linea.substr(posDatos, posMetadata - posDatos);
                                std::string metadata = linea.substr(posMetadata + 10);

                                std::map<int, std::string> camposMap;
                                std::istringstream metaStream(metadata);
                                std::string campoMeta;
                                int campoActual = 0;

                                std::istringstream datosStream(datos);
                                std::string campo;
                                while (std::getline(datosStream, campo, '|'))
                                {
                                    size_t escapePos;
                                    while ((escapePos = campo.find("\\|")) != std::string::npos)
                                    {
                                        campo.replace(escapePos, 2, "|");
                                    }
                                    while ((escapePos = campo.find("\\#")) != std::string::npos)
                                    {
                                        campo.replace(escapePos, 2, "#");
                                    }

                                    if (campoActual == atributoIndex)
                                    {
                                        valorCampo = campo;
                                    }
                                    valoresCampos.push_back(campo);
                                    campoActual++;
                                }
                            }

                            bool cumpleCondicion = false;
                            if (!valorCampo.empty())
                            {
                                if (tipoAtributo == "int" || tipoAtributo == "float")
                                {
                                    try
                                    {
                                        double valorNumerico = std::stod(valorCampo);
                                        double valorComparar = valor;

                                        if (signo == "<")
                                            cumpleCondicion = (valorNumerico < valorComparar);
                                        else if (signo == ">")
                                            cumpleCondicion = (valorNumerico > valorComparar);
                                        else if (signo == "<=")
                                            cumpleCondicion = (valorNumerico <= valorComparar);
                                        else if (signo == ">=")
                                            cumpleCondicion = (valorNumerico >= valorComparar);
                                        else if (signo == "==")
                                            cumpleCondicion = (valorNumerico == valorComparar);
                                    }
                                    catch (...)
                                    {
                                        continue;
                                    }
                                }
                                else
                                {
                                    if (signo == "==")
                                        cumpleCondicion = (valorCampo == std::to_string(valor));
                                }
                            }

                            if (cumpleCondicion)
                            {
                                std::string registro_csv;
                                for (size_t idx = 0; idx < valoresCampos.size(); ++idx)
                                {
                                    if (GetTipoAtributo(esquema, idx) == "str" &&
                                        valoresCampos[idx].find(',') != std::string::npos)
                                    {
                                        registro_csv += "\"" + valoresCampos[idx] + "\"";
                                    }
                                    else
                                    {
                                        registro_csv += valoresCampos[idx];
                                    }

                                    if (idx < valoresCampos.size() - 1)
                                    {
                                        registro_csv += ",";
                                    }
                                }
                                csv_file << registro_csv << "\n";
                                registros_encontrados++;
                            }
                        }
                        catch (const std::exception &e)
                        {
                            std::cerr << "Error procesando registro: " << e.what() << std::endl;
                            continue;
                        }
                    }
                }
            }
        }
    }

    csv_file.close();

    if (registros_encontrados > 0)
    {
        std::string nuevo_esquema = esquema;
        size_t pos = nuevo_esquema.find('#');
        if (pos != std::string::npos)
        {
            nuevo_esquema.replace(0, pos, nueva_relacion);

            // Guardar el nuevo esquema
            std::ofstream esquema_file(fs::current_path().string() + "/Discos/" + NDisco +
                                           "/Plato_1/Superficie_1/Pista_1/Sector_1/1111.txt",
                                       std::ios::app);
            if (esquema_file.is_open())
            {
                esquema_file << nuevo_esquema << "\n";
                esquema_file.close();
            }
        }


        Cargar(NDisco, !formatoFijo, temp_csv);
         
    }

    std::cout << "Se encontraron " << registros_encontrados << " registros. "
              << "Nueva relación creada: " << nueva_relacion << std::endl;
}

bool Records_Manager::ReemplazarRegistro(const std::string &Name_Disk, const std::string &Name_Scheme)
{
    std::string id_buscar;
    std::cout << "Ingrese el ID del registro a reemplazar: ";
    std::cin.ignore();
    getline(std::cin, id_buscar);

    std::cout << "Ingrese los nuevos datos para el registro (en formato CSV): ";
    std::string nuevos_datos;
    getline(std::cin, nuevos_datos);

    std::array<int, 4> disk_info = Info_Disk(Name_Disk);
    bool encontrado = false;
    bool reemplazado = false;

    for (int i = 1; i <= disk_info[0]; ++i)
    {
        for (int j = 1; j <= disk_info[1]; ++j)
        {
            for (int k = 1; k <= disk_info[2]; ++k)
            {
                for (int l = 1; l <= disk_info[3]; ++l)
                {
                    std::string file_path = fs::current_path().string() + "/" + Name_Disk +
                                            "/Plato_" + std::to_string(i) +
                                            "/Superficie_" + std::to_string(j) +
                                            "/Pista_" + std::to_string(k) +
                                            "/Sector_" + std::to_string(l) + "/" +
                                            std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l) + ".txt";

                    std::ifstream in_file(file_path);
                    if (!in_file.is_open())
                        continue;

                    std::string contenido;
                    std::string linea;
                    bool modificado = false;

                    while (getline(in_file, linea))
                    {
                        size_t pos1 = linea.find('#');
                        size_t pos2 = linea.find('#', pos1 + 1);
                        std::string id_registro = linea.substr(pos1 + 1, pos2 - pos1 - 1);
                        id_registro = Erase_Blanks(id_registro);

                        if (id_registro == id_buscar)
                        {
                            encontrado = true;

                            std::string nuevo_registro = RVariable(nuevos_datos, Name_Scheme,
                                                                   std::stoi(id_buscar),
                                                                   std::to_string(i) + std::to_string(j) + std::to_string(k) + std::to_string(l),
                                                                   -1, Name_Disk);
                            contenido += nuevo_registro + "\n";
                            modificado = true;
                        }
                        else
                        {
                            contenido += linea + "\n";
                        }
                    }
                    in_file.close();

                    if (modificado)
                    {
                        std::ofstream out_file(file_path);
                        if (out_file.is_open())
                        {
                            out_file << contenido;
                            out_file.close();
                            reemplazado = true;
                        }
                    }
                }
            }
        }
    }

    if (!encontrado)
    {
        std::cout << "Registro con ID " << id_buscar << " no encontrado." << std::endl;
        return false;
    }

    if (reemplazado)
    {
        std::cout << "Registro reemplazado exitosamente." << std::endl;
    }
    else
    {
        std::cout << "Error al guardar los cambios." << std::endl;
    }

    return reemplazado;
}

std::string Records_Manager::GetTipoAtributo(const std::string &esquema, int atributo_index)
{
    size_t pos = 0;
    int campo = 0;
    int atributo_actual = 0;

    while ((pos = esquema.find('#', pos)) != std::string::npos)
    {
        campo++;
        if (campo % 3 == 2)
        { // Posición del tipo de dato
            if (atributo_actual == atributo_index)
            {
                size_t start = pos + 1;
                pos = esquema.find('#', start);
                return esquema.substr(start, pos - start);
            }
            atributo_actual++;
        }
        pos++;
    }
    return ""; // Retorna string vacío si no se encuentra
}
