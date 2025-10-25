# Objetivo por defecto: compilar el programa. Se ejecuta con "make" o "make all".
all: run

# Regla para crear el ARCHIVO EJECUTABLE llamado 'run'.
# Depende de los archivos .o.
run: main.o funcionesProtocolo.o funcionesMenu.o
	g++ -Wall -o run main.o funcionesProtocolo.o funcionesMenu.o -lwiringPi

# Reglas para crear los archivos objeto (.o) a partir de los archivos de código (.cpp).
main.o: main.cpp
	g++ -Wall -c main.cpp

funcionesProtocolo.o: funcionesProtocolo.cpp
	g++ -Wall -c funcionesProtocolo.cpp

funcionesMenu.o: funcionesMenu.cpp
	g++ -Wall -c funcionesMenu.cpp

# --- ACCIONES ---

# Regla para ejecutar el programa.
# Primero se asegura de que el archivo 'run' esté compilado y luego lo ejecuta.
run_program: run
	./run

# Regla para limpiar el directorio.
clean:
	rm -f *.o run

# Declara los objetivos que son acciones y no archivos.
.PHONY: all clean run_program