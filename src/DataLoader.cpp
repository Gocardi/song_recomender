// src/DataLoader.cpp
#include "DataLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// --- Definiciones de las estructuras globales ---
std::unordered_map<int, std::vector<std::pair<int, int>>> usuario_a_canciones;
std::unordered_map<int, std::vector<std::pair<int, int>>> cancion_a_usuarios;
std::unordered_map<int, std::unordered_map<int, int>> matriz_usuario_cancion;

// --- Funciones ---

bool cargarCSV(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << rutaArchivo << std::endl;
        return false;
    }

    std::string linea;
    int lineaNum = 0;
    while (std::getline(archivo, linea)) {
        lineaNum++;
        if (linea.empty())
            continue;

        std::istringstream ss(linea);
        std::string token;
        int idUsuario, idCancion, puntaje;

        // Parsear los tres campos
        if (!std::getline(ss, token, ',')) continue;
        idUsuario = std::stoi(token);

        if (!std::getline(ss, token, ',')) continue;
        idCancion = std::stoi(token);

        if (!std::getline(ss, token, ',')) continue;
        puntaje = std::stoi(token);

        // Rellenar las estructuras
        usuario_a_canciones[idUsuario].emplace_back(idCancion, puntaje);
        cancion_a_usuarios[idCancion].emplace_back(idUsuario, puntaje);
        matriz_usuario_cancion[idUsuario][idCancion] = puntaje;
    }

    archivo.close();
    return true;
}

std::vector<std::pair<int, int>> getCancionesDeUsuario(int idUsuario) {
    if (usuario_a_canciones.count(idUsuario))
        return usuario_a_canciones[idUsuario];
    else
        return {};
}

std::vector<std::pair<int, int>> getUsuariosDeCancion(int idCancion) {
    if (cancion_a_usuarios.count(idCancion))
        return cancion_a_usuarios[idCancion];
    else
        return {};
}

int getPuntaje(int idUsuario, int idCancion) {
    if (matriz_usuario_cancion.count(idUsuario)) {
        auto& inner = matriz_usuario_cancion[idUsuario];
        if (inner.count(idCancion))
            return inner[idCancion];
    }
    return -1;
}
