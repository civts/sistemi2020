#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // pid_t -> in crawler I use pid_t without types.h
#include "packets.h"
#include "utils.c"
#include "p.c"
#include "datastructures/namesList.c"

#define READ 0
#define WRITE 1

/**
 * Arrivato fino ad implementare processCStartAnalysis().
 * Controllo sulle altre funzioni di ricezione pacchetto.
 * C'è ancora da fare un po' di renaming e sistemazione degli argomenti
 *  es: oldController(...) non riceve più una lista di stringhe come parametro (con i nomi dei file)
 */


/** TODO: remove from here datastructure and put in utils.c or packets.h */
typedef struct{
    pid_t pid;
    int pipeAC[2];
    int pipeCA[2];
    pInstance *pInstances; // P processes associated to C
    NamesList *fileNameList;
    int currN;
    int currM;
} ControllerInstance;


/** FUNCTIONS **/
void controller(ControllerInstance*);
void waitForMessagesInController(ControllerInstance*);
void oldController(int, int, string[], int, ControllerInstance*);
int  shapeTree(int, int, ControllerInstance*);
void notifyNewMToPInstance(pInstance*, int);
void killInstanceOfP(int, ControllerInstance*);
void sendPathNameToP(string, int, bool, ControllerInstance*);
int  processCNewFilePacket(byte[], int, ControllerInstance*);
int  processCRemoveFilePacket(byte[], int, ControllerInstance*);
int  processCDeathPacket();
int  processCNewValueForM(byte[], ControllerInstance*);
int  processCNewValueForN(byte[], ControllerInstance*);



/**
 * "Empty" constructor for controller. 
 */
void controller(ControllerInstance *instanceOfMySelf){
    instanceOfMySelf->pInstances = NULL;
    instanceOfMySelf->currM = 0;
    instanceOfMySelf->currN = 0;
    instanceOfMySelf->fileNameList = constructorNamesList();
    waitForMessagesInController(instanceOfMySelf);
}

/**
 * Function that waits and reads messages from the Analyzer.
 */
void waitForMessagesInController(ControllerInstance *instanceOfMySelf){
    while(true){
        int numBytesRead, dataSectionSize, offset;
        byte packetHeader[1 + INT_SIZE];

        numBytesRead = read(instanceOfMySelf->pipeAC[READ], packetHeader, 1 + INT_SIZE);

        if (numBytesRead == (1 + INT_SIZE)){
            dataSectionSize = fromBytesToInt(packetHeader + 1);

            offset = 0;
            byte packetData[dataSectionSize];

            // if we get a header then block and wait the whole message
            while (offset != dataSectionSize){
                numBytesRead = read(instanceOfMySelf->pipeAC[READ], packetData + offset, dataSectionSize - offset);
                if (numBytesRead > 0){
                    offset += numBytesRead;
                } else if (numBytesRead < 0){
                    fprintf(stderr, "Error reading from pipe A->C\n");
                }
            }

            processMessageInControllerFromAnalyzer(packetHeader[0], packetData, dataSectionSize, instanceOfMySelf);
        }
    }
}


// main worker of the C process
void oldController(int n, int m, string files[], int numFiles, ControllerInstance *instanceOfMySelf){
    shapeTree(n, m, instanceOfMySelf);

    int i;
    for (i = 0; i < numFiles; i++){
        sendPathNameToP(files[i], i % instanceOfMySelf->currN, false, instanceOfMySelf);
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
        // TODO check for error -1 for fcntl
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
int shapeTree(int newN, int newM, ControllerInstance *instanceOfMySelf){
    int i, returnCode = 0;

    // free exceeding instances of P and update M value for old ones
    for (i = 0; i < instanceOfMySelf->currN; i++){
        if (i >= newN){
            killInstanceOfP(i, instanceOfMySelf);
        } else if (newM != currM){
            notifyNewMToPInstance(instanceOfMySelf->pInstances + i, newM);
        }
    }

    // allocate the space necessary for new_n
    instanceOfMySelf->pInstances = (pInstance*) realloc(instanceOfMySelf->pInstances, newN * sizeof(pInstance));
    
    if (instanceOfMySelf->pInstances == NULL){
        fprintf(stderr, "Not enough space to allocate new P table\n");
        returnCode = 1;
    } else {
        // generate new instances of P if necessary
        for (i = instanceOfMySelf->currN; i < newN && returnCode == 0; i++){
            if (generateNewPInstance(instanceOfMySelf->pInstances + i, i, newM) != 0){
                fprintf(stderr, "Failed to generate new P process\n");
                returnCode = 2;
            }
        }
    }
    
    instanceOfMySelf->currN = newN;
    instanceOfMySelf->currM = newM;

    return returnCode;
}

void notifyNewMToPInstance(pInstance *instanceOfP, int newM){
    printf("Update m to %d\n", newM);
    sendNewMPacket(instanceOfP->pipeCP, newM);
}

void killInstanceOfP(int pIndex, ControllerInstance *instanceOfMySelf){
    // TODO - free resources
    printf("Stacca stacca: %d\n", pIndex);
    sendDeathPacket(instanceOfMySelf->pInstances[pIndex].pipeCP);
}

void sendPathNameToP(string pathName, int indexOfP, bool isInsideFolder, ControllerInstance *instanceOfMySelf){
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

    write(instanceOfMySelf->pInstances[indexOfP].pipeCP[WRITE], packet, packetLength);
    free(packet);
}


int processMessageInControllerFromAnalyzer(byte packetCode, byte *packetData, int packetDataSize, pInstance *instanceOfMySelf){
    int returnCode;
    switch (packetCode){
        case 0:
            returnCode = processCNewFilePacket(packetData, packetDataSize, instanceOfMySelf);
            break;
        case 1:
            returnCode = processCRemoveFilePacket(packetData, packetDataSize, instanceOfMySelf);
            break;
        case 2:
            returnCode = processCDeathPacket();
            break;
        case 3:
            returnCode = processCNewValueForM(packetData, instanceOfMySelf);
            break;
        case 4:
            returnCode = processCNewValueForN(packetData, instanceOfMySelf);
            break;
        case 5:
            returnCode = processCStartAnalysis(instanceOfMySelf);
            break;
        default:
            fprintf(stderr, "Error, P received from C an unknown packet type %d\n", packetCode);
            returnCode = 1;
    }

    return returnCode;
}

int processCNewFilePacket(byte packetData[], int packetDataSize, ControllerInstance *instanceOfMySelf){
    int returnCode = 0;
    
    // read file name
    char buffer[packetDataSize];
    memcpy(buffer, packetData, packetDataSize);
    buffer[packetDataSize - 1] = '\0';
    // add file to the list
    append(instanceOfMySelf->fileNameList, buffer);

    return returnCode;
}


int processCRemoveFilePacket(byte packetData[], int packetDataSize, ControllerInstance *instanceOfMySelf){
    int returnCode = 0;
    
    // read file name
    char buffer[packetDataSize];
    memcpy(buffer, packetData, packetDataSize);
    buffer[packetDataSize - 1] = '\0';
    // remove file from the list
    append(instanceOfMySelf->fileNameList, buffer);

    return returnCode;
}


int processCDeathPacket(){
    // TODO kill Ps
    // TODO free list of Ps
    printf("Controller is dead\n");
    exit(0);
}


/**
 * Sets new value for M.
 * TODO: implement dynamic change! (with shapetree)
 */
int processCNewValueForM(byte packetData[], ControllerInstance *instanceOfMySelf){
    int returnCode = 0;
    
    uint new_m = fromBytesToInt(packetData);
    instanceOfMySelf->currM = new_m;

    return returnCode;
}

/**
 * Sets new value for N.
 * TODO: implement dynamic change! (with shapetree)
 */
int processCNewValueForN(byte packetData[], ControllerInstance *instanceOfMySelf){
    int returnCode = 0;
    
    uint new_n = fromBytesToInt(packetData);
    instanceOfMySelf->currN = new_n;

    return returnCode;
}

/**
 * Start the analysis.
 * TODO: check all the mechanics
 */
int processCStartAnalysis(ControllerInstance *instanceOfMySelf){
    int returnCode = 0;

    // Start analysis
    oldController(instanceOfMySelf->currN, instanceOfMySelf->currM,
                  instanceOfMySelf->fileNameList, instanceOfMySelf->fileNameList->counter,
                  instanceOfMySelf);    

    return returnCode;  
}


// only for debug... wait a certain amount of time
void wait_a_bit(){
    long long int i;
    for (i=0; i<999999999; i++){}
}

// int main(){
//     string files[3] = {"prova1.txt", "prova2.txt", "p.c"};
//     controller(2, 2, files, 3);
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