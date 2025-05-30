#ifndef RECORDS_MANAGER_H
#define RECORDS_MANAGER_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "Records_Manager_Helpers.h"

class Records_Manager
{
public:
    Records_Manager();
    ~Records_Manager();

    // Operaciones SELECT
    void Select_all(const std::string& NDisco);
    void Select_(const std::string& nEsquema, const std::string& atributo, const std::string& signo, int valor, const std::string& NDisco);
    void SelectArchivo(const std::string& nEsquema, const std::string& atributo, const std::string& signo, int valor, const std::string& NDisco);

    // Manejo de registros
    std::string EscapeCampoVariable(const std::string& campo);
    std::string GenerarMetadatos(const std::string& linea);
    std::string REstatico(const std::string& linea, const std::string& NTabla, int contador, const std::string& dir, int num_Registros,const std::string& NDisco);
    std::string RVariable(const std::string& linea, const std::string& NTabla, int contador, const std::string& dir, int num_Registros,const std::string& NDisco);
    std::string LlenarI(const std::string& linea, int cant);

    // Manejo de esquemas
    std::string get_Esquema(const std::string& tabla,const std::string &Name_Disk);
    int get_NumAtributos(const std::string& esquema, const std::string &Name_Disk);
    std::string get_NomAtributos(const std::string& tabla);
    int Max(const std::string& esq, int mult);

    // Funciones de utilidad
    bool Int(const std::string& texto);
    bool Float(const std::string& texto);
    std::string Erase_Blanks(const std::string& cadena);
    bool IsRecord_inTable(const std::string& linea, const std::string& NTabla);

    // Manejo de archivos y disco
    bool Check_Schemes(const std::string& Name_Scheme,const std::string& Name_Disk);
    void Create_Scheme(const std::string& Name_Scheme,const std::string& Name_Disk, bool formato);
    std::array<int, 4> Info_Disk(const std::string& Name_Disk);
    int RemainCapacity(const std::string& Archivo);
    std::string Cargar(const std::string &Name_Disk, const bool &formato);
    std::string Cargar(const std::string &Name_Disk, const bool &formato, const std::string &Name_File);
    void First_Line(const std::string& Directory_File, const std::string& Replace_Line);
    int NumRegistros(const std::string& DirArchivo);

    // Manejo de strings
    std::string* split(std::string& s, char delimiter, int& numTokens);
    int* findLongestEntryLengths(const std::string& filename, int& numAttributes);

    bool ReemplazarRegistro(const std::string& , const std::string& );
    std::string GetTipoAtributo(const std::string& esquema, int atributo_index);

private:
    // (Se pueden agregar miembros privados si es necesario)
};

#endif // RECORDS_MANAGER_H
