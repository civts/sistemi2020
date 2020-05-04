#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "utils.c"
#include "miniQ.c"

#define READ 0
#define WRITE 1

// TODO we need some exit(0), but where?

void q(int, int, int*, int*);
int  processQNewFilePacket(byte[], int, int);
int  processQRemoveFilePacket(byte[]);
int  processQDeathPacket(byte[]);
int  processQNewValueForM(byte[]);

void q(int numPartToSplit, int myIndex, int *pipeToP, int *pipeFromP){
    byte packetHeader[1 + INT_SIZE];
    int  packetLength = 0, receivedPacketFragmentsLength = 0;
    int  returnCodeFromPacketProcess = 0;
    byte *packetData;

    while (true){
        // TODO wait for the whole packet if it is fragmented
        receivedPacketFragmentsLength = read(pipeFromP[READ], packetHeader, 1 + INT_SIZE);
        packetLength = fromBytesToInt(packetHeader + 1);
        packetData   = (byte*) malloc(sizeof(byte) * packetLength);

        if (packetLength == 1 + INT_SIZE + read(pipeFromP[READ], packetData, packetLength)){
            switch (packetHeader[0]){
                case 0:
                    returnCodeFromPacketProcess = processNewFilePacket(packetData, numPartToSplit, myIndex);
                    break;
                case 1:
                    returnCodeFromPacketProcess = processRemoveFilePacket(packetData);
                    break;
                case 2:
                    returnCodeFromPacketProcess = processDeathPacket(packetData);
                    break;
                case 3:
                    returnCodeFromPacketProcess = processNewValueForM(packetData);
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

int processQNewFilePacket(byte packetData[], int numPartToSplit, int myIndex){
    // TODO extarct file path and isInside folder from packetData

    pid_t f;
    f = fork();
    if (f < 0){
        printf("Error, creating miniQ\n");
    } else if (f == 0){
        miniQ("file to pass", false, numPartToSplit, myIndex);
    } else {
        // parent do nothing
    }
}