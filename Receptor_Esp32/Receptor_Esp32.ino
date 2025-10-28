#include "recibe.h"
#include "funcionesReceptor.h" // <-- ¡Nuestro nuevo archivo de lógica!

protocolo rx_proto;

void setup() {
    Serial.begin(115200);
    pinMode(RX_PIN, INPUT_PULLUP);

    // Llamamos a la función que inicializa el hardware (OLED, LED, etc.)
    setupHardware();
    
    Serial.println("Receptor ESP32 Iniciado con PULLUP...");
    mostrarMensajeBienvenidaOLED();
}


void loop() {
    Serial.println("Esperando Frame...");
    
    bool frame_ok = recibirFrame(RX_PIN, SPEED, rx_proto);

    if (frame_ok) {
        // El frame se recibió bien (Stop bits y Paridad correctos)
        
        if (desempaquetar(rx_proto)) {
            // --- ¡PAQUETE VÁLIDO! (FCS COINCIDE) ---
            Serial.println("¡Paquete VÁLIDO! (FCS Coincide)");
            Serial.printf("CMD: %d, LNG: %d\n", rx_proto.cmd, rx_proto.lng);

            // Actualizar contadores (true = FCS OK)
            actualizarContadores(rx_proto.cmd, true); 

            // --- Despachar el comando a nuestra lógica ---
            ejecutarComando(rx_proto);
            
        } else {
            // --- PAQUETE CORRUPTO (FCS NO COINCIDE) ---
            Serial.println("Error: Paquete CORRUPTO (FCS no coincide)");
            
            // Actualizar contadores (false = FCS Error)
            actualizarContadores(rx_proto.cmd, false);
            flushRX();
        }

    } else {
        // --- FALLO DE SINCRONIZACIÓN (STOP BITS / PARIDAD FRAME) ---
        Serial.println("Error: Fallo de sincronización (Stop bits / Paridad Frame)");
        
        // Aquí no podemos confiar en el CMD, así que lo marcamos como error de paridad
        actualizarContadores(-1, false); // -1 = Comando desconocido
        flushRX();
    }
    
}