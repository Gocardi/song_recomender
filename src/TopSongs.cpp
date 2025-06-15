#include "TopSongs.hpp"
#include "DataLoader.hpp"  // Necesario para acceder a cancion_a_usuarios

#include <algorithm> // sort
#include <numeric>   // accumulate

std::vector<std::pair<int, float>> obtenerTopNCanciones(int N) {
    std::vector<std::pair<int, float>> promedios;

    for (const auto& [idCancion, usuariosPuntaje] : cancion_a_usuarios) {
        if (usuariosPuntaje.empty()) continue;

        float suma = 0.0;
        for (const auto& [idUsuario, puntaje] : usuariosPuntaje) {
            suma += puntaje;
        }

        float promedio = suma / usuariosPuntaje.size();
        promedios.emplace_back(idCancion, promedio);
    }

    // Ordenar de mayor a menor promedio
    std::sort(promedios.begin(), promedios.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    // Devolver solo las primeras N (o todas si hay menos)
    if (promedios.size() > N) {
        promedios.resize(N);
    }

    return promedios;
}
