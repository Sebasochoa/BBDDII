#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include "Disco.h"

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
    void Cargar(int, std::string);
    void Select_();
    void Select_(std::string, std::string, std::string, int);
    void Select_I();
    std::string Corregir(std::string);
    int Max(std::string, int);
    int *Max(std::string);
    std::string LlenarI(std::string, int);
    std::string LlenarD(std::string, int);
    std::string Vaciar(std::string, int *, std::string);
    std::string get_Esquema(std::string);
    int get_NumAtributos(std::string);
    std::string get_NomAtributos(std::string);
    int Encabezados(std::string, int);
    bool Entero(const std::string &, const std::string &, const std::string &);
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

    int contador = get_NumAtributos(nombreTabla);

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

int Megatron::get_NumAtributos(std::string esquema)
{
    std::string linea;
    std::ifstream archivo("../Esquemas.txt");
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

void Megatron::Cargar(int capacity, std::string name)
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

    std::string esq, atributo, tipe, tipo;
    getline(archivo, esq);
    getline(archivo, tipe);
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

    std::ofstream Esquem("../Esquemas.txt", std::ios::app);
    Esquem << Tuplas << std::endl;
    Esquem.close();

    std::ofstream txt("../" + nEsquema + ".txt");
    if (archivo.is_open())
    {
        std::string linea = tipe;
        txt << Corregir(linea) << std::endl;
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
            res += LlenarI(segmento, Max(esq, mult));
            segmento.clear();
            mult++;
        }
        else
        {
            segmento += linea[i];
        }
    }
    res += LlenarI(segmento, Max(esq, mult));
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
    std::ifstream archivo("../Esquemas.txt");

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

void Megatron::Select_()
{
    std::string nEsquema, linea, segmento;

    int mult = 1;
    std::cout << "Ingrese el nombre de la tabla a seleccionar: ";
    getline(std::cin, nEsquema);
    std::ifstream archivo("../" + nEsquema + ".txt");
    std::ofstream select("../Select" + nEsquema + ".txt");
    std::string nomatributos = get_NomAtributos(get_Esquema(nEsquema)), esq = get_Esquema(nEsquema);
    for (size_t i = 0; i < nomatributos.length(); i++)
    {
        if (nomatributos[i] == ',')
        {
            int enca = Encabezados(esq, mult), maxi = Max(esq, mult);
            if (enca < maxi)
            {
                select << LlenarD(segmento, maxi + 1);
            }
            else
            {
                select << LlenarD(segmento, enca + 1);
            }
            segmento.clear();
            mult++;
        }
        else
        {
            segmento += nomatributos[i];
        }
    }
    select << std::endl;

    while (getline(archivo, linea))
    {
        select << Vaciar(linea, Max(esq), nEsquema) << std::endl;
    }
    archivo.close();
    select.close();
}

void Megatron::Select_(std::string nEsquema, std::string atributo, std::string signo, int valor)
{
    std::string nomatributos, token, linea, compara_str;
    std::ifstream archivo("../" + nEsquema + ".txt");
    std::ofstream select("../Select" + nEsquema + ".txt");
    int contador = 0, sumatoria = 0, comparacion = 0;
    nomatributos = get_NomAtributos(get_Esquema(nEsquema));
    std::istringstream ss1(nomatributos);

    while (std::getline(ss1, token, ','))
    {
        contador++;
        if (token == atributo)
        {
            break;
        }
    }
    int *suma = Max(get_Esquema(nEsquema));

    int longitud = get_NumAtributos(get_Esquema(nEsquema));

    for (int i = 0; i < longitud; i++)
    {
        if (i < contador - 1)
        {
            sumatoria += suma[i];
        }
    }
    int maxi = Max(get_Esquema(nEsquema), contador);

    while (getline(archivo, linea))
    {
        for (int i = 0; i < maxi; i++)
        {
            if (linea[sumatoria + i] != ' ')
            {
                compara_str += linea[sumatoria + i];
            }
        }
        if (!compara_str.empty())
        {
            comparacion = std::stoi(compara_str);
            compara_str.clear();
            if (signo == "<")
            {
                if (comparacion < valor)
                {
                    select << linea << std::endl;
                }
            }
            else if (signo == ">")
            {
                if (comparacion > valor)
                {
                    select << linea << std::endl;
                }
            }
            else if (signo == "<=")
            {
                if (comparacion <= valor)
                {
                    select << linea << std::endl;
                }
            }
            else if (signo == ">=")
            {
                if (comparacion >= valor)
                {
                    select << linea << std::endl;
                }
            }
        }
    }

    archivo.close();
    select.close();
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

void Megatron::Select_I()
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

    Select_(nEsquema, atributo, signo, valor);
}

void Menu()
{
    std::cout << "1. Crear Esquema.\n2. Crear Registro.\n3. Cargar Archivo.\n4. Select *.\n5.Salir.\nIngrese la opcion a realizar:";
}

int main()
{
    Disco D("Hola1");
    D.FullCapacity();
    // g++ -o main Megatron.cpp Disco.cpp
    //Megatron DB;
    //DB.Cargar();

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
