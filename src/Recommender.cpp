#include "../include/Recommender.hpp"
#include "../include/Similarity.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>

std::vector<SongRecommendation> Recommender::generateRecommendations(
    int targetUserId,
    const UserRatings& userRatings,
    const SongRatings& songRatings,
    int numRecommendations,
    float minRating) {
    
    std::vector<SongRecommendation> recommendations;
    
    // Verificar que el usuario existe
    auto targetIt = userRatings.find(targetUserId);
    if (targetIt == userRatings.end()) {
        std::cerr << "Usuario " << targetUserId << " no encontrado." << std::endl;
        return recommendations;
    }
    
    // Encontrar usuarios similares
    std::vector<SimilarUser> similarUsers = Similarity::findSimilarUsers(
        targetUserId,
        userRatings,
        10, // Top 10 usuarios similares
        2   // Mínimo 2 canciones en común
    );
    
    if (similarUsers.empty()) {
        std::cout << "No se encontraron usuarios similares para generar recomendaciones." << std::endl;
        return recommendations;
    }
    
    // Obtener canciones candidatas
    std::unordered_set<int> candidateSongs = getCandidateSongs(
        targetUserId, similarUsers, userRatings, minRating
    );
    
    // Predecir puntuaciones para las canciones candidatas
    for (int songId : candidateSongs) {
        int recommenderCount;
        float predictedRating = predictRating(
            targetUserId, songId, similarUsers, userRatings, recommenderCount
        );
        
        if (predictedRating >= minRating && recommenderCount > 0) {
            float avgSimilarity = 0.0f;
            int validRecommenders = 0;
            
            // Calcular similitud promedio de los recomendadores
            for (const auto& simUser : similarUsers) {
                auto userIt = userRatings.find(simUser.userId);
                if (userIt != userRatings.end() && 
                    userIt->second.find(songId) != userIt->second.end()) {
                    avgSimilarity += simUser.similarity;
                    validRecommenders++;
                }
            }
            
            if (validRecommenders > 0) {
                avgSimilarity /= validRecommenders;
                float confidence = calculateConfidence(recommenderCount, avgSimilarity);
                
                recommendations.emplace_back(songId, predictedRating, recommenderCount, confidence);
            }
        }
    }
    
    // Ordenar por puntuación predicha y confianza
    std::sort(recommendations.begin(), recommendations.end(),
              [](const SongRecommendation& a, const SongRecommendation& b) {
                  if (std::abs(a.predictedRating - b.predictedRating) < 0.01f) {
                      return a.confidence > b.confidence;
                  }
                  return a.predictedRating > b.predictedRating;
              });
    
    // Limitar al número solicitado
    if (static_cast<int>(recommendations.size()) > numRecommendations) {
        recommendations.resize(numRecommendations);
    }
    
    return recommendations;
}

UserRecommendations Recommender::generateAllRecommendations(
    const UserRatings& userRatings,
    const SongRatings& songRatings,
    int numRecommendations) {
    
    UserRecommendations allRecommendations;
    
    std::cout << "Generando recomendaciones para todos los usuarios..." << std::endl;
    
    int processedUsers = 0;
    for (const auto& [userId, _] : userRatings) {
        auto recommendations = generateRecommendations(
            userId, userRatings, songRatings, numRecommendations, 3.0f
        );
        
        std::vector<int> songIds;
        for (const auto& rec : recommendations) {
            songIds.push_back(rec.songId);
        }
        
        allRecommendations[userId] = songIds;
        
        processedUsers++;
        if (processedUsers % 100 == 0) {
            std::cout << "Procesados " << processedUsers << " usuarios..." << std::endl;
        }
    }
    
    std::cout << "Recomendaciones generadas para " << processedUsers << " usuarios." << std::endl;
    return allRecommendations;
}

void Recommender::displayRecommendations(int targetUserId, 
                                        const std::vector<SongRecommendation>& recommendations) {
    if (recommendations.empty()) {
        std::cout << "No hay recomendaciones disponibles para el usuario " 
                  << targetUserId << std::endl;
        return;
    }
    
    std::cout << "\n🎵 RECOMENDACIONES PARA EL USUARIO " << targetUserId << " 🎵\n";
    std::cout << std::string(75, '=') << std::endl;
    std::cout << std::left << std::setw(8) << "Pos" 
              << std::setw(12) << "Canción ID" 
              << std::setw(15) << "Puntuación" 
              << std::setw(12) << "Confianza"
              << std::setw(12) << "Votos"
              << std::setw(16) << "Recomendación" << std::endl;
    std::cout << std::string(75, '-') << std::endl;
    
    for (size_t i = 0; i < recommendations.size(); ++i) {
        const auto& rec = recommendations[i];
        
        // Indicador visual de calidad de recomendación
        std::string quality;
        if (rec.confidence >= 0.8f && rec.predictedRating >= 4.5f) quality = "🌟🌟🌟";
        else if (rec.confidence >= 0.6f && rec.predictedRating >= 4.0f) quality = "🌟🌟";
        else if (rec.confidence >= 0.4f && rec.predictedRating >= 3.5f) quality = "🌟";
        else quality = "⭐";
        
        std::cout << std::left << std::setw(8) << (i + 1)
                  << std::setw(12) << rec.songId
                  << std::setw(15) << std::fixed << std::setprecision(2) << rec.predictedRating
                  << std::setw(12) << std::fixed << std::setprecision(1) << (rec.confidence * 100) << "%"
                  << std::setw(12) << rec.recommenderCount
                  << std::setw(16) << quality << std::endl;
    }
    std::cout << std::string(75, '=') << std::endl;
}

void Recommender::displayRecommendationStats(int targetUserId,
                                            const UserRatings& userRatings,
                                            const std::vector<SongRecommendation>& recommendations) {
    if (recommendations.empty()) return;
    
    auto userIt = userRatings.find(targetUserId);
    if (userIt == userRatings.end()) return;
    
    int totalSongsRated = userIt->second.size();
    float avgUserRating = 0.0f;
    for (const auto& [songId, rating] : userIt->second) {
        avgUserRating += rating;
    }
    avgUserRating /= totalSongsRated;
    
    float avgPredictedRating = 0.0f;
    float avgConfidence = 0.0f;
    int totalRecommenders = 0;
    
    for (const auto& rec : recommendations) {
        avgPredictedRating += rec.predictedRating;
        avgConfidence += rec.confidence;
        totalRecommenders += rec.recommenderCount;
    }
    
    avgPredictedRating /= recommendations.size();
    avgConfidence /= recommendations.size();
    
    std::cout << "\n📊 ESTADÍSTICAS DE RECOMENDACIÓN\n";
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Usuario: " << targetUserId << std::endl;
    std::cout << "Canciones ya puntuadas: " << totalSongsRated << std::endl;
    std::cout << "Puntuación promedio del usuario: " << std::fixed << std::setprecision(2) << avgUserRating << std::endl;
    std::cout << "Recomendaciones generadas: " << recommendations.size() << std::endl;
    std::cout << "Puntuación predicha promedio: " << std::fixed << std::setprecision(2) << avgPredictedRating << std::endl;
    std::cout << "Confianza promedio: " << std::fixed << std::setprecision(1) << (avgConfidence * 100) << "%" << std::endl;
    std::cout << "Total de votos de recomendación: " << totalRecommenders << std::endl;
    std::cout << std::string(40, '=') << std::endl;
}

float Recommender::predictRating(int targetUserId,
                                int songId,
                                const std::vector<SimilarUser>& similarUsers,
                                const UserRatings& userRatings,
                                int& recommenderCount) {
    float weightedSum = 0.0f;
    float weightSum = 0.0f;
    recommenderCount = 0;
    
    for (const auto& simUser : similarUsers) {
        auto userIt = userRatings.find(simUser.userId);
        if (userIt != userRatings.end()) {
            auto songIt = userIt->second.find(songId);
            if (songIt != userIt->second.end()) {
                float rating = songIt->second;
                float weight = simUser.similarity;
                
                weightedSum += rating * weight;
                weightSum += weight;
                recommenderCount++;
            }
        }
    }
    
    if (weightSum == 0.0f) {
        return 0.0f;
    }
    
    return weightedSum / weightSum;
}

float Recommender::calculateConfidence(int recommenderCount, float avgSimilarity) {
    // Confianza basada en número de recomendadores y similitud promedio
    float countFactor = std::min(1.0f, recommenderCount / 5.0f); // Máximo con 5 recomendadores
    float similarityFactor = avgSimilarity;
    
    return (countFactor + similarityFactor) / 2.0f;
}

std::unordered_set<int> Recommender::getCandidateSongs(
    int targetUserId,
    const std::vector<SimilarUser>& similarUsers,
    const UserRatings& userRatings,
    float minRating) {
    
    std::unordered_set<int> candidateSongs;
    
    // Obtener canciones ya puntuadas por el usuario objetivo
    std::unordered_set<int> userSongs;
    auto targetIt = userRatings.find(targetUserId);
    if (targetIt != userRatings.end()) {
        for (const auto& [songId, rating] : targetIt->second) {
            userSongs.insert(songId);
        }
    }
    
    // Recopilar canciones de usuarios similares con buenas puntuaciones
    for (const auto& simUser : similarUsers) {
        auto userIt = userRatings.find(simUser.userId);
        if (userIt != userRatings.end()) {
            for (const auto& [songId, rating] : userIt->second) {
                // Solo considerar canciones con buena puntuación que el usuario no haya puntuado
                if (rating >= minRating && userSongs.find(songId) == userSongs.end()) {
                    candidateSongs.insert(songId);
                }
            }
        }
    }
    
    return candidateSongs;
}