#include "structProtocolo.h"

int empaquetar(protocolo & proto);

int fcs(BYTE * array,int tam);

bool desempaquetar(protocolo & proto);

void enviarFrame(int pin, int speed, protocolo proto, int largo);
