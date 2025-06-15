// include/DataLoader.hpp
#ifndef DATALOADER_HPP
#define DATALOADER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
//#include "Types.h"

// --- Estructuras globales ---

// usuario -> vector de (cancion, puntaje)
extern std::unordered_map<int, std::vector<std::pair<int, int>>> usuario_a_canciones;

// cancion -> vector de (usuario, puntaje)
extern std::unordered_map<int, std::vector<std::pair<int, int>>> cancion_a_usuarios;

// usuario -> (cancion -> puntaje)
extern std::unordered_map<int, std::unordered_map<int, int>> matriz_usuario_cancion;

// Funciones de acceso y carga

// Carga un archivo CSV y rellena las estructuras
bool cargarCSV(const std::string& rutaArchivo);

// Devuelve vector de (idCancion, puntaje) para un usuario
std::vector<std::pair<int, int>> getCancionesDeUsuario(int idUsuario);

// Devuelve vector de (idUsuario, puntaje) para una cancion
std::vector<std::pair<int, int>> getUsuariosDeCancion(int idCancion);

// Devuelve el puntaje dado por un usuario a una cancion
int getPuntaje(int idUsuario, int idCancion);

#endif // DATALOADER_HPP
