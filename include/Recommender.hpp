#ifndef RECOMMENDER_HPP
#define RECOMMENDER_HPP

#include <unordered_map>
#include <vector>
#include <unordered_set>

using UserRatings = std::unordered_map<int, std::unordered_map<int, float>>;
using SongRatings = std::unordered_map<int, std::vector<std::pair<int, float>>>;
using UserRecommendations = std::unordered_map<int, std::vector<int>>;

struct SongRecommendation {
    int songId;
    float predictedRating;
    int recommenderCount;
    float confidence;
    
    SongRecommendation(int id, float rating, int count, float conf) 
        : songId(id), predictedRating(rating), recommenderCount(count), confidence(conf) {}
};

struct SimilarUser {
    int userId;
    float similarity;
    int commonSongs;
    
    SimilarUser(int id, float sim, int common) 
        : userId(id), similarity(sim), commonSongs(common) {}
};

class Recommender {
public:
    /**
     * Genera recomendaciones para un usuario específico
     * @param targetUserId ID del usuario objetivo
     * @param userRatings Datos de puntuaciones por usuario
     * @param songRatings Datos de puntuaciones por canción
     * @param numRecommendations Número de recomendaciones a generar
     * @param minRating Puntuación mínima para considerar una canción
     * @return Vector de recomendaciones ordenadas por puntuación predicha
     */
    static std::vector<SongRecommendation> generateRecommendations(
        int targetUserId,
        const UserRatings& userRatings,
        const SongRatings& songRatings,
        int numRecommendations = 10,
        float minRating = 3.5f);

    /**
     * Genera recomendaciones para todos los usuarios
     * @param userRatings Datos de puntuaciones por usuario
     * @param songRatings Datos de puntuaciones por canción
     * @param numRecommendations Número de recomendaciones por usuario
     * @return Mapa de recomendaciones por usuario
     */
    static UserRecommendations generateAllRecommendations(
        const UserRatings& userRatings,
        const SongRatings& songRatings,
        int numRecommendations = 5);

    /**
     * Muestra las recomendaciones en formato tabla
     * @param targetUserId ID del usuario objetivo
     * @param recommendations Vector de recomendaciones
     */
    static void displayRecommendations(int targetUserId, 
                                     const std::vector<SongRecommendation>& recommendations);

    /**
     * Obtiene estadísticas de recomendaciones
     * @param targetUserId ID del usuario objetivo
     * @param userRatings Datos de puntuaciones por usuario
     * @param recommendations Vector de recomendaciones
     */
    static void displayRecommendationStats(int targetUserId,
                                          const UserRatings& userRatings,
                                          const std::vector<SongRecommendation>& recommendations);

private:
    /**
     * Predice la puntuación de una canción para un usuario
     * @param targetUserId ID del usuario objetivo
     * @param songId ID de la canción
     * @param similarUsers Usuarios similares al objetivo
     * @param userRatings Datos de puntuaciones por usuario
     * @param recommenderCount Número de usuarios que recomiendan (salida)
     * @return Puntuación predicha
     */
    static float predictRating(int targetUserId,
                              int songId,
                              const std::vector<SimilarUser>& similarUsers,
                              const UserRatings& userRatings,
                              int& recommenderCount);

    /**
     * Calcula la confianza de una recomendación
     * @param recommenderCount Número de usuarios que recomiendan
     * @param avgSimilarity Similitud promedio de los recomendadores
     * @return Nivel de confianza [0, 1]
     */
    static float calculateConfidence(int recommenderCount, float avgSimilarity);

    /**
     * Obtiene todas las canciones candidatas para recomendar
     * @param targetUserId ID del usuario objetivo
     * @param similarUsers Usuarios similares
     * @param userRatings Datos de puntuaciones por usuario
     * @param minRating Puntuación mínima para considerar
     * @return Set de IDs de canciones candidatas
     */
    static std::unordered_set<int> getCandidateSongs(
        int targetUserId,
        const std::vector<SimilarUser>& similarUsers,
        const UserRatings& userRatings,
        float minRating);
};

#endif // RECOMMENDER_HPP