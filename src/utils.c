#include <string.h>
#define INT_SIZE 4
// How many characters we are considering
#define ASCII_LENGTH 256
#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif
typedef unsigned int uint;
typedef unsigned char byte;

uint fromBytesToInt(byte *);
void fromIntToBytes(uint, byte[]);

// convert 4 bytes in unsigned int (little endian)
uint fromBytesToInt(byte *bytes) {
  uint result = 0, base = 1;
  int i;
  for (i = INT_SIZE - 1; i >= 0; i--) {
    result += bytes[i] * base;
    base *= 256;
  }

  return result;
}

// convert an unsigned int in 4 bytes (little endian)
void fromIntToBytes(uint value, byte out[]) {
  uint result = 0, base = 0;
  int i;
  for (i = INT_SIZE - 1; i >= 0; i--) {
    out[i] = ((value >> base) & 0xFF);
    base += 8;
  }
}

// Util function for knowing if two paths (or strings in general) are the same
bool comparePaths(char *p1, char *p2) { return !strcmp(p1, p2); }