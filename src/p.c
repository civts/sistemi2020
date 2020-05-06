#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.c"
#include "q.c"

#define READ 0
#define WRITE 1

void p(int, int*);
void waitForMessagesFromController(pid_t*, int[2], int**, int**, int);
int  processPNewFilePacket(byte[], int, int**, int);
int  processPRemoveFilePacket(byte[], int**, int);
int  processPDeathPacket(byte[], int**, int);
int  processPNewValueForM(byte[], pid_t[], int**, int**, int, int[2]);
void killTheQ(int, int**);


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
            q(m, i, pipeListFromQ[i], pipeListToQ[i]);
            // Beccato!
            exit(0);
        } else {
            // parent
            close(pipeListToQ[i][READ]);
            close(pipeListFromQ[i][WRITE]);
            pids[i] = f;
        }
    }

    waitForMessagesFromController(pids, pipeFromController, pipeListToQ, pipeListFromQ, m);
}

void waitForMessagesFromController(pid_t *qPids, int pipeFromC[2], int **pipeListToQ, int **pipeListFromQ, int m){
    byte packetHeader[1 + INT_SIZE];
    int  packetLength = 0, receivedPacketFragmentsLength = 0;
    int  returnCodeFromPacketProcess = 0;
    byte *packetData;
    
    while (true){
        // TODO wait for the whole packet if it is fragmented
        receivedPacketFragmentsLength = read(pipeFromC[READ], packetHeader, 1 + INT_SIZE);
        packetLength = fromBytesToInt(packetHeader + 1);
        packetData   = (byte*) malloc(sizeof(byte) * packetLength);
        int numBytesRead = read(pipeFromC[READ], packetData, packetLength);
        

        if (packetLength == numBytesRead){
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
                    returnCodeFromPacketProcess = processPNewValueForM(packetData, qPids, pipeListToQ, pipeListFromQ, m, pipeFromC);
                    break;
                default:
                    printf("Error, P received from C an unknown packet type %d\n", packetHeader[0]);
            }

            // in case of death packet, exit from while whale cycle
            if (packetHeader[0] == 2 && returnCodeFromPacketProcess == 0){
                break;
            }
        } else if (numBytesRead==0){
        } else if (numBytesRead==-1){
            printf("-1 on P\n");
        } else {
            printf("Ho letto %d bytes\n", numBytesRead);
            printf("Error, write not atomic\n");
        }
    }
}

// Only rebuilds the header and forward with no modifications
int processPNewFilePacket(byte packetData[], int packetLength, int **pipeListToQ, int m){
    byte packetHeader[1 + INT_SIZE];
    packetHeader[0] = 0; // new file packet type
    fromIntToBytes(packetLength, packetHeader + 1);

    int i;
    for (i = 0; i < m; i++){
        write(pipeListToQ[i][WRITE], packetHeader, 1 + INT_SIZE);
        write(pipeListToQ[i][WRITE], packetData, packetLength);
    }
}

int processPRemoveFilePacket(byte packetData[], int **pipeListToQ, int m){
    // TODO (not now)
    // currently we are not able to remove files during analysis
}

int processPDeathPacket(byte packetData[], int **pipeListToQ, int m){
    int i, returnCode = 0;
    for (i = 0; i < m; i++){
        killTheQ(i, pipeListToQ);
        free(pipeListToQ[i]);
    }

    free(pipeListToQ);
    printf("P is dead\n");

    return returnCode;
}

// TODO: reassign files??
int processPNewValueForM(byte packetData[], pid_t oldPids[], int **pipeListToQ, int **pipeListFromQ, int m, int pipeFromC[2]){
    // the packet data conists only in the new m value
    uint new_m = fromBytesToInt(packetData);
    printf("P got new value for M\n");
    
    int **newPipesToQ   = (int**) malloc( ((int)new_m) * sizeof(int*) );
    int **newPipesFromQ = (int**) malloc( ((int)new_m) * sizeof(int*) );
    pid_t *newPids = (pid_t*) malloc(new_m * sizeof(pid_t));

    int difference = ((int)new_m) - m;

    if(difference < 0){
        // Gotta kill some children
        int i;
        for(i=0; i<m; i++){
            if(i<new_m) {
                newPipesToQ[i] = pipeListToQ[i];
                newPipesFromQ[i] = pipeListFromQ[i];
                newPids[i] = oldPids[i]; 
            }
            else {
                killTheQ(i, pipeListToQ);
                free(pipeListToQ[i]);
                free(pipeListFromQ[i]);
            }
        }
        free(oldPids);
    } else {
        // Create some new little Qs
        
        // Copy old ones
        int i;
        for(i = 0; i < m; i++){
            newPipesToQ[i] = pipeListToQ[i];
            newPipesFromQ[i] = pipeListFromQ[i];
            newPids[i] = oldPids[i]; 
        }
        
        // Create new ones
        for(i=m; i<new_m; i++){
            // TODO: if we need also pipesFromQ we have to update them HERE
            newPipesToQ[i]   = (int*) malloc(sizeof(int) * 2);
            newPipesFromQ[i] = (int*) malloc(sizeof(int) * 2);

            // TODO: check syscall return
            pipe(newPipesToQ[i]);
            pipe(newPipesFromQ[i]);

            int f = fork();
            if (f < 0){
                printf("Error creating Q\n");
                i--;
            } else if (f == 0){
                // child
                printf("Q%d created\n", i);
                close(newPipesToQ[i][WRITE]);
                close(newPipesFromQ[i][READ]);
                // AAAARG aggiornare m per i vecchi Q o ucciderli e crearne di nuovi?
                q(new_m, i, newPipesFromQ[i], newPipesToQ[i]);
                exit(0);
            } else {
            // parent
            close(newPipesToQ[i][READ]);
            close(newPipesFromQ[i][WRITE]);
            newPids[i] = f;
            }
        }
    }

    waitForMessagesFromController(newPids, pipeFromC, newPipesToQ, newPipesFromQ, new_m);
}

void killTheQ(int Qindex, int **pipeListToQ){
    byte *deathPacket = (byte*) malloc((1 + INT_SIZE) * sizeof(byte));
    deathPacket[0] = 2; // death packet header
    fromIntToBytes(0, deathPacket + 1);

    write(pipeListToQ[Qindex][WRITE], deathPacket, 1 + INT_SIZE);
    free(deathPacket);
}