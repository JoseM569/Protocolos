#ifndef FUNCIONES_RECEPTOR_H
#define FUNCIONES_RECEPTOR_H


#include "recibe.h" // Para acceder a 'protocolo'

// --- Funciones de Inicialización ---
void setupHardware();
void mostrarMensajeBienvenidaOLED();

// --- Función Principal de Despacho ---
void ejecutarComando(protocolo& proto);

// --- Funciones de Lógica (Contadores, LED) ---
void actualizarContadores(int cmd_recibido, bool fcs_ok);
void manejarParpadeoLED();
void flushRX(); // Movimos esta función aquí

#endif