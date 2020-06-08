#ifndef __PACKETS_H__
#define __PACKETS_H__

#include "utils.h"
#include <sys/types.h> // for pid_t

#define CODE_NEW_FILE_WITHOUT_ID 0
#define CODE_REMOVE_FILE_BY_NAME 1
#define CODE_DEATH_PACKET 2
#define CODE_NEW_M_VALUE 3
#define CODE_NEW_N_VALUE 4
#define CODE_START_ANALYSIS 5
#define CODE_FILE_OCCURENCES 6
#define CODE_REMOVE_FILE_BY_ID 7
#define CODE_NEW_FILENAME_TO_REPORT_FULL 8
#define CODE_NEW_FILENAME_TO_REPORT_PT1 9
#define CODE_NEW_FILENAME_TO_REPORT_PT2 10
#define CODE_REPORT_ERROR_ON_FILE 11
#define CODE_DELETE_FOLDER_FROM_REPORT_FULL 12
#define CODE_DELETE_FOLDER_FROM_REPORT_PT1 13
#define CODE_DELETE_FOLDER_FROM_REPORT_PT2 14
#define CODE_NEW_FILE_WITH_ID 15
#define CODE_FINISHED_ANALYSIS_TO_ANALYZER 16
#define CODE_UPDATE_ANALYSIS_STATUS_TO_ANALYZER 17
#define CODE_MESSAGE_TEXT 18

uint fromBytesToInt(byte[]);
void fromIntToBytes(uint, byte[]);

// Forward a packet without looking it's content in data section.
int forwardPacket(int[], byte, int, byte *);

/**
 * This function sends the newFilePacket to the file descriptor
 * in the arguments.
 * Error codes:
 * 1 - Error with the fd sending name packet
 */
int sendNewFilePacket(int[], const string);

/**
 * This function sends the removeFileByNamePacket to the 
 * file descriptor as argument.
 * 1 - Error with the fd sending name packet
 */
int removeFileByNamePacket(int[], const string);

// Send death packet to a certain file descriptor
// this causes the exit from the infinite loop of
// of the process
// Error codes:
// 1 - Error with fd sending the death packet
int sendDeathPacket(int[]);

// This function sends the packet with the new value of M.
int sendNewMPacket(int[], int);

// This function sends the packet with the new value of N.
int sendNewNPacket(int[], int);

/**
 * This function sends the start analysis packet.
 * Error codes:
 * 1 - Error with fd sending the death packet
 */
int sendStartAnalysisPacket(int[], pid_t);

// Remove a file (from report or miniQlist) given it's ID
int removeFileByIdPacket(int[], pid_t, int);

int sendNewFilePacketWithID(int[], int, string);

// -------------------------------- PACKETS TO REPORT --------------------------------------

int sendOccurencesPacketToReport(int[], int, int, int, int, ull, ull, ull[NUM_OCCURENCES]);

// Used to send a new file name to report in three cases:
// - unique packet if data can fit
// - packet pt1 and packet pt2 if file path can't fit in a single packet
int _internal_newFileNameToReportPacket(int, int[], pid_t, int, string, int);

int newFileNameToReportPacket(int[], pid_t, int, string);

int reportErrorOnFilePacket(int[], pid_t, int);

// Used to delete a folder from report in three cases:
// - unique packet if data can fit
// - packet pt1 and packet pt2 if file path can't fit in a single packet
int _internal_deleteFolderFromReportPacket(int, int[], pid_t, string, int);

int deleteFolderFromReportPacket(int[], pid_t, string);

// Send finished analysis packet
// Error codes:
// 1 - Error with fd sending the packet
int sendFinishedAnalysisPacket(int[]);

// Send finished file packet
// Error codes:
// 1 - Error with fd sending the packet
int sendFinishedFilePacket(int[], int, int);

int sendTextMessageToReport(int[], const string);

#endif