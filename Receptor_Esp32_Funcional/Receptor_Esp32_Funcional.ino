/**
 * @file Receptor_Esp32_V2.ino
 * @brief Punto de entrada principal para el programa Receptor (ESP32 LoRa).
 * @details Este archivo inicializa el hardware y ejecuta el bucle principal
 * que llama a las funciones no bloqueantes para recibir datos y
 * manejar el parpadeo del LED.
 */

#include "recibe.h"           // Para la capa de comunicación (revisarRecepcion)
#include "funcionesReceptor.h" // Para la lógica de la aplicación (manejarParpadeoLED, etc.)

/**
 * @brief Estructura global donde se almacenará el último paquete recibido y validado.
 */
protocolo rx_proto;

/**
 * @brief Función de configuración inicial. Se ejecuta una vez al encender/resetear.
 */
void setup() {
    Serial.begin(115200);           // Iniciar comunicación serial para depuración
    pinMode(RX_PIN, INPUT_PULLUP); // Configurar pin de recepción con resistencia pull-up

    // Llamar a la función que inicializa el OLED y el LED
    setupHardware();

    Serial.println("Receptor ESP32 (No-Bloqueante) Iniciado...");

    // Mostrar mensaje de bienvenida en la pantalla OLED
    mostrarMensajeBienvenidaOLED();

    // Inicializar la máquina de estados de recepción (ponerla en ESTADO_IDLE)
    setupRecepcion();
}

/**
 * @brief Bucle principal del programa. Se ejecuta continuamente.
 * @details Este loop es "no bloqueante". Llama a las funciones de
 * parpadeo y recepción en cada ciclo, permitiendo que ambas tareas
 * se ejecuten de forma concurrente (o pseudo-concurrente).
 */
void loop() {
    // Tarea A: Manejar el estado del LED (Parpadear si está activado)
    // Esta función usa millis() y no bloquea.
    manejarParpadeoLED();

    // Tarea B: Ejecutar la máquina de estados de recepción.
    // Esta función revisa (usando millis()) si ha llegado un nuevo bit
    // y procesa el paquete sin bloquear. Si recibe un paquete completo
    // y válido, llamará internamente a 'ejecutarComando'.
    revisarRecepcion();

    // Nota: No se necesita un 'delay()' aquí. El 'loop()' se ejecutará
    // tan rápido como pueda. Las pausas necesarias están DENTRO de
    // 'revisarRecepcion' (calculadas con millis()) y 'manejarParpadeoLED'.
    // El único delay() real está en 'revisarRecepcion' para leer el stop bit final.
}