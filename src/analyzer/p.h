#ifndef __P_H__
#define __P_H__

// #include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/types.h>
#include <unistd.h>
// #include <signal.h>
#include "q.h"
#include "instances.h"
#include "../common/packets.h"
#include "../common/utils.h"

int p(pInstance *instanceOfMySelf, int _currM);

int generateNewQInstance(qInstance *newQ, int index, int mValue);

// main worker: infinite loop to get messages
void waitForMessagesInP(pInstance *instanceOfMySelf);

// here the messages arrives always atomically, so we don't
// need to check if the message is complete
void waitForMessagesInPFromQ(pInstance *instanceOfMySelf);

// Here the messages can not be sent or received atomically
// since the new file packet contains the full file path as string
void waitForMessagesInPFromController(pInstance *instanceOfMySelf);

int processMessageInPFromQ(byte packetCode, byte *packetData, int packetDataSize, pInstance *instanceOfMySelf);

int processMessageInPFromController(byte packetCode, byte *packetData, int packetDataSize, pInstance *instanceOfMySelf);

// Only rebuilds the header and forward with no modifications
int processPNewFilePacket(byte packetData[], int packetDataSize);

// Forward the message to delete a file to all its Qs
int processPRemoveFilePacket(byte packetData[], int packetDataSize);

// Free resources and send a kill message to all its Qs
int processPDeathPacket();

// Forward back occurences packet if its M value is up to date
int processPFileResults(byte packetData[], int packetDataSize, pInstance *instanceOfMySelf);

int processPErrorOnFilePacket(byte packetData[], int packetDataSize, pInstance *instanceOfMySelf);

int processPNewValueForM(byte packetData[], pInstance *instanceOfMySelf);

void sig_handler_P();

#endif