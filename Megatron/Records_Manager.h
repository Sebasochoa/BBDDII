#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

class Records_Manager
{
private:
public:
    Records_Manager();
    ~Records_Manager();

    void Select_1(std::string);
    void Select_all(std::string);
    void Select_(std::string, std::string, std::string, int, std::string);
    void Select_I(std::string);
    std::string Corregir(std::string, std::string, int, std::string,int);
    int Max(std::string, int);
    std::string LlenarI(std::string, int);
    std::string get_Esquema(std::string);
    int get_NumAtributos(std::string);
    std::string get_NomAtributos(std::string);

    // Revisados
    bool Int(std::string);
    bool Float(std::string);
    bool Check_Schemes(std::string);
    void Create_Scheme(std::string);
    int *Info_Disk(std::string);
    int RemainCapacity(std::string);
    std::string Cargar(std::string);
    void Cargar1(std::string);
    void Cargarn(std::string);
    void First_Line(std::string, std::string);
    int NumRegistros(std::string);
    std::string Erase_Blanks(std::string);
    bool IsRecord_inTable(std::string, std::string);
    std::string *split(std::string &, char, int &);
    int *findLongestEntryLengths(std::string &, int &);
};