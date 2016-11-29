#include <EEPROM.h>

// Write a 4 byte long to the eeprom
void EEPROMWriteLong (int address, long value) {
  // One = Most significant
  // Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  EEPROM.write(4*address, four);
  EEPROM.write(4*address + 1, three);
  EEPROM.write(4*address + 2, two);
  EEPROM.write(4*address + 3, one);
}

long EEPROMReadLong (long address) {
  long four = EEPROM.read(4*address);
  long three = EEPROM.read(4*address + 1);
  long two = EEPROM.read(4*address + 2);
  long one = EEPROM.read(4*address + 3);

  return four + (three << 8) + (two << 16) + (one << 24);
}
