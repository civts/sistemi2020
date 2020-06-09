#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "../common/datastructures/fileList.h"
#include "../common/datastructures/namesList.h"
#include "../common/packets.h"
#include "../common/utils.h"
#include "instances.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // pid_t -> in crawler I use pid_t without types.h

#define REPORT 1

// Initialization of controller fields
void controller(controllerInstance *instanceOfMySelf);

// wait for messages from Analyzer and Ps
void waitForMessagesInController(controllerInstance *instanceOfMySelf);

// wait until a whole message from A arrives
void waitForMessagesInCFromA(controllerInstance *instanceOfMySelf);

// we surely receive only atomic packets
void waitForMessagesInCFromP(controllerInstance *instanceOfMySelf);

// TODO check for syscall close fails: what to do?
int generateNewPInstance(pInstance *newP, int index, int newM);

// Reshape all the three given new m and n values
// Error codes:
// 1 - Not enough space to allocate new P table
// 2 - Failed to generate new P process
int shapeTree(int newN, int newM, controllerInstance *instanceOfMySelf);

void notifyNewMToPInstance(pInstance *instanceOfP, int newM);

void killInstanceOfP(int pIndex, controllerInstance *instanceOfMySelf);

int processMessageInControllerFromAnalyzer(
    byte packetCode, byte *packetData, int packetDataSize,
    controllerInstance *instanceOfMySelf);

int processMessageInControllerFromP(byte packetCode, byte *packetData,
                                    int packetDataSize,
                                    controllerInstance *instanceOfMySelf);

int processCNewFilePacket(byte packetData[], int packetDataSize,
                          controllerInstance *instanceOfMySelf);

int processCRemoveFilePacket(byte packetData[], int packetDataSize,
                             controllerInstance *instanceOfMySelf);

int processCDeathPacket(controllerInstance *instanceOfMySelf);

// TODO: we can still receive some packets with old m value. What to do? -> we
// should save inside FileState the m value it has been created with
// Notify all Ps that M value has been changed. Even if they are analyzing
int processCNewValueForM(byte packetData[],
                         controllerInstance *instanceOfMySelf);

/**
 * Sets new value for N.
 * TODO: implement dynamic change! (with shapetree)
 */
int processCNewValueForN(byte packetData[],
                         controllerInstance *instanceOfMySelf);

// Start the analysis in 3 steps:
// 1) remove the files inside instanceOfMySelf->removedFileNames from the file
// list
// 2) insert the files inside instanceOfMySelf->fileNameList inside the file
// list
// 3) assign the files in fileList to P
int processCStartAnalysis(controllerInstance *instanceOfMySelf);

// Redirects the occurrences packet to the report adn check for end of analysis
int processCNewFileOccurrences(byte packetData[], int packetDataSize,
                               controllerInstance *instanceOfMySelf);

// Notify A and R that we found a problem accessing to a certain file
int processCErrorOnFilePacket(byte packetData[], int packetDataSize,
                              controllerInstance *instanceOfMySelf);

// Creates the named pipe to the report
int openFifoToReport(controllerInstance *instanceOfMySelf);

// During an analysis get the number of files already completed
int getNumOfCompletedFiles(controllerInstance *instanceOfMySelf);

#endif