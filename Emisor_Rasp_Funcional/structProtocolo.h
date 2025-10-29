/**
 * @file structProtocolo.h
 * @brief Define la estructura de datos principal para el protocolo de comunicación.
 * @details Este archivo es compartido por el Emisor (RPi) y el Receptor (ESP32).
 * Define el tamaño de los campos, los pines y la velocidad de la comunicación.
 */


#ifndef STRUCT_PROTOCOLO_H
#define STRUCT_PROTOCOLO_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <wiringPi.h>
#include <stdexcept>    // Para std::invalid_argument (para la validación)
#include <sstream>      // Para std::ostringstream (para formatear array)
#include <unistd.h>     // Para usleep() / sleep() (si es necesario un delay)


// --- Definiciones del Protocolo ---

/**
 * @brief Definición de un BYTE como un 'unsigned char' (8 bits, 0-255).
 */
#define BYTE unsigned char

/**
 * @brief Tamaño máximo del buffer de DATOS (payload).
 * @details Se eligió 63 bytes porque coincide con el valor máximo
 * que se puede almacenar en el campo 'lng' de 6 bits (2^6 - 1 = 63).
 */
#define LARGO_DATA 63

/**
 * @brief Número de bytes "extra" en el frame además de los datos.
 * @details (1 byte CMD) + (1 byte LNG) + (2 bytes FCS) = 4 bytes.
 */
#define BYTES_EXTRA 4 

// --- Definiciones de Hardware (Específicas del Emisor - RPi) ---

/**
 * @brief Pin GPIO de la RPi (numeración BCM) que se usará para TRANSMITIR.
 */
#define TX_PIN 17

/**
 * @brief Velocidad de la comunicación en bits por segundo (baudios).
 * @details Se define 10 bits/seg (100ms por bit).
 * Esta velocidad es extremadamente lenta, pero es la ÚNICA que resultó
 * 100% estable. Velocidades más rápidas (como 25 o 50) fallaban
 * intermitentemente debido al "Jitter" (retrasos del SO Linux de la RPi)
 * y al "Clock Drift" (diferencia de reloj) con la ESP32.
 */
#define SPEED 10

// --- Estructura Principal del Protocolo ---

typedef struct 
{
    /**
     * @brief El Comando (CMD) (0-8).
     * @details Se empaqueta usando solo 4 bits (0x0F).
     */
    BYTE cmd;
    
    /**
     * @brief El Largo (LNG) de los datos.
     * @details Indica cuántos bytes hay en el campo 'data'.
     * Se empaqueta usando solo 6 bits (0x3F), permitiendo un máx de 63.
     */
    BYTE lng;
    
    /**
     * @brief Buffer del Payload (Datos).
     * @details Aquí se guardan los mensajes de texto, temperaturas, etc.
     */
    BYTE data[LARGO_DATA];
    
    /**
     * @brief El buffer del frame completo que se envía por el cable.
     * @details Tamaño = 63 (data) + 4 (extra) = 67 bytes.
     * Contiene [CMD empaquetado] [LNG empaquetado] [DATA...] [FCS Alto] [FCS Bajo]
     */
    BYTE frame[LARGO_DATA + BYTES_EXTRA];
    
    /**
     * @brief Frame Check Sequence (FCS) - (Checksum).
     * @details Usamos un 'unsigned short' (2 bytes) porque nuestro FCS
     * (un conteo de todos los bits activos) puede ser mayor que 255
     * en un paquete grande, por lo que 1 byte no era suficiente.
     */
    unsigned short fcs;

} protocolo;


#endif // STRUCT_PROTOCOLO_H
#endif
