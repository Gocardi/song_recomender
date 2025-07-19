#ifndef TOP_SONGS_HPP
#define TOP_SONGS_HPP

#include <vector>
#include <utility> // para pair

// Devuelve los IDs de las Top N canciones con mejor promedio
std::vector<std::pair<int, float>> obtenerTopNCanciones(int N);

// Muestra en consola el ranking de canciones
void mostrarTopNCanciones(const std::vector<std::pair<int, float>>& topCanciones);

#endif // TOP_SONGS_HPP