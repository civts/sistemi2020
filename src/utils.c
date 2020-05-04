#ifndef UTILS_H
#define UTILS_H
#include <string.h>
#define INT_SIZE 4
// How many characters we are considering
#define ASCII_LENGTH 256
#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif
#define DEBUGGING true
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

// Util function for knowing if two strings are the same TESTED✔️
bool streq(const char *p1,const char *p2) { return !strcmp(p1, p2); }

// Returns if the array of strings [pool] contains the given option. TESTED✔️
// (useful for checking that params are correct)
bool contains(int poolLength,const char *pool[],const char *option) {
  int i;
  bool found = false;
  for (i = 0; i < poolLength; i++)
    if (streq(option, pool[i])) {
      found = true;
      break;
    }
  return found;
}
#endif
