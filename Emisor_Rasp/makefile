# --- Definimos las banderas del compilador (Flags) ---
#  -Wall (activa todos los warnings)
#  -std=c++0x (activa el estándar C++11 experimental para compiladores antiguos)
CXXFLAGS = -Wall -std=c++0x

# Objetivo por defecto: compilar el programa
all: run

# Regla para crear el ARCHIVO EJECUTABLE 'run'
run: main.o funcionesProtocolo.o funcionesMenu.o
	g++ $(CXXFLAGS) -o run main.o funcionesProtocolo.o funcionesMenu.o -lwiringPi

# Reglas para crear los archivos objeto (.o)
main.o: main.cpp
	g++ $(CXXFLAGS) -c main.cpp

funcionesProtocolo.o: funcionesProtocolo.cpp
	g++ $(CXXFLAGS) -c funcionesProtocolo.cpp

funcionesMenu.o: funcionesMenu.cpp
	g++ $(CXXFLAGS) -c funcionesMenu.cpp

# --- ACCIONES ---

run_program: run
	./run

clean:
	rm -f *.o run

.PHONY: all clean run_program