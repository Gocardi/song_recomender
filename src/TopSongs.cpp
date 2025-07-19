#include "TopSongs.hpp"
#include "DataLoader.hpp"

#include <algorithm> // para std::sort
#include <numeric>   // para std::accumulate (alternativa a bucle for)
#include <iostream>  // para std::cout
#include <iomanip>   // para std::setw, std::fixed, std::setprecision

// Tu función original (sin cambios, es correcta)
std::vector<std::pair<int, float>> obtenerTopNCanciones(int N) {
    std::vector<std::pair<int, float>> promedios;

    for (const auto& par : cancion_a_usuarios) {
        const auto& usuariosPuntaje = par.second;
        if (usuariosPuntaje.empty()) continue;

        float suma = 0.0f;
        for (const auto& puntajePar : usuariosPuntaje) {
            suma += static_cast<float>(puntajePar.second);
        }

        float promedio = suma / usuariosPuntaje.size();
        promedios.emplace_back(par.first, promedio);
    }

    // Ordenar de mayor a menor promedio
    std::sort(promedios.begin(), promedios.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // Devolver solo las primeras N (o todas si hay menos)
    if (static_cast<size_t>(N) < promedios.size()) {
        promedios.resize(N);
    }

    return promedios;
}

void mostrarTopNCanciones(const std::vector<std::pair<int, float>>& topCanciones) {
    if (topCanciones.empty()) {
        std::cout << "\nNo se encontraron canciones para mostrar en el ranking." << std::endl;
        return;
    }

    std::cout << "\n=== TOP " << topCanciones.size() << " CANCIONES POR PROMEDIO ===" << std::endl;
    std::cout << std::setw(12) << "Cancion ID" << std::setw(20) << "Puntaje Promedio" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    for (const auto& par : topCanciones) {
        std::cout << std::setw(12) << par.first
                  << std::setw(20) << std::fixed << std::setprecision(4)
                  << par.second << std::endl;
    }
    std::cout << "===================================\n" << std::endl;
}