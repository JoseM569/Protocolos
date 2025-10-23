#include "funcionesProtocolo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>



int fcs(BYTE * array, int tam){
    int res= 0;
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
    proto.frame[0] = (proto.cmd & 0x0F) << 2;
    proto.frame[1] = (proto.lng & 0x3F) << 1;
    int index=0;
    for (int i = 0; i < proto.lng; i++)
    {
        proto.frame[i+2] = proto.data[i]; 
        index++;
    } 
    printf("index : %i\n",index);
    proto.fcs = fcs(proto.frame,index+2); // se calcula fcs
    printf("fcs : %i\n",proto.fcs);
    
    proto.frame[index+2] = (proto.fcs & 0xFF);
    printf("index : %i\n",index+2);
    if (index == 0)
    {
        return 1;
    } else {
        
        return index+3;
    }
}
    
void enviarFrame(int pin, int speed, protocolo proto, int largo){
	unsigned long long nsTime = 1E6/speed;
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW); //bit de comienzo	-> 0
	delayMicroseconds(nsTime);
	bool level;
	int paridad=0;
    for(int j = 0; j<largo; j++){
        BYTE byteEnvio = proto.frame[j];
	    for(int i=0;i<8;i++){
		    level = ((0x01) & (byteEnvio>>i));	// HIGH=1, LOW=0
		    digitalWrite(pin, level ); // se escribe el bit i
		    if(level)
			    paridad++;
            delayMicroseconds(nsTime);
	    }
        digitalWrite(pin, HIGH); //bit de parada		-> 1
        delayMicroseconds(nsTime*2); //duración del bit de parada
        printf("bit %d enviado\n",j);
}
if(paridad%2==0)		//bit de paridad
digitalWrite(pin, LOW); //paridad par
else
digitalWrite(pin, HIGH); //paridad impar
delayMicroseconds(nsTime);
}

bool desempaquetar(protocolo & proto){
    proto.cmd = (proto.frame[0] >> 2) & 0x0F ; // se obtiene cmd
    proto.lng = (proto.frame[1] >> 1) & 0x3F;
    for(int i=0 ; i < proto.lng ; i++){

        proto.data[i] = proto.frame[i+2];

    }
    proto.fcs = (proto.frame[proto.lng+ 2]);
    printf("fcs : %i\n",proto.fcs);

    int fcs_calculado=fcs(proto.frame,proto.lng+2);

    printf("fcs_calculado : %i\n",fcs_calculado);
    if (fcs_calculado == proto.fcs)
    {
        return true;
    }
    return false;
}