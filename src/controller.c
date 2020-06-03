#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // pid_t -> in crawler I use pid_t without types.h
#include <signal.h>
#include "packets.h"
#include "utils.c"
#include "p.c"
// #include "datastructures/namesList.c"
// #include "datastructures/fileList.c"

#define READ 0
#define WRITE 1

string REPORT_FIFO = "/tmp/record_controller_fifo";

// Functions
void controller(controllerInstance*);
void waitForMessagesInController(controllerInstance*);
void waitForMessagesInCFromA(controllerInstance*);
void waitForMessagesInCFromP(controllerInstance*);
int  shapeTree(int, int, controllerInstance*);
void notifyNewMToPInstance(pInstance*, int);
void killInstanceOfP(int, controllerInstance*);
int  processCNewFilePacket(byte[], int, controllerInstance*);
int  processCRemoveFilePacket(byte[], int, controllerInstance*);
int  processCDeathPacket(controllerInstance*);
int  processCNewValueForM(byte[], controllerInstance*);
int  processCNewValueForN(byte[], controllerInstance*);
int  processCNewFileOccurrences(byte[], int, controllerInstance *);
int  processMessageInControllerFromAnalyzer(byte, byte*, int, controllerInstance*);
int  processMessageInControllerFromP(byte, byte*, int, controllerInstance*);
int  openFifoToRecord(controllerInstance*);

void wait_a_bit();

// Initialization of controller fields
void controller(controllerInstance *instanceOfMySelf){
    instanceOfMySelf->pInstances = NULL;
    instanceOfMySelf->currM = 0;
    instanceOfMySelf->currN = 0;
    instanceOfMySelf->nextFileID = 0;
    instanceOfMySelf->filesFinished = 0;
    instanceOfMySelf->isAnalysing = false;
    instanceOfMySelf->fileNameList = constructorNamesList();
    instanceOfMySelf->removedFileNames = constructorNamesList();
    instanceOfMySelf->fileList = constructorFileNameList();
/*  for finishedFile and finishedAnalisys debug
    sendFinishedFilePacket(instanceOfMySelf->pipeCA, 0, 3);
    wait_a_bit();
    sendFinishedFilePacket(instanceOfMySelf->pipeCA, 1, 3);
    wait_a_bit();
    sendFinishedFilePacket(instanceOfMySelf->pipeCA, 2, 3);
    wait_a_bit();
    sendFinishedFilePacket(instanceOfMySelf->pipeCA, 3, 3);
    wait_a_bit();
    sendFinishedAnalysisPacket(instanceOfMySelf->pipeCA);
*/
    openFifoToRecord(instanceOfMySelf); 
    waitForMessagesInController(instanceOfMySelf);
}

// wait for messages from Analyzer and Ps
void waitForMessagesInController(controllerInstance *instanceOfMySelf){
    while (true){
        waitForMessagesInCFromA(instanceOfMySelf);
        waitForMessagesInCFromP(instanceOfMySelf);
    }
}

// wait until a whole message from A arrives
void waitForMessagesInCFromA(controllerInstance *instanceOfMySelf){
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

// we surely receive only atomic packets
void waitForMessagesInCFromP(controllerInstance *instanceOfMySelf){
    int numBytesRead, dataSectionSize, offset;
    byte packetHeader[1 + INT_SIZE];

    int i;
    for (i = 0; i < instanceOfMySelf->currN; i++){
        numBytesRead = read(instanceOfMySelf->pInstances[i]->pipePC[READ], packetHeader, 1 + INT_SIZE);
        if (numBytesRead == (1 + INT_SIZE)){
            dataSectionSize = fromBytesToInt(packetHeader + 1);
            byte packetData[dataSectionSize];

            numBytesRead  = read(instanceOfMySelf->pInstances[i]->pipePC[READ], packetData, dataSectionSize);
            processMessageInControllerFromP(packetHeader[0], packetData, dataSectionSize, instanceOfMySelf);
        }
    }
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
int shapeTree(int newN, int newM, controllerInstance *instanceOfMySelf){
    int i, returnCode = 0;

    // free exceeding instances of P and update M value for old ones
    for (i = 0; i < instanceOfMySelf->currN; i++){
        if (i >= newN){
            killInstanceOfP(i, instanceOfMySelf);
        } else if (newM != currM){
            notifyNewMToPInstance(instanceOfMySelf->pInstances[i], newM);
        }
    }

    // allocate the space necessary for new_n
    instanceOfMySelf->pInstances = (pInstance **) realloc(instanceOfMySelf->pInstances, newN * sizeof(pInstance*));
    
    if (instanceOfMySelf->pInstances == NULL){
        fprintf(stderr, "Not enough space to allocate new P table\n");
        returnCode = 1;
    } else {
        // generate new instances of P if necessary
        for (i = instanceOfMySelf->currN; i < newN && returnCode == 0; i++){
            if (generateNewPInstance(instanceOfMySelf->pInstances[i], i, newM) != 0){
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

void killInstanceOfP(int pIndex, controllerInstance *instanceOfMySelf){
    // TODO - free resources
    printf("Stacca stacca: %d\n", pIndex);
    sendDeathPacket(instanceOfMySelf->pInstances[pIndex]->pipeCP);
}

int processMessageInControllerFromAnalyzer(byte packetCode, byte *packetData, int packetDataSize, controllerInstance *instanceOfMySelf){
    int returnCode;
    switch (packetCode){
        case 0:
            returnCode = processCNewFilePacket(packetData, packetDataSize, instanceOfMySelf);
            break;
        case 1:
            returnCode = processCRemoveFilePacket(packetData, packetDataSize, instanceOfMySelf);
            break;
        case 2:
            returnCode = processCDeathPacket(instanceOfMySelf);
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

int processMessageInControllerFromP(byte packetCode, byte *packetData, int packetDataSize, controllerInstance *instanceOfMySelf){
    int returnCode;
    switch (packetCode){
        case 6:
            returnCode = processCNewFileOccurrences(packetData, packetDataSize, instanceOfMySelf);
            break;
        default:
            fprintf(stderr, "Error, P received from C an unknown packet type %d\n", packetCode);
            returnCode = 1;
    }

    return returnCode;
}

int processCNewFilePacket(byte packetData[], int packetDataSize, controllerInstance *instanceOfMySelf){
    int returnCode = 0;
    
    // get path to the file
    char buffer[packetDataSize + 1];
    memcpy(buffer, packetData, packetDataSize);
    buffer[packetDataSize] = '\0';

    if (!instanceOfMySelf->isAnalysing){
        // add file to the list
        appendNameToNamesList(instanceOfMySelf->fileNameList, buffer);
        // remove the file from the list of removed files (in case it has been removed first and readded then)
        removeNodeNameByName(instanceOfMySelf->removedFileNames, buffer);
    } else {
        // TODO: dobbiamo darlo ai P che hanno gestito meno file
        //       oppure seguiamo l'aritmetica dell'orologio??

        // sendNewFilePacketWithID()
    }

    return returnCode;
}


int processCRemoveFilePacket(byte packetData[], int packetDataSize, controllerInstance *instanceOfMySelf){
    int returnCode = 0;
    
    // get path to the file
    char buffer[packetDataSize + 1];
    memcpy(buffer, packetData, packetDataSize);
    buffer[packetDataSize] = '\0';
    
    if (!instanceOfMySelf->isAnalysing){
        // add the file to the removed files
        appendNameToNamesList(instanceOfMySelf->removedFileNames, buffer);
        // remove the file from the list of added files
        removeNodeNameByName(instanceOfMySelf->fileNameList, buffer);
    } else {
        // TODO: dobbiamo cercare il P che l'ha processato e dirgli
        // che quel file non ci serve più
    }

    return returnCode;
}

int processCDeathPacket(controllerInstance *instanceOfMySelf){
    int i;
    for (i = 0; i < instanceOfMySelf->currN; i++){
        if (sendDeathPacket(instanceOfMySelf->pInstances[i]->pipeCP) != 0){
            // another mechanism to kill the P if the pipes C->P are down
            kill(instanceOfMySelf->pInstances[i]->pid, SIGINT);
        }
    }

    // TODO: delete list of files
    deleteNamesList(instanceOfMySelf->fileNameList);
    deleteNamesList(instanceOfMySelf->removedFileNames);
    free(instanceOfMySelf->pInstances);
    free(instanceOfMySelf);

    printf("Controller is dead\n");
    exit(0);
}

// Notify all Ps that M value has been changed. Even if they are analyzing
int processCNewValueForM(byte packetData[], controllerInstance *instanceOfMySelf){
    int returnCode = 0;

    uint new_m = fromBytesToInt(packetData);
    instanceOfMySelf->currM = new_m;

    int i;
    for (i = 0; i < instanceOfMySelf->currN; i++){
        sendNewMPacket(instanceOfMySelf->pInstances[i]->pipeCP, new_m);
    }
    
    return returnCode;
}

/**
 * Sets new value for N.
 * TODO: implement dynamic change! (with shapetree)
 */
int processCNewValueForN(byte packetData[], controllerInstance *instanceOfMySelf){
    int returnCode = 0;
    uint new_n = fromBytesToInt(packetData);
    
    // TODO: manage dynamic change of N during the analysis
    if (instanceOfMySelf->isAnalysing){
        fprintf(stderr, "We currently can't change N during analysis\n");
        // decide if we should use shape tree or not
        if (new_n < instanceOfMySelf->currN){
            // delete all exceeding Ps.
            // we need to redistribute their load to the remaining Ps
        } else if (new_n > instanceOfMySelf->currN){
            // create new Ps
            // redistribute the existing load or do it only with new files?
        }
    }

    instanceOfMySelf->currN = new_n;
    
    return returnCode;
}

/**
 * Start the analysis.
 * TODO: check all the mechanics
 */
int processCStartAnalysis(controllerInstance *instanceOfMySelf){
    int returnCode = 0, i;
    instanceOfMySelf->isAnalysing = true;

    shapeTree(instanceOfMySelf->currN, instanceOfMySelf->currM, instanceOfMySelf);

    // 1) remove the files inside instanceOfMySelf->removedFileNames from the file list
    NodeName *deleteFileNamePointer = instanceOfMySelf->removedFileNames->first;
    for (i = 0; i < instanceOfMySelf->removedFileNames->counter; i++){
        removeNode(instanceOfMySelf->fileList, deleteFileNamePointer->name);
        deleteFileNamePointer = deleteFileNamePointer->next;
    }

    // 2) insert the files inside instanceOfMySelf->fileNameList inside the file list
    NodeName *newFileNamePointer = instanceOfMySelf->fileNameList->first;
    for (i = 0; i < instanceOfMySelf->fileNameList->counter; i++){
        FileState *newFileState = constructorFileState(newFileNamePointer->name, instanceOfMySelf->nextFileID, -1, -1);
        NodeFileState *newFileStateNode = constructorFileNode(newFileState);
        appendFileState(instanceOfMySelf->fileList, newFileStateNode);

        instanceOfMySelf->nextFileID++;
        newFileNamePointer = newFileNamePointer->next;
    }

    // 3) assign the files in fileList to P
    NodeFileState *nodeFileState = instanceOfMySelf->fileList->first;
    for (i = 0; i < instanceOfMySelf->fileList->number_of_nodes; i++){
        // assign the file to a P process and divide it into M portions
        nodeFileState->data->numOfRemainingPortionsToRead = instanceOfMySelf->currM;
        nodeFileState->data->pIndex = i % instanceOfMySelf->currN;

        int successfulSend = sendNewFilePacketWithID(instanceOfMySelf->pInstances[nodeFileState->data->pIndex]->pipeCP,
                                nodeFileState->data->idFile,
                                nodeFileState->data->fileName);
        if (successfulSend != 0){
            // TODO: since we were not able to send that file, then we should remove it from the fileList
            // delete file from fileList; decrease i; pay attention to nodeFileState
        }

        nodeFileState = nodeFileState->next;
    }
    
    return returnCode;  
}

// Redirects the occurrences packet to the report
int processCNewFileOccurrences(byte packetData[], int packetDataSize, controllerInstance *instanceOfMySelf){
    // insert pid analyzer in the occurrences packet to report
    fromIntToBytes(instanceOfMySelf->pidAnalyzer, packetData + 1 + INT_SIZE);
    int idFile = fromBytesToInt(packetData + 1 + 2 * INT_SIZE);

    // update status in file list
    if (decrementRemainingPortionsById(instanceOfMySelf->fileList, idFile) != -1){
        int dummyPipe[2] = {-1, instanceOfMySelf->pipeToRecord};
        forwardPacket(dummyPipe, 6, packetDataSize, packetData);
    }
    
    instanceOfMySelf->filesFinished++;
    sendFinishedFilePacket(instanceOfMySelf->pipeCA, instanceOfMySelf->filesFinished, instanceOfMySelf->fileNameList->counter);

    // check if we have analyzed everything
    if (isAnalisiFinita(instanceOfMySelf->fileList)){
        instanceOfMySelf->isAnalysing = false;

        // TODO: notify the used we have finished to analyze
        sendFinishedAnalysisPacket(instanceOfMySelf->pipeCA);
    }
}

    

// Creates the named pipe to the report
int openFifoToRecord(controllerInstance *instanceOfMySelf){
    mkfifo(REPORT_FIFO, 0666);
    instanceOfMySelf->pipeToRecord = open(REPORT_FIFO, O_WRONLY);
}

// only for debug... wait a certain amount of time
void wait_a_bit(){
    long long int i;
    for (i=0; i<99999999; i++){}
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