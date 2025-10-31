/**
 * @file recibe.cpp
 * @brief Implementación de la máquina de estados de recepción no bloqueante.
 * @details Contiene la lógica principal que lee bits usando 'millis()' sin
 * bloquear el loop, permitiendo que otras tareas (como el parpadeo del LED)
 * se ejecuten concurrentemente.
 */

#include "recibe.h"
#include "Arduino.h" // Necesario para millis(), digitalRead(), delay()
#include "funcionesReceptor.h" // Para llamar a ejecutarComando, flushRX, actualizarContadores

// --- Variables de la Máquina de Estados (static para ser privadas a este archivo) ---

/**
 * @enum EstadoRecepcion
 * @brief Define los posibles estados de la máquina de recepción.
 */
enum EstadoRecepcion {
    ESTADO_IDLE,         // Esperando un bit de inicio (LOW)
    ESTADO_RECIBIENDO,   // Recibiendo los 8 bits de datos de un byte
    ESTADO_PARADA_1,     // Esperando/verificando el primer bit de parada (HIGH)
    ESTADO_PARADA_2,     // Esperando/verificando el segundo bit de parada (HIGH)
    ESTADO_PARIDAD_FRAME // Esperando/verificando el bit de paridad final del frame
};

static EstadoRecepcion g_estado = ESTADO_IDLE; // Estado actual de la máquina
static unsigned long g_tiempoSiguienteBit = 0; // Marca de tiempo (millis()) para leer el próximo bit
static unsigned long g_bitTime_ms = 1000 / SPEED; // Tiempo de duración de un bit (100ms)
static unsigned long g_halfBitTime_ms = g_bitTime_ms / 2; // Mitad del tiempo de un bit (50ms)

static protocolo proto_en_construccion; // Estructura temporal donde se arma el paquete
static int g_byte_index = 0;   // Índice del byte actual que estamos recibiendo (0 a lng+3)
static int g_bit_index = 0;    // Índice del bit actual dentro del byte (0 a 7)
static BYTE g_byte_actual = 0; // Variable temporal para armar el byte bit a bit
static int g_paridad_frame_calc = 0; // Acumulador para calcular la paridad de todo el frame

/**
 * @brief Inicializa la máquina de estados. Se llama desde setup().
 */
void setupRecepcion() {
    g_estado = ESTADO_IDLE; // Empezar esperando un bit de inicio
}

/**
 * @brief Función principal de la máquina de estados. Se llama en CADA loop().
 */
void revisarRecepcion() {
    unsigned long tiempoActual = millis(); // Obtener tiempo actual

    // --- Switch principal de la máquina de estados ---
    switch (g_estado) {

        // --- Estado: Esperando Bit de Inicio ---
        case ESTADO_IDLE: {
            // Si la línea está en BAJO, hemos detectado un posible bit de inicio
            if (digitalRead(RX_PIN) == LOW) {
                // ¡Lógica clave de Sincronización!
                // Calculamos cuándo debemos leer el *primer bit de dato*.
                // Esperamos 1.5 "bit-times" (50ms + 100ms = 150ms desde ahora)
                // para posicionar el muestreo justo en el CENTRO del primer bit de datos.
                g_tiempoSiguienteBit = tiempoActual + g_halfBitTime_ms + g_bitTime_ms;

                // Reiniciamos las variables para empezar a armar un nuevo byte
                g_byte_actual = 0;
                g_bit_index = 0;

                // Cambiamos al estado de recepción de bits
                g_estado = ESTADO_RECIBIENDO;
                // Serial.println("DEBUG: Start bit detectado!"); // (Descomentar para depurar)
            }
            // Si la línea sigue en ALTO, no hacemos nada y salimos.
            // El loop() se repetirá y volveremos a comprobar.
            break;
        }

        // --- Estado: Recibiendo los 8 Bits de Datos ---
        case ESTADO_RECIBIENDO: {
            // Comprobamos si ya ha pasado el tiempo calculado para leer el bit actual
            if (tiempoActual >= g_tiempoSiguienteBit) {

                // Leemos el nivel del pin (HIGH o LOW)
                bool level = digitalRead(RX_PIN);

                // Armamos el byte bit a bit (LSB primero)
                if (level) {
                    // Si el bit es 1 (HIGH), lo añadimos al byte actual en la posición correcta
                    g_byte_actual |= (1 << g_bit_index);
                }

                g_bit_index++; // Pasamos al siguiente bit (0 -> 7)

                // Calculamos cuándo debemos leer el *próximo* bit (1 bit-time desde ahora)
                g_tiempoSiguienteBit = tiempoActual + g_bitTime_ms;

                // Si ya hemos leído los 8 bits...
                if (g_bit_index >= 8) {
                    // Cambiamos al estado para verificar el primer bit de parada
                    g_estado = ESTADO_PARADA_1;
                    // Serial.printf("DEBUG: Byte %d armado: 0x%02X\n", g_byte_index, g_byte_actual); // (Depuración)
                }
            }
            // Si aún no es tiempo, no hacemos nada y salimos.
            break;
        }

        // --- Estado: Verificando Primer Bit de Parada ---
        case ESTADO_PARADA_1: {
            // Comprobamos si ya es tiempo de leer el bit de parada 1
            if (tiempoActual >= g_tiempoSiguienteBit) {
                // ¡Validación Crucial! El bit de parada DEBE ser HIGH.
                if (digitalRead(RX_PIN) == LOW) {
                    // Error: El emisor no envió HIGH. La sincronización se rompió.
                    Serial.println("Error: Fallo de Stop Bit 1");
                    actualizarContadores(-1, false); // Marcar error de sincronización
                    flushRX(); // Limpiar la línea RX
                    g_estado = ESTADO_IDLE; // Volver al estado inicial
                    return; // Salir inmediatamente de la función
                }

                // El primer bit de parada fue correcto (HIGH).
                // Calculamos cuándo leer el segundo bit de parada.
                g_tiempoSiguienteBit = tiempoActual + g_bitTime_ms;
                // Cambiamos al estado para verificar el segundo bit de parada.
                g_estado = ESTADO_PARADA_2;
            }
            break;
        }

        // --- Estado: Verificando Segundo Bit de Parada ---
        case ESTADO_PARADA_2: {
            // Comprobamos si ya es tiempo de leer el bit de parada 2
            if (tiempoActual >= g_tiempoSiguienteBit) {
                // El segundo bit de parada también DEBE ser HIGH.
                if (digitalRead(RX_PIN) == LOW) {
                    Serial.println("Error: Fallo de Stop Bit 2");
                    actualizarContadores(-1, false);
                    flushRX();
                    g_estado = ESTADO_IDLE;
                    return;
                }

                // --- ¡Byte Recibido con Éxito! (Incluyendo Stops) ---

                // Guardamos el byte que acabamos de armar ('g_byte_actual')
                // en el buffer del frame ('proto_en_construccion')
                proto_en_construccion.frame[g_byte_index] = g_byte_actual;

                // Acumulamos la paridad de este byte para el checksum final del frame
                for(int j=0; j<8; j++) {
                    if ((g_byte_actual >> j) & 0x01) {
                        g_paridad_frame_calc++; // Contamos los bits '1'
                    }
                }

                g_byte_index++; // Pasamos al siguiente índice de byte

                // --- Lógica de Control del Paquete ---

                // 1. Si acabamos de recibir el SEGUNDO byte (índice 1),
                //    podemos desempaquetar CMD y LNG para saber cuántos
                //    bytes más esperar.
                if (g_byte_index == 2) {
                    // Desempaquetamos CMD (bits 2-5 del byte 0)
                    proto_en_construccion.cmd = (proto_en_construccion.frame[0] >> 2) & 0x0F;
                    // Desempaquetamos LNG (bits 1-6 del byte 1)
                    proto_en_construccion.lng = (proto_en_construccion.frame[1] >> 1) & 0x3F;

                    // Validación de seguridad: si LNG es inválido, abortar.
                    if (proto_en_construccion.lng > LARGO_DATA) {
                         Serial.println("Error: LNG recibido inválido!");
                         actualizarContadores(-1, false); // Marcar como error
                         flushRX();
                         g_estado = ESTADO_IDLE;
                         return;
                    }
                }

                // 2. Comprobamos si ya hemos recibido TODOS los bytes del paquete.
                //    Largo total = 2 (CMD/LNG) + N (DATA) + 2 (FCS)
                if (g_byte_index >= (proto_en_construccion.lng + BYTES_EXTRA)) {
                    // Sí, hemos recibido todos los bytes.
                    // Ahora solo falta verificar el bit de paridad final del frame.
                    // Calculamos cuándo leerlo (1 bit-time desde ahora)
                    g_tiempoSiguienteBit = tiempoActual + g_bitTime_ms;
                    g_estado = ESTADO_PARIDAD_FRAME; // Cambiar al estado final
                } else {
                    // No, aún faltan bytes por recibir.
                    // Volvemos al estado IDLE para esperar el bit de inicio
                    // del *siguiente* byte que enviará el emisor.
                    g_estado = ESTADO_IDLE;
                }
            }
            break;
        }

        // --- Estado: Verificando Bit de Paridad Final ---
        case ESTADO_PARIDAD_FRAME: {
            // Comprobamos si ya es tiempo de leer el bit de paridad final
            if (tiempoActual >= g_tiempoSiguienteBit) {

                // Leemos el nivel del pin (HIGH o LOW) en el centro del bit
                bool paridad_recibida = digitalRead(RX_PIN);

                // Calculamos la paridad esperada (true si el conteo de bits fue impar)
                bool paridad_calculada = (g_paridad_frame_calc % 2 != 0);

                // ¡Lógica clave! Verificamos el bit de STOP final.
                // Este delay es necesario porque necesitamos esperar exactamente
                // 1 bit-time para leer el bit de stop después del bit de paridad.
                delay(g_bitTime_ms);

                if (digitalRead(RX_PIN) == LOW) {
                     // El bit de stop final DEBE ser HIGH.
                     Serial.println("Error: Fallo de Stop Bit Final");
                     actualizarContadores(-1, false);
                     flushRX();
                     g_estado = ESTADO_IDLE;
                     return;
                }

                // --- ¡PAQUETE COMPLETO RECIBIDO! ---

                // Comprobamos si la paridad del frame coincide
                if (paridad_recibida == paridad_calculada) {
                    Serial.println("Recepción OK (Paridad de Frame correcta)");

                    // Si la paridad es correcta, ahora validamos el FCS
                    if (desempaquetar(proto_en_construccion)) {
                        // ¡FCS Correcto! Paquete 100% Válido.
                        Serial.println("¡Paquete VÁLIDO! (FCS Coincide)");
                        Serial.printf("CMD: %d, LNG: %d\n", proto_en_construccion.cmd, proto_en_construccion.lng);
                        actualizarContadores(proto_en_construccion.cmd, true); // Marcar como OK
                        // Pasamos el paquete válido a la capa de aplicación
                        ejecutarComando(proto_en_construccion);
                    } else {
                        // FCS Falló. El paquete está corrupto.
                        Serial.println("Error: Paquete CORRUPTO (FCS no coincide)");
                        actualizarContadores(proto_en_construccion.cmd, false); // Marcar como error FCS
                        flushRX(); // Limpiar línea
                    }
                } else {
                    // La paridad del frame no coincidió.
                    Serial.println("Error: Fallo de Paridad de Frame");
                    // No podemos confiar en el CMD si la paridad falló
                    actualizarContadores(-1, false); // Marcar como error de paridad/sync
                    flushRX(); // Limpiar línea
                }

                // --- Reinicio para el Próximo Paquete ---
                // Independientemente del resultado (éxito o fallo),
                // reiniciamos la máquina de estados para estar listos
                // para el siguiente paquete.
                g_estado = ESTADO_IDLE;
                g_byte_index = 0;
                g_paridad_frame_calc = 0;
                // (No es necesario limpiar 'proto_en_construccion', se sobrescribirá)
            }
            break;
        } // Fin case ESTADO_PARIDAD_FRAME

    } // Fin del switch(g_estado)
}


// --- Funciones de Validación (las mismas de antes) ---

/**
 * @brief Desempaqueta los datos y valida el FCS del frame recibido.
 */
bool desempaquetar(protocolo & proto) {
    // Nota: CMD y LNG ya fueron desempaquetados en la máquina de estados

    // Copiar los 'lng' bytes de datos desde el frame al campo 'data'
    for (int i = 0; i < proto.lng; i++) {
        proto.data[i] = proto.frame[i + 2];
    }

    // ¡Lógica clave! Añadir el terminador nulo '\0'.
    // Esto asegura que funciones como printf("%s") no lean basura
    // de memoria de paquetes anteriores si el paquete actual es más corto.
    proto.data[proto.lng] = '\0';

    // Reconstruir el FCS de 2 bytes (Big Endian) desde el final del frame
    unsigned short fcs_recibido = (proto.frame[proto.lng + 2] << 8) | (proto.frame[proto.lng + 3]);
    proto.fcs = fcs_recibido; // Guardar en la estructura (opcional)

    // Calcular el FCS esperado sobre los bytes recibidos (CMD, LNG, DATA)
    unsigned short fcs_calculado = fcs(proto.frame, proto.lng + 2);

    // Imprimir para depuración
    Serial.printf("FCS Recibido: %u\n", fcs_recibido);
    Serial.printf("FCS Calculado: %u\n", fcs_calculado);

    // Comparar y devolver true si coinciden
    return (fcs_calculado == fcs_recibido);
}

/**
 * @brief Calcula el FCS (conteo de bits activos).
 */
unsigned short fcs(BYTE * array, int tam){
    unsigned short res = 0;
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < 8; j++) {
            res += (array[i] >> j) & 0x01;
        }
    }
    return res;
}