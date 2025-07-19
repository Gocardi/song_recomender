#include "DataLoader.hpp"
#include "Similarity.hpp"
#include "TopSongs.hpp"
#include "Recommender.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <limits> 

void mostrarMenu() {
    std::cout << "\n=== SISTEMA DE RECOMENDACIONES DE CANCIONES ===" << std::endl;
    std::cout << "1. Ver canciones de un usuario" << std::endl;
    std::cout << "2. Top N canciones por promedio" << std::endl;
    std::cout << "3. Mostrar usuarios similares a un usuario" << std::endl;
    std::cout << "4. Obtener recomendaciones para un usuario" << std::endl;
    std::cout << "0. Salir" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "Seleccione una opcion: ";
}

int main() {
    if (!cargarCSV("data/ratings_big.csv")) {
         std::cerr << "Error critico: no se pudo cargar el archivo CSV.\n";
        return 1;
    }
    std::cout << "Archivo cargado correctamente.\n";
    
 /*
    auto start_serial = std::chrono::high_resolution_clock::now();
    [[maybe_unused]] auto similitudes_serial = calcularSimilitudesGlobalesSerial();     
    auto end_serial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_serial = end_serial - start_serial;

    std::cout << "\n-------------------------------------------------\n"; */
    
    auto start_parallel = std::chrono::high_resolution_clock::now();
    UserSimilarity similitudes = calcularSimilitudesGlobalesParalelo();
    auto end_parallel = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_parallel = end_parallel - start_parallel;

    std::cout << "\n--- ANALISIS DE RENDIMIENTO ---\n";
    std::cout << "Tiempo de ejecucion PARALELO: " << duration_parallel.count() << " ms\n";
    
    std::cout << "-------------------------------------------------\n";

    int opcion;
    while (true) {
        mostrarMenu();
        std::cin >> opcion;
        
        if (std::cin.fail()) {
            std::cout << "Entrada invalida. Por favor, ingrese un numero." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (opcion) {
            case 1: {
                int usuario_id;
                std::cout << "Ingrese el ID del usuario: ";
                std::cin >> usuario_id;
                auto canciones = getCancionesDeUsuario(usuario_id);
                if (canciones.empty()) {
                    std::cout << "Usuario no encontrado o sin canciones valoradas." << std::endl;
                } else {
                    std::cout << "\nCanciones valoradas por el usuario " << usuario_id << ":\n";
                    for (const auto& par : canciones) {
                        std::cout << "   Cancion ID: " << par.first << " - Puntaje: " << par.second << "\n";
                    }
                }
                break;
            }
            case 2: {
                int N;
                std::cout << "Cuantas canciones quieres ver en el ranking? ";
                std::cin >> N;
                auto topCanciones = obtenerTopNCanciones(N); 
                mostrarTopNCanciones(topCanciones); 
                break;
            }
            case 3: {
                int usuario_id;
                std::cout << "Ingrese el ID del usuario para buscar similares: ";
                std::cin >> usuario_id;
                auto usuariosSimilares = obtenerUsuariosSimilares(usuario_id, similitudes);
                if (usuariosSimilares.empty()) {
                    std::cout << "No se encontraron usuarios similares." << std::endl;
                } else {
                    mostrarUsuariosSimilares(usuario_id, usuariosSimilares);
                }
                break;
            }
            case 4: {
                int usuario_id;
                std::cout << "Ingrese el ID del usuario para obtener recomendaciones: ";
                std::cin >> usuario_id;
                auto recomendaciones = recomendarCanciones(usuario_id, similitudes); 
                mostrarRecomendaciones(usuario_id, recomendaciones); 
                break;
            }
            case 0:
                std::cout << "Saliendo del programa." << std::endl;
                return 0;
            default:
                std::cout << "Opcion no valida. Intente de nuevo." << std::endl;
                break;
        }
    }

    return 0;
}