#ifndef __EEPROM_H
#define __EEPROM_H

#include <xboxkrnl/xboxkrnl.h>
#include <cstdint>

template<typename T>
T getEEPROMValue(uint32_t valueIndex) {
  ULONG Type = 0;
  return getEEPROMValue<T>(valueIndex, Type);
}

template<typename T>
T getEEPROMValue(uint32_t valueIndex, ULONG& Type) {
  T value;
  ULONG ResultLength = 0;
  ExQueryNonVolatileSetting(valueIndex, &Type, &value, sizeof(value), &ResultLength);
  return value;
}

#endif
