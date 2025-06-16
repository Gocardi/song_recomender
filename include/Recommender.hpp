#ifndef RECOMMENDER_HPP
#define RECOMMENDER_HPP

#include <unordered_map>
#include <vector>
#include "Similarity.hpp"

// usuario_id -> vector de canciones recomendadas
using UserRecommendations = std::unordered_map<int, std::vector<int>>;

// Recomienda canciones a un usuario basándose en usuarios similares
std::vector<int> recomendarCanciones(int idUsuario, const UserSimilarity& similitudes, int cantidad = 10);

// Genera recomendaciones para todos los usuarios
UserRecommendations generarTodasLasRecomendaciones(const UserSimilarity& similitudes);

// Muestra las recomendaciones para un usuario
void mostrarRecomendaciones(int idUsuario, const std::vector<int>& recomendaciones);

// Calcula la puntuación ponderada de una canción para un usuario
float calcularPuntuacionPonderada(int idUsuario, int idCancion, const UserSimilarity& similitudes);

#endif // RECOMMENDER_HPP
