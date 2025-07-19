# Sistema de Recomendaciones de Canciones

Un sistema de recomendaciones implementado en C++ que utiliza datos de calificaciones de usuarios para sugerir canciones basándose en similitud entre usuarios.

## 🚀 Compilación y Ejecución

### Prerrequisitos
- **Compilador**: g++ con soporte para C++17
- **Sistema Operativo**: Linux, Windows, macOS
- **Dependencias**: Solo STL (sin librerías externas)

### Linux/macOS
```bash
make
```

### Windows (con MinGW/MSYS2)
```cmd
mingw32-make -f Makefile.win
# o si tienes make instalado:
make -f Makefile.win
```

### Windows (compilación manual)
```cmd
g++ -std=c++17 -Iinclude src/*.cpp -o song_recommender.exe
```

### Ejecutar el programa

**Linux/macOS:**
```bash
make run
# o directamente:
./song_recommender
```

**Windows:**
```cmd
mingw32-make -f Makefile.win run
# o directamente:
song_recommender.exe
```

### Limpiar archivos compilados

**Linux/macOS:**
```bash
make clean
```

**Windows:**
```cmd
mingw32-make -f Makefile.win clean
```

## 📂 Estructura del Proyecto

```
song_recommender/
├── data/
│   └── ratings_big.csv              # Datos: usuario_id,cancion_id,rating
├── include/
│   ├── DataLoader.hpp           # Carga y parseo del CSV
│   ├── TopSongs.hpp             # Top N canciones más valoradas
│   ├── Similarity.hpp           # Cálculo de similitud entre usuarios
│   └── Recommender.hpp          # Generación de recomendaciones
├── src/
│   ├── main.cpp                 # Programa principal con menú
│   ├── DataLoader.cpp
│   ├── TopSongs.cpp
│   ├── Similarity.cpp
│   └── Recommender.cpp
├── obj/                         # Archivos objeto (generados)
├── Makefile
└── README.md
```

## 📊 Formato de Datos

El archivo `data/ratings_big.csv` debe tener el formato:
```csv
user_id,song_id,rating
1,101,4.5
1,102,3.0
2,101,4.0
...
```

## 🎯 Funcionalidades

### 1. **Ver Canciones de Usuarios**
- Muestra todas las canciones calificadas por un usuario específico
- Incluye ID de canción y puntuación otorgada

### 2. **Top N Canciones**
- Lista las canciones mejor valoradas por promedio
- Ordenadas de mayor a menor puntuación promedio
- Configurable el número de canciones a mostrar

### 3. **Usuarios Similares**
- Encuentra los 6 usuarios más similares a uno dado
- Utiliza correlación de Pearson como métrica principal
- Muestra porcentaje de similitud

### 4. **Recomendaciones Personalizadas**
- Sugiere canciones basándose en usuarios similares
- Evita recomendar canciones ya calificadas
- Usa puntuación ponderada por similitud

## 🧮 Algoritmos Implementados

### Métricas de Similitud
- **Pearson**: Correlación entre vectores de calificaciones
- **Euclidiana**: Distancia euclidiana invertida
- **Coseno**: Similitud del coseno entre vectores

### Sistema de Recomendación
- Filtrado colaborativo basado en usuarios
- Puntuación ponderada por similitud
- Ranking por puntuación estimada

## 💡 Uso del Sistema

1. **Preparar datos**: Colocar archivo CSV en `data/ratings_big.csv`
2. **Compilar**: `make`
3. **Ejecutar**: `./song_recommender`
4. **Navegar**: Usar el menú interactivo para explorar funcionalidades

### Ejemplo de Sesión
```
=== SISTEMA DE RECOMENDACIONES DE CANCIONES ===
1. Ver canciones de usuarios específicos
2. Top N canciones por promedio
3. Calcular usuarios similares
4. Obtener recomendaciones para un usuario
0. Salir
================================================
Seleccione una opción: 4
Ingrese el ID del usuario: 1

=== RECOMENDACIONES PARA USUARIO 1 ===
Canciones recomendadas:
1. Canción ID: 105
2. Canción ID: 108
3. Canción ID: 203
...
```

## 📈 Rendimiento

- **Carga de datos**: O(R) donde R = número de ratings
- **Top N canciones**: O(R + S log S) donde S = número de canciones
- **Similitudes**: O(U² × K) donde U = usuarios, K = canciones promedio
- **Recomendaciones**: O(K log N) por usuario

## 🔧 Personalización

- Cambiar métrica de similitud en `Similarity.cpp`
- Ajustar número de usuarios similares (actualmente 6)
- Modificar cantidad de recomendaciones (actualmente 10)
- Agregar filtros adicionales en `Recommender.cpp`
