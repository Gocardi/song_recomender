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
#include <algorithm>
#include <iomanip>

#define FONT_PATH "gui/OpenSans-Reguar.ttf"

void extraerColumnas(const std::string& fila, int modo, std::string& col1, std::string& col2) {
    std::istringstream iss(fila);
    if (modo == 1) {
        std::string palabra;
        int id, puntaje;
        iss >> palabra >> palabra >> id >> palabra >> palabra >> puntaje;
        col1 = std::to_string(id);
        col2 = std::to_string(puntaje);
    } else if (modo == 2) {
        std::string palabra;
        int id;
        float promedio;
        iss >> palabra >> id >> palabra >> promedio;
        col1 = std::to_string(id);
        char buffer[16]; 
        snprintf(buffer, sizeof(buffer), "%.2f", promedio);
        col2 = buffer;
    } else if (modo == 3) {
        size_t pos1 = fila.find("Usuario ");
        size_t pos2 = fila.find(" → Similitud: ");
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            std::string idStr = fila.substr(pos1 + 8, pos2 - pos1 - 8);
            std::string similStr = fila.substr(pos2 + 14);
            similStr = similStr.substr(0, similStr.find("%"));
            col1 = idStr;
            col2 = similStr + "%";
        }
    } else if (modo == 4) {
        // Ahora solo mostramos el ID de canción, sin recomendación
        size_t pos1 = fila.find("Canción ");
        if (pos1 != std::string::npos) {
            std::string idStr = fila.substr(pos1 + 8);
            col1 = idStr;
            col2 = ""; // No hay segunda columna
        }
    }
}

int main() {
    if (!cargarCSV("data/ratings.csv")) {
        std::cerr << "Error al cargar el archivo CSV.\n";
        return 1;
    }

    auto similitudes = calcularSimilitudesGlobales();

    InitWindow(700, 420, "Song Recommender - GUI");
    SetTargetFPS(60);

    Font customFont = GetFontDefault();
    if (FileExists(FONT_PATH)) {
        customFont = LoadFont(FONT_PATH);
        GuiSetFont(customFont);
    }

    char usuarioID[16] = "";
    char nTop[8] = "5";
    bool usuarioID_edit = false, nTop_edit = false;

    int modo = 0;
    std::vector<std::string> resultados;
    std::string mensajeError;

    Rectangle panelRect = { 20, 110, 660, 200 };
    Vector2 scroll = { 0, 0 };

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (!CheckCollisionPointRec(mousePos, (Rectangle){140, 55, 80, 20})) usuarioID_edit = false;
            if (!CheckCollisionPointRec(mousePos, (Rectangle){350, 55, 50, 20})) nTop_edit = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(20, 20, 660, 80, LIGHTGRAY);
        DrawTextEx(customFont, "Ingrese datos para la acción seleccionada:", (Vector2){30, 28}, 18, 1, DARKGRAY);

        GuiLabel((Rectangle){30, 55, 110, 20}, "ID de Usuario:");
        if (GuiTextBox((Rectangle){140, 55, 80, 20}, usuarioID, 16, usuarioID_edit)) usuarioID_edit = !usuarioID_edit;

        GuiLabel((Rectangle){240, 55, 100, 20}, "N (Top/Similares):");
        if (GuiTextBox((Rectangle){350, 55, 50, 20}, nTop, 8, nTop_edit)) nTop_edit = !nTop_edit;

        DrawTextEx(customFont, "Resultados:", (Vector2){30, 118}, 16, 1, DARKGRAY);

        int totalRows = (int)resultados.size();
        Rectangle contentRect = { 0, 0, 640.0f, (float)(22 * std::max(totalRows + 2, 10)) };
        Rectangle view = {0};
        GuiScrollPanel(panelRect, NULL, contentRect, &scroll, &view);

        BeginScissorMode(panelRect.x, panelRect.y, panelRect.width, panelRect.height);

        float headerY = panelRect.y + 5 + scroll.y;
        if (modo == 1) {
            DrawTextEx(customFont, "ID Canción", (Vector2){panelRect.x + 40, headerY}, 16, 1, DARKBLUE);
            DrawTextEx(customFont, "Puntaje",    (Vector2){panelRect.x + 210, headerY}, 16, 1, DARKBLUE);
        } else if (modo == 2) {
            DrawTextEx(customFont, "ID Canción", (Vector2){panelRect.x + 40, headerY}, 16, 1, DARKBLUE);
            DrawTextEx(customFont, "Promedio",   (Vector2){panelRect.x + 210, headerY}, 16, 1, DARKBLUE);
        } else if (modo == 3) {
            DrawTextEx(customFont, "ID Usuario", (Vector2){panelRect.x + 40, headerY}, 16, 1, DARKBLUE);
            DrawTextEx(customFont, "Similitud",  (Vector2){panelRect.x + 210, headerY}, 16, 1, DARKBLUE);
        } else if (modo == 4) {
            DrawTextEx(customFont, "ID Canción", (Vector2){panelRect.x + 40, headerY}, 16, 1, DARKBLUE);
            // No segunda columna
        }

        if (!mensajeError.empty()) {
            DrawTextEx(customFont, mensajeError.c_str(), (Vector2){panelRect.x + 40, panelRect.y + 40 + scroll.y}, 16, 1, RED);
        } else if (!resultados.empty()) {
            for (int i = 0; i < totalRows; i++) {
                float y = panelRect.y + 28 + i * 22 + scroll.y;
                if (y >= panelRect.y - 22 && y <= panelRect.y + panelRect.height) {
                    std::string col1, col2;
                    extraerColumnas(resultados[i], modo, col1, col2);
                    DrawTextEx(customFont, col1.c_str(), (Vector2){panelRect.x + 40, y}, 16, 1, BLACK);
                    if (modo != 4 && !col2.empty())
                        DrawTextEx(customFont, col2.c_str(), (Vector2){panelRect.x + 210, y}, 16, 1, BLACK);
                }
            }
        } else {
            DrawTextEx(customFont, "Sin resultados.", (Vector2){panelRect.x + 40, panelRect.y + 40 + scroll.y}, 16, 1, GRAY);
        }
        EndScissorMode();

        if (GuiButton((Rectangle){30, 330, 140, 32}, "Ver Canciones Usuario")) {
            modo = 1;
            resultados.clear();
            mensajeError.clear();
            int usuario = atoi(usuarioID);
            if (usuario <= 0) {
                mensajeError = "Ingrese un ID de usuario válido.";
            } else {
                auto canciones = getCancionesDeUsuario(usuario);
                if (canciones.empty()) {
                    mensajeError = "Usuario no encontrado.";
                } else {
                    for (auto& [idCancion, puntaje] : canciones) {
                        std::ostringstream ss;
                        ss << "   Canción ID: " << idCancion << " - Puntaje: " << puntaje;
                        resultados.push_back(ss.str());
                    }
                }
            }
            scroll.y = 0;
        }

        if (GuiButton((Rectangle){190, 330, 120, 32}, "Top N Canciones")) {
            modo = 2;
            resultados.clear();
            mensajeError.clear();
            int N = atoi(nTop);
            if (N <= 0) N = 10;
            auto topCanciones = obtenerTopNCanciones(N);
            if (topCanciones.empty()) {
                mensajeError = "No hay canciones suficientes.";
            } else {
                for (const auto& [idCancion, promedio] : topCanciones) {
                    std::ostringstream ss;
                    ss << "Canción " << idCancion << " → Promedio: " << std::fixed << std::setprecision(2) << promedio;
                    resultados.push_back(ss.str());
                }
            }
            scroll.y = 0;
        }

        if (GuiButton((Rectangle){330, 330, 170, 32}, "Usuarios Similares")) {
            modo = 3;
            resultados.clear();
            mensajeError.clear();
            int usuario = atoi(usuarioID);
            int N = atoi(nTop);
            if (usuario <= 0) {
                mensajeError = "Ingrese un ID de usuario válido.";
            } else {
                if (N <= 0) N = 6;
                auto usuariosSimilares = obtenerUsuariosSimilares(usuario, similitudes, N);
                if (usuariosSimilares.empty()) {
                    mensajeError = "No se encontraron usuarios similares.";
                } else {
                    for (const auto& [idUsuario, similitud] : usuariosSimilares) {
                        std::ostringstream ss;
                        ss << "Usuario " << idUsuario << " → Similitud: " << (int)(similitud*100) << "%";
                        resultados.push_back(ss.str());
                    }
                }
            }
            scroll.y = 0;
        }

        if (GuiButton((Rectangle){520, 330, 160, 32}, "Recomendar Canciones")) {
            modo = 4;
            resultados.clear();
            mensajeError.clear();
            int usuario = atoi(usuarioID);
            if (usuario <= 0) {
                mensajeError = "Ingrese un ID de usuario válido.";
            } else {
                auto recomendaciones = recomendarCanciones(usuario, similitudes, 10);
                if (recomendaciones.empty()) {
                    mensajeError = "No hay recomendaciones para este usuario.";
                } else {
                    for (const auto& idCancion : recomendaciones) {
                        std::ostringstream ss;
                        ss << "Canción " << idCancion;
                        resultados.push_back(ss.str());
                    }
                }
            }
            scroll.y = 0;
        }

        if (GuiButton((Rectangle){560, 370, 120, 32}, "Salir")) { break; }

        EndDrawing();
    }

    if (customFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(customFont);
    }
    CloseWindow();
    return 0;
}
