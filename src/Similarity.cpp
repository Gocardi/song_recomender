#include "Similarity.hpp"
#include "DataLoader.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

float calcularSimilitudPearson(int usuario1, int usuario2) {
    auto canciones1 = getCancionesDeUsuario(usuario1);
    auto canciones2 = getCancionesDeUsuario(usuario2);
    
    if (canciones1.empty() || canciones2.empty()) return 0.0f;
    
    // Encontrar canciones en común
    std::vector<int> cancionesComunes;
    for (const auto& [cancion, _] : canciones1) {
        if (getPuntaje(usuario2, cancion) != -1) {
            cancionesComunes.push_back(cancion);
        }
    }
    
    if (cancionesComunes.size() < 2) return 0.0f;
    
    float suma1 = 0, suma2 = 0, suma1Cuad = 0, suma2Cuad = 0, sumaProductos = 0;
    
    for (int cancion : cancionesComunes) {
        float puntaje1 = getPuntaje(usuario1, cancion);
        float puntaje2 = getPuntaje(usuario2, cancion);
        
        suma1 += puntaje1;
        suma2 += puntaje2;
        suma1Cuad += puntaje1 * puntaje1;
        suma2Cuad += puntaje2 * puntaje2;
        sumaProductos += puntaje1 * puntaje2;
    }
    
    float n = cancionesComunes.size();
    float numerador = sumaProductos - (suma1 * suma2 / n);
    float denominador = std::sqrt((suma1Cuad - suma1 * suma1 / n) * (suma2Cuad - suma2 * suma2 / n));
    
    if (denominador == 0) return 0.0f;
    
    return numerador / denominador;
}

float calcularSimilitudEuclidiana(int usuario1, int usuario2) {
    auto canciones1 = getCancionesDeUsuario(usuario1);
    auto canciones2 = getCancionesDeUsuario(usuario2);
    
    if (canciones1.empty() || canciones2.empty()) return 0.0f;
    
    // Encontrar canciones en común
    std::vector<int> cancionesComunes;
    for (const auto& [cancion, _] : canciones1) {
        if (getPuntaje(usuario2, cancion) != -1) {
            cancionesComunes.push_back(cancion);
        }
    }
    
    if (cancionesComunes.empty()) return 0.0f;
    
    float sumaCuadrados = 0.0f;
    for (int cancion : cancionesComunes) {
        float puntaje1 = getPuntaje(usuario1, cancion);
        float puntaje2 = getPuntaje(usuario2, cancion);
        float diferencia = puntaje1 - puntaje2;
        sumaCuadrados += diferencia * diferencia;
    }
    
    // Convertir distancia a similitud (entre 0 y 1)
    return 1.0f / (1.0f + std::sqrt(sumaCuadrados));
}

float calcularSimilitudCoseno(int usuario1, int usuario2) {
    auto canciones1 = getCancionesDeUsuario(usuario1);
    auto canciones2 = getCancionesDeUsuario(usuario2);
    
    if (canciones1.empty() || canciones2.empty()) return 0.0f;
    
    // Encontrar canciones en común
    std::vector<int> cancionesComunes;
    for (const auto& [cancion, _] : canciones1) {
        if (getPuntaje(usuario2, cancion) != -1) {
            cancionesComunes.push_back(cancion);
        }
    }
    
    if (cancionesComunes.empty()) return 0.0f;
    
    float productoEscalar = 0.0f;
    float norma1 = 0.0f, norma2 = 0.0f;
    
    for (int cancion : cancionesComunes) {
        float puntaje1 = getPuntaje(usuario1, cancion);
        float puntaje2 = getPuntaje(usuario2, cancion);
        
        productoEscalar += puntaje1 * puntaje2;
        norma1 += puntaje1 * puntaje1;
        norma2 += puntaje2 * puntaje2;
    }
    
    float denominador = std::sqrt(norma1) * std::sqrt(norma2);
    if (denominador == 0) return 0.0f;
    
    return productoEscalar / denominador;
}

UserSimilarity calcularSimilitudesGlobales() {
    UserSimilarity similitudes;
    std::cout << "Calculando similitudes entre usuarios..." << std::endl;
    
    // Obtener todos los usuarios
    std::vector<int> usuarios;
    for (const auto& [idUsuario, _] : usuario_a_canciones) {
        usuarios.push_back(idUsuario);
    }
    
    // Calcular similitudes entre todos los pares de usuarios
    for (size_t i = 0; i < usuarios.size(); ++i) {
        int usuario1 = usuarios[i];
        std::vector<std::pair<int, float>> similitudesUsuario;
        
        for (size_t j = 0; j < usuarios.size(); ++j) {
            if (i == j) continue; // No calcular similitud consigo mismo
            
            int usuario2 = usuarios[j];
            
            // Usar correlación de Pearson como métrica principal
            float similitud = calcularSimilitudPearson(usuario1, usuario2);
            
            if (similitud > 0.0f) {
                similitudesUsuario.emplace_back(usuario2, similitud);
            }
        }
        
        // Ordenar por similitud descendente
        std::sort(similitudesUsuario.begin(), similitudesUsuario.end(),
                  [](const auto& a, const auto& b) {
                      return a.second > b.second;
                  });
        
        // Mantener solo los 6 más similares
        if (similitudesUsuario.size() > 6) {
            similitudesUsuario.resize(6);
        }
        
        similitudes[usuario1] = similitudesUsuario;
    }
    
    std::cout << "Similitudes calculadas para " << usuarios.size() << " usuarios." << std::endl;
    return similitudes;
}

std::vector<std::pair<int, float>> obtenerUsuariosSimilares(int idUsuario, const UserSimilarity& similitudes, int cantidad) {
    auto it = similitudes.find(idUsuario);
    if (it == similitudes.end()) {
        return {};
    }
    
    std::vector<std::pair<int, float>> resultado = it->second;
    if (cantidad > 0 && static_cast<size_t>(cantidad) < resultado.size()) {
        resultado.resize(static_cast<size_t>(cantidad));
    }
    
    return resultado;
}

void mostrarUsuariosSimilares(int idUsuario, const std::vector<std::pair<int, float>>& similares) {
    std::cout << "\n=== USUARIOS MÁS SIMILARES AL USUARIO " << idUsuario << " ===" << std::endl;
    std::cout << std::setw(12) << "Usuario ID" << std::setw(15) << "Similitud" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    
    for (const auto& [usuarioId, similitud] : similares) {
        std::cout << std::setw(12) << usuarioId 
                  << std::setw(15) << std::fixed << std::setprecision(4) 
                  << similitud << std::endl;
    }
    std::cout << "===================================\n" << std::endl;
}
