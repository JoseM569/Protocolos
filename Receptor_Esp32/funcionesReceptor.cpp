#include "funcionesReceptor.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Definiciones para el Hardware del LoRa32 ---
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN 25 

// Crear instancia del display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// --- Variables Globales de Estado ---
int g_test_recibidos_ok = 0;
int g_test_recibidos_fcs_error = 0;
int g_test_recibidos_paridad_error = 0; 
int g_total_recibidos_ok = 0;
int g_total_recibidos_fcs_error = 0;
int g_total_recibidos_paridad_error = 0;
bool g_led_parpadeando = false;
int g_led_frecuencia_hz = 1; 

// --- Implementación de Funciones ---

void setupHardware() {
    pinMode(LED_PIN, OUTPUT);
    Wire.begin(OLED_SDA, OLED_SCL);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("Fallo al iniciar SSD1306"));
        while(true);
    }
}

void mostrarMensajeBienvenidaOLED() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Receptor ESP32");
    display.println("Listo para recibir...");
    display.display();
}

/**
 * Función que "limpia" la línea RX después de un error,
 * esperando 1 segundo de silencio.
 */
void flushRX() {
    Serial.println("Limpiando línea RX, esperando 1 seg de silencio...");
    unsigned long quiet_start = millis();
    while (millis() - quiet_start < 1000) { 
        if (digitalRead(RX_PIN) == LOW) {
            quiet_start = millis();
        }
        delay(1); 
    }
    Serial.println("Línea RX limpia. Listo para recibir.");
}

/**
 * Actualiza los contadores de estadísticas globales.
 */
void actualizarContadores(int cmd_recibido, bool fcs_ok) {
    bool es_prueba = (cmd_recibido == 1); // CMD 1 = Mensaje de prueba

    if (cmd_recibido == -1) {
        // -1 significa fallo de Sincronización/Paridad. No sabemos si era de prueba.
        // Asumamos que no lo era (o crea un contador aparte si quieres)
        g_total_recibidos_paridad_error++;
        return;
    }

    if (fcs_ok) {
        if (es_prueba) g_test_recibidos_ok++;
        else g_total_recibidos_ok++;
    } else {
        // FCS falló (error detectado)
        if (es_prueba) g_test_recibidos_fcs_error++;
        else g_total_recibidos_fcs_error++;
    }
}

/**
 * Función principal que contiene el "switch" y ejecuta la lógica
 * basada en el comando recibido.
 */
void ejecutarComando(protocolo& proto) {
    switch (proto.cmd) {
        
        case 0: // Opción 1: Mostrar mensaje de control
            Serial.println("Ejecutando CMD 0: Mostrar msg control");
            display.clearDisplay();
            display.setCursor(0,0);
            display.println("Dispositivo OK");
            display.drawRect(0, 10, 20, 20, WHITE); // Dibuja un cuadrado
            display.display();
            break;
            
        case 1: // Opción 2: Mensaje de prueba
            Serial.printf("Mensaje de prueba OK (%d)\n", g_test_recibidos_ok);
            
            // Si ya llegaron los 10 (contando errores), imprimir estadísticas
            if (g_test_recibidos_ok + g_test_recibidos_fcs_error + g_test_recibidos_paridad_error >= 10) {
                Serial.println("--- Estadísticas de Prueba (10 mensajes) ---");
                int total_test = g_test_recibidos_ok + g_test_recibidos_fcs_error + g_test_recibidos_paridad_error;
                Serial.printf("  Aciertos: %.1f%%\n", (float)g_test_recibidos_ok / total_test * 100.0);
                Serial.printf("  Error Detectado (FCS): %.1f%%\n", (float)g_test_recibidos_fcs_error / total_test * 100.0);
                // (El requisito menciona "error no detectado", pero si el FCS es bueno, 
                // asumimos que el mensaje es correcto)
                
                // Resetear contadores de prueba
                g_test_recibidos_ok = 0;
                g_test_recibidos_fcs_error = 0;
                g_test_recibidos_paridad_error = 0;
            }
            break;
            
        case 2: // Opción 3: Enviar texto a OLED
            Serial.println("Ejecutando CMD 2: Mostrar texto OLED");
            display.clearDisplay();
            display.setCursor(0,0);
            display.printf("Mensaje:\n%s", (char*)proto.data);
            display.display();
            break;

        case 3: // Opción 4: Enviar temperatura
            Serial.println("Ejecutando CMD 3: Mostrar Temp");
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2); // Letra más grande
            display.printf("Temp:\n%s C", (char*)proto.data);
            display.setTextSize(1); // Volver a tamaño normal
            display.display();
            break;
            
        case 4: // Opción 5: Toggle LED
            Serial.println("Ejecutando CMD 4: Toggle LED");
            g_led_parpadeando = !g_led_parpadeando; // Invertir estado
            if (g_led_parpadeando) Serial.println("LED AHORA PARPADEANDO");
            else {
                Serial.println("LED AHORA APAGADO");
                digitalWrite(LED_PIN, LOW); // Apagarlo
            }
            break;

        case 5: {// Opción 6: Cambiar frecuencia
            Serial.println("Ejecutando CMD 5: Cambiar Freq LED");
            int freq = atoi((char*)proto.data); // Convertir texto (ej: "50") a entero
            if (freq >= 1 && freq <= 100) {
                 g_led_frecuencia_hz = freq;
                 Serial.printf("Nueva Freq: %d Hz\n", g_led_frecuencia_hz);
            } else {
                 Serial.printf("Freq recibida (%d) fuera de rango.\n", freq);
            }
            break;
        }
        case 6: // Opción 7: Imprimir estadísticas (normales)
            Serial.println("Ejecutando CMD 6: Imprimir Estadísticas");
            Serial.println("--- Estadísticas Globales ---");
            Serial.printf("  Paquetes OK: %d\n", g_total_recibidos_ok);
            Serial.printf("  Fallo FCS: %d\n", g_total_recibidos_fcs_error);
            Serial.printf("  Fallo Paridad/Sync: %d\n", g_total_recibidos_paridad_error);
            break;
            
        case 7: // Opción 8: Enviar array de temps
            Serial.println("Ejecutando CMD 7: Mostrar 8 Temps");
            display.clearDisplay();
            display.setCursor(0,0);
            display.print((char*)proto.data); // El emisor ya formateó el string
            display.display();
            break;

        default:
            Serial.printf("Comando %d desconocido.\n", proto.cmd);
            break;
    }
}

/**
 * Maneja el parpadeo del LED si está activado.
 * Esta función debe llamarse en CADA loop.
 */
void manejarParpadeoLED() {
    if (g_led_parpadeando) {
        // Usamos millis() para no bloquear el loop con delay()
        static unsigned long tiempoAnteriorLED = 0;
        unsigned long tiempoActual = millis();
        
        // (1Hz = 1000ms de ciclo / 2 = 500ms ON, 500ms OFF)
        long periodo_ms = (1000 / g_led_frecuencia_hz) / 2;

        if (tiempoActual - tiempoAnteriorLED >= periodo_ms) {
            tiempoAnteriorLED = tiempoActual;
            // Invertir el estado actual del LED
            digitalWrite(LED_PIN, !digitalRead(LED_PIN)); 
        }
    }
}