#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "utils.c"
#include "miniQ.c"

#define READ 0
#define WRITE 1

// TODO check for exit(0) here... we do not want zombies

void q(int, int, int*, int*);
int  processQNewFilePacket(byte[], int, int, int);
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
        int numBytesRead = read(pipeFromP[READ], packetData, packetLength);
        
        if (packetLength == numBytesRead){
            switch (packetHeader[0]){
                case 0:
                    returnCodeFromPacketProcess = processQNewFilePacket(packetData, packetLength, numPartToSplit, myIndex);
                    break;
                case 1:
                    returnCodeFromPacketProcess = processQRemoveFilePacket(packetData);
                    break;
                case 2:
                    returnCodeFromPacketProcess = processQDeathPacket(packetData);
                    break;
                case 3:
                    returnCodeFromPacketProcess = processQNewValueForM(packetData);
                    break;
                default:
                    printf("Error, P received from C an unknown packet type %d\n", packetHeader[0]);
            }

            // in case of death packet, exit from 
            if (packetHeader[0] == 2 && returnCodeFromPacketProcess == 0){
                break;
            }
        } else if (numBytesRead==0){
            printf("0 on Q\n");
        } else if (numBytesRead==-1){
            printf("-1 on Q\n");
        } else {
            printf("Error, write not atomic in Q\n");
        }
        free(packetData);
    }
}

// TODO do I have to free pathName in both parent and child?
int processQNewFilePacket(byte packetData[], int packetDataSize, int numPartToSplit, int myIndex){
    bool isInsideFolder = packetData[0];
    string pathName = (string) malloc((packetDataSize) * sizeof(char));
    memcpy(pathName, packetData + 1, packetDataSize - 1);
    pathName[packetDataSize - 1] = '\0';

    pid_t f;
    f = fork();
    if (f < 0){
        printf("Error, creating miniQ\n");
    } else if (f == 0){
        printf("Created miniQ\n");
        miniQ(pathName, isInsideFolder, numPartToSplit, myIndex);
    } else {
        // do nothing
        free(pathName);
    }

    return 0;
}

int processQRemoveFilePacket(byte packetData[]){
    return -1;
}
int processQDeathPacket(byte packetData[]){
    printf("Q is dead\n");
    return 0;
}
int processQNewValueForM(byte packetData[]){
    return -1;
}
