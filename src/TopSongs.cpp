#include "../include/TopSongs.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>

std::vector<SongInfo> TopSongs::getTopSongs(const SongRatings& songRatings, 
                                           int n, 
                                           int minRatings) {
    std::vector<SongInfo> allSongs;
    
    // Calcular promedio para cada canción
    for (const auto& [songId, ratings] : songRatings) {
        if (static_cast<int>(ratings.size()) >= minRatings) {
            float avgRating = calculateAverage(ratings);
            allSongs.emplace_back(songId, avgRating, static_cast<int>(ratings.size()));
        }
    }
    
    // Ordenar por promedio descendente, y en caso de empate por número de ratings
    std::sort(allSongs.begin(), allSongs.end(), 
              [](const SongInfo& a, const SongInfo& b) {
                  if (std::abs(a.averageRating - b.averageRating) < 0.001f) {
                      return a.ratingCount > b.ratingCount;
                  }
                  return a.averageRating > b.averageRating;
              });
    
    // Retornar solo los primeros n elementos
    if (static_cast<int>(allSongs.size()) > n) {
        allSongs.resize(n);
    }
    
    return allSongs;
}

void TopSongs::displayTopSongs(const std::vector<SongInfo>& topSongs) {
    if (topSongs.empty()) {
        std::cout << "No hay canciones para mostrar." << std::endl;
        return;
    }
    
    std::cout << "\n🎵 TOP " << topSongs.size() << " CANCIONES MÁS PREFERIDAS 🎵\n";
    std::cout << std::string(60, '=') << std::endl;
    std::cout << std::left << std::setw(8) << "Pos" 
              << std::setw(12) << "ID Canción" 
              << std::setw(15) << "Promedio" 
              << std::setw(12) << "# Ratings"
              << std::setw(13) << "Popularidad" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (size_t i = 0; i < topSongs.size(); ++i) {
        const auto& song = topSongs[i];
        
        // Calcular indicador de popularidad
        std::string popularity;
        if (song.ratingCount >= 100) popularity = "🔥🔥🔥";
        else if (song.ratingCount >= 50) popularity = "🔥🔥";
        else if (song.ratingCount >= 20) popularity = "🔥";
        else popularity = "⭐";
        
        std::cout << std::left << std::setw(8) << (i + 1)
                  << std::setw(12) << song.songId
                  << std::setw(15) << std::fixed << std::setprecision(3) << song.averageRating
                  << std::setw(12) << song.ratingCount
                  << std::setw(13) << popularity << std::endl;
    }
    std::cout << std::string(60, '=') << std::endl;
}

float TopSongs::calculateAverage(const std::vector<std::pair<int, float>>& ratings) {
    if (ratings.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (const auto& [userId, rating] : ratings) {
        sum += rating;
    }
    
    return sum / static_cast<float>(ratings.size());
}