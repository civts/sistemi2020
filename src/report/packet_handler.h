#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H
#include "../common/packets.h"
#include "./data_structures/analyzer_list.h"

#include <fcntl.h>
#include <unistd.h>

// analyzer (eventually creating the fws if needed)
void gotAddFilePacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// callback for error packets
void gotErrorFilePacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// Callback for A_NEW_FILE_INCOMPLETE_PART1 packets.
//(1st half of a file path)
void got1stPathPartPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// Callback for A_NEW_FILE_INCOMPLETE_PART2 packets.
//(2nd half of a file path)
void got2ndPathPartPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// Callback for Q_NEW_DATA_CODE packets.
void gotNewDataPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// Callback for A_DELETE_FILE_CODE packets.
void gotDeleteFilePacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// Callback for A_DELETE_FOLDER_CODE
void gotDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// Callback for A_DELETE:FOLDER_INCOMPLETE_PART1 packets.
//(1st half of a file path)
void got1stPathPartDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// Callback for A_NEW_FILE_INCOMPLETE_PART2 packets.
//(2nd half of a file path)
void got2ndPathPartDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// errors
void gotErrorLogPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
// restart the analyzer, resetting all data in it
void gotStartPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati);
//reads up to BATCH_SIZE
int reportReadBatch(int pipe, analyzerList *analyzers,int batch);
// This is the function that implements report buisiness logic. READS 1 PACKET AT THE TIME
int reportReadOnePacket(int pipe, analyzerList *analyzers);

#endif