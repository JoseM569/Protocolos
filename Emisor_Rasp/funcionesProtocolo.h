#include "structProtocolo.h"

int empaquetar(protocolo & proto);

unsigned short fcs(BYTE * array,int tam);

void enviarFrame(int pin, int speed, protocolo proto, int largo);
