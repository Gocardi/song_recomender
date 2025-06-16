#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "../include/DataLoader.hpp"
#include "../include/TopSongs.hpp"
#include "../include/Similarity.hpp"
#include "../include/Recommender.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#define FONT_PATH "gui/OpenSans-Regular.ttf"

void mostrarEncabezado(Font font, int modo) {
    if (modo == 1) {
        DrawTextEx(font, "ID Canción", (Vector2){60, 142}, 16, 1, DARKGRAY);
        DrawTextEx(font, "Puntaje",    (Vector2){230, 142}, 16, 1, DARKGRAY);
    } else if (modo == 2) {
        DrawTextEx(font, "ID Canción", (Vector2){60, 142}, 16, 1, DARKGRAY);
        DrawTextEx(font, "Promedio",   (Vector2){230, 142}, 16, 1, DARKGRAY);
    } else if (modo == 3) {
        DrawTextEx(font, "ID Usuario", (Vector2){60, 142}, 16, 1, DARKGRAY);
        DrawTextEx(font, "Similitud",  (Vector2){230, 142}, 16, 1, DARKGRAY);
    } else if (modo == 4) {
        DrawTextEx(font, "ID Canción",      (Vector2){60, 142}, 16, 1, DARKGRAY);
        DrawTextEx(font, "Recomendación",   (Vector2){230, 142}, 16, 1, DARKGRAY);
    }
}

void mostrarTabla(Font font, int modo, std::vector<std::string>& resultados) {
    for (size_t i = 0; i < resultados.size(); i++) {
        int y = 165 + (int)i * 22;
        std::string col1, col2;

        std::istringstream iss(resultados[i]);
        if (modo == 1) {
            int id, punt;
            iss.ignore(8); // "Canción "
            iss >> id;
            iss.ignore(11); // "   Puntaje: "
            iss >> punt;
            col1 = std::to_string(id);
            col2 = std::to_string(punt);
        } else if (modo == 2) {
            int id;
            float promedio;
            iss.ignore(8); // "Canción "
            iss >> id;
            iss.ignore(12); // "    Promedio: "
            iss >> promedio;
            col1 = std::to_string(id);
            char buffer[16]; snprintf(buffer, sizeof(buffer), "%.2f", promedio);
            col2 = buffer;
        } else if (modo == 3) {
            int id, simil;
            iss.ignore(8); // "Usuario "
            iss >> id;
            iss.ignore(12); // "   Similitud: "
            iss >> simil;
            col1 = std::to_string(id);
            col2 = std::to_string(simil) + "%";
        } else if (modo == 4) {
            int id, rec;
            iss.ignore(8); // "Canción "
            iss >> id;
            iss.ignore(17); // "   Recomendación: "
            iss >> rec;
            col1 = std::to_string(id);
            col2 = std::to_string(rec) + "%";
        }
        DrawTextEx(font, col1.c_str(), (Vector2){60, (float)y}, 16, 1, BLACK);
        DrawTextEx(font, col2.c_str(), (Vector2){230, (float)y}, 16, 1, BLACK);
    }
}

int main() {
    if (!cargarCSV("../data/ratings.csv")) {
        std::cerr << "Error al cargar el archivo CSV.\n";
        return 1;
    }

    auto similitudes = calcularSimilitudesGlobales();

    InitWindow(700, 420, "Song Recommender - GUI");
    SetTargetFPS(60);
    Font customFont = LoadFont(FONT_PATH);
    GuiSetFont(customFont);

    char usuarioID[16] = "";
    char nTop[8] = "5";
    char nSimilares[8] = "5";
    bool usuarioID_edit = false, nTop_edit = false, nSimilares_edit = false;

    int modo = 0;
    std::vector<std::string> resultados;
    std::string mensajeError;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(20, 20, 660, 80, LIGHTGRAY);
        DrawTextEx(customFont, "Ingrese datos para la acción seleccionada:", (Vector2){30, 28}, 18, 1, DARKGRAY);

        GuiLabel((Rectangle){30, 55, 110, 20}, "ID de Usuario:");
        if (GuiTextBox((Rectangle){140, 55, 80, 20}, usuarioID, 16, usuarioID_edit))
            usuarioID_edit = true;
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){140, 55, 80, 20}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            usuarioID_edit = true; nTop_edit = false; nSimilares_edit = false;
        }

        GuiLabel((Rectangle){240, 55, 100, 20}, "N (para Top o Similares):");
        if (GuiTextBox((Rectangle){350, 55, 50, 20}, nTop, 8, nTop_edit))
            nTop_edit = true;
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){350, 55, 50, 20}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            usuarioID_edit = false; nTop_edit = true; nSimilares_edit = false;
        }

        if (GuiTextBox((Rectangle){410, 55, 50, 20}, nSimilares, 8, nSimilares_edit))
            nSimilares_edit = true;
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){410, 55, 50, 20}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            usuarioID_edit = false; nTop_edit = false; nSimilares_edit = true;
        }

        DrawRectangle(20, 110, 660, 200, LIGHTGRAY);
        DrawTextEx(customFont, "Resultados:", (Vector2){30, 118}, 16, 1, DARKGRAY);

        if (!mensajeError.empty()) {
            DrawTextEx(customFont, mensajeError.c_str(), (Vector2){60, 160}, 16, 1, RED);
        } else if (!resultados.empty()) {
            mostrarEncabezado(customFont, modo);
            mostrarTabla(customFont, modo, resultados);
        } else {
            DrawTextEx(customFont, "Sin resultados.", (Vector2){60, 160}, 16, 1, GRAY);
        }

        // ----------- BOTONES: cada uno usa lógica real -----------
        if (GuiButton((Rectangle){30, 330, 140, 32}, "Ver Canciones de Usuario")) {
            modo = 1;
            resultados.clear();
            mensajeError.clear();
            int usuario = atoi(usuarioID);
            auto canciones = getCancionesDeUsuario(usuario);
            if (canciones.empty()) {
                mensajeError = "Usuario no encontrado o sin canciones.";
            } else {
                for (auto& [idCancion, puntaje] : canciones) {
                    std::ostringstream ss;
                    ss << "Canción " << idCancion << "   Puntaje: " << puntaje;
                    resultados.push_back(ss.str());
                }
            }
        }
        if (GuiButton((Rectangle){190, 330, 120, 32}, "Top N Canciones")) {
            modo = 2;
            resultados.clear();
            mensajeError.clear();
            int N = atoi(nTop);
            auto topCanciones = obtenerTopNCanciones(N);
            if (topCanciones.empty()) {
                mensajeError = "No hay canciones suficientes.";
            } else {
                for (const auto& [idCancion, promedio] : topCanciones) {
                    std::ostringstream ss;
                    ss << "Canción " << idCancion << "    Promedio: " << promedio;
                    resultados.push_back(ss.str());
                }
            }
        }
        if (GuiButton((Rectangle){330, 330, 170, 32}, "Ver Usuarios Similares")) {
            modo = 3;
            resultados.clear();
            mensajeError.clear();
            int usuario = atoi(usuarioID);
            int N = atoi(nSimilares);
            auto usuariosSimilares = obtenerUsuariosSimilares(usuario, similitudes, N);
            if (usuariosSimilares.empty()) {
                mensajeError = "No se encontraron usuarios similares.";
            } else {
                for (const auto& [idUsuario, similitud] : usuariosSimilares) {
                    std::ostringstream ss;
                    ss << "Usuario " << idUsuario << "   Similitud: " << (int)(similitud*100) << "%";
                    resultados.push_back(ss.str());
                }
            }
        }
        if (GuiButton((Rectangle){520, 330, 160, 32}, "Recomendar Canciones")) {
            modo = 4;
            resultados.clear();
            mensajeError.clear();
            int usuario = atoi(usuarioID);
            auto recomendaciones = recomendarCanciones(usuario, similitudes, 10);
            if (recomendaciones.empty()) {
                mensajeError = "No hay recomendaciones para este usuario.";
            } else {
                for (const auto& idCancion : recomendaciones) {
                    std::ostringstream ss;
                    // Si quieres mostrar el score:
                    float score = calcularPuntuacionPonderada(usuario, idCancion, similitudes);
                    ss << "Canción " << idCancion << "   Recomendación: " << (int)(score * 100) << "%";
                    resultados.push_back(ss.str());
                }
            }
        }

        if (GuiButton((Rectangle){560, 370, 120, 32}, "Salir")) { break; }

        EndDrawing();
    }

    UnloadFont(customFont);
    CloseWindow();
    return 0;
}
