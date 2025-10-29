/**
 * @file main.cpp
 * @brief Punto de entrada principal para el programa Emisor (Raspberry Pi).
 * @details Este archivo inicializa wiringPi, muestra el menú en un bucle
 * y despacha las acciones a las funciones 'opcion_X'.
 */

#include "funcionesMenu.h"
#include <iostream>  // Para std::cout, std::cin, std::getline
#include <string>    // Para std::string, std::stol
#include <stdexcept> // Para std::invalid_argument (manejo de errores de conversión)

int main() {
    
    // --- Inicialización de Hardware (RPi) ---
    
    //Usamos 'wiringPiSetupGpio()' como depuramos.
    // Esto nos permite referirnos a los pines por su número BCM (ej: TX_PIN 17).
    if (wiringPiSetupGpio() == -1) {
        printf("ERROR: No se pudo inicializar WiringPi. (¿Ejecutaste con sudo?)\n");
        return 1; // Salir con error
    }
    
    // Configura el pin de transmisión como SALIDA
    pinMode(TX_PIN, OUTPUT);
    // Pone la línea en HIGH (estado de reposo) inmediatamente.
    // Esto evita que el receptor (ESP32) detecte ruido al inicio.
    digitalWrite(TX_PIN, HIGH); 

    std::string input_linea; // Variable para leer la entrada del usuario
    long opt = 0;

    // --- Bucle Infinito del Menú ---
    for (;;) {
        // Imprimir el menú
        puts(""); 
        for (size_t i = 0; i < sizeof(menu)/sizeof(menu[0]); ++i) {
            puts(menu[i]);
        }
        printf("Seleccione opción [0-9]: ");

        // --- Lectura de Opción ---
        
        // Usamos std::getline para leer la línea entera.
        // Eso evita conflictos de búfer (que sí ocurren con 'scanf').
        if (!std::getline(std::cin, input_linea)) {
            clearerr(stdin); // Limpiar flag de error (ej. si presionan Ctrl+D)
            std::cin.clear();
            continue; 
        }
        
        // Si la línea está vacía (porque el usuario solo presionó Enter),
        // simplemente volvemos a mostrar el menú, sin dar un error.
        if (input_linea.empty()) {
            continue;
        }

        try {
            // Convertimos el string (ej: "3") a un número (long).
            // (Requiere la bandera -std=c++0x en el makefile)
            opt = std::stol(input_linea); 
        } catch (const std::invalid_argument& e) {
            // Detecta si el usuario escribió texto y no un numero.
            puts("Entrada inválida."); 
            continue; 
        }
        // --- Fin Lectura ---

        // Validación de rango
        if (opt < 0 || opt > 9) { 
            puts("Fuera de rango (0-9)."); 
            continue; 
        }
        // Opción de salida
        if (opt == 0) { 
            puts("Saliendo..."); 
            break; // Rompe el bucle 'for (;;)'
        }

        // --- Despacho de Opción ---
        // Llama a la función correspondiente según el número.
        switch ((int)opt) {
            case 1: opcion_1(); break;
            case 2: opcion_2(); break;
            case 3: opcion_3(); break;
            case 4: opcion_4(); break;
            case 5: opcion_5(); break;
            case 6: opcion_6(); break;
            case 7: opcion_7(); break;
            case 8: opcion_8(); break;
            case 9: opcion_9(); break;
            default: puts("Opción no reconocida."); break; 
        }
    } // Fin del bucle 'for (;;)'
    
    return 0; // Salir del programa
}