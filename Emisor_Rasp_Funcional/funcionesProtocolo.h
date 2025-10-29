/**
 * @file funcionesProtocolo.h
 * @brief Declaraciones de las funciones de bajo nivel del protocolo.
 * @details Estas son las funciones "core" que arman el paquete (empaquetar),
 * calculan el checksum (fcs) y envían los bits (enviarFrame).
 */

// Incluimos la estructura principal para que las funciones
// sepan qué es un 'protocolo' y un 'BYTE'.
#include "structProtocolo.h"

/**
 * @brief Arma el 'proto.frame' a partir de los datos en 'proto.data'.
 * @details Esta función toma cmd, lng y data, los empaqueta en el 'proto.frame',
 * calcula el FCS, y añade el FCS al final del frame.
 * @param proto Una referencia (por eso el '&') a la estructura del protocolo.
 * Se modifica directamente.
 * @return El largo total en bytes del frame que se debe enviar.
 */
int empaquetar(protocolo & proto);

/**
 * @brief Calcula el Frame Check Sequence (FCS) para un array de bytes.
 * @details Nuestro FCS es un simple conteo de todos los 'bits' activos (bits en '1').
 * @param array Puntero al array de bytes (ej: proto.frame).
 * @param tam El número de bytes del array que se deben revisar.
 * @return Un 'unsigned short' con el conteo total de bits activos.
 */
unsigned short fcs(BYTE * array,int tam);

/**
 * @brief Transmite el frame completo, bit por bit (bit-banging).
 * @details Esta es la función de transmisión manual (UART asíncrono).
 * Envía cada byte con 1 bit de inicio (LOW), 8 bits de datos (LSB primero)
 * y 2 bits de parada (HIGH). Al final del frame, envía 1 bit de paridad.
 * @param pin El pin GPIO de la RPi que se usará para transmitir (ej: TX_PIN).
 * @param speed La velocidad (ej: 10) para calcular el delay.
 * @param proto La estructura que contiene el 'proto.frame' a enviar.
 * @param largo El largo total del frame (devuelto por empaquetar()).
 */
void enviarFrame(int pin, int speed, protocolo proto, int largo);