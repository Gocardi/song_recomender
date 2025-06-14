#ifndef TOPSONGS_HPP
#define TOPSONGS_HPP

#include <vector>
#include <unordered_map>

using SongRatings = std::unordered_map<int, std::vector<std::pair<int, float>>>;

struct SongInfo {
    int songId;
    float averageRating;
    int ratingCount;
    
    SongInfo(int id, float avg, int count) 
        : songId(id), averageRating(avg), ratingCount(count) {}
};

class TopSongs {
public:
    /**
     * Obtiene las N canciones con mejor promedio de puntuación
     * @param songRatings Datos de puntuaciones por canción
     * @param n Número de canciones a retornar
     * @param minRatings Mínimo número de puntuaciones para considerar una canción
     * @return Vector de SongInfo ordenado por promedio descendente
     */
    static std::vector<SongInfo> getTopSongs(const SongRatings& songRatings, 
                                           int n, 
                                           int minRatings = 1);

    /**
     * Muestra las top canciones en formato tabla
     * @param topSongs Vector de canciones ordenadas
     */
    static void displayTopSongs(const std::vector<SongInfo>& topSongs);

private:
    /**
     * Calcula el promedio de puntuaciones para una canción
     * @param ratings Vector de puntuaciones de la canción
     * @return Promedio de las puntuaciones
     */
    static float calculateAverage(const std::vector<std::pair<int, float>>& ratings);
};

#endif // TOPSONGS_HPP