/**
 * @file funcionesReceptor.h
 * @brief Declaraciones de las funciones de la lógica de aplicación del Receptor (ESP32).
 * @details Define la interfaz para inicializar el hardware, manejar los comandos
 * recibidos, actualizar estadísticas y controlar el parpadeo del LED.
 */

#ifndef FUNCIONES_RECEPTOR_H
#define FUNCIONES_RECEPTOR_H

#include "recibe.h" // Necesario para que las funciones conozcan la struct 'protocolo'

// --- Definiciones Globales ---

/**
 * @brief Pin GPIO del LED azul integrado en la placa ESP32 LoRa.
 */
#define LED_PIN 25

// --- Variables Globales (Declaración 'extern') ---
// 'extern' indica que estas variables están definidas en 'funcionesReceptor.cpp'
// y que otros archivos (como 'recibe.cpp') pueden usarlas.

/**
 * @brief Estado actual del parpadeo del LED (true = parpadeando, false = apagado).
 */
extern bool g_led_parpadeando;

// --- Funciones de Inicialización ---

/**
 * @brief Inicializa el hardware específico del receptor (OLED, LED).
 * @details Configura los pines, inicia la comunicación I2C y el display OLED.
 * Se llama una vez desde setup().
 */
void setupHardware();

/**
 * @brief Muestra un mensaje de bienvenida en la pantalla OLED al iniciar.
 */
void mostrarMensajeBienvenidaOLED();

// --- Función Principal de Despacho de Comandos ---

/**
 * @brief Ejecuta la acción correspondiente al comando (CMD) recibido.
 * @details Contiene el 'switch' principal que dirige la lógica de la aplicación
 * (mostrar en OLED, contar, controlar LED, etc.) basado en 'proto.cmd'.
 * @param proto Referencia a la estructura del protocolo que contiene el paquete
 * válido recibido (incluyendo 'cmd', 'lng' y 'data').
 */
void ejecutarComando(protocolo& proto);

// --- Funciones de Lógica Auxiliar ---

/**
 * @brief Actualiza los contadores globales de estadísticas de recepción.
 * @details Incrementa los contadores correctos (prueba vs normal, OK vs error)
 * basado en el comando recibido y el resultado de la validación FCS/Paridad.
 * @param cmd_recibido El comando (CMD) del paquete, o -1 si hubo error de sincronización.
 * @param fcs_ok 'true' si el FCS fue válido, 'false' si falló (o si hubo error de sync).
 */
void actualizarContadores(int cmd_recibido, bool fcs_ok);

/**
 * @brief Controla el parpadeo del LED si está activado ('g_led_parpadeando').
 * @details Implementa la lógica "BlinkWithoutDelay" usando millis().
 * Debe llamarse en CADA ciclo del loop() para funcionar correctamente.
 */
void manejarParpadeoLED();

/**
 * @brief Limpia la línea RX después de un error de recepción.
 * @details Espera 1 segundo de "silencio" (línea en HIGH) para asegurar
 * que cualquier transmisión corrupta del emisor haya terminado antes de
 * intentar recibir de nuevo.
 */
void flushRX();

#endif // FUNCIONES_RECEPTOR_H