#include "recibe.h"
#include "Arduino.h"

int readByte(int pin, int speed, BYTE* byte) {
    unsigned long msTime = 1000 / speed; // 100ms
    
    *byte = 0;
    int paridad_byte = 0; 

    while (digitalRead(pin) == HIGH); 
    delay(msTime / 2); // Esperamos 50ms (mitad del bit de inicio)

    if (digitalRead(pin) == HIGH) {
        return -1; 
    }

    delay(msTime); // Nos movemos al centro del primer bit de dato

    for (int i = 0; i < 8; i++) {
        bool level = digitalRead(pin);
        if (level) {
            *byte = *byte | (1 << i);
            paridad_byte++; 
        }
        delay(msTime); // Esperamos 100ms
    }

    if (digitalRead(pin) == LOW) {
        return -1; 
    }
    
    delay(msTime); 
    if (digitalRead(pin) == LOW) {
        return -1; 
    }
    
    return paridad_byte;
}

bool recibirFrame(int pin, int speed, protocolo & proto) {
    memset(&proto, 0, sizeof(protocolo)); 
    unsigned long msTime = 1000 / speed; // 100ms
    int paridad_frame = 0; 
    int paridad_byte = 0;
    
    paridad_byte = readByte(pin, speed, &proto.frame[0]);
    if (paridad_byte == -1) return false; 
    paridad_frame += paridad_byte;
    proto.cmd = (proto.frame[0] >> 2) & 0x0F; 

    paridad_byte = readByte(pin, speed, &proto.frame[1]);
    if (paridad_byte == -1) return false; 
    paridad_frame += paridad_byte;
    proto.lng = (proto.frame[1] >> 1) & 0x3F; 

    if (proto.lng > LARGO_DATA) {
        return false; 
    }

    int bytes_totales = proto.lng + 2; 
    for (int i = 0; i < bytes_totales; i++) {
        paridad_byte = readByte(pin, speed, &proto.frame[i + 2]);
        if (paridad_byte == -1) return false; 
        paridad_frame += paridad_byte;
    }
 
    unsigned long start_wait = millis();
    while (digitalRead(pin) == HIGH) {
        // Timeout: Si la línea se queda en HIGH por más de ~1.5 bit-times
        // (150ms), asumimos que es un bit de paridad IMPAR (HIGH) y salimos.
        if (millis() - start_wait > (msTime * 1.5)) {
            break; 
        }
    }
    
    // Ahora estamos al inicio del bit de paridad (o hemos salido por timeout)
    
    delay(msTime / 2); // Esperamos 50ms para ponernos en el centro
    
    bool paridad_recibida = digitalRead(pin); // Leemos el bit (HIGH o LOW)
    
    // Ahora esperamos el bit de stop final
    delay(msTime); 
    if (digitalRead(pin) == LOW) {
        return false; // El bit de stop final falló
    }

    // Comparamos la paridad
    bool paridad_calculada = (paridad_frame % 2 != 0); // true si es impar

    if (paridad_recibida == paridad_calculada) {
        return true; // ¡ÉXITO!
    } else {
        return false; // Error de paridad
    }
}

bool desempaquetar(protocolo & proto) {

    for (int i = 0; i < proto.lng; i++) {
        proto.data[i] = proto.frame[i + 2];
    }


    unsigned short fcs_recibido = (proto.frame[proto.lng + 2] << 8) | (proto.frame[proto.lng + 3]);
    proto.fcs = fcs_recibido; // Guardamos el fcs recibido

    // Calcular el FCS
    // Calculamos sobre cmd, lng y data (total: proto.lng + 2 bytes)
    unsigned short fcs_calculado = fcs(proto.frame, proto.lng + 2);

    Serial.printf("FCS Recibido: %u\n", fcs_recibido);
    Serial.printf("FCS Calculado: %u\n", fcs_calculado);

    // 4. Comparar
    return (fcs_calculado == fcs_recibido);
}

unsigned short fcs(BYTE * array, int tam){
    unsigned short res = 0;
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < 8; j++) {
            res += (array[i] >> j) & 0x01;  
        }
    }
    return res;
}

