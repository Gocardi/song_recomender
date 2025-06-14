#include "../include/DataLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool DataLoader::loadData(const std::string& filename, 
                         UserRatings& userRatings, 
                         SongRatings& songRatings) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
        return false;
    }

    std::string line;
    int lineNumber = 0;
    int recordsLoaded = 0;

    // Leer línea por línea
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Saltar líneas vacías
        if (line.empty() || trim(line).empty()) {
            continue;
        }

        int userId, songId;
        float rating;

        if (parseLine(line, userId, songId, rating)) {
            // Almacenar en UserRatings
            userRatings[userId][songId] = rating;
            
            // Almacenar en SongRatings
            songRatings[songId].emplace_back(userId, rating);
            
            recordsLoaded++;
        } else {
            std::cerr << "Advertencia: Error parseando línea " << lineNumber 
                      << ": " << line << std::endl;
        }
    }

    file.close();
    
    std::cout << "Datos cargados exitosamente:" << std::endl;
    std::cout << "- Registros procesados: " << recordsLoaded << std::endl;
    std::cout << "- Usuarios únicos: " << userRatings.size() << std::endl;
    std::cout << "- Canciones únicas: " << songRatings.size() << std::endl;

    return recordsLoaded > 0;
}

bool DataLoader::parseLine(const std::string& line, int& userId, int& songId, float& rating) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    // Dividir por comas
    while (std::getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    // Verificar que tenemos exactamente 3 campos
    if (tokens.size() != 3) {
        return false;
    }

    try {
        userId = std::stoi(tokens[0]);
        songId = std::stoi(tokens[1]);
        rating = std::stof(tokens[2]);
        
        // Validar que los valores sean razonables
        if (userId <= 0 || songId <= 0 || rating < 0.0f || rating > 5.0f) {
            return false;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string DataLoader::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}