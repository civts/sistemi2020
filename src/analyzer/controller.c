#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // pid_t -> in crawler I use pid_t without types.h
#include <signal.h>
#include "../common/packets.h"
#include "../common/utils.h"
#include "../common/datastructures/namesList.h"
#include "../common/datastructures/fileList.h"
#include "instances.h"
#include "p.c"

// #define REPORT 1

string REPORT_FIFO = "/tmp/fifo";

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
int  processCStartAnalysis(controllerInstance*);
int  processCNewValueForM(byte[], controllerInstance*);
int  processCNewValueForN(byte[], controllerInstance*);
int  processCNewFileOccurrences(byte[], int, controllerInstance *);
int  processCErrorOnFilePacket(byte[], int, controllerInstance *);
int  processMessageInControllerFromAnalyzer(byte, byte*, int, controllerInstance*);
int  processMessageInControllerFromP(byte, byte*, int, controllerInstance*);
int  openFifoToReport(controllerInstance*);

void wait_a_bit();

// Initialization of controller fields
void controller(controllerInstance *instanceOfMySelf){
    instanceOfMySelf->pInstances = NULL;
    instanceOfMySelf->currM = 0;
    instanceOfMySelf->currN = 0;
    instanceOfMySelf->tempN = 0;
    instanceOfMySelf->tempM = 0;
    instanceOfMySelf->nextFileID = 0;
    instanceOfMySelf->filesFinished = 0;
    instanceOfMySelf->isAnalysing = false;
    instanceOfMySelf->fileNameList = constructorNamesList();
    instanceOfMySelf->removedFileNames = constructorNamesList();
    instanceOfMySelf->fileList = constructorFileNameList();

    // wait until a report process has been opened
    #ifdef REPORT
    openFifoToReport(instanceOfMySelf); 
    #endif
    
    waitForMessagesInController(instanceOfMySelf);
}

// wait for messages from Analyzer and Ps
void waitForMessagesInController(controllerInstance *instanceOfMySelf){
    while (true){
        waitForMessagesInCFromA(instanceOfMySelf);
        // if (instanceOfMySelf->isAnalysing){
        waitForMessagesInCFromP(instanceOfMySelf);
        // }
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
        printf("Got packet %d in C from A\n", packetHeader[0]);
        processMessageInControllerFromAnalyzer(packetHeader[0], packetData, dataSectionSize, instanceOfMySelf);
    }
}

// we surely receive only atomic packets
void waitForMessagesInCFromP(controllerInstance *instanceOfMySelf){
    int numBytesRead, dataSectionSize;
    byte packetHeader[1 + INT_SIZE];

    int i;
    for (i = 0; i < instanceOfMySelf->currN; i++){
        numBytesRead = read(instanceOfMySelf->pInstances[i]->pipePC[READ], packetHeader, 1 + INT_SIZE);
        if (numBytesRead == (1 + INT_SIZE)){
            dataSectionSize = fromBytesToInt(packetHeader + 1);
            byte packetData[dataSectionSize];

            numBytesRead = read(instanceOfMySelf->pInstances[i]->pipePC[READ], packetData, dataSectionSize);
            printf("Got packet %d in C from P\n", packetHeader[0]);
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

        newP->workload = 0; // no file associated to this P yet

        pInstance pChild = *newP;
        newP->pid = fork();

        if (newP->pid < 0){
            fprintf(stderr, "Found an error creating P%d\n", index);
            returnCode = 2;
        } else if (newP->pid == 0){
            // child: new instance of P
            fprintf(stderr, "New P%d created\n", index);
            close(pChild.pipeCP[WRITE]);
            close(pChild.pipePC[READ]);
            p(&pChild, newM);

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
        } else {
            if (newM != instanceOfMySelf->currM){
                notifyNewMToPInstance(instanceOfMySelf->pInstances[i], newM);
            }
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
            instanceOfMySelf->pInstances[i] = (pInstance*) malloc(sizeof(pInstance));
            if (generateNewPInstance(instanceOfMySelf->pInstances[i], i, newM) != 0){
                fprintf(stderr, "Failed to generate new P process\n");
                returnCode = 2;
            } else {
                printf("Generated new P in tree\n");
            }
        }
    }
    
    instanceOfMySelf->currN = newN;
    instanceOfMySelf->currM = newM;

    return returnCode;
}

void notifyNewMToPInstance(pInstance *instanceOfP, int newM){
    printf("Update m to %d to P\n", newM);
    sendNewMPacket(instanceOfP->pipeCP, newM);
}

void killInstanceOfP(int pIndex, controllerInstance *instanceOfMySelf){
    // TODO - free resources
    printf("C is killing P%d\n", pIndex);
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
        case 11:
            returnCode = processCErrorOnFilePacket(packetData, packetDataSize, instanceOfMySelf);
            break;
        default:
            fprintf(stderr, "Error, C received from P an unknown packet type %d\n", packetCode);
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

    printf("C - received file %s\n", buffer);
    if (!instanceOfMySelf->isAnalysing){
        // add file to the list
        appendNameToNamesList(instanceOfMySelf->fileNameList, buffer);
        // remove the file from the list of removed files (in case it has been removed first and readded then)
        removeNodeNameByName(instanceOfMySelf->removedFileNames, buffer);
    } else {
        // assign the new file to the P process with less worload
        int minPindex = 0;
        pInstance *pWithMinWorkload = instanceOfMySelf->pInstances[minPindex];

        int i;
        for (i = 0; i < instanceOfMySelf->currN; i++){
            if (instanceOfMySelf->pInstances[i]->workload < pWithMinWorkload->workload){
                pWithMinWorkload = instanceOfMySelf->pInstances[i];
                minPindex = i;
            }
        }
        
        if (sendNewFilePacketWithID(pWithMinWorkload->pipeCP, instanceOfMySelf->nextFileID, buffer) == 0){
            FileState *newFile = constructorFileState(buffer, instanceOfMySelf->nextFileID, instanceOfMySelf->currM, minPindex);
            NodeFileState *newNodeFile = constructorFileNode(newFile);
            appendFileState(instanceOfMySelf->fileList, newNodeFile);

            #ifdef REPORT
            newFileNameToReportPacket(instanceOfMySelf->pipeToReport, instanceOfMySelf->pidAnalyzer, newFile->idFile, newFile->fileName);
            #endif

            pWithMinWorkload->workload++;
            instanceOfMySelf->nextFileID++;
        }
    }

    return returnCode;
}


int processCRemoveFilePacket(byte packetData[], int packetDataSize, controllerInstance *instanceOfMySelf){
    int returnCode = 0;
    
    // get path to the file
    char buffer[packetDataSize + 1];
    memcpy(buffer, packetData, packetDataSize);
    buffer[packetDataSize] = '\0';

    // 0 if file 1 if foder
    int pathType = inspectPath(buffer);
    
    if (!instanceOfMySelf->isAnalysing){
        if(pathType == 0){
            // It's a file
            // add the file to the removed files
            appendNameToNamesList(instanceOfMySelf->removedFileNames, buffer);
            // remove the file from the list of added files
            removeNodeNameByName(instanceOfMySelf->fileNameList, buffer);
        } else {
            // It's a folder
            deleteFolderNamesList(buffer, instanceOfMySelf->fileNameList, instanceOfMySelf->removedFileNames);
        }
    } else {
        if(pathType == 0){
            // Dynamic removal of a file
            NodeFileState *node = getNodeByName(instanceOfMySelf->fileList, buffer);
            removeFileByIdPacket(instanceOfMySelf->pInstances[node->data->pIndex]->pipeCP,
                                instanceOfMySelf->pidAnalyzer,
                                node->data->idFile);
                                
            #ifdef REPORT
            removeFileByIdPacket(instanceOfMySelf->pipeToReport,
                                instanceOfMySelf->pidAnalyzer,
                                node->data->idFile);
            #endif

            instanceOfMySelf->pInstances[node->data->pIndex]->workload--;
            removeNode(instanceOfMySelf->fileList, node->data->fileName);
        } else {
            // Dynamic removal of folder
            NamesList *filesToRemove = constructorNamesList();
            deleteFolderNamesList(buffer, instanceOfMySelf->fileNameList, filesToRemove);
            NodeName *nameNode = filesToRemove->first;
            while(nameNode!=NULL){
                // Same as dynamic removal of file

                NodeFileState *node = getNodeByName(instanceOfMySelf->fileList, nameNode->name);
                removeFileByIdPacket(instanceOfMySelf->pInstances[node->data->pIndex]->pipeCP,
                                    instanceOfMySelf->pidAnalyzer,
                                    node->data->idFile);
                                    
                #ifdef REPORT
                removeFileByIdPacket(instanceOfMySelf->pipeToReport,
                                    instanceOfMySelf->pidAnalyzer,
                                    node->data->idFile);
                #endif

                instanceOfMySelf->pInstances[node->data->pIndex]->workload--;
                removeNode(instanceOfMySelf->fileList, node->data->fileName);

                nameNode = nameNode->next;
            } 
        }
    }

    return returnCode;
}

int processCDeathPacket(controllerInstance *instanceOfMySelf){
    int i;
    for (i = 0; i < instanceOfMySelf->currN; i++){
        if (sendDeathPacket(instanceOfMySelf->pInstances[i]->pipeCP) != 0){
            // another mechanism to kill the P if the pipes C->P are down
            kill(instanceOfMySelf->pInstances[i]->pid, SIGKILL);
        }
    }

    deleteList(instanceOfMySelf->fileList);
    deleteNamesList(instanceOfMySelf->fileNameList);
    deleteNamesList(instanceOfMySelf->removedFileNames);
    free(instanceOfMySelf->pInstances);

    printf("Controller is dead\n");
    exit(0);
}

// TODO: we can still receive some packets with old m value. What to do? -> we should save inside FileState the m value it has been created with
// Notify all Ps that M value has been changed. Even if they are analyzing
int processCNewValueForM(byte packetData[], controllerInstance *instanceOfMySelf){
    int returnCode = 0;
    uint new_m = fromBytesToInt(packetData);

    // if we are not analysing we are not supposed to update now m values for Ps
    // it will do it for us the shape tree before starting analyzing
    if (instanceOfMySelf->isAnalysing && instanceOfMySelf->currM != new_m){
        int i;
        for (i = 0; i < instanceOfMySelf->currN; i++){
            sendNewMPacket(instanceOfMySelf->pInstances[i]->pipeCP, new_m);
        }

        NodeFileState *nodo = instanceOfMySelf->fileList->first;
        for (i = 0; i < instanceOfMySelf->fileList->number_of_nodes; i++){
            if (nodo->data->numOfRemainingPortionsToRead != 0){
                nodo->data->numOfRemainingPortionsToRead = new_m;

                removeFileByIdPacket(instanceOfMySelf->pInstances[nodo->data->pIndex]->pipeCP,
                                     instanceOfMySelf->pidAnalyzer,
                                     nodo->data->idFile);

                // we need to resend the file to process since we have new Q value
                sendNewFilePacketWithID(instanceOfMySelf->pInstances[nodo->data->pIndex]->pipeCP,
                                        nodo->data->idFile,
                                        nodo->data->fileName);
            }
            nodo = nodo->next;
        }
        instanceOfMySelf->currM = new_m;
    } else {
        instanceOfMySelf->tempM = new_m;
    }

    // printf("C - update m to %d\n", new_m);
    return returnCode;
}

/**
 * Sets new value for N.
 * TODO: implement dynamic change! (with shapetree)
 */
int processCNewValueForN(byte packetData[], controllerInstance *instanceOfMySelf){
    int returnCode = 0;
    uint new_n = fromBytesToInt(packetData);
    
    if (instanceOfMySelf->isAnalysing){
        // reshape tree according to new n value
        shapeTree(new_n, instanceOfMySelf->currM, instanceOfMySelf);

        if (new_n < instanceOfMySelf->currN){
            // redistribute their load to the remaining Ps
            NodeFileState *node = instanceOfMySelf->fileList->first;
            int i, loadIndex = 0;
            for (i = 0; i < instanceOfMySelf->fileList->number_of_nodes; i++){
                if (node->data->pIndex >= new_n){
                    node->data->numOfRemainingPortionsToRead = instanceOfMySelf->currM;
                    node->data->pIndex = loadIndex % new_n;

                    sendNewFilePacketWithID(instanceOfMySelf->pInstances[node->data->pIndex]->pipeCP,
                                            node->data->idFile,
                                            node->data->fileName);
                }

                node = node->next;
            }
        } else if (new_n > instanceOfMySelf->currN){
            // TODO: redistribute the existing load or do it only with new files?
        }
        instanceOfMySelf->currN = new_n;
    } else {
        instanceOfMySelf->tempN = new_n;
    }
    
    // printf("C - update n to %d\n", new_n);
    
    return returnCode;
}

// Start the analysis in 3 steps:
// 1) remove the files inside instanceOfMySelf->removedFileNames from the file list
// 2) insert the files inside instanceOfMySelf->fileNameList inside the file list
// 3) assign the files in fileList to P
int processCStartAnalysis(controllerInstance *instanceOfMySelf){
    printf("C - Starting analysis\n");
    int returnCode = 0, i;
    instanceOfMySelf->isAnalysing = true;

    shapeTree(instanceOfMySelf->tempN, instanceOfMySelf->tempM, instanceOfMySelf);

    instanceOfMySelf->currN = instanceOfMySelf->tempN;
    instanceOfMySelf->currM = instanceOfMySelf->tempM;
    printf("Tree shaped -> n: %d, m: %d\n", instanceOfMySelf->currN, instanceOfMySelf->currM);

    // reset the worklaod for all Ps
    for (i = 0; i < instanceOfMySelf->currN; i++){
        instanceOfMySelf->pInstances[i]->workload = 0;
    }

    // Send start analysis packet to report
    #ifdef REPORT
    sendStartAnalysisPacket(instanceOfMySelf->pipeToReport, instanceOfMySelf->pidAnalyzer);
    #endif

    // 1) remove the files inside instanceOfMySelf->removedFileNames from the file list
    int idRemovedFile = -1;
    NodeName *deleteFileNamePointer = instanceOfMySelf->removedFileNames->first;
    for (i = 0; i < instanceOfMySelf->removedFileNames->counter; i++){
        printf("Delete from file list %s\n", deleteFileNamePointer->name);
        idRemovedFile = removeNode(instanceOfMySelf->fileList, deleteFileNamePointer->name);
        // delete file from report
        #ifdef REPORT
        removeFileByIdPacket(instanceOfMySelf->pipeToReport, instanceOfMySelf->pidAnalyzer, idRemovedFile);
        #endif

        deleteFileNamePointer = deleteFileNamePointer->next;
    }

    // remove all elements from removedFileNames list
    deleteNamesList(instanceOfMySelf->removedFileNames);
    constructorFileNameList(instanceOfMySelf->removedFileNames);

    // 2) insert the files inside instanceOfMySelf->fileNameList inside the file list
    NodeName *newFileNamePointer = instanceOfMySelf->fileNameList->first;
    for (i = 0; i < instanceOfMySelf->fileNameList->counter; i++){
        printf("Adding to file list %s\n", newFileNamePointer->name);
        FileState *newFileState = constructorFileState(newFileNamePointer->name, instanceOfMySelf->nextFileID, -1, -1);
        NodeFileState *newFileStateNode = constructorFileNode(newFileState);
        appendFileState(instanceOfMySelf->fileList, newFileStateNode);

        // sending to report new files
        #ifdef REPORT
        newFileNameToReportPacket(instanceOfMySelf->pipeToReport, instanceOfMySelf->pidAnalyzer, newFileState->idFile, newFileState->fileName);
        #endif
        
        instanceOfMySelf->nextFileID++;
        newFileNamePointer = newFileNamePointer->next;
    }


    // remove all elements from fileNameList list
    deleteNamesList(instanceOfMySelf->fileNameList);
    constructorFileNameList(instanceOfMySelf->fileNameList);

    // 3) assign the files in fileList to P
    NodeFileState *nodeFileState = instanceOfMySelf->fileList->first;
    NodeFileState *previousFileState = NULL;
    for (i = 0; i < instanceOfMySelf->fileList->number_of_nodes; i++){
        // assign the file to a P process and divide it into M portions
        nodeFileState->data->numOfRemainingPortionsToRead = instanceOfMySelf->currM;
        nodeFileState->data->pIndex = i % instanceOfMySelf->currN;

        int successfulSend = sendNewFilePacketWithID(instanceOfMySelf->pInstances[nodeFileState->data->pIndex]->pipeCP,
                                nodeFileState->data->idFile,
                                nodeFileState->data->fileName);
        
        instanceOfMySelf->pInstances[nodeFileState->data->pIndex]->workload++;

        previousFileState = nodeFileState;
        nodeFileState = nodeFileState->next;

        // since we were not able to send that file, then we should remove it from the fileList
        // to prevent errors we should already be on the next element
        if (successfulSend != 0){
            // since we can't have identical absolute paths in fileList, we are always
            // deleting the current node
            removeNode(instanceOfMySelf->fileList, previousFileState->data->fileName);
            instanceOfMySelf->pInstances[nodeFileState->data->pIndex]->workload--;
            i--;
        }
    }

    // printList(instanceOfMySelf->fileList);
    
    return returnCode;  
}

// Redirects the occurrences packet to the report adn check for end of analysis
int processCNewFileOccurrences(byte packetData[], int packetDataSize, controllerInstance *instanceOfMySelf){
    int returnCode = 0;
    int m = fromBytesToInt(packetData + 2 * INT_SIZE);

    if (instanceOfMySelf->currM == m){
        // insert pid analyzer in the occurrences packet to report
        fromIntToBytes(instanceOfMySelf->pidAnalyzer, packetData);
        int idFile = fromBytesToInt(packetData + INT_SIZE);
        printf("C - send occurences packet to fifo of file %d\n", idFile);

        // update status in file list
        if (decrementRemainingPortionsById(instanceOfMySelf->fileList, idFile) != -1){
            #ifdef REPORT
            printf("Occurences packet with charsread %d\n", fromBytesToInt(packetData + 5*INT_SIZE));
            forwardPacket(instanceOfMySelf->pipeToReport, 6, packetDataSize, packetData);
            #endif
        }
        
        instanceOfMySelf->filesFinished++;
        sendFinishedFilePacket(instanceOfMySelf->pipeCA, instanceOfMySelf->filesFinished, instanceOfMySelf->fileList->number_of_nodes);

        // check if we have analyzed everything
        if (isAnalisiFinita(instanceOfMySelf->fileList)){
            instanceOfMySelf->isAnalysing = false;

            // notify the used we have finished to analyze
            sendFinishedAnalysisPacket(instanceOfMySelf->pipeCA);
            printf("C - Finished analysis\n");
        }
    } else {
        printf("Got file with old m\n");
        returnCode = 2;
    }
    
    return returnCode;
}

// Notify A and R that we found a problem accessing to a certain file
int processCErrorOnFilePacket(byte packetData[], int packetDataSize, controllerInstance *instanceOfMySelf){
    char messageBuffer[MINIQ_MAX_BUFFER_SIZE];
    int returnCode = 0;
    int fileID = fromBytesToInt(packetData + INT_SIZE);
    sprintf(messageBuffer, "Error processing file %d\n", fileID);
    
    NodeFileState *node = instanceOfMySelf->fileList->first;
    int i;
    for (i = 0; i < instanceOfMySelf->fileList->number_of_nodes; i++){
        if (node->data->idFile == fileID){
            break;
        }
        node = node->next;
    }

    if (node->data->numOfRemainingPortionsToRead > 0){
        node->data->numOfRemainingPortionsToRead = -1;
        #ifdef REPORT
        if (sendTextMessageToReport(instanceOfMySelf->pipeCA, messageBuffer) < 0){
            returnCode += 1;
        }
        #endif

        if (reportErrorOnFilePacket(instanceOfMySelf->pipeToReport, instanceOfMySelf->pidAnalyzer, fileID)){
            returnCode += 2;
        }
    }
    
    printf("C - got error on file packet\n");
    
    return returnCode;
}

// Creates the named pipe to the report
int openFifoToReport(controllerInstance *instanceOfMySelf){
    int returnCode = 0;
    printf("Waiting for the report to open...\n");
    if (mkfifo(REPORT_FIFO, 0666) == 0){
        instanceOfMySelf->pipeToReport[READ] = -1;
        instanceOfMySelf->pipeToReport[WRITE] = open(REPORT_FIFO, O_WRONLY);
        if (instanceOfMySelf->pipeToReport[WRITE] != -1){
            printf("Found a report process!\n");
        } else {
            returnCode = 2;
        }
    } else {
        returnCode = 1;
    }
    
    return returnCode;
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