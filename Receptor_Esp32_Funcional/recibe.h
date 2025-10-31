/**
 * @file recibe.h
 * @brief Declaraciones de las funciones para la recepción no bloqueante y validación.
 * @details Define la interfaz para la máquina de estados de recepción y las
 * funciones de validación (desempaquetar, fcs).
 */

#ifndef RECIBE_H
#define RECIBE_H

// Incluimos la estructura para que las funciones sepan qué es 'protocolo' y 'BYTE'.
#include "structProtocolo.h"

// --- Máquina de Estados de Recepción No-Bloqueante ---

/**
 * @brief Inicializa las variables globales de la máquina de estados de recepción.
 * @details Debe llamarse una vez en el setup().
 */
void setupRecepcion();

/**
 * @brief Función principal de la máquina de estados. Se llama en CADA ciclo del loop().
 * @details Revisa si ha llegado un nuevo bit (usando millis()), reconstruye el
 * paquete byte a byte sin bloquear el programa, y llama a las funciones de
 * validación y ejecución ('desempaquetar', 'ejecutarComando') cuando
 * un paquete completo y válido es recibido.
 */
void revisarRecepcion();

// --- Funciones de Validación (Usadas por la máquina de estados) ---

/**
 * @brief Desempaqueta los datos y valida el FCS del frame recibido.
 * @details Copia los bytes de 'proto.frame' a 'proto.data', añade el
 * terminador nulo ('\0'), reconstruye el FCS recibido y lo compara
 * con un FCS calculado sobre los datos recibidos.
 * @param proto Referencia a la estructura del protocolo que contiene el frame
 * recibido (se modifica para añadir el '\0').
 * @return 'true' si el FCS coincide (paquete válido), 'false' si no coincide.
 */
bool desempaquetar(protocolo & proto);

/**
 * @brief Calcula el Frame Check Sequence (FCS) (conteo de bits activos).
 * @details Idéntica a la función del emisor.
 * @param array Puntero al array de bytes (ej: proto.frame).
 * @param tam El número de bytes del array que se deben revisar (CMD+LNG+DATA).
 * @return Un 'unsigned short' con el conteo total de bits activos.
 */
unsigned short fcs(BYTE * array, int tam);

#endif // RECIBE_H