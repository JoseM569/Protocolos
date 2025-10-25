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
    // 1. LIMPIAR *TODA* LA ESTRUCTURA PRIMERO
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 0;
    int largo = empaquetar(tx);
    printf("largo: %i\n",largo);
    enviarFrame(TX_PIN,SPEED,tx,largo);

}


void opcion_2(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 1;

    // --- Versión C++ (Segura) ---
    std::string input_texto;
    printf("Ingrese el texto a enviar (max 63): ");
    
    // Usamos std::cin para leer la línea de forma segura
    std::getline(std::cin, input_texto); 

    // Verificamos el largo
    int largo_real = input_texto.length();
    if (largo_real > LARGO_DATA) {
        largo_real = LARGO_DATA; // Truncamos si es más largo
    }

    // Copiamos los datos al buffer
    memcpy(tx.data, input_texto.c_str(), largo_real);
    tx.lng = largo_real;
    // --- Fin versión C++ ---

    
    int largo_frame = empaquetar(tx);
    enviarFrame(TX_PIN, SPEED, tx, largo_frame);
}

void opcion_3(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 2; // Comando de la opcion 3
    int largo = empaquetar(tx);
    printf("largo: %i. Tiempo esperado (aprox): %i ms\n", largo, (largo * 110) + 10);

    // --- Medición (Estilo C) ---
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

