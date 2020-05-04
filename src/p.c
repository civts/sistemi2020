#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.c"
#include "q.c"

// TODO we need some exit(0) here, but where?

#define READ 0
#define WRITE 1

void p(int, int*);
void waitForMessagesFromController(pid_t*, int[2], int**, int);
int  processPNewFilePacket(byte[], int, int**, int);
int  processPRemoveFilePacket(byte[], int**, int);
int  processPDeathPacket(byte[], int**, int);
int  processPNewValueForM(byte[], pid_t[], int**, int);

void p(int m, int *pipeFromController){
    pid_t *pids = (pid_t*) malloc(m * sizeof(pid_t));
    // Here I am building 2 pipes for each Q, one for the communication P->Q
    // and the other for the communication Q->P
    int **pipeListToQ   = (int **) malloc(m * sizeof(int*));
    int **pipeListFromQ = (int **) malloc(m * sizeof(int*));

    int i, f=1;
    for (i = 0; i < m && f != 0; i++){
        pipeListToQ[i]   = (int*) malloc(sizeof(int) * 2);
        pipeListFromQ[i] = (int*) malloc(sizeof(int) * 2);

        // TODO: check syscall return
        pipe(pipeListToQ[i]);
        pipe(pipeListFromQ[i]);

        f = fork();
        if (f < 0){
            printf("Error creating Q\n");
            i--;
        } else if (f == 0){
            // child
            printf("Q%d created\n", i);
            close(pipeListToQ[i][WRITE]);
            close(pipeListFromQ[i][READ]);
            q(i, m, pipeListToQ[i], pipeListFromQ[i]);
        } else {
            // parent
            close(pipeListToQ[i][READ]);
            close(pipeListFromQ[i][WRITE]);
            pids[i] = f;
        }
    }

    waitForMessagesFromController(pids, pipeFromController, pipeListToQ, m);
}

void waitForMessagesFromController(pid_t *qPids, int pipeFromC[2], int **pipeListToQ, int m){
    byte packetHeader[1 + INT_SIZE];
    int  packetLength = 0, receivedPacketFragmentsLength = 0;
    int  returnCodeFromPacketProcess = 0;
    byte *packetData;
    
    while (true){
        // TODO wait for the whole packet if it is fragmented
        receivedPacketFragmentsLength = read(pipeFromC[READ], packetHeader, 1 + INT_SIZE);
        packetLength = fromBytesToInt(packetHeader + 1);
        packetData   = (byte*) malloc(sizeof(byte) * packetLength);

        if (packetLength == 1 + INT_SIZE + read(pipeFromC[READ], packetData, packetLength)){
            switch (packetHeader[0]){
                case 0:
                    returnCodeFromPacketProcess = processPNewFilePacket(packetData, packetLength, pipeListToQ, m);
                    break;
                case 1:
                    returnCodeFromPacketProcess = processPRemoveFilePacket(packetData, pipeListToQ, m);
                    break;
                case 2:
                    returnCodeFromPacketProcess = processPDeathPacket(packetData, pipeListToQ, m);
                    break;
                case 3:
                    returnCodeFromPacketProcess = processPNewValueForM(packetData, qPids, pipeListToQ, m);
                    break;
                default:
                    printf("Error, P received from C an unknown packet type %d\n", packetHeader[0]);
            }

            // in case of death packet, exit from 
            if (packetHeader[0] == 2 && returnCodeFromPacketProcess == 0){
                break;
            }
        } else {
            printf("Error, write not atomic\n");
        }
    }
}

// Only rebuilds the header and forward with no modifications
int processPNewFilePacket(byte packetData[], int packetLength, int **pipeListToQ, int m){
    byte packetHeader[1 + INT_SIZE];
    packetHeader[0] = 0; // new file packet type
    fromIntToBytes(0, packetHeader + 1);

    int i;
    for (i = 0; i < m; i++){
        write(pipeListToQ[i][WRITE], packetHeader, packetLength);
    }
}

int processPRemoveFilePacket(byte packetData[], int **pipeListToQ, int m){
    // TODO (not now)
    // currently we are not able to remove files during analysis
}

int processPDeathPacket(byte packetData[], int **pipeListToQ, int m){
    // 1. inform q to stop
    byte packetHeader[1 + INT_SIZE];
    packetHeader[0] = 2; // death packet type
    fromIntToBytes(0, packetHeader + 1);

    int i;
    for (i = 0; i < m; i++){
        write(pipeListToQ[i][WRITE], packetHeader, 1 + INT_SIZE); // send death packet to all its Qs
    }

    // TODO
    // 2. free resources
    // we need to pass to this function even pipesFromQ
}

int processPNewValueForM(byte packetData[], pid_t qPids[], int **pipeListToQ, int m){
    // the packet data conists only in the new m value
    uint m_new = fromBytesToInt(packetData);
    
    // TODO reshape for new M value

    // to delete Q, send packet of death
}