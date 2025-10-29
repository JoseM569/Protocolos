/**
 * @file funcionesProtocolo.cpp
 * @brief Implementación de las funciones de bajo nivel del protocolo (empaquetar, fcs, enviar).
 */

#include "funcionesProtocolo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h> // Necesario para pinMode, digitalWrite, delay

/**
 * @brief Calcula el Frame Check Sequence (FCS) (conteo de bits activos).
 */
unsigned short fcs(BYTE * array, int tam){
    unsigned short res= 0;
    for (int i = 0; i < tam; i++) // Recorre cada byte
    {
        for (int j = 0; j < 8; j++) // Recorre cada bit
        {
            res += (array[i] >> j) & 0x01;  
        }
    }
    return res;
}

/**
 * @brief Arma el frame completo para la transmisión.
 */
int empaquetar(protocolo & proto){
    // --- Empaquetado de bits ---
    // El CMD (4 bits) se desplaza 2 bits a la izquierda.
    // (Ej: CMD 2 (0b0010) se guarda como 0b1000)
    proto.frame[0] = (proto.cmd & 0x0F)<<2;
    // El LNG (6 bits) se desplaza 1 bit a la izquierda.
    proto.frame[1] = (proto.lng & 0x3F)<<1;
    
    int index=0;
    
    // Copia los datos (payload) al frame
    for (int i = 0; i < proto.lng; i++)
    {
        proto.frame[i+2] = proto.data[i]; 
        index++;
    } 

    // --- Cálculo y guardado del FCS ---
    
    // Calculamos el FCS DESPUÉS de haber copiado CMD, LNG y DATA al frame.
    // El FCS se calcula sobre los 2 primeros bytes + los N bytes de datos.
    proto.fcs = fcs(proto.frame, index + 2); 

    // Guardamos el FCS (unsigned short de 2 bytes) en los siguientes dos
    // bytes del frame (formato Big Endian: Byte Alto primero, Byte Bajo después).
    proto.frame[index+2] = (proto.fcs >> 8) & 0xFF; // Byte Alto
    proto.frame[index+3] = proto.fcs & 0xFF;      // Byte Bajo
    
    // Retorna el largo total: 2 (cmd/lng) + N (data) + 2 (fcs)
    return index + 4; 
}

/**
 * @brief Transmite el frame completo, bit por bit (bit-banging).
 */
void enviarFrame(int pin, int speed, protocolo proto, int largo){
    
    // Calcula el tiempo de delay por cada bit.
    // (1000ms / 10 bits/s = 100ms por bit)
    unsigned long msTime = 1000 / speed; 
    
    pinMode(pin, OUTPUT);
    bool level;
    int paridad=0; // Acumulador para la paridad de todo el frame

    // --- Bucle de Bytes ---
    // Recorre cada byte del frame (cmd, lng, data, fcs...)
    for(int j = 0; j<largo; j++){
        
        // 1. Bit de Inicio
        digitalWrite(pin, LOW);
        delay(msTime); 
        
        BYTE byteEnvio = proto.frame[j];
        
        // --- Bucle de Bits ---
        // Envía los 8 bits de datos, LSB (menos significativo) primero
        for(int i=0;i<8;i++){
            level = ((0x01) & (byteEnvio>>i)); // Aísla el bit 'i'
            digitalWrite(pin, level); 
            
            // Acumula la paridad (cuenta bits '1')
            if(level)
                paridad++;
            
            delay(msTime); 
        }
        
        // 2. Bits de Parada
        digitalWrite(pin, HIGH); 
        delay(msTime * 2); // 2 bits de parada para dar tiempo al receptor
    }
    
    // --- Bit de Paridad del Frame ---
    // Se envía UN solo bit de paridad al FINAL de todo el frame.
    if(paridad % 2 == 0)
        digitalWrite(pin, LOW); // Paridad Par
    else
        digitalWrite(pin, HIGH); // Paridad Impar
    
    delay(msTime); // Duración del bit de paridad
    
    // 3. Bit de Parada Final y Reposo
    // Dejamos la línea en HIGH (estado de reposo)
    digitalWrite(pin, HIGH);
}