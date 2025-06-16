#ifndef SIMILARITY_HPP
#define SIMILARITY_HPP

#include <unordered_map>
#include <vector>
#include <utility>

// usuario_id -> vector de (usuario_similar_id, similitud)
using UserSimilarity = std::unordered_map<int, std::vector<std::pair<int, float>>>;

// Calcula similitud entre dos usuarios usando correlación de Pearson
float calcularSimilitudPearson(int usuario1, int usuario2);

// Calcula similitud entre dos usuarios usando distancia euclidiana
float calcularSimilitudEuclidiana(int usuario1, int usuario2);

// Calcula similitud entre dos usuarios usando similitud del coseno
float calcularSimilitudCoseno(int usuario1, int usuario2);

// Encuentra los 6 usuarios más similares para cada usuario
UserSimilarity calcularSimilitudesGlobales();

// Obtiene los usuarios similares a uno específico
std::vector<std::pair<int, float>> obtenerUsuariosSimilares(int idUsuario, const UserSimilarity& similitudes, int cantidad = 6);

// Muestra los usuarios similares
void mostrarUsuariosSimilares(int idUsuario, const std::vector<std::pair<int, float>>& similares);

#endif // SIMILARITY_HPP
