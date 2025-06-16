#include "../include/DataLoader.hpp"
#include "TopSongs.hpp"
#include "Similarity.hpp"
#include "Recommender.hpp"
#include <iostream>

void mostrarMenu() {
    std::cout << "\n=== SISTEMA DE RECOMENDACIONES DE CANCIONES ===" << std::endl;
    std::cout << "1. Ver canciones de usuarios específicos" << std::endl;
    std::cout << "2. Top N canciones por promedio" << std::endl;
    std::cout << "3. Calcular usuarios similares" << std::endl;
    std::cout << "4. Obtener recomendaciones para un usuario" << std::endl;
    std::cout << "0. Salir" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "Seleccione una opción: ";
}

int main() {
    if (cargarCSV("data/ratings.csv")) {
        std::cout << "Archivo cargado correctamente.\n";

        // Mostrar ejemplo inicial de canciones de usuarios
        int idUsuario1 = 1;
        auto canciones1 = getCancionesDeUsuario(idUsuario1);
        std::cout << "\nCanciones valoradas por el usuario " << idUsuario1 << ":\n";
        for (auto& [idCancion, puntaje] : canciones1) {
            std::cout << "   Canción ID: " << idCancion << " - Puntaje: " << puntaje << "\n";
        }

        int idUsuario2 = 2;
        auto canciones2 = getCancionesDeUsuario(idUsuario2);
        std::cout << "\nCanciones valoradas por el usuario " << idUsuario2 << ":\n";
        for (auto& [idCancion, puntaje] : canciones2) {
            std::cout << "   Canción ID: " << idCancion << " - Puntaje: " << puntaje << "\n";
        }

        // Calcular similitudes una vez al inicio
        auto similitudes = calcularSimilitudesGlobales();

        // Menú interactivo
        int opcion;
        while (true) {
            mostrarMenu();
            std::cin >> opcion;

            switch (opcion) {
                case 1: {
                    int usuario;
                    std::cout << "Ingrese el ID del usuario: ";
                    std::cin >> usuario;
                    
                    auto canciones = getCancionesDeUsuario(usuario);
                    if (canciones.empty()) {
                        std::cout << "Usuario no encontrado." << std::endl;
                    } else {
                        std::cout << "\nCanciones valoradas por el usuario " << usuario << ":\n";
                        for (auto& [idCancion, puntaje] : canciones) {
                            std::cout << "   Canción ID: " << idCancion << " - Puntaje: " << puntaje << "\n";
                        }
                    }
                    break;
                }
                
                case 2: {
                    int N;
                    std::cout << "¿Cuántas canciones quieres ver en el ranking? ";
                    std::cin >> N;

                    auto topCanciones = obtenerTopNCanciones(N);
                    std::cout << "\nTop " << N << " canciones por promedio:\n";
                    for (const auto& [idCancion, promedio] : topCanciones) {
                        std::cout << "Canción " << idCancion << " → Promedio: " << promedio << "\n";
                    }
                    break;
                }
                
                case 3: {
                    int usuario;
                    std::cout << "Ingrese el ID del usuario: ";
                    std::cin >> usuario;
                    
                    auto usuariosSimilares = obtenerUsuariosSimilares(usuario, similitudes, 6);
                    if (usuariosSimilares.empty()) {
                        std::cout << "No se encontraron usuarios similares." << std::endl;
                    } else {
                        mostrarUsuariosSimilares(usuario, usuariosSimilares);
                    }
                    break;
                }
                
                case 4: {
                    int usuario;
                    std::cout << "Ingrese el ID del usuario: ";
                    std::cin >> usuario;
                    
                    auto recomendaciones = recomendarCanciones(usuario, similitudes, 10);
                    mostrarRecomendaciones(usuario, recomendaciones);
                    break;
                }
                
                case 0: {
                    std::cout << "¡Gracias por usar el sistema!" << std::endl;
                    return 0;
                }
                
                default: {
                    std::cout << "Opción inválida." << std::endl;
                    break;
                }
            }
        }

    } else {
        std::cerr << "Error al cargar el archivo CSV.\n";
        return 1;
    }

    return 0;
}
