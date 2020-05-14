#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // pid_t -> in crawler I use pid_t without types.h
#include "packets.h"
#include "utils.c"
#include "p.c"

#define READ 0
#define WRITE 1

void controller(int, int, string[], int);
int  shapeTree(int, int);
void notifyNewMToPInstance(pInstance*, int);
void killInstanceOfP(int);
void sendPathNameToP(string, int, bool);

pInstance *pInstances = NULL; // P processes associated to C

int currN = 0;
int currM = 0;

// main worker of the C process
void controller(int n, int m, string files[], int numFiles){
    shapeTree(n, m);

    int i;
    for (i = 0; i < numFiles; i++){
        sendPathNameToP(files[i], i % currN, false);
    }

    // backbone of the correct implementation
    // while (true){
    //     getMessagesFromA()
    //     sendMessagesToA()
    //     getMessagesFromPs()
    //     sendMessagesToP()
    //     sendMessagesToR()
    // }
}

// TODO check for syscall close fails: what to do?
int generateNewPInstance(pInstance *newP, int index, int newM){
    int returnCode = 0;

    if (pipe(newP->pipeCP) != -1 && pipe(newP->pipePC) != -1){
        // TODO check for error -1
        // make the pipes non blocking
        fcntl(newP->pipeCP[READ], F_SETFL, O_NONBLOCK);
        fcntl(newP->pipePC[READ], F_SETFL, O_NONBLOCK);

        newP->pid = fork();

        if (newP->pid < 0){
            fprintf(stderr, "Found an error creating P%d\n", index);
            returnCode = 2;
        } else if (newP->pid == 0){
            // child: new instance of P
            fprintf(stderr, "New P%d created\n", index);
            close(newP->pipeCP[WRITE]);
            close(newP->pipePC[READ]);

            p(newP, newM);
            exit(0); // just to be sure... it should not be necessary
        } else {
            // parent
            close(newP->pipeCP[READ]);
            close(newP->pipePC[WRITE]);
        }
    } else {
        fprintf(stderr, "Found an error creting pipes to P%d\n", index);
        returnCode = 1;
    }

    return returnCode;
}

// Reshape all the three given new m and n values
// Error codes:
// 1 - Not enough space to allocate new P table
// 2 - Failed to generate new P process
int shapeTree(int newN, int newM){
    int i, returnCode = 0;

    // free exceeding instances of P and update M value for old ones
    for (i = 0; i < currN; i++){
        if (i >= newN){
            killInstanceOfP(i);
        } else if (newM != currM){
            notifyNewMToPInstance(pInstances + i, newM);
        }
    }

    // allocate the space necessary for new_n
    pInstances = (pInstance*) realloc(pInstances, newN * sizeof(pInstance));

    if (pInstances == NULL){
        fprintf(stderr, "Not enough space to allocate new P table\n");
        returnCode = 1;
    } else {
        // generate new instances of P if necessary
        for (i = currN; i < newN && returnCode == 0; i++){
            if (generateNewPInstance(pInstances + i, i, newM) != 0){
                fprintf(stderr, "Failed to generate new P process\n");
                returnCode = 2;
            }
        }
    }
    
    currN = newN;
    currM = newM;

    return returnCode;
}

void notifyNewMToPInstance(pInstance *instanceOfP, int newM){
    printf("Update m to %d\n", newM);
    sendNewMPacket(instanceOfP->pipeCP, newM);
}

void killInstanceOfP(int pIndex){
    // TODO - free resources
    printf("Stacca stacca: %d\n", pIndex);
    sendDeathPacket(pInstances[pIndex].pipeCP);
}

void sendPathNameToP(string pathName, int indexOfP, bool isInsideFolder){
    int packetLength = 1 + INT_SIZE + 1 + strlen(pathName); // packet type, data length, isInFolder, pathName
    byte* packet = (byte*) malloc(packetLength * sizeof(byte));
    int offset = 0;

    // packet type: new file packet
    packet[offset] = 0;
    offset++;

    // data length
    fromIntToBytes(packetLength - 1 - INT_SIZE, packet + offset);
    offset += INT_SIZE;

    // isInsideFolder flag
    packet[offset] = (byte)isInsideFolder;
    offset++;

    // pathName
    memcpy(packet + offset, pathName, strlen(pathName));

    write(pInstances[indexOfP].pipeCP[WRITE], packet, packetLength);
    free(packet);
}

// only for debug... wait a certain amount of time
void wait_a_bit(){
    long long int i;
    for (i=0; i<999999999; i++){}
}

// int main(){
//     string files[2] = {"prova1.txt", "prova2.txt"};
//     controller(2, 2, files, 2);
//     fflush(stdout);
//     wait_a_bit(); 

//     shapeTree(4, 2);
//     wait_a_bit(); 

//     int y;
//     for (y = 0; y < currN; y++){
//         killInstanceOfP(y);
//     }

//     fflush(stdout);
//     wait_a_bit();
    
//     printf("Fine\n");
//     free(pInstances);
//     return 0;
// }