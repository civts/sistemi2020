#ifndef __UTILS_H__
#define __UTILS_H__

#include <string.h>
#define INT_SIZE 4
#define true 1
#define false 0
#define MINIQ_MAX_BUFFER_SIZE 4096

typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned long long ull;
typedef char* string;
typedef int bool;

uint fromBytesToInt(byte[]);
void fromIntToBytes(uint, byte[]);
bool isDirectory(string, char, int *out);
long min_l(long, long);
long max_l(long, long);
int  min_i(int, int);
int  max_i(int, int);

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
// out: 0 for ok, 1 for path null pointer
bool isDirectory(string path, char endingChar, int *out){
    bool isDirectory = false;

    if (path != NULL){
        int pathLen = strlen(path);
        *out = 0;

        if (pathLen == 0){
            isDirectory = false;
        } else {
            isDirectory = (path[pathLen-1] == endingChar);
        }

    } else {
        *out = 1;
    }

    return isDirectory;
}

long min_l(long a, long b){
    return a < b ? a : b;
}

long max_l(long a, long b){
    return a > b ? a : b;
}

int min_i(int a, int b){
    return a < b ? a : b;
}

int max_i(int a, int b){
    return a > b ? a : b;
}


#endif