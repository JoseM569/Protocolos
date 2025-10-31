/**
 * @file funcionesReceptor.cpp
 * @brief Implementación de la lógica de la aplicación del Receptor (ESP32).
 * @details Contiene el código que interactúa con el hardware (OLED, LED)
 * y mantiene las estadísticas, basado en los comandos recibidos.
 */

#include "funcionesReceptor.h"
#include <Wire.h>               // Para I2C (OLED)
#include <Adafruit_GFX.h>       // Librería gráfica base
#include <Adafruit_SSD1306.h>   // Driver del OLED

// --- Definiciones para el Hardware del LoRa32 ---
#define OLED_SDA 4  // Pin I2C SDA
#define OLED_SCL 15 // Pin I2C SCL
#define OLED_RST 16 // Pin Reset del OLED (puede no ser necesario si está conectado a RST)
#define SCREEN_WIDTH 128 // Ancho en píxeles
#define SCREEN_HEIGHT 64 // Alto en píxeles
// LED_PIN ya está definido en el .h

// Crear instancia del objeto display (global a este archivo)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// --- Variables Globales de Estado (Definición) ---
// (Las 'extern' en el .h apuntan a estas definiciones)
int g_test_recibidos_ok = 0;            // Contador Opción 2: Pruebas OK
int g_test_recibidos_fcs_error = 0;     // Contador Opción 2: Pruebas con error FCS
int g_test_recibidos_paridad_error = 0; // Contador Opción 2: Pruebas con error Sync/Paridad
int g_total_recibidos_ok = 0;           // Contador Opción 7: Normales OK
int g_total_recibidos_fcs_error = 0;    // Contador Opción 7: Normales con error FCS
int g_total_recibidos_paridad_error = 0;// Contador Opción 7: Normales con error Sync/Paridad

bool g_led_parpadeando = false; // Estado del LED (Opción 5)
int g_led_frecuencia_hz = 1;    // Frecuencia del LED (Opción 6), 1 Hz por defecto

// --- Implementación de Funciones ---

/**
 * @brief Inicializa el hardware (OLED, LED). Llamada desde setup().
 */
void setupHardware() {
    pinMode(LED_PIN, OUTPUT);   // Configurar pin del LED como salida
    Wire.begin(OLED_SDA, OLED_SCL); // Iniciar I2C

    // Iniciar el display OLED
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Dirección I2C 0x3C
        Serial.println(F("Fallo al iniciar SSD1306"));
        while(true); // Detener si el OLED no funciona
    }
}

/**
 * @brief Muestra mensaje de bienvenida en OLED. Llamada desde setup().
 */
void mostrarMensajeBienvenidaOLED() {
    display.clearDisplay();     // Borrar pantalla
    display.setTextSize(1);     // Tamaño de texto pequeño
    display.setTextColor(WHITE); // Color de texto blanco
    display.setCursor(0,0);     // Posicionar cursor en esquina superior izquierda
    display.println("Receptor ESP32");
    display.println("Listo para recibir...");
    display.display();          // ¡Importante! Enviar buffer a la pantalla
}

/**
 * @brief Limpia la línea RX esperando 1 segundo de silencio.
 */
void flushRX() {
    Serial.println("Limpiando línea RX, esperando 1 seg de silencio...");
    unsigned long quiet_start = millis();
    // Bucle mientras no haya pasado 1 segundo Y detectemos actividad
    while (millis() - quiet_start < 1000) {
        if (digitalRead(RX_PIN) == LOW) { // Si hay actividad (LOW)
            quiet_start = millis(); // Reiniciar contador de silencio
        }
        delay(1); // Pequeña pausa para no saturar CPU
    }
    Serial.println("Línea RX limpia. Listo para recibir.");
}

/**
 * @brief Actualiza los contadores de estadísticas globales.
 */
void actualizarContadores(int cmd_recibido, bool fcs_ok) {
    // Determinar si el comando era un mensaje de prueba (CMD 1)
    bool es_prueba = (cmd_recibido == 1);

    if (cmd_recibido == -1) {
        // -1 indica un error de Sincronización o Paridad.
        // No podemos confiar en el CMD, así que lo contamos por separado.
        // Podríamos decidir si contarlo como prueba o normal, aquí lo asigno a normal.
        g_total_recibidos_paridad_error++;
        // El requisito Tarea1 pide contar errores "no detectados" para las pruebas.
        // Podríamos asumir que un error de sync/paridad en CMD 1 es "no detectado".
        if (es_prueba) { // Si *creemos* que era de prueba (basado en el último CMD válido?) -> Difícil saber con certeza
           // g_test_recibidos_paridad_error++; // (Decidir cómo manejar esto)
        }
        return; // Salir
    }

    // Si llegamos aquí, el CMD es válido.
    if (fcs_ok) {
        // El FCS coincidió. Paquete OK.
        if (es_prueba) g_test_recibidos_ok++;
        else g_total_recibidos_ok++;
    } else {
        // El FCS falló (Error Detectado).
        if (es_prueba) g_test_recibidos_fcs_error++;
        else g_total_recibidos_fcs_error++;
    }
}

/**
 * @brief Ejecuta la acción correspondiente al comando recibido.
 */
void ejecutarComando(protocolo& proto) {
    // Switch principal basado en el comando recibido
    switch (proto.cmd) {

        case 0: // Opción 1: Mostrar mensaje de control en OLED
            Serial.println("Ejecutando CMD 0: Mostrar msg control");
            display.clearDisplay();
            display.setCursor(0,0);
            display.println("Dispositivo OK");
            display.drawRect(0, 10, 20, 20, WHITE); // Dibuja un cuadrado
            display.display();
            break;

        case 1:{ // Opción 2: Mensaje de prueba recibido
            // Ya fue contado en actualizarContadores()
            Serial.printf("Mensaje de prueba OK (%d)\n", g_test_recibidos_ok);

            // Verificar si ya se completaron los 10 mensajes de prueba
            int total_recibidos_test = g_test_recibidos_ok + g_test_recibidos_fcs_error + g_test_recibidos_paridad_error;
            if (total_recibidos_test >= 10) {
                Serial.println("--- Estadísticas de Prueba (10 mensajes) ---");
                if (total_recibidos_test > 0) {
                    // Calcular y mostrar porcentajes (Requisito Tarea1)
                    Serial.printf("  Aciertos: %.1f%%\n", (float)g_test_recibidos_ok / total_recibidos_test * 100.0);
                    Serial.printf("  Error Detectado (FCS): %.1f%%\n", (float)g_test_recibidos_fcs_error / total_recibidos_test * 100.0);
                    // El requisito pide "Error No Detectado". Asumiremos que son los de Paridad/Sync.
                    Serial.printf("  Error No Detectado (Sync): %.1f%%\n", (float)g_test_recibidos_paridad_error / total_recibidos_test * 100.0);
                } else {
                    Serial.println("  No se recibieron mensajes de prueba.");
                }

                // Resetear contadores de prueba para la próxima vez
                g_test_recibidos_ok = 0;
                g_test_recibidos_fcs_error = 0;
                g_test_recibidos_paridad_error = 0;
            }
            break;
        }
        case 2: // Opción 3: Mostrar texto en OLED
            Serial.println("Ejecutando CMD 2: Mostrar texto OLED");
            display.clearDisplay();
            display.setCursor(0,0);
            // Imprimir el contenido de proto.data (que ya tiene '\0' por desempaquetar)
            display.printf("Mensaje:\n%s", (char*)proto.data);
            display.display();
            break;

        case 3: // Opción 4: Mostrar temperatura en OLED
            Serial.println("Ejecutando CMD 3: Mostrar Temp");
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2); // Usar letra más grande para la temperatura
            // Imprimir el texto de la temperatura (ej: "27.5")
            display.printf("Temp:\n%s C", (char*)proto.data);
            display.setTextSize(1); // Volver al tamaño normal por defecto
            display.display();
            break;

        case 4: // Opción 5: Toggle (activar/desactivar) parpadeo del LED
            Serial.println("Ejecutando CMD 4: Toggle LED");
            g_led_parpadeando = !g_led_parpadeando; // Invertir el estado actual
            if (g_led_parpadeando) {
                Serial.println("LED AHORA PARPADEANDO (a " + String(g_led_frecuencia_hz) + " Hz)");
            } else {
                Serial.println("LED AHORA APAGADO");
                digitalWrite(LED_PIN, LOW); // Asegurarse de apagarlo al detener
            }
            break;

        case 5: { // Opción 6: Cambiar frecuencia de parpadeo del LED
            // Usamos llaves { } aquí para crear un ámbito local para 'freq',
            // evitando el error de compilación "jump to case label".
            Serial.println("Ejecutando CMD 5: Cambiar Freq LED");

            // Convertir el texto recibido (ej: "50") a un número entero
            int freq = atoi((char*)proto.data);

            // Validar que la frecuencia esté en el rango permitido [1..100]
            if (freq >= 1 && freq <= 100) {
                 g_led_frecuencia_hz = freq; // Actualizar la variable global
                 Serial.printf("Nueva Freq: %d Hz\n", g_led_frecuencia_hz);
            } else {
                 // Informar si el valor recibido está fuera de rango
                 Serial.printf("Freq recibida (%d) fuera de rango [1..100].\n", freq);
            }
            break;
        } // Fin del case 5

        case 6: // Opción 7: Imprimir estadísticas normales por Serial
            Serial.println("Ejecutando CMD 6: Imprimir Estadísticas Globales");
            Serial.println("--- Estadísticas Globales ---");
            Serial.printf("  Paquetes OK: %d\n", g_total_recibidos_ok);
            Serial.printf("  Fallo FCS (Detectado): %d\n", g_total_recibidos_fcs_error);
            Serial.printf("  Fallo Paridad/Sync (No Det.): %d\n", g_total_recibidos_paridad_error);
            break;

        case 7: // Opción 8: Mostrar el array de 8 temperaturas en OLED
            Serial.println("Ejecutando CMD 7: Mostrar 8 Temps");
            display.clearDisplay();
            display.setCursor(0,0);
            // El emisor ya envió el string formateado "Ultimas 8 Temps: ...",
            // así que solo lo imprimimos directamente.
            display.print((char*)proto.data);
            display.display();
            break;

        // Caso 'default' por si se recibe un comando desconocido
        default:
            Serial.printf("Comando %d desconocido recibido.\n", proto.cmd);
            break;
    } // Fin del switch(proto.cmd)
}

/**
 * @brief Maneja el parpadeo del LED (si está activado). Usa millis() para no bloquear.
 */
void manejarParpadeoLED() {
    // Solo hacer algo si el flag de parpadeo está activo
    if (g_led_parpadeando) {
        // 'static' hace que esta variable conserve su valor entre llamadas a la función
        static unsigned long tiempoAnteriorLED = 0;
        unsigned long tiempoActual = millis(); // Tiempo actual

        // Calcular la mitad del período en milisegundos
        // (Ej: 1 Hz -> 1000ms ciclo -> 500ms ON / 500ms OFF)
        long periodo_ms = (1000 / g_led_frecuencia_hz) / 2;

        // Manejo de caso extremo: si la frecuencia es muy alta (ej > 500 Hz),
        // el período es tan corto que 'millis()' no es preciso.
        // En ese caso, simplemente dejamos el LED encendido.
        if (periodo_ms <= 1) { // (Ej: 1000Hz / 2 = 1ms)
            digitalWrite(LED_PIN, HIGH);
            return; // Salir
        }

        // Comprobar si ha pasado suficiente tiempo desde el último cambio
        if (tiempoActual - tiempoAnteriorLED >= periodo_ms) {
            tiempoAnteriorLED = tiempoActual; // Guardar el tiempo actual
            // Invertir el estado del LED (si estaba HIGH -> LOW, si LOW -> HIGH)
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        }
    }
    // Si g_led_parpadeando es false, no se hace nada (el LED queda apagado
    // gracias al digitalWrite(LOW) en el case 4).
}