#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "utils.c"
#include "miniQ.c"
#include "datastructures/miniQlist.c"

#define READ 0
#define WRITE 1

void q(qInstance*);

void waitForMessagesInQ(qInstance*);
void waitForMessagesInQFromP(qInstance*);
void waitForMessagesInQFromMiniQ(qInstance*);
int  processMessageInQFromP(byte, byte*, int, qInstance*);
int  processMessageInQFromMiniQ(byte, byte*, int, qInstance*);
int  processQNewFilePacket(byte[], int, qInstance*);
int  processQRemoveFilePacket(byte[], int);
int  processQDeathPacket();
int  processQNewValueForM(byte[], qInstance*);
int  processQFileResults(byte[], int, qInstance*);
void sig_handler_Q();

miniQlist *miniQs = NULL;

void q(qInstance *instanceOfMySelf){
    signal(SIGINT, sig_handler_Q);
    signal(SIGKILL, sig_handler_Q);
    miniQs = constructorMiniQlist();

    waitForMessagesInQ(instanceOfMySelf);
}

// infinite loop in which we read messages from its
// parent P and its children miniQ
void waitForMessagesInQ(qInstance *instanceOfMySelf){
    while (true){
        waitForMessagesInQFromP(instanceOfMySelf);
        waitForMessagesInQFromMiniQ(instanceOfMySelf);
    }
}

// here the messages can not be sent or received atomically
// since the new file pacekt contains the full file path as string
void waitForMessagesInQFromP(qInstance *instanceOfMySelf){
    int numBytesRead, dataSectionSize, offset;
    byte packetHeader[1 + INT_SIZE];

    numBytesRead = read(instanceOfMySelf->pipePQ[READ], packetHeader, 1 + INT_SIZE);

    if (numBytesRead == (1 + INT_SIZE)){
        dataSectionSize = fromBytesToInt(packetHeader + 1);

        offset = 0;
        byte packetData[dataSectionSize];

        // if we get a header then block and wait the whole message
        while (offset != dataSectionSize){
            numBytesRead = read(instanceOfMySelf->pipePQ[READ], packetData + offset, dataSectionSize - offset);
            if (numBytesRead > 0){
                offset += numBytesRead;
            } else if (numBytesRead < 0){
                fprintf(stderr, "Error reading from pipe P->Q\n");
            }
        }

        processMessageInQFromP(packetHeader[0], packetData, dataSectionSize, instanceOfMySelf);
    }
}

// here the messages arrives always atomically, so we don't
// need to check if the message is complete
void waitForMessagesInQFromMiniQ(qInstance *instanceOfMySelf){
    int numBytesRead, dataSectionSize, offset;
    byte packetHeader[1 + INT_SIZE];

    int i;
    NodeMiniQ *currElement = miniQs->first;
    for (i = 0; i < miniQs->counter; i++){
        numBytesRead = read(currElement->data->pipeToQ[READ], packetHeader, 1 + INT_SIZE);
        if (numBytesRead == (1 + INT_SIZE)){
            dataSectionSize = fromBytesToInt(packetHeader + 1);
            byte packetData[dataSectionSize];

            numBytesRead  = read(currElement->data->pipeToQ[READ], packetData, dataSectionSize);
            processMessageInQFromMiniQ(packetHeader[0], packetData, dataSectionSize, instanceOfMySelf);
        }
        currElement = currElement->next;
    }
}

int processMessageInQFromP(byte packetCode, byte *packetData, int packetDataSize, qInstance *instanceOfMySelf){
    int returnCode;
    switch (packetCode){
        case 0:
            returnCode = processQNewFilePacket(packetData, packetDataSize, instanceOfMySelf);
            break;
        case 1:
            returnCode = processQRemoveFilePacket(packetData, packetDataSize);
            break;
        case 2:
            returnCode = processQDeathPacket();
            break;
        case 3:
            returnCode = processQNewValueForM(packetData, instanceOfMySelf);
            break;
        default:
            fprintf(stderr, "Error, Q received from P an unknown packet type %d\n", packetCode);
            returnCode = 1;
    }

    return returnCode;
}

int processMessageInQFromMiniQ(byte packetCode, byte *packetData, int packetDataSize, qInstance *instanceOfMySelf){
    int returnCode;
    switch (packetCode){
        case 6:
            returnCode = processQFileResults(packetData, packetDataSize, instanceOfMySelf);
            break;
        default:
            fprintf(stderr, "Error, Q received from miniQ an unknown packet type %d\n", packetCode);
            returnCode = 1;
    }

    return returnCode;
}

int processQNewFilePacket(byte packetData[], int packetDataSize, qInstance* instanceOfMySelf){
    bool isInsideFolder = packetData[0];
    char pathName[packetDataSize];
    // string pathName = (string) malloc((packetDataSize) * sizeof(char));
    memcpy(pathName, packetData + 1, packetDataSize - 1);
    pathName[packetDataSize - 1] = '\0';

    int pipeMiniQQ[2];
    pipe(pipeMiniQQ);
    fcntl(pipeMiniQQ[READ], F_SETFL, O_NONBLOCK);
    miniQinfo *newMiniQ = constructorMiniQinfo(0, 0, pipeMiniQQ, instanceOfMySelf->currM, instanceOfMySelf->index);

    // create miniQ process
    pid_t f;
    f = fork();
    if (f < 0){
        fprintf(stderr, "Error, creating miniQ\n");
    } else if (f == 0){
        printf("Created miniQ\n");
        miniQ(pathName, isInsideFolder, newMiniQ);
    } else {
        newMiniQ->pid = f;
        appendMiniQ(miniQs, constructorNodeMiniQ(newMiniQ));
    }

    return 0;
}

int processQRemoveFilePacket(byte packetData[], int packetDataSize){
    // TODO search in miniQ list for that file and kill it
    return -1;
}

int processQDeathPacket(){
    // TODO kill miniQs
    // TODO free list of miniQ
    printf("Q is dead\n");
    exit(0);
    return 0;
}

int processQNewValueForM(byte packetData[], qInstance* instanceOfMySelf){
    instanceOfMySelf->currM = fromBytesToInt(packetData);

    // TODO kill all miniQ created with the old M value and restart them
    return 0;
}

int processQFileResults(byte packetData[], int packetDataSize, qInstance *instanceOfMySelf){
    int returnCode = 0;
    if (forwardPacket(instanceOfMySelf->pipeQP, 4, packetDataSize, packetData) < 0){
        returnCode = 1;
    }
    // TODO - get file id from packetData
    removeMiniQByFileId(miniQs, 0);

    return returnCode;
}

void sig_handler_Q(){
    printf("\nQ killed with signal\n");
    processQDeathPacket();
    exit(0);
}