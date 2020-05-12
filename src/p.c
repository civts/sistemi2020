#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "packets.h"
#include "utils.c"
#include "q.c"

#define READ 0
#define WRITE 1

int  p(pInstance*, int);
int  generateNewQInstance(qInstance*, int, int);
void waitForMessages(pInstance*);
void waitForMessagesFromQ();
void waitForMessagesFromController(pInstance*);
int  processMessageFromQ(byte, byte*, int, pInstance*);
int  processMessageFromController(byte, byte*, int, pInstance*);

int  processPNewFilePacket(byte[], int);
int  processPRemoveFilePacket(byte[]);
int  processPDeathPacket();
int  processPNewValueForM(byte[], pInstance*);

qInstance *qInstances = NULL; // Q processes associated to this P
int currM;

int p(pInstance *instanceOfMySelf, int _currM){
    int i, returnCode = 0;
    qInstances = (qInstance*) realloc(qInstances, sizeof(qInstance) * _currM);

    if (qInstances == NULL){
        fprintf(stderr, "Error allocating Q table inside P\n");
        returnCode = 1;
    } else {
        for (i = 0; i < _currM; i++){
            generateNewQInstance(qInstances + i, i, _currM);
        }
    }

    currM = _currM;
    waitForMessages(instanceOfMySelf);

    return returnCode;
}

int generateNewQInstance(qInstance *newQ, int index, int currM){
    int returnCode = 0;

    if (pipe(newQ->pipePQ) != -1 && pipe(newQ->pipeQP) != -1){
        newQ->pid = fork();

        if (newQ->pid < 0){
            fprintf(stderr, "Found an error creating Q%d\n", index);
            returnCode = 2;
        } else if (newQ->pid == 0){
            // child: new instance of Q
            fprintf(stderr, "New Q%d created\n", index);
            close(newQ->pipePQ[WRITE]);
            close(newQ->pipeQP[READ]);

            q(currM, index, newQ->pipeQP, newQ->pipePQ);
            exit(0); // just to be sure... it should not be necessary
        } else {
            // parent
            close(newQ->pipePQ[READ]);
            close(newQ->pipeQP[WRITE]);
        }
    } else {
        fprintf(stderr, "Found an error creting pipes to Q%d\n", index);
        returnCode = 1;
    }

    return returnCode;
}

void waitForMessages(pInstance *instanceOfMySelf){

    while (true){
        waitForMessagesFromQ();
        waitForMessagesFromController(instanceOfMySelf);
    }
}

void waitForMessagesFromQ(){
    // check for done file and forward info to C
}

void waitForMessagesFromController(pInstance *instanceOfMySelf){
    int numBytesRead, dataSectionSize, offset;
    byte packetHeader[1 + INT_SIZE];

    numBytesRead = read(instanceOfMySelf->pipeCP[READ], packetHeader, 1 + INT_SIZE);

    if (numBytesRead == (1 + INT_SIZE)){
        dataSectionSize = fromBytesToInt(packetHeader + 1);

        offset = 0;
        byte packetData[dataSectionSize];

        // if we get a header then block and wait the whole message
        while (offset != dataSectionSize){
            numBytesRead = read(instanceOfMySelf->pipeCP[READ], packetData + offset, dataSectionSize - offset);
            if (numBytesRead > 0){
                offset += numBytesRead;
            } else if (numBytesRead < 0){
                fprintf(stderr, "Error reading from pipe C->P\n");
            }
        }

        processMessageFromController(packetHeader[0], packetData, dataSectionSize, instanceOfMySelf);
    }
}

int processMessageFromQ(byte packetCode, byte *packetData, int packetDataSize, pInstance *instanceOfMySelf){
    return -1;
}

int processMessageFromController(byte packetCode, byte *packetData, int packetDataSize, pInstance *instanceOfMySelf){
    int returnCode;
    switch (packetCode){
        case 0:
            returnCode = processPNewFilePacket(packetData, packetDataSize);
            break;
        case 1:
            returnCode = processPRemoveFilePacket(packetData);
            break;
        case 2:
            returnCode = processPDeathPacket();
            break;
        case 3:
            returnCode = processPNewValueForM(packetData, instanceOfMySelf);
            break;
        default:
            printf("Error, P received from C an unknown packet type %d\n", packetCode);
            returnCode = 1;
    }

    return returnCode;
}

// Only rebuilds the header and forward with no modifications
int processPNewFilePacket(byte packetData[], int packetDataSize){
    int i, returnCode = 0;
    for (i = 0; i < currM; i++){
        returnCode = forwardPacket(qInstances[i].pipePQ, 0, packetDataSize, packetData);
        if (returnCode < 0){
            fprintf(stderr, "Could not forward file packet to Q\n");
        }
    }

    return 0;
}

int processPRemoveFilePacket(byte packetData[]){
    // TODO (not now)
    // currently we are not able to remove files during analysis
}

int processPDeathPacket(){
    int i, returnCode;
    for (i = 0; i < currM; i++){
        returnCode = sendDeathPacket(qInstances[i].pipePQ);
        if (returnCode == 1){
            // error killing the process Q[i]
            // we should try to kill the process manually
        }
    }

    free(qInstances);
    printf("P is dead\n");

    exit(0); // to exit from infinite loop in waitForMessages()
}

// TODO: reassign files??
int processPNewValueForM(byte packetData[], pInstance *instanceOfMySelf){
    // // the packet data conists only in the new m value
    // uint new_m = fromBytesToInt(packetData);
    // printf("P got new value for M\n");
    
    // int **newPipesToQ   = (int**) malloc( ((int)new_m) * sizeof(int*) );
    // int **newPipesFromQ = (int**) malloc( ((int)new_m) * sizeof(int*) );
    // pid_t *newPids = (pid_t*) malloc(new_m * sizeof(pid_t));

    // int difference = ((int)new_m) - m;

    // if(difference < 0){
    //     // Gotta kill some children
    //     int i;
    //     for(i=0; i<m; i++){
    //         if(i<new_m) {
    //             newPipesToQ[i] = pipeListToQ[i];
    //             newPipesFromQ[i] = pipeListFromQ[i];
    //             newPids[i] = oldPids[i]; 
    //         }
    //         else {
    //             killTheQ(i, pipeListToQ);
    //             free(pipeListToQ[i]);
    //             free(pipeListFromQ[i]);
    //         }
    //     }
    //     free(oldPids);
    // } else {
    //     // Create some new little Qs
        
    //     // Copy old ones
    //     int i;
    //     for(i = 0; i < m; i++){
    //         newPipesToQ[i] = pipeListToQ[i];
    //         newPipesFromQ[i] = pipeListFromQ[i];
    //         newPids[i] = oldPids[i]; 
    //     }
        
    //     // Create new ones
    //     for(i=m; i<new_m; i++){
    //         // TODO: if we need also pipesFromQ we have to update them HERE
    //         newPipesToQ[i]   = (int*) malloc(sizeof(int) * 2);
    //         newPipesFromQ[i] = (int*) malloc(sizeof(int) * 2);

    //         // TODO: check syscall return
    //         pipe(newPipesToQ[i]);
    //         pipe(newPipesFromQ[i]);

    //         int f = fork();
    //         if (f < 0){
    //             printf("Error creating Q\n");
    //             i--;
    //         } else if (f == 0){
    //             // child
    //             printf("Q%d created\n", i);
    //             close(newPipesToQ[i][WRITE]);
    //             close(newPipesFromQ[i][READ]);
    //             // AAAARG aggiornare m per i vecchi Q o ucciderli e crearne di nuovi?
    //             q(new_m, i, newPipesFromQ[i], newPipesToQ[i]);
    //             exit(0);
    //         } else {
    //         // parent
    //         close(newPipesToQ[i][READ]);
    //         close(newPipesFromQ[i][WRITE]);
    //         newPids[i] = f;
    //         }
    //     }
    // }

    // waitForMessagesFromController(newPids, pipeFromC, newPipesToQ, newPipesFromQ, new_m);
}