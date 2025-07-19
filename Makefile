# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# --- MODIFICACIÓN CLAVE ---
# Banderas del enlazador (Linker):
# -pthread : ¡ESENCIAL! Enlaza la librería de hilos POSIX (Pthreads) que std::thread utiliza.
LDFLAGS = -pthread

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
DATADIR = data

# Target executable
TARGET = song_recommender

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Default target
.PHONY: all
all: $(TARGET)

# Link object files to create executable
# --- LÍNEA MODIFICADA ---
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Create obj directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean compiled files
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Run the program
.PHONY: run
run: $(TARGET)
	./$(TARGET)

# Install (copy to system path)
.PHONY: install
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Debug build
.PHONY: debug
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(TARGET)

# Create data directory and sample CSV
.PHONY: setup
setup:
	mkdir -p $(DATADIR)
	@echo "Creating sample data file..."
	@echo "user_id,song_id,rating" > $(DATADIR)/ratings_big.csv
	@echo "1,101,4.5" >> $(DATADIR)/ratings_big.csv
	@echo "1,102,3.0" >> $(DATADIR)/ratings_big.csv
	@echo "2,101,4.0" >> $(DATADIR)/ratings_big.csv
	@echo "2,103,5.0" >> $(DATADIR)/ratings_big.csv

# Show help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove compiled files"
	@echo "  run      - Build and run the program"
	@echo "  debug    - Build with debug symbols"
	@echo "  setup    - Create data directory with sample CSV"
	@echo "  install  - Install to system path"
	@echo "  help     - Show this help message"