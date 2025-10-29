/**
 * @file funcionesMenu.h
 * @brief Declaraciones de las funciones que implementan la lógica del menú del emisor.
 * @details Cada 'opcion_X' corresponde a una acción del menú.
 */

// Incluimos el protocolo para que las funciones
// sepan cómo llamar a empaquetar() y enviarFrame().
#include "funcionesProtocolo.h"

#ifndef FUNCIONES_MENU_H
#define FUNCIONES_MENU_H

/**
 * @brief Array 'extern' que contiene el texto del menú.
 * 'extern' significa que está definido en otro archivo (funcionesMenu.cpp).
 */
extern const char* menu[11];

// --- Declaraciones de Funciones de Opción ---

void opcion_1();
void opcion_2();
void opcion_3();
void opcion_4();
void opcion_5();
void opcion_6();
void opcion_7();
void opcion_8();
void opcion_9();
// (opcion_0 se maneja en el main.cpp, por eso no se declara aquí)

#endif // FUNCIONES_MENU_H