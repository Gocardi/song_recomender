#ifndef DATALOADER_HPP
#define DATALOADER_HPP

#include <unordered_map>
#include <vector>
#include <string>

// Definiciones de tipos
using UserRatings = std::unordered_map<int, std::unordered_map<int, float>>;
using SongRatings = std::unordered_map<int, std::vector<std::pair<int, float>>>;

class DataLoader {
public:
    /**
     * Carga los datos desde un archivo CSV
     * @param filename Ruta del archivo CSV
     * @param userRatings Estructura donde se almacenan las puntuaciones por usuario
     * @param songRatings Estructura donde se almacenan las puntuaciones por canción
     * @return true si la carga fue exitosa, false en caso contrario
     */
    static bool loadData(const std::string& filename, 
                        UserRatings& userRatings, 
                        SongRatings& songRatings);

private:
    /**
     * Parsea una línea del CSV
     * @param line Línea a parsear
     * @param userId ID del usuario (salida)
     * @param songId ID de la canción (salida)
     * @param rating Puntuación (salida)
     * @return true si el parseo fue exitoso
     */
    static bool parseLine(const std::string& line, int& userId, int& songId, float& rating);
    
    /**
     * Elimina espacios en blanco al inicio y final de una cadena
     * @param str Cadena a limpiar
     * @return Cadena sin espacios en blanco
     */
    static std::string trim(const std::string& str);
};

#endif // DATALOADER_HPP