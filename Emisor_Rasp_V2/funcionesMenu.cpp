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
    enviarFrame(TX_PIN,SPEED,tx,largo);

}


void opcion_2(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 1;

    std::string input_texto;
    std::string dummy; // Para atrapar el 'Enter' fantasma

    printf("Ingrese el texto a enviar (max %d): ", LARGO_DATA);
    
    // 1. Intentamos leer la línea.
    // Si el búfer tiene un '\n' fantasma, 'dummy' quedará vacío.
    // Si el búfer está limpio, el usuario escribirá aquí y 'dummy' tendrá el texto.
    std::getline(std::cin, dummy); 

    if (dummy.empty()) {
        // El búfer estaba sucio (tenía un '\n' fantasma).
        // Lo acabamos de limpiar, así que ahora leemos el texto real.
        std::getline(std::cin, input_texto);
    } else {
        // El búfer estaba limpio. El usuario ya ingresó el texto en 'dummy'.
        input_texto = dummy;
    }
    // --- Fin de la corrección ---

    // Verificamos el largo
    int largo_real = input_texto.length();
    if (largo_real > LARGO_DATA) {
        printf("Mensaje muy largo, truncando a %d bytes.\n", LARGO_DATA);
        largo_real = LARGO_DATA; 
    }

    // Copiamos los datos al buffer
    memcpy(tx.data, input_texto.c_str(), largo_real);
    tx.lng = largo_real;
    
    int largo_frame = empaquetar(tx);
    enviarFrame(TX_PIN, SPEED, tx, largo_frame);
    printf("Mensaje enviado (CMD: 1, LNG: %d).\n", largo_real);
}

void opcion_3(){
    memset(&tx, 0, sizeof(protocolo));
    tx.cmd = 2;
    int largo = empaquetar(tx);
    enviarFrame(TX_PIN,SPEED,tx,largo);
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

