# BBDDII

This repository contains a simple database management system written in C++.
The core logic lives under the `Megatron/` directory and `Megatron/main.cpp`
serves as the entry point of the application.

## Building

Compile the project with a C++17 compiler that supports `<filesystem>`:

```bash
g++ -std=c++17 -o sgbd \
    Megatron/main.cpp Megatron/SGBD.cpp \
    Megatron/Disco.cpp Megatron/Bloques.cpp \
    Megatron/BufferManager.cpp
```

## Example data

Some CSV files are included for testing:

- `Megatron/Housing.csv`
- `Megatron/Titanic.csv`
- `Megatron/Movie_Ratings.csv`
- `Megatron/titanicG.csv`

Use menu option **2** to load these files into a created disk.

## Menu options

Running the compiled program displays the following options:

```
1. Crear Disco Personalizado.
2. Cargar Archivo.
3. Select *
4. Select <
5. Mostrar Estado del Disco.
6. Mostrar Sectores Ocupados.
7. Mostrar Bloques Ocupados.
8. Salir.
```

## Directory layout

When a disk is created, a directory called `Discos/` is generated. Inside it you
will find a subdirectory with the disk name, for example `Discos/MyDisk/`, which
contains structures like `Plato_1/Superficie_1/Pista_1/Sector_1/` and so on.
Another folder `Discos/Bloques_<disk>/` stores block files `Bloque_#.txt`.

## Requirements

- A C++17 compliant compiler with `<filesystem>` support.
