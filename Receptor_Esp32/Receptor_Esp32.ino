#include "recibe.h"

protocolo rx_proto;

void setup() {
    Serial.begin(115200);
  pinMode(RX_PIN, INPUT_PULLUP); // Activa la resistencia de pull-up interna

  Serial.println("Receptor ESP32 Iniciado con PULLUP...");
}

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

void loop() {
    Serial.println("Esperando Frame...");
    
    bool frame_ok = recibirFrame(RX_PIN, SPEED, rx_proto);

    if (frame_ok) {
        Serial.println("Recepción OK (Paridad de Frame correcta)");

        if (desempaquetar(rx_proto)) {
            Serial.println("¡Paquete VÁLIDO! (FCS Coincide)");
            Serial.printf("CMD: %d, LNG: %d\n", rx_proto.cmd, rx_proto.lng);
            
            
        } else {
            Serial.println("Error: Paquete CORRUPTO (FCS no coincide)");
            // Un FCS corrupto es un fallo, mejor limpiar
            flushRX(); 
        }

    } else {
        Serial.println("Error: Fallo de sincronización (Stop bits / Paridad Frame)");

        flushRX();
    }
    
}