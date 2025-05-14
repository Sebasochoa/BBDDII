#ifndef RECORDS_MANAGER_HELPERS_H
#define RECORDS_MANAGER_HELPERS_H

#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace RecordsManagerHelpers {

/**
 * @brief Elimina espacios en blanco de una cadena
 * @param str Cadena a procesar
 * @return Cadena sin espacios
 */
inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Obtiene el contenido entre los numerales en una cadena
 * @param linea Cadena de entrada
 * @param numero Número del segmento a obtener (1-based)
 * @return Segmento entre los numerales especificados
 */
inline std::string obtenerEntreHash(const std::string& linea, int numero) {
    size_t pos = 0;
    int contador = 0;
    
    while (contador < numero && pos != std::string::npos) {
        pos = linea.find('#', pos);
        if (pos != std::string::npos) {
            contador++;
            pos++;
        }
    }
    
    if (contador != numero) return "";
    
    size_t end = linea.find('#', pos);
    return trim(linea.substr(pos, end - pos));
}

/**
 * @brief Imprime los nombres de los atributos formateados
 * @param entrada Cadena con el esquema de atributos
 */
inline void Print_Name_Atribute(const std::string& entrada) {
    size_t pos = 0;
    int campo = 0;
    
    while (pos < entrada.length()) {
        size_t siguiente = entrada.find('#', pos);
        std::string valor = entrada.substr(pos, siguiente - pos);

        if ((campo - 1) % 3 == 0 && campo > 0) {
            int ancho = 0;
            if ((campo + 2) % 3 == 0) {
                size_t next = entrada.find('#', siguiente + 1);
                ancho = std::stoi(entrada.substr(siguiente + 1, next - siguiente - 1));
            }
            std::cout << valor << std::string(std::max(0, ancho - (int)valor.length()), ' ') << " ";
        }

        pos = (siguiente == std::string::npos) ? entrada.length() : siguiente + 1;
        campo++;
    }
    std::cout << std::endl;
}

/**
 * @brief Imprime datos formateados según el esquema
 * @param esquema Definición del esquema
 * @param registro Cadena con los datos del registro
 */
inline void imprimirDatosFormateados(const std::string& esquema, const std::string& registro) {
    // Encontrar inicio de datos (después del 4to '#')
    size_t inicioDatos = 0;
    int numHash = 0;
    for (size_t i = 0; i < registro.length(); ++i) {
        if (registro[i] == '#') {
            if (++numHash == 4) {
                inicioDatos = i + 1;
                break;
            }
        }
    }

    std::string datos = registro.substr(inicioDatos);
    size_t cursor = 0;

    // Procesar cada atributo según el esquema
    size_t p = 0;
    int campoEsquema = 0;
    while (p < esquema.length()) {
        size_t siguiente = esquema.find('#', p);
        std::string valor = esquema.substr(p, siguiente - p);

        if ((campoEsquema - 1) % 3 == 0 && campoEsquema > 0) {
            // Es un nombre de atributo, obtener su ancho
            size_t next = esquema.find('#', siguiente + 1);
            next = esquema.find('#', next + 1);
            int ancho = std::stoi(esquema.substr(next + 1, esquema.find('#', next + 1) - next - 1));

            std::string valorDato = datos.substr(cursor, ancho);
            cursor += ancho;

            // Eliminar espacios finales
            size_t end = valorDato.find_last_not_of(' ');
            valorDato = (end == std::string::npos) ? "" : valorDato.substr(0, end + 1);

            // Ajustar formato
            int faltan = ancho - valorDato.length();
            if (faltan > 0) std::cout << std::string(faltan, ' ');
            std::cout << valorDato;
        }

        p = (siguiente == std::string::npos) ? esquema.length() : siguiente + 1;
        campoEsquema++;
    }

    std::cout << datos.substr(cursor) << std::endl;
}

} 

#endif // RECORDS_MANAGER_HELPERS_H