#include "../include/Similarity.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>

std::vector<SimilarUser> Similarity::findSimilarUsers(int targetUserId,
                                                     const UserRatings& userRatings,
                                                     int topK,
                                                     int minCommonSongs) {
    std::vector<SimilarUser> similarUsers;
    
    // Verificar que el usuario objetivo existe
    auto targetIt = userRatings.find(targetUserId);
    if (targetIt == userRatings.end()) {
        std::cerr << "Usuario " << targetUserId << " no encontrado." << std::endl;
        return similarUsers;
    }
    
    const auto& targetRatings = targetIt->second;
    
    // Calcular similitud con todos los otros usuarios
    for (const auto& [otherUserId, otherRatings] : userRatings) {
        if (otherUserId == targetUserId) {
            continue; // Saltar el mismo usuario
        }
        
        int commonSongs;
        float similarity = calculateCosineSimilarity(targetRatings, otherRatings, commonSongs);
        
        // Solo considerar usuarios con suficientes canciones en común
        if (commonSongs >= minCommonSongs && similarity > 0.0f) {
            similarUsers.emplace_back(otherUserId, similarity, commonSongs);
        }
    }
    
    // Ordenar por similitud descendente
    std::sort(similarUsers.begin(), similarUsers.end(),
              [](const SimilarUser& a, const SimilarUser& b) {
                  if (std::abs(a.similarity - b.similarity) < 0.001f) {
                      return a.commonSongs > b.commonSongs; // Desempate por canciones en común
                  }
                  return a.similarity > b.similarity;
              });
    
    // Retornar solo los primeros topK usuarios
    if (static_cast<int>(similarUsers.size()) > topK) {
        similarUsers.resize(topK);
    }
    
    return similarUsers;
}

float Similarity::calculateCosineSimilarity(const std::unordered_map<int, float>& user1Ratings,
                                          const std::unordered_map<int, float>& user2Ratings,
                                          int& commonSongs) {
    std::vector<int> commonSongIds = getCommonSongs(user1Ratings, user2Ratings);
    commonSongs = static_cast<int>(commonSongIds.size());
    
    if (commonSongs == 0) {
        return 0.0f;
    }
    
    // Calcular producto punto y magnitudes
    float dotProduct = 0.0f;
    float magnitude1 = 0.0f;
    float magnitude2 = 0.0f;
    
    for (int songId : commonSongIds) {
        float rating1 = user1Ratings.at(songId);
        float rating2 = user2Ratings.at(songId);
        
        dotProduct += rating1 * rating2;
        magnitude1 += rating1 * rating1;
        magnitude2 += rating2 * rating2;
    }
    
    // Evitar división por cero
    if (magnitude1 == 0.0f || magnitude2 == 0.0f) {
        return 0.0f;
    }
    
    // Similitud de coseno
    float similarity = dotProduct / (std::sqrt(magnitude1) * std::sqrt(magnitude2));
    
    // Asegurar que esté en el rango [0, 1]
    return std::max(0.0f, std::min(1.0f, similarity));
}

void Similarity::displaySimilarUsers(int targetUserId, 
                                   const std::vector<SimilarUser>& similarUsers) {
    if (similarUsers.empty()) {
        std::cout << "No se encontraron usuarios similares para el usuario " 
                  << targetUserId << std::endl;
        return;
    }
    
    std::cout << "\n👥 USUARIOS MÁS SIMILARES AL USUARIO " << targetUserId << " 👥\n";
    std::cout << std::string(65, '=') << std::endl;
    std::cout << std::left << std::setw(8) << "Pos" 
              << std::setw(12) << "Usuario ID" 
              << std::setw(15) << "Similitud %" 
              << std::setw(15) << "Canciones"
              << std::setw(15) << "Compatibilidad" << std::endl;
    std::cout << std::string(65, '-') << std::endl;
    
    for (size_t i = 0; i < similarUsers.size(); ++i) {
        const auto& user = similarUsers[i];
        float percentage = user.similarity * 100.0f;
        
        // Indicador visual de compatibilidad
        std::string compatibility;
        if (percentage >= 90.0f) compatibility = "🔥🔥🔥";
        else if (percentage >= 80.0f) compatibility = "🔥🔥";
        else if (percentage >= 70.0f) compatibility = "🔥";
        else if (percentage >= 60.0f) compatibility = "⭐⭐";
        else compatibility = "⭐";
        
        std::cout << std::left << std::setw(8) << (i + 1)
                  << std::setw(12) << user.userId
                  << std::setw(15) << std::fixed << std::setprecision(2) << percentage
                  << std::setw(15) << user.commonSongs
                  << std::setw(15) << compatibility << std::endl;
    }
    std::cout << std::string(65, '=') << std::endl;
}

UserSimilarity Similarity::calculateAllSimilarities(const UserRatings& userRatings) {
    UserSimilarity allSimilarities;
    
    std::cout << "Calculando matriz de similitudes..." << std::endl;
    
    for (const auto& [userId, _] : userRatings) {
        auto similarUsers = findSimilarUsers(userId, userRatings, 10, 1);
        
        for (const auto& simUser : similarUsers) {
            allSimilarities[userId].emplace_back(simUser.userId, simUser.similarity);
        }
    }
    
    std::cout << "Matriz de similitudes calculada." << std::endl;
    return allSimilarities;
}

std::vector<int> Similarity::getCommonSongs(const std::unordered_map<int, float>& user1Ratings,
                                          const std::unordered_map<int, float>& user2Ratings) {
    std::vector<int> commonSongs;
    
    // Iterar sobre el usuario con menos canciones para optimizar
    const auto& smallerMap = (user1Ratings.size() <= user2Ratings.size()) ? user1Ratings : user2Ratings;
    const auto& largerMap = (user1Ratings.size() <= user2Ratings.size()) ? user2Ratings : user1Ratings;
    
    for (const auto& [songId, rating] : smallerMap) {
        if (largerMap.find(songId) != largerMap.end()) {
            commonSongs.push_back(songId);
        }
    }
    
    return commonSongs;
}