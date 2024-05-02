#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <string>
#include <algorithm>
#include <cctype>

class Megatron
{
private:
    std::string Tuplas;

public:
    Megatron();
    ~Megatron();
    bool Existe(const char *);
    void Esquemas();
    void Tablas();
    void Cargar();
    void Select_();
    void Select_(std::string);
    std::string Corregir(std::string);
    int Max(std::string, int);
    int *Max(std::string);
    std::string Llenar(std::string, int);
    std::string Vaciar(std::string, int *);
    std::string get_Esquema();
};

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

void Megatron::Esquemas()
{
    Tuplas.clear();
    std::cout << "Ingrese el nombre para el Esquema: ";
    std::getline(std::cin, Tuplas);
    std::string respuesta, nombreAtributo, tipoDato;
    do
    {
        std::cout << "Ingrese el nombre del atributo: ";
        std::getline(std::cin, nombreAtributo);
        std::cout << "Ingrese el tipo de dato: ";
        std::getline(std::cin, tipoDato);

        Tuplas += "#" + nombreAtributo + "#" + tipoDato + "#";

        std::cout << "Desea agregar otro atributo? (si/no): ";
        std::getline(std::cin, respuesta);

    } while (respuesta == "si");

    std::ofstream archivo("../Esquemas.txt", std::ios::out);
    if (archivo.is_open())
    {
        archivo << Tuplas << std::endl;
        std::cout << "Esquema guardado correctamente." << std::endl;
    }
    else
    {
        std::cerr << "Error al abrir el archivo." << std::endl;
    }
}

void Megatron::Tablas()
{
    Tuplas.clear();
    std::string nombreTabla;
    std::cout << "Ingrese el nombre para la Tabla: ";
    getline(std::cin, nombreTabla);
    std::string Tabla = "../" + nombreTabla + ".txt";

    int contador = 0;
    size_t pos = 0;
    std::ifstream esquemas("../Esquemas.txt");
    std::string linea;
    std::getline(esquemas, linea);
    while ((pos = linea.find('#', pos)) != std::string::npos)
    {
        ++contador;
        ++pos;
    }
    contador = (contador - 1) / 2;

    for (int i = 0; i < contador; i++)
    {
        std::string nombreAtributo;
        std::cout << "Ingrese el Registro: ";
        getline(std::cin, nombreAtributo);

        if (i == contador - 1)
        {
            Tuplas += nombreAtributo;
        }
        else
        {
            Tuplas += nombreAtributo + "#";
        }
    }

    std::ofstream archivo(Tabla, std::ios::app);
    archivo << Tuplas << std::endl;
    std::cout << "Registro guardado correctamente." << std::endl;
}

void Megatron::Cargar()
{
    Tuplas.clear();
    std::string nArchivo, nEsquema;
    std::cout << "Ingrese el nombre del archivo a cargar: ";
    getline(std::cin, nArchivo);

    std::string arch = "../" + nArchivo + ".csv";
    std::ifstream archivo(arch);

    std::cout << "Ingrese nombre a su Esquema: ";
    getline(std::cin, nEsquema);

    Tuplas += nEsquema + "#";

    std::string esq, atributo, tipo, max;
    getline(archivo, esq);

    for (size_t i = 0; i <= esq.length() + 1; i++)
    {
        if (esq[i] == ',' || i == esq.length())
        {
            std::cout << "Tipo de Dato y bytes para " << atributo << ":";
            std::cin >> tipo >> max;
            Tuplas += atributo + '#' + tipo + '#' + max + '#';
            atributo.clear();
        }
        if (esq[i] != ',')
        {
            atributo += esq[i];
        }
    }
    std::ofstream Esquem("../Esquemas.txt", std::ios::app);
    Esquem << Tuplas << std::endl;
    Esquem.close();

    std::ofstream txt("../" + nEsquema + ".txt");
    if (archivo.is_open())
    {
        std::string linea;

        while (getline(archivo, linea))
        {
            txt << Corregir(linea) << std::endl;
        }
        archivo.close();
        txt.close();
    }
}

std::string Megatron::Corregir(std::string linea)
{
    int contador = 0;
    size_t pos = 0;
    std::string res, esq;
    std::ifstream esquemas("../Esquemas.txt");
    std::getline(esquemas, esq);
    while ((pos = esq.find('#', pos)) != std::string::npos)
    {
        ++contador;
        ++pos;
    }
    contador = (contador - 1) / 3;
    int mult = 1;
    bool str = false;
    std::string segmento;
    for (size_t i = 0; i < linea.length(); i++)
    {
        if (linea[i] == '"')
        {
            str = !str;
        }
        else if (linea[i] == ',' && !str)
        {

            res += Llenar(segmento, Max(esq, mult));
            segmento.clear();
            mult++;
        }
        else
        {
            segmento += linea[i];
        }
    }
    res += Llenar(segmento, Max(esq, mult));
    return res;
}

std::string Megatron::Llenar(std::string linea, int cant)
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

std::string Megatron::Vaciar(std::string linea, int *max)
{
    std::string resultado = linea;
    int offset = 0;
    for (int i = 0; i < 12; ++i)
    {
        int pos = max[i];
        if (pos + offset < resultado.size())
        {
            resultado.insert(pos + offset, ",");
            offset++;
            offset += max[i];
        }
    }
    resultado.erase(std::remove_if(resultado.begin(), resultado.end(), [](unsigned char c) { return std::isspace(c); }), resultado.end());
    
    return resultado;
}

std::string Megatron::get_Esquema()
{
    std::string res;
    std::ifstream esquemas("../Esquemas.txt");
    std::getline(esquemas, res);
    return res;
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
    for (size_t i = 0; i < contador; i++)
    {
        numero_str[i] = Max(esq, i + 1);
    }

    return numero_str;
}

void Megatron::Select_()
{
    std::string nEsquema, linea, esq = get_Esquema();
    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    getline(std::cin, nEsquema);
    std::ifstream archivo("../" + nEsquema + ".txt");
    // std::ofstream select("../Select" + nEsquema + ".txt");

    while (getline(archivo, linea))
    {
        std::cout << Vaciar(linea, Max(esq)) << std::endl;
    }
    archivo.close();
    // select.close();
}

void Megatron::Select_(std::string sign)
{
    std::string nEsquema;
    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    getline(std::cin, nEsquema);
    std::ifstream archivo("../" + nEsquema + ".txt");
    std::ofstream select("../Select" + nEsquema + ".txt");
    std::string linea;
    while (getline(archivo, linea))
    {
        select << linea << std::endl;
    }
    archivo.close();
    select.close();
    sign = "#";
}

void Menu()
{
    std::cout << "1. Crear Esquema.\n2. Crear Registro.\n3. Cargar Archivo.\n4. Select *.\n5.Salir.\nIngrese la opcion a realizar:";
}

int main()
{
    Megatron DB;
    DB.Select_();

    /*int opc;
    while (opc != 5)
    {
        Menu();
        std::cin >> opc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (opc)
        {
        case 1:
            DB.Esquemas();
            break;
        case 2:
            DB.Tablas();
            break;
        case 3:
            DB.Cargar();
            break;
        case 4:
            DB.Select_();
            break;
        case 5:
            opc = 5;
            break;
        default:
            std::cout << "Seleccione una opcion: ";
            std::cin >> opc;
            break;
        }
    }*/

    return 0;
}
