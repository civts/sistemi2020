#ifndef __PACKETS_H__
#define __PACKETS_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.c"

#define READ 0
#define WRITE 1

typedef struct{
    pid_t pid;
    int pipeCP[2];
    int pipePC[2];
    // int numFilesProcessing;
} pInstance;

typedef struct{
    pid_t pid;
    int pipePQ[2];
    int pipeQP[2];
    int currM; // number of parts in which to split the file
    int index; // index of the file part this Q needs to analyze
} qInstance;

typedef struct{
    pid_t pid;
    int pipeMiniQQ[2];
    int currM; // number of parts in which to split the file
    int index; // index of the file part this Q needs to analyze
} miniQInstance;

// forward a packet without looking inside it's content. Useful for new filepath
int forwardPacket(int fd[], byte packetCode, int dataSectionSize, byte *dataSection){
    byte completePacket[1 + INT_SIZE + dataSectionSize];

    completePacket[0] = packetCode;
    fromIntToBytes(dataSectionSize, completePacket + 1);
    memcpy(completePacket + 1 + INT_SIZE, dataSection, dataSectionSize);

    return write(fd[WRITE], completePacket, 1 + INT_SIZE + dataSectionSize);
}

// Here there is a list of packets we transfer inside
// the components of the analyzer by packetCode:
// 0: new file
// 1: remove file packet
// 2: death packet
// 3: notify new M value
// 4: file results (backpropagation)

// Send death packet to a certain file descriptor
// this causes the exit from the infinite loop of
// of the process
// Error codes:
// 1 - Error with fd sending the death packet
int sendDeathPacket(int fd[]){
    int returnCode = 0;

    byte deathPacket[1 + INT_SIZE];
    deathPacket[0] = 2;
    fromIntToBytes(0, deathPacket + 1);

    if (write(fd[WRITE], deathPacket, 1 + INT_SIZE) != (1 + INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the death packet\n");
    }

    return returnCode;
}

int sendNewMPacket(int fd[], int newM){
    byte packet[1 + 2 * INT_SIZE];

    packet[0] = 3;
    fromIntToBytes(INT_SIZE, packet + 1);
    fromIntToBytes(newM, packet + 1 + INT_SIZE); // new value for m

    write(fd[WRITE], packet, 1 + 2 * INT_SIZE);
}

#endif