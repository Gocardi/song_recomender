#include "include/DataLoader.hpp"
#include "include/TopSongs.hpp"
#include "include/Similarity.hpp"
#include "include/Recommender.hpp"
#include <iostream>
#include <chrono>
#include <fstream>

void showMenu() {
    std::cout << "\n🎵 SISTEMA DE RECOMENDACIÓN DE CANCIONES 🎵\n";
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "1. Mostrar top N canciones más preferidas" << std::endl;
    std::cout << "2. Encontrar usuarios similares" << std::endl;
    std::cout << "3. Generar recomendaciones para un usuario" << std::endl;
    std::cout << "4. Mostrar estadísticas del dataset" << std::endl;
    std::cout << "5. Exportar recomendaciones a CSV" << std::endl;
    std::cout << "6. Análisis de similitud completo" << std::endl;
    std::cout << "0. Salir" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << "Seleccione una opción: ";
}

void showStats(const UserRatings& userRatings, const SongRatings& songRatings) {
    std::cout << "\n📊 ESTADÍSTICAS DEL DATASET\n";
    std::cout << std::string(40, '=') << std::endl;
    
    // Estadísticas básicas
    int totalRatings = 0;
    float totalRatingSum = 0.0f;
    int minRatingsPerUser = INT_MAX;
    int maxRatingsPerUser = 0;
    int minRatingsPerSong = INT_MAX;
    int maxRatingsPerSong = 0;
    
    // Análisis por usuario
    for (const auto& [userId, ratings] : userRatings) {
        int userRatingCount = ratings.size();
        totalRatings += userRatingCount;
        
        minRatingsPerUser = std::min(minRatingsPerUser, userRatingCount);
        maxRatingsPerUser = std::max(maxRatingsPerUser, userRatingCount);
        
        for (const auto& [songId, rating] : ratings) {
            totalRatingSum += rating;
        }
    }
    
    // Análisis por canción
    for (const auto& [songId, ratings] : songRatings) {
        int songRatingCount = ratings.size();
        minRatingsPerSong = std::min(minRatingsPerSong, songRatingCount);
        maxRatingsPerSong = std::max(maxRatingsPerSong, songRatingCount);
    }
    
    float avgRating = totalRatings > 0 ? totalRatingSum / totalRatings : 0.0f;
    float avgRatingsPerUser = userRatings.size() > 0 ? static_cast<float>(totalRatings) / userRatings.size() : 0.0f;
    float avgRatingsPerSong = songRatings.size() > 0 ? static_cast<float>(totalRatings) / songRatings.size() : 0.0f;
    
    std::cout << "Total de usuarios: " << userRatings.size() << std::endl;
    std::cout << "Total de canciones: " << songRatings.size() << std::endl;
    std::cout << "Total de puntuaciones: " << totalRatings << std::endl;
    std::cout << "Puntuación promedio: " << std::fixed << std::setprecision(2) << avgRating << std::endl;
    std::cout << "\nPor usuario:" << std::endl;
    std::cout << "  Promedio de puntuaciones: " << std::fixed << std::setprecision(1) << avgRatingsPerUser << std::endl;
    std::cout << "  Mínimo: " << minRatingsPerUser << ", Máximo: " << maxRatingsPerUser << std::endl;
    std::cout << "\nPor canción:" << std::endl;
    std::cout << "  Promedio de puntuaciones: " << std::fixed << std::setprecision(1) << avgRatingsPerSong << std::endl;
    std::cout << "  Mínimo: " << minRatingsPerSong << ", Máximo: " << maxRatingsPerSong << std::endl;
    
    // Distribución de puntuaciones
    std::vector<int> ratingDistribution(6, 0); // 0-5 estrellas
    for (const auto& [userId, ratings] : userRatings) {
        for (const auto& [songId, rating] : ratings) {
            int ratingInt = static_cast<int>(std::round(rating));
            if (ratingInt >= 0 && ratingInt <= 5) {
                ratingDistribution[ratingInt]++;
            }
        }
    }
    
    std::cout << "\nDistribución de puntuaciones:" << std::endl;
    for (int i = 0; i <= 5; ++i) {
        if (ratingDistribution[i] > 0) {
            float percentage = (static_cast<float>(ratingDistribution[i]) / totalRatings) * 100.0f;
            std::cout << "  " << i << " estrellas: " << ratingDistribution[i] 
                      << " (" << std::fixed << std::setprecision(1) << percentage << "%)" << std::endl;
        }
    }
    
    std::cout << std::string(40, '=') << std::endl;
}

void exportRecommendations(const UserRatings& userRatings, const SongRatings& songRatings) {
    std::cout << "Generando recomendaciones para todos los usuarios..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::ofstream file("recommendations_export.csv");
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo de exportación." << std::endl;
        return;
    }
    
    file << "usuario_id,cancion_recomendada,puntuacion_predicha,confianza\n";
    
    int processedUsers = 0;
    for (const auto& [userId, _] : userRatings) {
        auto recommendations = Recommender::generateRecommendations(
            userId, userRatings, songRatings, 5, 3.0f
        );
        
        for (const auto& rec : recommendations) {
            file << userId << "," << rec.songId << "," 
                 << std::fixed << std::setprecision(2) << rec.predictedRating << ","
                 << std::fixed << std::setprecision(3) << rec.confidence << "\n";
        }
        
        processedUsers++;
        if (processedUsers % 50 == 0) {
            std::cout << "Procesados " << processedUsers << "/" << userRatings.size() << " usuarios..." << std::endl;
        }
    }
    
    file.close();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "✅ Recomendaciones exportadas a 'recommendations_export.csv'" << std::endl;
    std::cout << "Tiempo de procesamiento: " << duration.count() << " segundos" << std::endl;
}

int main() {
    std::cout << "🎵 Iniciando Sistema de Recomendación de Canciones 🎵\n" << std::endl;
    
    // Cargar datos
    UserRatings userRatings;
    SongRatings songRatings;
    
    std::cout << "Cargando datos desde 'data/ratings.csv'..." << std::endl;
    
    auto startLoad = std::chrono::high_resolution_clock::now();
    
    if (!DataLoader::loadData("data/ratings.csv", userRatings, songRatings)) {
        std::cerr << "Error: No se pudieron cargar los datos. Verifique que el archivo 'data/ratings.csv' existe." << std::endl;
        return 1;
    }
    
    auto endLoad = std::chrono::high_resolution_clock::now();
    auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endLoad - startLoad);
    
    std::cout << "⏱️  Tiempo de carga: " << loadDuration.count() << " ms\n" << std::endl;
    
    // Menú principal
    int option;
    while (true) {
        showMenu();
        std::cin >> option;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "❌ Opción inválida. Intente nuevamente." << std::endl;
            continue;
        }
        
        switch (option) {
            case 1: {
                int n;
                std::cout << "Ingrese el número de canciones a mostrar: ";
                std::cin >> n;
                
                if (n <= 0) {
                    std::cout << "❌ Número inválido." << std::endl;
                    break;
                }
                
                auto start = std::chrono::high_resolution_clock::now();
                auto topSongs = TopSongs::getTopSongs(songRatings, n, 3);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                TopSongs::displayTopSongs(topSongs);
                std::cout << "⏱️  Tiempo de cálculo: " << duration.count() << " ms" << std::endl;
                break;
            }
            
            case 2: {
                int userId;
                std::cout << "Ingrese el ID del usuario: ";
                std::cin >> userId;
                
                auto start = std::chrono::high_resolution_clock::now();
                auto similarUsers = Similarity::findSimilarUsers(userId, userRatings, 6, 2);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                Similarity::displaySimilarUsers(userId, similarUsers);
                std::cout << "⏱️  Tiempo de cálculo: " << duration.count() << " ms" << std::endl;
                break;
            }
            
            case 3: {
                int userId, numRecs;
                std::cout << "Ingrese el ID del usuario: ";
                std::cin >> userId;
                std::cout << "Ingrese el número de recomendaciones (1-20): ";
                std::cin >> numRecs;
                
                if (numRecs <= 0 || numRecs > 20) {
                    std::cout << "❌ Número de recomendaciones inválido." << std::endl;
                    break;
                }
                
                auto start = std::chrono::high_resolution_clock::now();
                auto recommendations = Recommender::generateRecommendations(
                    userId, userRatings, songRatings, numRecs, 3.5f
                );
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                Recommender::displayRecommendations(userId, recommendations);
                Recommender::displayRecommendationStats(userId, userRatings, recommendations);
                std::cout << "⏱️  Tiempo de cálculo: " << duration.count() << " ms" << std::endl;
                break;
            }
            
            case 4: {
                showStats(userRatings, songRatings);
                break;
            }
            
            case 5: {
                exportRecommendations(userRatings, songRatings);
                break;
            }
            
            case 6: {
                std::cout << "⚠️  Advertencia: El análisis completo puede tomar varios minutos." << std::endl;
                std::cout << "¿Desea continuar? (s/n): ";
                char confirm;
                std::cin >> confirm;
                
                if (confirm == 's' || confirm == 'S') {
                    auto start = std::chrono::high_resolution_clock::now();
                    auto allSimilarities = Similarity::calculateAllSimilarities(userRatings);
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                    
                    std::cout << "✅ Análisis completado en " << duration.count() << " segundos" << std::endl;
                    std::cout << "Matriz de similitudes calculada para " << allSimilarities.size() << " usuarios." << std::endl;
                }
                break;
            }
            
            case 0: {
                std::cout << "👋 ¡Gracias por usar el Sistema de Recomendación de Canciones!" << std::endl;
                return 0;
            }
            
            default: {
                std::cout << "❌ Opción inválida. Intente nuevamente." << std::endl;
                break;
            }
        }
        
        std::cout << "\nPresione Enter para continuar...";
        std::cin.ignore();
        std::cin.get();
    }
    
    return 0;
}