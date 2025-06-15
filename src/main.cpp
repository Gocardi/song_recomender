#include "../include/DataLoader.hpp"
#include "TopSongs.hpp"
#include <iostream>

int main() {
    if (cargarCSV("data/ratings.csv")) {
        std::cout << " Archivo cargado correctamente.\n";

        int idUsuario1 = 1;
        auto canciones1 = getCancionesDeUsuario(idUsuario1);
        std::cout << "\n Canciones valoradas por el usuario " << idUsuario1 << ":\n";
        for (auto& [idCancion, puntaje] : canciones1) {
            std::cout << "   Canción ID: " << idCancion << " - Puntaje: " << puntaje << "\n";
        }

        int idUsuario2 = 2;
        auto canciones2 = getCancionesDeUsuario(idUsuario2);
        std::cout << "\n Canciones valoradas por el usuario " << idUsuario2 << ":\n";
        for (auto& [idCancion, puntaje] : canciones2) {
            std::cout << "   Canción ID: " << idCancion << " - Puntaje: " << puntaje << "\n";
        }

        int N;
        std::cout << "\n ¿Cuántas canciones quieres ver en el ranking? ";
        std::cin >> N;

        auto topCanciones = obtenerTopNCanciones(N);

        std::cout << "\n Top " << N << " canciones por promedio:\n";
        for (const auto& [idCancion, promedio] : topCanciones) {
            std::cout << "Canción " << idCancion << " → Promedio: " << promedio << "\n";
        }

    } else {
        std::cerr << "Error al cargar el archivo CSV.\n";
    }

    return 0;
}
