#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <limits>
#include <iomanip>
#include <sstream>
 #include <wiringPi.h>
 #include <wiringSerial.h>
 #include <errno.h>
 #include <cstring>

// Emisor de pruebas (Raspberry Pi) - Menú interactivo
// Este archivo implementa un menú que construye y "envía" comandos
// mediante una función stub sendPacket() que puedes conectar a tu
// transporte real (LoRa, UART, TCP, etc.).
//
// Comandos requeridos por el enunciado:
// 1) Mostrar mensaje de control/imagen en OLED (receptor)
// 2) Enviar 10 mensajes de prueba (receptor calcula % acierto/error)
// 3) Enviar texto para mostrar en OLED (receptor)
// 4) Enviar temperatura flotante [-40.0, 40.0] (receptor la muestra)
// 5) Toggle parpadeo LED gpio25 (receptor, 1 Hz por defecto)
// 6) Cambiar frecuencia parpadeo LED [1..100] Hz (receptor)
// 7) Solicitar e imprimir contador/estadísticas (sin contar test)
// 8) Extra: enviar arreglo con últimas 8 temperaturas
// 9) Mostrar contador local de mensajes enviados
// 0) Salir

namespace Proto {
    enum class Cmd : uint8_t {
        ShowControlMsg   = 0x01,
        SendTestBurst10  = 0x02,
        ShowTextOLED     = 0x03,
        SendTemperature  = 0x04,
        ToggleBlinkLED   = 0x05,
        SetBlinkFreqHz   = 0x06,
        PrintStats       = 0x07,
        SendLast8Temps   = 0x08,
    };

    struct Packet {
        uint16_t length;             // Largo total o de DATA según definas tu protocolo
        uint8_t  cmd;                // Código de comando
        std::vector<uint8_t> data;   // Payload
        uint16_t fcs;                // FCS (CRC/Checksum) - aquí a modo de placeholder
    };
}

// Transporte por Serial (wiringPi)
static int g_serialFd = -1;
static const char* SERIAL_DEV = "/dev/ttyUSB0";   // Ajusta según tu hardware: /dev/ttyAMA0, /dev/ttyS0, etc.
static const int   SERIAL_BAUD = 115200;           // Ajusta si es necesario

static bool transportInit() {
    // wiringPiSetupGpio usa numeración BCM
    if (wiringPiSetupGpio() != 0) {
        std::cerr << "[ERR] wiringPiSetupGpio() falló" << std::endl;
        return false;
    }
    g_serialFd = serialOpen(SERIAL_DEV, SERIAL_BAUD);
    if (g_serialFd < 0) {
        std::cerr << "[ERR] No se pudo abrir " << SERIAL_DEV << " a " << SERIAL_BAUD
                  << " baud: " << std::strerror(errno) << std::endl;
        return false;
    }
    std::cout << "[OK] Serial abierto en " << SERIAL_DEV << " @" << SERIAL_BAUD << " baud\n";
    return true;
}

static void transportClose() {
    if (g_serialFd >= 0) {
        serialClose(g_serialFd);
        g_serialFd = -1;
    }
}

static bool transportSend(const std::vector<uint8_t>& bytes) {
    if (g_serialFd < 0) {
        std::cerr << "[ERR] Serial no inicializado" << std::endl;
        return false;
    }
    for (auto b : bytes) {
        serialPutchar(g_serialFd, b);
    }
    serialFlush(g_serialFd);
    return true;
}

static uint64_t g_sentCounter = 0;               // Contador de mensajes enviados (emisor)
static std::vector<float> g_lastTemps;           // Buffer rotativo (hasta 8)
static const size_t MAX_TEMPS = 8;

// Placeholder de cálculo de FCS. Reemplazar por CRC-16/Checksum real.
static uint16_t computeFCS(const std::vector<uint8_t>& bytes) {
    uint16_t s = 0;
    for (auto b : bytes) s = static_cast<uint16_t>((s + b) & 0xFFFF);
    return s;
}

// Codifica un paquete simple para depuración (ejemplo): [CMD | DATA...]
static std::vector<uint8_t> buildPayload(Proto::Cmd cmd, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> out;
    out.push_back(static_cast<uint8_t>(cmd));
    out.insert(out.end(), data.begin(), data.end());
    return out;
}

// Stub de envío: en esta versión sólo imprime en consola lo que se enviaría.
// Conéctalo a tu capa de transporte real.
static void sendPacket(Proto::Cmd cmd, const std::vector<uint8_t>& data) {
    auto payload = buildPayload(cmd, data);

    // Construir paquete de ejemplo (no fija estructura definitiva)
    Proto::Packet pkt;
    pkt.cmd    = static_cast<uint8_t>(cmd);
    pkt.data   = data;
    pkt.length = static_cast<uint16_t>(1 + pkt.data.size()); // ejemplo: CMD + DATA
    pkt.fcs    = computeFCS(payload);

    // Log de lo que "enviamos"
    std::cout << "\n[EMISOR] Enviando paquete" << std::endl;
    std::cout << "  CMD: 0x" << std::hex << std::uppercase
              << std::setw(2) << std::setfill('0')
              << static_cast<int>(pkt.cmd) << std::dec << std::nouppercase
              << "  LEN: " << pkt.length
              << "  FCS: 0x" << std::hex << std::uppercase
              << std::setw(4) << std::setfill('0') << pkt.fcs
              << std::dec << std::nouppercase << std::endl;

    if (!pkt.data.empty()) {
        std::cout << "  DATA(" << pkt.data.size() << "): ";
        for (auto b : pkt.data) {
            std::cout << std::hex << std::uppercase
                      << std::setw(2) << std::setfill('0')
                      << static_cast<int>(b) << ' ';
        }
        std::cout << std::dec << std::nouppercase << std::endl;
    }

    // Envío real por serial (payload = [CMD | DATA])
    if (!transportSend(payload)) {
        std::cerr << "[ERR] Falló el envío por serial" << std::endl;
    } else {
        ++g_sentCounter;
    }
}

static int readIntInRange(const std::string& prompt, int minV, int maxV) {
    while (true) {
        std::cout << prompt;
        int v; 
        if (std::cin >> v) {
            if (v >= minV && v <= maxV) return v;
            std::cout << "Valor fuera de rango. Debe estar entre " << minV << " y " << maxV << ".\n";
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada inválida. Intente nuevamente.\n";
        }
    }
}

static float readFloatInRange(const std::string& prompt, float minV, float maxV) {
    while (true) {
        std::cout << prompt;
        float v; 
        if (std::cin >> v) {
            if (v >= minV && v <= maxV) return v;
            std::cout << "Valor fuera de rango. Debe estar entre " << minV << " y " << maxV << ".\n";
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada inválida. Intente nuevamente.\n";
        }
    }
}

static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin >> std::ws, s);
    return s;
}

static std::vector<uint8_t> toBytesFromString(const std::string& s) {
    return {s.begin(), s.end()};
}

static std::vector<uint8_t> toBytesFromFloat(float f) {
    // Representación simple como texto (ej: "10.1").
    // Si prefieres binario IEEE-754, reemplace por reinterpret_cast a 4 bytes.
    std::string s = std::to_string(f);
    return toBytesFromString(s);
}

static std::vector<uint8_t> toBytesFromUInt16(uint16_t v) {
    // Little endian por defecto. Ajusta según tu protocolo.
    return { static_cast<uint8_t>(v & 0xFF), static_cast<uint8_t>((v >> 8) & 0xFF) };
}

static void addTemperatureToBuffer(float t) {
    if (g_lastTemps.size() < MAX_TEMPS) {
        g_lastTemps.push_back(t);
    } else {
        // buffer rotativo
        for (size_t i = 1; i < MAX_TEMPS; ++i) g_lastTemps[i-1] = g_lastTemps[i];
        g_lastTemps[MAX_TEMPS-1] = t;
    }
}

static void sendLast8Temperatures() {
    // Empaqueta como texto separado por comas para simplicidad (puedes cambiar a binario)
    std::ostringstream oss;
    for (size_t i = 0; i < g_lastTemps.size(); ++i) {
        if (i) oss << ',';
        oss << g_lastTemps[i];
    }
    auto data = toBytesFromString(oss.str());
    sendPacket(Proto::Cmd::SendLast8Temps, data);
}

static void printMenu() {
    std::cout << "\n===== MENÚ EMISOR (TEST) =====\n";
    std::cout << "1) Mostrar mensaje de control/imagen en OLED\n";
    std::cout << "2) Enviar 10 mensajes de prueba\n";
    std::cout << "3) Enviar texto para mostrar en OLED\n";
    std::cout << "4) Enviar temperatura flotante [-40.0, 40.0]\n";
    std::cout << "5) Toggle parpadeo LED (gpio 25)\n";
    std::cout << "6) Cambiar frecuencia parpadeo LED [1..100] Hz\n";
    std::cout << "7) Solicitar impresión de contador/estadísticas (receptor)\n";
    std::cout << "8) Enviar arreglo con últimas 8 temperaturas (extra)\n";
    std::cout << "9) Mostrar contador local de mensajes enviados\n";
    std::cout << "0) Salir\n";
}

int main() {
    if (!transportInit()) {
        return 1;
    }

    bool running = true;
    while (running) {
        printMenu();
        int opt = readIntInRange("Seleccione opción: ", 0, 9);

        switch (opt) {
            case 1: {
                std::string msg = "dispositivo funcionando"; // o una figura codificada
                sendPacket(Proto::Cmd::ShowControlMsg, toBytesFromString(msg));
                break;
            }
            case 2: {
                // Enviar 10 mensajes de prueba
                for (int i = 1; i <= 10; ++i) {
                    std::string msg = "TEST " + std::to_string(i);
                    sendPacket(Proto::Cmd::SendTestBurst10, toBytesFromString(msg));
                }
                std::cout << "Se enviaron 10 mensajes de prueba.\n";
                break;
            }
            case 3: {
                auto text = readLine("Texto para OLED: ");
                sendPacket(Proto::Cmd::ShowTextOLED, toBytesFromString(text));
                break;
            }
            case 4: {
                float t = readFloatInRange("Temperatura [-40.0..40.0]: ", -40.0f, 40.0f);
                addTemperatureToBuffer(t);
                sendPacket(Proto::Cmd::SendTemperature, toBytesFromFloat(t));
                break;
            }
            case 5: {
                // Toggle parpadeo: sin payload
                sendPacket(Proto::Cmd::ToggleBlinkLED, {});
                break;
            }
            case 6: {
                int hz = readIntInRange("Frecuencia LED [1..100] Hz: ", 1, 100);
                auto data = toBytesFromUInt16(static_cast<uint16_t>(hz));
                sendPacket(Proto::Cmd::SetBlinkFreqHz, data);
                break;
            }
            case 7: {
                // Solicitar impresión de contador/estadísticas (receptor)
                sendPacket(Proto::Cmd::PrintStats, {});
                break;
            }
            case 8: {
                if (g_lastTemps.empty()) {
                    std::cout << "No hay temperaturas almacenadas. Envía alguna primero (opción 4).\n";
                } else {
                    sendLast8Temperatures();
                }
                break;
            }
            case 9: {
                std::cout << "Contador local de mensajes enviados: " << g_sentCounter << "\n";
                break;
            }
            case 0: {
                running = false;
                break;
            }
        }
    }

    std::cout << "Saliendo... Total enviados: " << g_sentCounter << "\n";
    transportClose();
    return 0;
}

