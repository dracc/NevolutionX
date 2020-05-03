#ifndef XBE_H
#define XBE_H

#define XBETYPEMAGIC 0x48454258
#define XBENAMESIZE 40
#define BASEADDRESS 0x10000

typedef struct _XBE{
  uint32_t type; // +0x00
  uint8_t  padding_4[0xFF];
  uint32_t baseAddress; // +0x104
  uint32_t sizeOfHeaders; // +0x108
  uint8_t  padding_268[0x0C];
  uint32_t certAddress;  // +0x118
  uint8_t  padding_284[0x58];
} XBE;

typedef struct _XBECert{
  uint8_t padding_0[0x0C];
  uint16_t xbeName[XBENAMESIZE]; // +0x0C
  uint8_t padding_92[0x0190];
} XBECert;

#endif
