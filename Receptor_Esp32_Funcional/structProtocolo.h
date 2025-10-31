/**
 * @file structProtocolo.h
 * @brief Define la estructura de datos principal para el protocolo de comunicación (versión ESP32).
 * @details Este archivo es compartido (pero con diferencias menores) por el Emisor (RPi) y el Receptor (ESP32).
 * Define el tamaño de los campos, los pines y la velocidad de la comunicación.
 */

#ifndef STRUCT_PROTOCOLO_H
#define STRUCT_PROTOCOLO_H

// --- Includes (Comunes para ESP32/Arduino) ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Definiciones del Protocolo (Idénticas al Emisor) ---

/**
 * @brief Definición de un BYTE como un 'unsigned char' (8 bits, 0-255).
 */
#define BYTE unsigned char

/**
 * @brief Tamaño máximo del buffer de DATOS (payload).
 * @details 63 bytes, coincide con el valor máximo del campo 'lng' de 6 bits.
 */
#define LARGO_DATA 63

/**
 * @brief Número de bytes "extra" en el frame además de los datos.
 * @details (1 byte CMD) + (1 byte LNG) + (2 bytes FCS) = 4 bytes.
 */
#define BYTES_EXTRA 4

// --- Definiciones de Hardware (Específicas del Receptor - ESP32) ---

/**
 * @brief Pin GPIO de la ESP32 que se usará para RECIBIR.
 */
#define RX_PIN 13

/**
 * @brief Velocidad de la comunicación en bits por segundo (baudios).
 * @details Debe ser IDÉNTICA a la del Emisor (RPi).
 * Usamos 10 bits/seg (100ms por bit), que fue la velocidad más estable encontrada
 * para superar el "Jitter" de la RPi y el "Clock Drift" entre las placas.
 */
#define SPEED 10 

// --- Estructura Principal del Protocolo (Idéntica al Emisor) ---

typedef struct
{
    /**
     * @brief El Comando (CMD) (0-8) recibido.
     * @details Se desempaqueta de los 4 bits del primer byte del frame.
     */
    BYTE cmd;

    /**
     * @brief El Largo (LNG) de los datos recibidos.
     * @details Indica cuántos bytes hay en el campo 'data'.
     * Se desempaqueta de los 6 bits del segundo byte del frame (máx 63).
     */
    BYTE lng;

    /**
     * @brief Buffer del Payload (Datos) recibido.
     */
    BYTE data[LARGO_DATA];

    /**
     * @brief El buffer donde se reconstruye el frame recibido.
     * @details Tamaño = 63 (data) + 4 (extra) = 67 bytes.
     */
    BYTE frame[LARGO_DATA + BYTES_EXTRA];

    /**
     * @brief Frame Check Sequence (FCS) - (Checksum) recibido.
     * @details Se reconstruye a partir de los últimos 2 bytes del frame.
     */
    unsigned short fcs;

} protocolo;


#endif // STRUCT_PROTOCOLO_H