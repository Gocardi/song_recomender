#include "Similarity.hpp"
#include "DataLoader.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <mutex>
#include <numeric>


float calcularSimilitudPearson(int usuario1, int usuario2) {
    auto canciones1 = getCancionesDeUsuario(usuario1);
    auto canciones2 = getCancionesDeUsuario(usuario2);

    if (canciones1.empty() || canciones2.empty()) return 0.0f;

    std::vector<int> cancionesComunes;
    for (const auto& par : canciones1) {
        if (getPuntaje(usuario2, par.first) != -1) {
            cancionesComunes.push_back(par.first); 
        }
    }

    if (cancionesComunes.size() < 2) return 0.0f;

    float suma1 = 0, suma2 = 0, suma1Cuad = 0, suma2Cuad = 0, sumaProductos = 0;
    for (int cancion : cancionesComunes) {
        float puntaje1 = static_cast<float>(getPuntaje(usuario1, cancion));
        float puntaje2 = static_cast<float>(getPuntaje(usuario2, cancion));
        suma1 += puntaje1;
        suma2 += puntaje2;
        suma1Cuad += puntaje1 * puntaje1;
        suma2Cuad += puntaje2 * puntaje2;
        sumaProductos += puntaje1 * puntaje2;
    }

    float n = static_cast<float>(cancionesComunes.size());
    float numerador = sumaProductos - (suma1 * suma2 / n);
    float denominador = sqrt((suma1Cuad - (suma1 * suma1) / n) * (suma2Cuad - (suma2 * suma2) / n));

    if (denominador == 0) return 0.0f;
    return numerador / denominador;
}

UserSimilarity calcularSimilitudesGlobalesSerial() {
    UserSimilarity similitudes;
    std::cout << "Calculando similitudes entre usuarios (Version Secuencial)..." << std::endl;

    std::vector<int> usuarios;
    for (const auto& par : usuario_a_canciones) {
        usuarios.push_back(par.first);
    }
    similitudes.reserve(usuarios.size());

    for (size_t i = 0; i < usuarios.size(); ++i) {
        int usuario1 = usuarios[i];
        std::vector<std::pair<int, float>> similitudesUsuario;
        for (size_t j = 0; j < usuarios.size(); ++j) {
            if (i == j) continue;
            int usuario2 = usuarios[j];
            float similitud = calcularSimilitudPearson(usuario1, usuario2);
            if (similitud > 0.0f) {
                similitudesUsuario.emplace_back(usuario2, similitud);
            }
        }
        std::sort(similitudesUsuario.begin(), similitudesUsuario.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });
        if (similitudesUsuario.size() > 6) {
            similitudesUsuario.resize(6);
        }
        similitudes[usuario1] = similitudesUsuario;
    }
    std::cout << "Similitudes secuenciales calculadas para " << usuarios.size() << " usuarios." << std::endl;
    return similitudes;
}

UserSimilarity calcularSimilitudesGlobalesParalelo() {
    std::cout << "Calculando similitudes en paralelo entre usuarios..." << std::endl;

    std::vector<int> usuarios;
    for (const auto& par : usuario_a_canciones) {
        usuarios.push_back(par.first);
    }
    const size_t num_usuarios = usuarios.size();
    if (num_usuarios == 0) return {};

    const unsigned int num_threads = std::max(1u, std::thread::hardware_concurrency());
    std::cout << "Usando " << num_threads << " hilos" << std::endl;

    std::vector<std::thread> workers;
    UserSimilarity resultados_globales;
    std::mutex merge_mutex;
    resultados_globales.reserve(num_usuarios);

    auto task_for_thread = [&](unsigned int thread_id) {
        UserSimilarity resultados_locales;

        for (size_t i = thread_id; i < num_usuarios; i += num_threads) {
            int usuario1 = usuarios[i];
            std::vector<std::pair<int, float>> similitudesUsuario;
            for (size_t j = 0; j < num_usuarios; ++j) {
                if (i == j) continue;
                int usuario2 = usuarios[j];
                float similitud = calcularSimilitudPearson(usuario1, usuario2);
                if (similitud > 0.0f) {
                    similitudesUsuario.emplace_back(usuario2, similitud);
                }
            }

            std::sort(similitudesUsuario.begin(), similitudesUsuario.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });
            if (similitudesUsuario.size() > 6) {
                similitudesUsuario.resize(6);
            }
            resultados_locales[usuario1] = similitudesUsuario;
        }
        
        {
            std::lock_guard<std::mutex> lock(merge_mutex);
            resultados_globales.insert(resultados_locales.begin(), resultados_locales.end());
        }
    };

    for (unsigned int i = 0; i < num_threads; ++i) {
        workers.emplace_back(task_for_thread, i);
    }
    for (auto& worker : workers) {
        worker.join();
    }

    std::cout << "Similitudes paralelas calculadas para " << num_usuarios << " usuarios." << std::endl;
    return resultados_globales;
}

std::vector<std::pair<int, float>> obtenerUsuariosSimilares(int idUsuario, const UserSimilarity& similitudes, int cantidad) {
    auto it = similitudes.find(idUsuario);
    if (it == similitudes.end()) return {};

    auto resultado = it->second;
    if (cantidad > 0 && static_cast<size_t>(cantidad) < resultado.size()) {
        resultado.resize(static_cast<size_t>(cantidad));
    }
    return resultado;
}

void mostrarUsuariosSimilares(int idUsuario, const std::vector<std::pair<int, float>>& similares) {
    std::cout << "\n=== USUARIOS MAS SIMILARES AL USUARIO " << idUsuario << " ===" << std::endl;
    std::cout << std::setw(12) << "Usuario ID" << std::setw(15) << "Similitud" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    if (similares.empty()){
        std::cout << "No se encontraron usuarios similares." << std::endl;
    } else {
        for (const auto& par : similares) {
            std::cout << std::setw(12) << par.first
                 << std::setw(15) << std::fixed << std::setprecision(4)
                 << par.second << std::endl;
        }
    }
    std::cout << "===================================\n" << std::endl;
}