#ifndef STRUCT_PROTOCOLO_H
#define STRUCT_PROTOCOLO_H
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

#define BYTE unsigned char
#define LARGO_DATA 63
#define BYTES_EXTRA 4 // CMD + LNG + FCS[2]
#define TX_PIN 17
#define SPEED 10

typedef struct 
{
    BYTE cmd;// (0x0F)<<2  -  4 bits -> 0-0-1-1 | 1-1-0-0
    BYTE lng;// (0x3F)<<1  -  6 bits -> 0-1-1-1 | 1-1-1-0  
    BYTE data[LARGO_DATA];
    BYTE frame[LARGO_DATA+BYTES_EXTRA];
    unsigned short fcs;//2 Byte -> Para poder contar los bits activos, 63 de data + 4 de bits y 6 de lng.

}protocolo;


#endif
