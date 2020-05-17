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
// 4: notify new N value
// 5: start analisys packet
// 6: file results (backpropagation)

/**
 * TODO: Fracheck !
 * This function sends the newFilePacket to the file descriptor
 * in the arguments.
 * Error codes:
 * 1 - Error with the fd sending name packet
 */
int sendNewFilePacket(int fd[], string fileName){
    int returnCode = 0;

    int thisSize = 1 + INT_SIZE + sizeof(fileName);
    byte newFilePacket[thisSize];
    newFilePacket[0] = 0;
    fromIntToBytes(sizeof(fileName), newFilePacket + 1);
    memcpy(newFilePacket+2, fileName, sizeof(fileName));

    if (write(fd[WRITE], newFilePacket, thisSize) != (thisSize)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the new file packet\n");
    }

    return returnCode;
}

/**
 * TODO: Fracheck !
 * This function sends the removeFilePacket to the 
 * file descriptor as argument.
 * 1 - Error with the fd sending name packet
 */
int removeFilePacket(int fd[], string fileName){
    int returnCode = 0;

    int thisSize = 1 + INT_SIZE + sizeof(fileName);
    byte newFilePacket[thisSize];
    newFilePacket[0] = 1;
    fromIntToBytes(sizeof(fileName), newFilePacket + 1);
    memcpy(newFilePacket+2, fileName, sizeof(fileName));

    if (write(fd[WRITE], newFilePacket, thisSize) != (thisSize)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the remove file packet\n");
    }

    return returnCode;
}

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

/**
 * This function sends the packet with the new value of M.
 */
int sendNewMPacket(int fd[], int newM){
    byte packet[1 + 2 * INT_SIZE];

    packet[0] = 3;
    fromIntToBytes(INT_SIZE, packet + 1);
    fromIntToBytes(newM, packet + 1 + INT_SIZE); // new value for m

    write(fd[WRITE], packet, 1 + 2 * INT_SIZE);
}

/**
 * TODO: Fracheck !
 * This function sends the packet with the new value of N.
 */
int sendNewNPacket(int fd[], int newN){
    byte packet[1 + 2 * INT_SIZE];

    packet[0] = 4;
    fromIntToBytes(INT_SIZE, packet + 1);
    fromIntToBytes(newN, packet + 1 + INT_SIZE); // new value for n

    write(fd[WRITE], packet, 1 + 2 * INT_SIZE);
}

/**
 * TODO: Fracheck !
 * This function sends the start analisys packet.
 * Error codes:
 * 1 - Error with fd sending the death packet
 */
int startAnalisysPacket(int fd[]){
    int returnCode = 0;
    byte packet[1 + INT_SIZE];
    
    packet[0] = 5;
    fromIntToBytes(0, packet + 1);
    if (write(fd[WRITE], packet, 1 + INT_SIZE) != (1 + INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the death packet\n");
    }

    return returnCode;
}


#endif