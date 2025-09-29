// =========================
// Cosas por hacer (Better Comments)
// Fecha: 2025-09-25
// Convención:
// - TODO [ ] Pendiente
// - TODO [x] Completado
// - ! Urgente
// - ? Duda
// - NOTE Nota/Contexto
// =========================

// ===== Proyecto: Protocolos de Comunicación =====
// NOTE Emisor en Raspberry Pi, Receptor en ESP32 (LoRa 32 con OLED). No se corrige si están invertidos.
// TODO [ ] Validar que el emisor compila y corre en Raspberry Pi
// TODO [ ] Validar que el receptor compila y corre en ESP32 LoRa 32 (OLED)

// ===== Diseño de Protocolo Único =====
// TODO [ ] Definir estructura de trama con los campos requeridos
// TODO [ ] Campo LENGTH: permite al menos 40 bytes de DATA (mensaje mínimo 40 bytes)
// TODO [ ] Campo CMD: identifica el comando (mismos comandos para emisor/receptor)
// TODO [ ] Campo FCS: redundancia (e.g., CRC-8/16, checksum); documentar algoritmo
// TODO [ ] Campo DATA: payload (texto, temperaturas, arreglo, etc.)
// TODO [ ] Endianness, alineación y versión del protocolo
// TODO [ ] Documentar formato exacto (byte a byte) y ejemplos
// TODO [ ] Implementar encode/decode + validación de FCS
// TODO [ ] Manejo de errores: trama inválida, longitud errónea, FCS fallido

// ===== Emisor (Raspberry Pi) =====
// TODO [ ] Implementar menú interactivo para seleccionar acciones/comandos
// TODO [ ] Mostrar contador de mensajes enviados en el emisor
// TODO [ ] Solicitar temperatura flotante en rango [-40.0, 40.0] (validar)
// TODO [ ] Almacenar últimos 8 valores de temperatura en buffer rotativo
// TODO [ ] Opción para enviar arreglo de últimas 8 temperaturas (extra)
// TODO [ ] Comando: enviar texto para mostrar en OLED receptor
// TODO [ ] Comando: mensaje de control/imagen para OLED ("dispositivo funcionando" o figura)
// TODO [ ] Comando: test de 10 mensajes (para estadísticas en receptor)
// TODO [ ] Comando: enviar temperatura flotante y formato de impresión
// TODO [ ] Comando: toggle parpadeo LED (gpio 25) en receptor (1 Hz por defecto)
// TODO [ ] Comando: cambiar frecuencia del LED (1 a 100 Hz)
// TODO [ ] Comando: solicitar e imprimir desde receptor el contador y estadísticas (sin contar test)
// TODO [ ] Manejo de reintentos/timeout a nivel aplicación (si aplica)

// ===== Receptor (ESP32 LoRa 32 con OLED) =====
// TODO [ ] Inicializar OLED y GPIO 25 para LED
// TODO [ ] Implementar bucle de recepción + parseo de protocolo + verificación FCS
// TODO [ ] Comando: mostrar mensaje de control/imagen en OLED
// TODO [ ] Comando: mostrar texto recibido en OLED
// TODO [ ] Comando: recibir y mostrar temperatura: "Temperatura : <v> °C"
// TODO [ ] Comando: activar/desactivar parpadeo LED (toggle); frecuencia por defecto 1 Hz
// TODO [ ] Comando: cambiar frecuencia parpadeo LED (1 a 100 Hz)
// TODO [ ] Comando: test de 10 mensajes -> actualizar contadores internos
// TODO [ ] Comando: imprimir por consola contador general + estadísticas (sin contar test)
// TODO [ ] Comando (extra): recibir arreglo de 8 temperaturas y mostrarlas
// TODO [ ] Diseñar tareas/RTOS o timers para parpadeo estable independientemente de recepción

// ===== Contadores y Estadísticas =====
// TODO [ ] Receptor: contar mensajes recibidos correctamente (generales)
// TODO [ ] Receptor: contar mensajes con error detectado (FCS fallido u otras validaciones)
// TODO [ ] Receptor: contar mensajes con error NO detectado (definir criterio y test)
// TODO [ ] Receptor: para comando de prueba (10 mensajes), calcular % acierto y % error
// TODO [ ] Receptor: imprimir estadísticas por consola cuando se invoque el comando específico
// TODO [ ] Emisor: mostrar contador de mensajes enviados
// NOTE Aclarar qué queda dentro/fuera de estadísticas generales (excluir test donde se indique)

// ===== Robustez y Manejo de Errores =====
// TODO [ ] Timeouts y reintentos en emisor (si capa inferior no garantiza entrega)
// TODO [ ] Validar rangos (temperatura, frecuencia LED)
// TODO [ ] Logs de depuración (nivel DEBUG/INFO/ERROR)
// TODO [ ] Simular/inyectar errores para validar contadores (detectado/no detectado)

// ===== Pruebas =====
// TODO [ ] Prueba: OLED muestra "dispositivo funcionando" al comando
// TODO [ ] Prueba: envío de texto arbitrario y visualización correcta en OLED
// TODO [ ] Prueba: test 10 mensajes -> verificar contadores y % en consola receptor
// TODO [ ] Prueba: temperatura en límites (-40.0 y 40.0) y valores intermedios
// TODO [ ] Prueba: toggle LED (gpio 25) y frecuencia por defecto 1 Hz
// TODO [ ] Prueba: cambio de frecuencia LED (1-100 Hz) con medidas visibles
// TODO [ ] Prueba (extra): envío/recepción de arreglo de 8 temperaturas y despliegue
// TODO [ ] Prueba: estadísticas generales sin incluir los de prueba

// ===== Entregables y Documentación =====
// TODO [ ] README: arquitectura, protocolo (campos, tamaños, ejemplos), cómo compilar y ejecutar
// TODO [ ] Especificación de comandos (tabla CMD, payload, respuestas, errores)
// TODO [ ] Capturas/fotos o video demostrativo (OLED, LED, consola)
// TODO [ ] Código modular (puntaje extra): separar capas (protocolo, transporte, UI/menú, drivers)
// TODO [ ] Comentarios en código y convenciones (Better Comments)
// TODO [ ] Script o instrucciones para dependencias (librerías OLED/LoRa)

// ===== Urgente =====
// ! TODO [ ] Confirmar que el emisor corre en Raspberry y el receptor en ESP32 LoRa 32
// ! TODO [ ] Definir y fijar el algoritmo de FCS (CRC/checksum) y pruebas asociadas

// ===== Hecho =====
// TODO [x] Crear archivo Cosas_que_hacer
// NOTE Mueve aquí las tareas completadas con [x]

