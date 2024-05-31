#include "Bloques.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

Bloques::Bloques()
{
}

void Bloques::Initialize(int capacity, int numBlocksx, int maxCapacity, std::string nameDisk)
{
    Capacity = capacity * numBlocksx;
    NumBlocks = maxCapacity / (capacity * numBlocksx);
    NameDisk = nameDisk;
    std::string dirDisco = fs::current_path().string() + "/" + nameDisk + "/Bloques";
    fs::create_directory(dirDisco);
    for (size_t i = 0; i < NumBlocks; i++)
    {
        std::string dirBloques = dirDisco + "/Bloque_" + std::to_string(i + 1)+ ".txt";

        std::ofstream archivo(dirBloques);
        if (archivo.is_open())
        {
            archivo << Capacity << std::endl;
        }
    }
}

int Bloques::get_NumBlocks()
{
    return NumBlocks;
}

void Bloques::set_NumBlocks(int numBlocks)
{
    NumBlocks = numBlocks;
}

int Bloques::get_Capacity()
{
    return Capacity;
}

void Bloques::set_Capacity(int capacity)
{
    Capacity = capacity;
}
