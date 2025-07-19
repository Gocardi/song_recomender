#ifndef SIMILARITY_HPP
#define SIMILARITY_HPP

#include <unordered_map>
#include <vector>
#include <utility>
#include <thread>
#include <mutex>

// usuario_id -> vector de (usuario_similar_id, similitud)
using UserSimilarity = std::unordered_map<int, std::vector<std::pair<int, float>>>;

float calcularSimilitudPearson(int usuario1, int usuario2);

// --- VERSIONES DE CÁLCULO GLOBAL ---

// Versión secuencial original, para comparación. Representa un único "flujo de control".
UserSimilarity calcularSimilitudesGlobalesSerial();

// Versión paralela. Creará "múltiples puntos de ejecución".
UserSimilarity calcularSimilitudesGlobalesParalelo();


// --- FUNCIONES DE AYUDA ---
std::vector<std::pair<int, float>> obtenerUsuariosSimilares(int idUsuario, const UserSimilarity& similitudes, int cantidad = 6);
void mostrarUsuariosSimilares(int idUsuario, const std::vector<std::pair<int, float>>& similares);

#endif 