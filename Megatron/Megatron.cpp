#include <iostream>
#include <fstream>
#include <cstring>
#include <limits> // Para std::numeric_limits

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
    std::string respuesta,nombreAtributo,tipoDato;
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
    std::string esq;
    getline(archivo, esq);
    for (int i = 0; i < esq.length(); i++)
    {
        if (esq[i] != ',')
        {
            Tuplas += esq[i];
        }
        else
        {
            Tuplas += "#str#";
        }
    }
    Tuplas += "#str";

    std::ofstream Esquem("../Esquemas.txt", std::ios::app);
    Esquem << Tuplas << std::endl;
    std::ofstream txt("../" + nEsquema + ".txt");
    if (archivo.is_open())
    {
        std::string linea;

        while (getline(archivo, linea))
        {
            for (size_t i = 0; i < linea.length(); i++)
            {
                if (linea[i] == ',')
                {
                    linea.replace(i, 1, "#");
                }
            }

            txt << linea << std::endl;
        }
        archivo.close();
        txt.close();
    }
}

void Megatron::Select_()
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
}

void Menu()
{
    std::cout << "1. Crear Esquema.\n2. Crear Registro.\n3. Cargar Archivo.\n4. Select *.\n5.Salir.\nIngrese la opcion a realizar:";
}

int main()
{
    Megatron DB;
    int opc;
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
    }

    return 0;
}
