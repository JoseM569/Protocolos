#include "funcionesMenu.h"
#include <iostream> 
#include <string>   
#include <stdexcept>  

int main() {
    if (wiringPiSetupGpio() == -1) {
        printf("ERROR: No se pudo inicializar WiringPi.\n");
        return 1;
    }
    
    pinMode(TX_PIN, OUTPUT);
    digitalWrite(TX_PIN, HIGH); // Poner la línea en reposo

    std::string input_linea;
    long opt = 0;

    for (;;) {
        puts(""); 
        for (size_t i = 0; i < sizeof(menu)/sizeof(menu[0]); ++i) {
            puts(menu[i]);
        }
        printf("Seleccione opción [0-9]: ");

        // --- Lectura de Opción (Estilo C++) ---
        if (!std::getline(std::cin, input_linea)) {
            clearerr(stdin); // Limpiar error si algo falla
            std::cin.clear();
            continue; 
        }
        
        // ¡¡ESTA ES LA CORRECCIÓN CLAVE!!
        // Si la línea está vacía (porque el usuario solo presionó Enter,
        // o por un '\n' fantasma), simplemente vuelve a mostrar el menú.
        if (input_linea.empty()) {
            continue;
        }
        // --- Fin Corrección ---

        try {
            opt = std::stol(input_linea); // Usamos stol (requiere -std=c++0x)
        } catch (const std::invalid_argument& e) {
            puts("Entrada inválida."); 
            continue; 
        }

        if (opt < 0 || opt > 9) { 
            puts("Fuera de rango (0-9)."); 
            continue; 
        }
        if (opt == 0) { 
            puts("Saliendo..."); 
            break; 
        }

        // Despacho de opción
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
    }
    return 0;
}