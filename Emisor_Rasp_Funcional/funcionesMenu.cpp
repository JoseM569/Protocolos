/**
 * @file funcionesMenu.cpp
 * @brief Implementación de la lógica para cada opción del menú del emisor.
 * @details Incluye la validación de entrada, manejo de contadores,
 * y el array rotativo de temperaturas.
 */

#include "funcionesMenu.h"
#include <iostream>     // Para std::cin, std::getline
#include <string>       // Para std::string, std::stof, std::stoi
#include <string.h>     // Para memset, memcpy, strlen, strncpy
#include <stdexcept>    // Para std::invalid_argument (para la validación)
#include <sstream>      // Para std::ostringstream (para formatear array)
#include <unistd.h>     // Para usleep() / sleep()

// --- Constantes y variables globales (Definición) ---

// Definición del array de strings para el menú (declarado 'extern' en el .h)
const char* menu[11] = {
        "===== MENÚ EMISOR (PREVIA) =====",
        "1) Mostrar mensaje de control/imagen en OLED",
        "2) Enviar 10 mensajes de prueba",
        "3) Enviar texto para mostrar en OLED",
        "4) Enviar temperatura flotante [-40.0, 40.0]",
        "5) Toggle parpadeo LED (gpio 25)",
        "6) Cambiar frecuencia parpadeo LED [1..100] Hz",
        "7) Solicitar impresión de contador/estadísticas (receptor)",
        "8) Enviar arreglo con últimas 8 temperaturas (extra)",
        "9) Mostrar contador local de mensajes enviados",
        "0) Salir"
    };

protocolo tx; // Variable global para la estructura del protocolo
int g_contador_local_emisor = 0; // Contador para Opción 9
float g_temperaturas[8] = {0.0f};    // Array rotativo para Opción 8
int g_temp_index = 0;              // Índice del array rotativo

/**
 * @brief Función auxiliar (wrapper) para enviar un frame.
 * @details Llama a la función 'enviarFrame' original y luego
 * incrementa el contador local de mensajes enviados.
 */
void enviarFrameConContador(int pin, int speed, protocolo& proto, int largo) {
    enviarFrame(pin, speed, proto, largo);
    g_contador_local_emisor++; // Incrementa el contador local (Opción 9)
}


// --- Implementación de Funciones del Menú ---

/**
 * @brief Opción 1: Envía un comando de control simple (CMD 0).
 */
void opcion_1(){
    // Limpiar TODA la estructura 'tx' al inicio.
    // Esto es necesario para poner 'tx.lng' en 0 y evitar que se
    // envíen datos de una opción anterior.
    memset(&tx, 0, sizeof(protocolo));
    
    tx.cmd = 0;
    // tx.lng es 0 (gracias al memset)
    int largo = empaquetar(tx);
    printf("Mensaje de control enviado (CMD 0).\n");
    enviarFrameConContador(TX_PIN,SPEED,tx,largo);
}

/**
 * @brief Opción 2: Pide un mensaje de prueba y lo envía 10 veces.
 */
void opcion_2(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 1;
    
    printf("Ingrese un mensaje de prueba (se enviará 10 veces, max 62 char): ");
    std::cin.getline(reinterpret_cast<char*>(tx.data), 63); // Leer 62 chars + 1 nulo
    
    tx.lng = strlen(reinterpret_cast<const char*>(tx.data)); 

    if (tx.lng == 0) {
        printf("Mensaje vacío. No se enviará nada.\n");
        return;
    }

    int largo = empaquetar(tx);
    
    // Bucle para enviar 10 veces
    printf("Enviando 10 mensajes de prueba...\n");
    for (int i = 0; i < 10; i++) {
        enviarFrameConContador(TX_PIN, SPEED, tx, largo);
        printf("Mensaje %d/10 enviado.\n", i + 1);
        
        // Pausa entre mensajes.
        // Damos 1.5s de pausa para que el receptor (ESP32) tenga tiempo
        // de procesar el mensaje Y ejecutar su 'flushRX()' (que dura 1s)
        // si hubo un error. Esto evita saturar al receptor.
        usleep(1500 * 1000); // 1.5 segundos de espera
    }
    printf("Envío de 10 mensajes completado.\n");
}

/**
 * @brief Opción 3: Pide un texto y lo envía al OLED (CMD 2).
 */
void opcion_3(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 2;
    
    printf("Ingrese un mensaje para mostrar en OLED (max 62 char): ");
    std::cin.getline(reinterpret_cast<char*>(tx.data), 63); 
    
    tx.lng = strlen( reinterpret_cast<const char*>(tx.data) ); 
    
    if (tx.lng > 0) {
        int largo = empaquetar(tx);
        enviarFrameConContador(TX_PIN,SPEED,tx,largo);
        printf("Mensaje OLED enviado.\n");
    } else {
        printf("Mensaje vacío. No se envió nada.\n");
    }
}

/**
 * @brief Opción 4: Pide y valida una temperatura (CMD 3).
 * @details También guarda la temperatura en el array rotativo (Opción 8).
 */
void opcion_4(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 3;
    
    printf("Ingrese una temperatura flotante [-40.0, 40.0]: ");
    
    std::string input;
    std::getline(std::cin, input); // Leer como string

    try {
        // --- Validación ---
        float temp = std::stof(input); // Convertir string a float
        
        if (temp >= -40.0f && temp <= 40.0f) { // Validar rango
            
            // Copiar el TEXTO (ej: "25.5") al buffer
            strncpy(reinterpret_cast<char*>(tx.data), input.c_str(), LARGO_DATA - 1);
            tx.lng = strlen(reinterpret_cast<const char*>(tx.data));
            
            int largo = empaquetar(tx);
            enviarFrameConContador(TX_PIN,SPEED,tx,largo);

            // --- Lógica Array Rotativo ---
            g_temperaturas[g_temp_index] = temp;
            g_temp_index = (g_temp_index + 1) % 8; // Rota el índice (0-7)
            printf("Temperatura %.1fC enviada y almacenada.\n", temp);

        } else {
            printf("Error: Temperatura fuera de rango [-40.0, 40.0]. No se envió.\n");
        }
    } catch (const std::invalid_argument& e) {
        // Atrapa si el usuario escribió "hola" en lugar de "23.5"
        printf("Error: Entrada no válida. Debe ser un número (ej: 23.5). No se envió.\n");
    }
}

/**
 * @brief Opción 5: Envía comando para "Togglear" el LED (CMD 4).
 */
void opcion_5(){
    memset(&tx, 0, sizeof(protocolo));
    printf("Activando/Desactivando parpadeo del LED (gpio 25)\n");
    tx.cmd = 4;
    int largo = empaquetar(tx);
    enviarFrameConContador(TX_PIN,SPEED,tx,largo);
}

/**
 * @brief Opción 6: Pide y valida una frecuencia para el LED (CMD 5).
 */
void opcion_6(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 5;
    
    printf("Ingrese la frecuencia deseada del LED [1..100] Hz: ");
    
    std::string input;
    std::getline(std::cin, input); 

    try {
        // --- Validación ---
        int freq = std::stoi(input); // Convertir string a int
        
        if (freq >= 1 && freq <= 100) { // Validar rango
            
            // Copiar el TEXTO (ej: "50") al buffer
            strncpy(reinterpret_cast<char*>(tx.data), input.c_str(), LARGO_DATA - 1);
            tx.lng = strlen(reinterpret_cast<const char*>(tx.data));
            
            int largo = empaquetar(tx);
            enviarFrameConContador(TX_PIN,SPEED,tx,largo);
            printf("Frecuencia %d Hz enviada.\n", freq);
            
        } else {
            printf("Error: Frecuencia fuera de rango [1..100]. No se envió.\n");
        }
    } catch (const std::invalid_argument& e) {
        printf("Error: Entrada no válida. Debe ser un número entero. No se envió.\n");
    }
}

/**
 * @brief Opción 7: Envía comando para pedir estadísticas al receptor (CMD 6).
 */
void opcion_7(){
    memset(&tx, 0, sizeof(protocolo));
    printf("Solicitando impresión de contador/estadísticas (receptor)\n");
    tx.cmd = 6;
    int largo = empaquetar(tx);
    enviarFrameConContador(TX_PIN,SPEED,tx,largo);
}

/**
 * @brief Opción 8: Envía el array rotativo de 8 temperaturas (CMD 7).
 */
void opcion_8(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 7;
    printf("Enviando ultimas 8 temperaturas (Extra)...\n");

    // Usamos stringstream para construir el string "T1=20.5 T2=21.0 ..."
    std::ostringstream ss;
    ss << "Ultimas 8 Temps: ";
    for (int i = 0; i < 8; i++) {
        ss << g_temperaturas[i] << "C ";
    }
    std::string temp_string = ss.str();

    // Asegurarnos de no pasarnos del buffer
    if (temp_string.length() > (LARGO_DATA - 1)) {
        temp_string = temp_string.substr(0, LARGO_DATA - 1);
    }

    // Copiar el string formateado a los datos
    strncpy(reinterpret_cast<char*>(tx.data), temp_string.c_str(), LARGO_DATA - 1);
    tx.lng = strlen(reinterpret_cast<const char*>(tx.data));

    int largo = empaquetar(tx);
    enviarFrameConContador(TX_PIN,SPEED,tx,largo);
}

/**
 * @brief Opción 9: Muestra el contador local.
 */
void opcion_9(){
    // Esta función NO envía nada
    // Solo muestra el contador local en la RPi.
    printf("--- Contador Local del Emisor ---\n");
    printf("Total de mensajes enviados hasta ahora: %d\n", g_contador_local_emisor);
}