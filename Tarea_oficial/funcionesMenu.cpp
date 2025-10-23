#include "funcionesMenu.h"
const char* menu[11] = {
        "===== MENÚ EMISOR (PREVIA) =====",
        "1) Mostrar mensaje de control/imagen en OLED",
        "2) Enviar 10 mensajes de prueba",
        "3) Enviar texto para mostrar en OLED",
        "4) Enviar temperatura flotante [-40.0, 40.0]",
        "5) Toggle parpadeo LED (gpio 25)",
        "6) Cambiar frecuencia parpadeo LED [1..100] Hz",
        "7) Solicitar impresión de contador/estadísticas (receptor)",
        "8) Enviar arreglo con últimas 8 temperaturas (extra)",
        "9) Mostrar contador local de mensajes enviados",
        "0) Salir"
    };
protocolo tx;

void opcion_1(){
    tx.cmd = 0;
    int largo = empaquetar(tx);
    printf("largo: %i\n",largo);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_2(){
    tx.cmd = 1;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_3(){
    tx.cmd = 2;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_4(){
    tx.cmd = 3;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_5(){
    tx.cmd = 4;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_6(){
    tx.cmd = 5;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_7(){
    tx.cmd = 6;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_8(){
    tx.cmd = 7;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_9(){
    tx.cmd = 8;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}

void opcion_0(){
    tx.cmd = 9;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,100,tx,largo);
}
