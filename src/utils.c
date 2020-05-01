#ifndef __UTILS_H__
#define __UTILS_H__

#include <string.h>
#define INT_SIZE 4
#define true 1
#define false 0

typedef unsigned int uint;
typedef unsigned char byte;
typedef char* string;
typedef int bool;

uint fromBytesToInt(byte[]);
void fromIntToBytes(uint, byte[]);
bool isDirectory(string, char);

// convert 4 bytes in unsigned int (little endian)
uint fromBytesToInt(byte *bytes){
    uint result = 0, base = 1;

    int i;
    for (i=INT_SIZE-1; i>=0; i--){
        result += bytes[i] * base;
        base *= 256;
    }

    return result;
}

// convert an unsigned int in 4 bytes (little endian)
void fromIntToBytes(uint value, byte out[]){
    uint result = 0, base = 0;
    int i;
    for (i=INT_SIZE-1; i>=0; i--){
        out[i] = ((value >> base) & 0xFF);
        base += 8;
    }
}

// check is a given path is a folder or not. You need
// to specify the endingChar (: or /).
bool isDirectory(string path, char endingChar){
    bool isDirectory = false;
    int pathLen = strlen(path);

    if (pathLen == 0){
        isDirectory = false;
    } else {
        isDirectory = (path[pathLen-1] == endingChar);
    }

    return isDirectory;
}

#endif