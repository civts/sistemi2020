#ifndef __Q_H__
#define __Q_H__
// #include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
#include "../common/packets.h"
#include "../common/utils.h"
#include "../common/datastructures/miniQlist.h"
#include "instances.h"
#include "miniQ.h"

void q(qInstance *instanceOfMySelf);

// infinite loop in which we read messages from its
// parent P and its children miniQ
void waitForMessagesInQ(qInstance *instanceOfMySelf);

// here the messages can not be sent or received atomically
// since the new file pacekt contains the full file path as string
void waitForMessagesInQFromP(qInstance *instanceOfMySelf);

// here the messages arrives always atomically, so we don't
// need to check if the message is complete
void waitForMessagesInQFromMiniQ(qInstance *instanceOfMySelf);

int processMessageInQFromP(byte packetCode, byte *packetData, int packetDataSize, qInstance *instanceOfMySelf);

int processMessageInQFromMiniQ(byte packetCode, byte *packetData, int packetDataSize, qInstance *instanceOfMySelf);

int processQNewFilePacketWithID(byte packetData[], int packetDataSize, qInstance* instanceOfMySelf);

int processQRemoveFilePacket(byte packetData[], int packetDataSize);

int processQDeathPacket();

int processQNewValueForM(byte packetData[], qInstance* instanceOfMySelf);

int processQFileResults(byte packetData[], int packetDataSize, qInstance *instanceOfMySelf);

int processQErrorOnFilePacket(byte packetData[], int packetDataSize, qInstance *instanceOfMySelf);

void sig_handler_Q();

#endif