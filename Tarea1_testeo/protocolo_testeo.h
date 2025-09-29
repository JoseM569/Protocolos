#ifndef PROTOCOLO_TESTEO_H
#define PROTOCOLO_TESTEO_H

#include <stdint.h>

#define BYTE unsigned char

// Tamaños máximos según el layout elegido: LNG usa 6 bits => 0..63
#define LARGO_DATA 63
#define BYTES_EXTRA 4 // CMD + LNG + FCS(2 bytes)

// Layout de bits
// CMD ocupa los bits [5:2] del primer byte -> 0b0011 1100
#define CMD_MASK   0x3C
#define CMD_SHIFT  2

// LNG ocupa los bits [6:1] del segundo byte -> 0b0111 1110
#define LNG_MASK   0x7E
#define LNG_SHIFT  1

// Helpers para empaquetar/desempaquetar los campos lógicos
static inline BYTE pack_cmd(BYTE cmd4) {
    return (BYTE)((cmd4 & 0x0F) << CMD_SHIFT);
}

static inline BYTE unpack_cmd(BYTE byte) {
    return (BYTE)((byte & CMD_MASK) >> CMD_SHIFT);
}

static inline BYTE pack_lng(BYTE len6) {
    return (BYTE)((len6 & 0x3F) << LNG_SHIFT);
}

static inline BYTE unpack_lng(BYTE byte) {
    return (BYTE)((byte & LNG_MASK) >> LNG_SHIFT);
}

// FCS (CRC-16/CCITT-FALSE): 16 bits. Se envía en 2 bytes (big-endian recomendado)
//  - Byte alto: bits [15:8]
//  - Byte bajo: bits [7:0]
static inline BYTE fcs_hi(uint16_t fcs) {
    return (BYTE)((fcs >> 8) & 0xFF);
}

static inline BYTE fcs_lo(uint16_t fcs) {
    return (BYTE)(fcs & 0xFF);
}

static inline uint16_t fcs_from_bytes(BYTE hi, BYTE lo) {
    return (uint16_t)(((uint16_t)hi << 8) | lo);
}

// Estructura lógica del protocolo (valores lógicos, no empaquetados)
// - cmd: 0..15 (4 bits)
// - lng: 0..63 (6 bits)
// - data: hasta 63 bytes
// - fcs: 16 bits (CRC-16/CCITT-FALSE)
// - frame: buffer de salida para transmisión (cmd,lng,data,fcs)
struct protocolo
{
    BYTE cmd;                             // valor lógico 0..15
    BYTE lng;                             // valor lógico 0..63
    BYTE data[LARGO_DATA];                // payload
    uint16_t fcs;                         // 16 bits usados
    BYTE frame[LARGO_DATA + BYTES_EXTRA]; // buffer de frame serializable
};

#endif