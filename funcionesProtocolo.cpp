#include "funcionesProtocolo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>



unsigned short fcs(BYTE * array, int tam){
    unsigned short res= 0;
    for (int i = 0; i < tam; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            res += (array[i] >> j) & 0x01;  
        }
        
    }
    return res;
}

int empaquetar(protocolo & proto){
    proto.frame[0] = (proto.cmd & 0x0F)<<2;
    proto.frame[1] = (proto.lng & 0x3F)<<1;
    int index=0;
    
    for (int i = 0; i < proto.lng; i++)
    {
        proto.frame[i+2] = proto.data[i]; 
        index++;
    } 

    proto.fcs = fcs(proto.frame, index + 2); 

    proto.frame[index+2] = (proto.fcs >> 8) & 0xFF; 
    proto.frame[index+3] = proto.fcs & 0xFF;      
    
    return index + 4; 
}
    
// En funcionesProtocolo.cpp (RPi)
void enviarFrame(int pin, int speed, protocolo proto, int largo){
    // unsigned long long nsTime = 1E6/speed; // Ya no usamos microsegundos
    unsigned long msTime = 1000 / speed; // 1000ms / 10 = 100ms por bit
    
    pinMode(pin, OUTPUT);
    bool level;
    int paridad=0;

    for(int j = 0; j<largo; j++){
        
        digitalWrite(pin, LOW); //bit de comienzo
        delay(msTime); // Usamos delay()
        
        BYTE byteEnvio = proto.frame[j];
        
        for(int i=0;i<8;i++){
            level = ((0x01) & (byteEnvio>>i)); 
            digitalWrite(pin, level); 
            
            if(level)
                paridad++;
            
            delay(msTime); // Usamos delay()
        }
        
        digitalWrite(pin, HIGH); //bit de parada
        delay(msTime * 2); // Usamos delay()
    }
    
    if(paridad%2==0)
        digitalWrite(pin, LOW);
    else
        digitalWrite(pin, HIGH);
    
    delay(msTime); // Usamos delay()
    digitalWrite(pin, HIGH); // Devolver a reposo
}
