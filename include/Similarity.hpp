#ifndef SIMILARITY_HPP
#define SIMILARITY_HPP

#include <unordered_map>
#include <vector>

using UserRatings = std::unordered_map<int, std::unordered_map<int, float>>;
using UserSimilarity = std::unordered_map<int, std::vector<std::pair<int, float>>>;

struct SimilarUser {
    int userId;
    float similarity;
    int commonSongs;
    
    SimilarUser(int id, float sim, int common) 
        : userId(id), similarity(sim), commonSongs(common) {}
};

class Similarity {
public:
    /**
     * Encuentra los usuarios más similares a un usuario dado
     * @param targetUserId ID del usuario objetivo
     * @param userRatings Datos de puntuaciones por usuario
     * @param topK Número de usuarios similares a retornar (default: 6)
     * @param minCommonSongs Mínimo número de canciones en común (default: 2)
     * @return Vector de usuarios similares ordenados por similitud
     */
    static std::vector<SimilarUser> findSimilarUsers(int targetUserId,
                                                   const UserRatings& userRatings,
                                                   int topK = 6,
                                                   int minCommonSongs = 2);

    /**
     * Calcula la similitud de coseno entre dos usuarios
     * @param user1Ratings Puntuaciones del usuario 1
     * @param user2Ratings Puntuaciones del usuario 2
     * @param commonSongs Número de canciones en común (salida)
     * @return Similitud de coseno [0, 1]
     */
    static float calculateCosineSimilarity(const std::unordered_map<int, float>& user1Ratings,
                                         const std::unordered_map<int, float>& user2Ratings,
                                         int& commonSongs);

    /**
     * Muestra los usuarios similares en formato tabla
     * @param targetUserId ID del usuario objetivo
     * @param similarUsers Vector de usuarios similares
     */
    static void displaySimilarUsers(int targetUserId, 
                                  const std::vector<SimilarUser>& similarUsers);

    /**
     * Calcula matriz de similitud para todos los usuarios (para análisis)
     * @param userRatings Datos de puntuaciones por usuario
     * @return Mapa de similitudes entre usuarios
     */
    static UserSimilarity calculateAllSimilarities(const UserRatings& userRatings);

private:
    /**
     * Obtiene las canciones en común entre dos usuarios
     * @param user1Ratings Puntuaciones del usuario 1
     * @param user2Ratings Puntuaciones del usuario 2
     * @return Vector de IDs de canciones en común
     */
    static std::vector<int> getCommonSongs(const std::unordered_map<int, float>& user1Ratings,
                                         const std::unordered_map<int, float>& user2Ratings);
};

#endif // SIMILARITY_HPP