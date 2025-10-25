#ifndef RECIBE_H
#define RECIBE_H
#include "structProtocolo.h"

int readByte(int pin, int speed, BYTE* byte);

bool recibirFrame(int pin, int speed, protocolo & proto);

bool desempaquetar(protocolo & proto);

unsigned short fcs(BYTE * array, int tam);
#endif
