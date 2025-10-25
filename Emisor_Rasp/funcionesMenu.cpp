#include "funcionesMenu.h"
#include <sys/time.h>
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
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 0;
    int largo = empaquetar(tx);
    printf("largo: %i\n",largo);
    enviarFrame(TX_PIN,SPEED,tx,largo);

}


void opcion_2(){

    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 1;

    // --- BLOQUE DE PRUEBA LÍMITE (63 Bytes) ---
    // (Usamos memcpy para evitar std::cin)

    printf("Iniciando prueba de 63 bytes...\n");
    const char* MENSJ_LIMITE = "012345678901234567890123456789012345678901234567890123456789012";

    // Copiamos los 63 bytes exactos
    memcpy(tx.data, MENSJ_LIMITE, 63); 

    // Forzamos el largo a 63
    tx.lng = 63; 
    // ----------------------------------------

    int largo = empaquetar(tx);
    enviarFrame(TX_PIN, SPEED, tx, largo);
    printf("Prueba de 63 bytes enviada.\n");
}

void opcion_3(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 2; 
    int largo = empaquetar(tx);
    printf("largo: %i. Tiempo esperado (aprox): %i ms\n", largo, (largo * 110) + 10);

    // --- Medición tiempo de envío --- (no estaba seguro de si los delays estaban funcionando)
    struct timeval start, stop;
    long seconds, useconds, mtime;

    gettimeofday(&start, NULL); // Inicia el reloj

    enviarFrame(TX_PIN,SPEED, tx, largo);

    gettimeofday(&stop, NULL); // Detiene el reloj

    // Cálculo del tiempo transcurrido en milisegundos
    seconds  = stop.tv_sec  - start.tv_sec;
    useconds = stop.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000) + (useconds / 1000.0);
    // --- Fin Medición ---

    printf("==> Tiempo de envío REAL: %ld milisegundos\n", mtime);
}
 // Opciones a completar 4 -> 9;
void opcion_4(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 3;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,SPEED,tx,largo);
}

void opcion_5(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 4;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,SPEED,tx,largo);
}

void opcion_6(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 5;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,SPEED,tx,largo);
}

void opcion_7(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 6;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,SPEED,tx,largo);
}

void opcion_8(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 7;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,SPEED,tx,largo);
}

void opcion_9(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 8;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,SPEED,tx,largo);
}

