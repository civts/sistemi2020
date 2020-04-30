#define INT_SIZE 4
typedef unsigned int uint;
typedef unsigned char byte;
typedef char* string;

uint fromBytesToInt(byte[]);
void fromIntToBytes(uint, byte[]);

// convert 4 bytes in unsigned int (little endian)
uint fromBytesToInt(byte *bytes){
    uint result = 0, base = 1;

    for (int i=INT_SIZE-1; i>=0; i--){
        result += bytes[i] * base;
        base *= 256;
    }

    return result;
}

// convert an unsigned int in 4 bytes (little endian)
void fromIntToBytes(uint value, byte out[]){
    uint result = 0, base = 0;

    for (int i=INT_SIZE-1; i>=0; i--){
        out[i] = ((value >> base) & 0xFF);
        base += 8;
    }
}