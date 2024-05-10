#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

class Megatron
{
private:
    std::string Tuplas;

public:
    Megatron();
    ~Megatron();
    bool Existe(const char *);
    void Cargar(std::string);
    void Cargar1(std::string);
    void Cargarn(std::string);
    void Select_1(std::string);
    void Select_(std::string);
    void Select_(std::string, std::string, std::string, int, std::string);
    void Select_I(std::string);
    std::string Corregir(std::string,std::string, int, std::string);
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