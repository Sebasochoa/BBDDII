#include <iostream>
#include <fstream>
#include <cstring>
class Megatron
{
private:
    char Tuplas[100];

public:
    Megatron();
    ~Megatron();
    bool Existe(const char *);
    void Esquemas();
    void Tablas();
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
    std::cout << "Ingrese el nombre para el Esquema: ";
    std::cin.getline(Tuplas, 100);
    strcat(Tuplas, "#");
    char respuesta[10];
    do
    {
        char nombreAtributo[50], tipoDato[50];
        std::cout << "Ingrese el nombre del atributo: ";
        std::cin.getline(nombreAtributo, 50);
        std::cout << "Ingrese el tipo de dato: ";
        std::cin.getline(tipoDato, 50);

        strcat(Tuplas, nombreAtributo);
        strcat(Tuplas, "#");
        strcat(Tuplas, tipoDato);
        strcat(Tuplas, "#");

        std::cout << "Desea agregar otro atributo? (si/no): ";
        std::cin.getline(respuesta, 10);

    } while (strcmp(respuesta, "si") == 0);

    std::ofstream archivo("../Esquemas.txt", std::ios::app);
    archivo << Tuplas << std::endl;
    std::cout << "Esquema guardado correctamente." << std::endl;
}

void Megatron::Tablas()
{
    memset(Tuplas, 0, sizeof(Tuplas));
    char nombreTabla[50], n[50] = "../";
    std::cout << "Ingrese el nombre para la Tabla: ";
    std::cin.getline(nombreTabla, 50);
    strcat(n, nombreTabla);
    strcat(n, ".txt");

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
        char nombreAtributo[50];
        std::cout << "Ingrese el Registro: ";
        std::cin.getline(nombreAtributo, 50);

        if (i == contador - 1)
        {
            strcat(Tuplas, nombreAtributo);
        }
        else
        {
            strcat(Tuplas, nombreAtributo);
            strcat(Tuplas, "#");
        }
    }

    std::ofstream archivo(n, std::ios::app);
    archivo << Tuplas << std::endl;
    std::cout << "Registro guardado correctamente." << std::endl;
}

int main()
{
    Megatron DB;
    DB.Esquemas();
    DB.Tablas();
    return 0;
}
