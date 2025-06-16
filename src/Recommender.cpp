#include "Recommender.hpp"
#include "DataLoader.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <unordered_set>

std::vector<int> recomendarCanciones(int idUsuario, const UserSimilarity& similitudes, int cantidad) {
    // Verificar que el usuario existe
    auto cancionesUsuario = getCancionesDeUsuario(idUsuario);
    if (cancionesUsuario.empty()) {
        std::cout << "Usuario " << idUsuario << " no encontrado." << std::endl;
        return {};
    }
    
    // Obtener canciones ya calificadas por el usuario
    std::unordered_set<int> cancionesYaCalificadas;
    for (const auto& [idCancion, _] : cancionesUsuario) {
        cancionesYaCalificadas.insert(idCancion);
    }
    
    // Obtener usuarios similares
    auto usuariosSimilares = obtenerUsuariosSimilares(idUsuario, similitudes, 6);
    if (usuariosSimilares.empty()) {
        std::cout << "No se encontraron usuarios similares para " << idUsuario << std::endl;
        return {};
    }
    
    // Candidatos a recomendación con puntuación ponderada
    std::unordered_map<int, float> candidatos;
    std::unordered_map<int, float> sumaSimilitudes;
    
    for (const auto& [usuarioSimilar, similitud] : usuariosSimilares) {
        auto cancionesSimilares = getCancionesDeUsuario(usuarioSimilar);
        
        for (const auto& [idCancion, puntaje] : cancionesSimilares) {
            // Solo considerar canciones no calificadas por el usuario objetivo
            if (cancionesYaCalificadas.find(idCancion) == cancionesYaCalificadas.end()) {
                candidatos[idCancion] += similitud * puntaje;
                sumaSimilitudes[idCancion] += similitud;
            }
        }
    }
    
    // Normalizar puntuaciones y crear lista de recomendaciones
    std::vector<std::pair<int, float>> recomendacionesPuntuadas;
    for (const auto& [idCancion, puntuacionTotal] : candidatos) {
        float puntuacionNormalizada = puntuacionTotal / sumaSimilitudes[idCancion];
        recomendacionesPuntuadas.emplace_back(idCancion, puntuacionNormalizada);
    }
    
    // Ordenar por puntuación descendente
    std::sort(recomendacionesPuntuadas.begin(), recomendacionesPuntuadas.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    // Extraer solo los IDs de las canciones
    std::vector<int> recomendaciones;
    int maxRecomendaciones = std::min(cantidad, static_cast<int>(recomendacionesPuntuadas.size()));
    for (int i = 0; i < maxRecomendaciones; ++i) {
        recomendaciones.push_back(recomendacionesPuntuadas[i].first);
    }
    
    return recomendaciones;
}

UserRecommendations generarTodasLasRecomendaciones(const UserSimilarity& similitudes) {
    UserRecommendations todasLasRecomendaciones;
    
    std::cout << "Generando recomendaciones para todos los usuarios..." << std::endl;
    
    for (const auto& [idUsuario, _] : usuario_a_canciones) {
        auto recomendaciones = recomendarCanciones(idUsuario, similitudes, 10);
        if (!recomendaciones.empty()) {
            todasLasRecomendaciones[idUsuario] = recomendaciones;
        }
    }
    
    return todasLasRecomendaciones;
}

void mostrarRecomendaciones(int idUsuario, const std::vector<int>& recomendaciones) {
    std::cout << "\n=== RECOMENDACIONES PARA USUARIO " << idUsuario << " ===" << std::endl;
    
    if (recomendaciones.empty()) {
        std::cout << "No hay recomendaciones disponibles." << std::endl;
    } else {
        std::cout << "Canciones recomendadas:" << std::endl;
        for (size_t i = 0; i < recomendaciones.size(); ++i) {
            std::cout << (i + 1) << ". Canción ID: " << recomendaciones[i] << std::endl;
        }
    }
    std::cout << "=======================================\n" << std::endl;
}

float calcularPuntuacionPonderada(int idUsuario, int idCancion, const UserSimilarity& similitudes) {
    auto usuariosSimilares = obtenerUsuariosSimilares(idUsuario, similitudes, 6);
    
    float puntuacionTotal = 0.0f;
    float sumaSimilitudes = 0.0f;
    
    for (const auto& [usuarioSimilar, similitud] : usuariosSimilares) {
        int puntaje = getPuntaje(usuarioSimilar, idCancion);
        if (puntaje != -1) {
            puntuacionTotal += similitud * puntaje;
            sumaSimilitudes += similitud;
        }
    }
    
    if (sumaSimilitudes == 0.0f) return 0.0f;
    
    return puntuacionTotal / sumaSimilitudes;
}
