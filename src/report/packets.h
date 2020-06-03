#ifndef __PACKETS_H__
#define __PACKETS_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../utils.c"
#define READ 0
#define WRITE 1

// forward a packet without looking inside it's content. Useful for new filepath
int forwardPacket(int fd[], byte packetCode, int dataSectionSize, byte *dataSection){
    byte completePacket[1 + INT_SIZE + dataSectionSize];

    completePacket[0] = packetCode;
    fromIntToBytes(dataSectionSize, completePacket + 1);
    memcpy(completePacket + 1 + INT_SIZE, dataSection, dataSectionSize);

    return write(fd[WRITE], completePacket, 1 + INT_SIZE + dataSectionSize);
}

// Here there is a list of packets we transfer inside
// the components of the analyzer by packetCode:
// 0:  new file only path
// 1:  remove file by name packet
// 2:  death packet
// 3:  notify new M value
// 4:  notify new N value
// 5:  start analisys packet
// 6:  file occurences results
// 7:  remove file by id packet
// 8:  newFileNameToReportPacket if file name fits in one packet
// 9:  newFileNameToReportPacket pt1 if file name doesn't fit in one packet
// 10: newFileNameToReportPacket pt2 if file name doesn't fit in one packet
// 11: reportErrorOnFilePacket
// 12: deleteFolderFromReportPacket if file name fits in one packet
// 13: deleteFolderFromReportPacket pt1 if file name doesn't fit in one packet
// 14: deleteFolderFromReportPacket pt2 if file name doesn't fit in one packet
// 15: sendNewFilePacketWithID used in C->P->Q
// 16: sendFinishedAnalysis packet sent from controller to analyzer at the end of an analisys

/**
 * This function sends the newFilePacket to the file descriptor
 * in the arguments.
 * Error codes:
 * 1 - Error with the fd sending name packet
 */
int sendNewFilePacket(int fd[], string fileName){
    int returnCode = 0;
    int fileNameLength = strlen(fileName);
    int packetSize = 1 + INT_SIZE + fileNameLength;
    byte newFilePacket[packetSize];

    newFilePacket[0] = 0;
    fromIntToBytes(fileNameLength, newFilePacket + 1);
    memcpy(newFilePacket + 1 + INT_SIZE, fileName, fileNameLength); // no \0 for fileName inside the packet

    if (write(fd[WRITE], newFilePacket, packetSize) != (packetSize)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the new file packet\n");
    }

    return returnCode;
}

/**
 * This function sends the removeFileByNamePacket to the 
 * file descriptor as argument.
 * 1 - Error with the fd sending name packet
 */
int removeFileByNamePacket(int fd[], string fileName){
    int returnCode = 0;
    int fileNameLength = strlen(fileName);
    int packetSize = 1 + INT_SIZE + fileNameLength;
    byte newFilePacket[packetSize];

    newFilePacket[0] = 1;
    fromIntToBytes(fileNameLength, newFilePacket + 1);
    memcpy(newFilePacket + 1 + INT_SIZE, fileName, fileNameLength);

    if (write(fd[WRITE], newFilePacket, packetSize) != (packetSize)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the remove file packet\n");
    }

    return returnCode;
}

// Send death packet to a certain file descriptor
// this causes the exit from the infinite loop of
// of the process
// Error codes:
// 1 - Error with fd sending the death packet
int sendDeathPacket(int fd[]){
    int returnCode = 0;
    byte deathPacket[1 + INT_SIZE];

    deathPacket[0] = 2;
    fromIntToBytes(0, deathPacket + 1);

    if (write(fd[WRITE], deathPacket, 1 + INT_SIZE) != (1 + INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the death packet\n");
    }

    return returnCode;
}

// This function sends the packet with the new value of M.
int sendNewMPacket(int fd[], int newM){
    int returnCode = 0;
    byte packet[1 + 2 * INT_SIZE];

    packet[0] = 3;
    fromIntToBytes(INT_SIZE, packet + 1);
    fromIntToBytes(newM, packet + 1 + INT_SIZE); // new value for m

    if (write(fd[WRITE], packet, 1 + 2 * INT_SIZE) != (1 + 2 * INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the new M packet\n");
    }

    return returnCode;
}

// This function sends the packet with the new value of N.
int sendNewNPacket(int fd[], int newN){
    int returnCode = 0;
    byte packet[1 + 2 * INT_SIZE];

    packet[0] = 4;
    fromIntToBytes(INT_SIZE, packet + 1);
    fromIntToBytes(newN, packet + 1 + INT_SIZE); // new value for n

    if (write(fd[WRITE], packet, 1 + 2 * INT_SIZE) != (1 + 2 * INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the new N packet\n");
    }

    return returnCode;
}

/**
 * This function sends the start analysis packet.
 * Error codes:
 * 1 - Error with fd sending the death packet
 */
int sendStartAnalysisPacket(int fd[]){
    int returnCode = 0;
    byte packet[1 + INT_SIZE];
    
    packet[0] = 5;
    fromIntToBytes(0, packet + 1);

    if (write(fd[WRITE], packet, 1 + INT_SIZE) != (1 + INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the death packet\n");
    }

    return returnCode;
}

// Remove a file (from report or miniQlist) given it's ID
int removeFileByIdPacket(int fd[], pid_t pidAnalyzer, int fileId){
    int returnCode = 0;
    byte packet[1 + 3*INT_SIZE];

    packet[0] = 7;
    fromIntToBytes(2 * INT_SIZE, packet + 1);
    fromIntToBytes(pidAnalyzer, packet + 1 + INT_SIZE);
    fromIntToBytes(fileId, packet + 1 + 2*INT_SIZE);

    if (write(fd[WRITE], packet, 1 + 3*INT_SIZE) != (1 + 3*INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the remove file packet\n");
    }

    return returnCode;
}

int sendNewFilePacketWithID(int fd[], int idFile, string fileName){
    int returnCode = 0, offset = 0;
    int fileNameLength = strlen(fileName);
    int packetSize = 1 + 2*INT_SIZE + fileNameLength;
    byte newFilePacket[packetSize];

    // header
    newFilePacket[offset++] = 15;
    fromIntToBytes(packetSize - 1 - INT_SIZE, newFilePacket + offset);
    offset += INT_SIZE;

    // data section
    fromIntToBytes(idFile, newFilePacket + offset);
    offset += INT_SIZE;
    memcpy(newFilePacket + offset, fileName, fileNameLength); // no \0 for fileName inside the packet

    if (write(fd[WRITE], newFilePacket, packetSize) != (packetSize)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the new file with ID packet\n");
    }

    return returnCode;
}

// -------------------------------- PACKETS TO REPORT --------------------------------------

int sendOccurencesPacketToReport(int fd[], int pidAnalyzer, int idFile, int m, int index,
                         ull totalFileSize, ull numCharsReadInThisSection, int occurences[NUM_OCCURENCES]){
    int returnCode = 0, offset = 0, i;
    byte packet[1 + 263 * INT_SIZE];

    // header: packet code and data section size
    packet[offset++] = 6;
    fromIntToBytes(262 * INT_SIZE, packet + offset);
    offset += INT_SIZE;

    // data section
    fromIntToBytes(pidAnalyzer, packet + offset);
    offset += INT_SIZE;
    fromIntToBytes(idFile, packet + offset);
    offset += INT_SIZE;
    fromIntToBytes(m, packet + offset);
    offset += INT_SIZE;
    fromIntToBytes(index, packet + offset);
    offset += INT_SIZE;
    fromIntToBytes(totalFileSize, packet + offset);
    offset += INT_SIZE;
    fromIntToBytes(numCharsReadInThisSection, packet + offset);
    offset += INT_SIZE;

    for (i = 0; i < NUM_OCCURENCES; i++){
        fromIntToBytes(occurences[i], packet + offset);
        offset += INT_SIZE;
    }

    if (write(fd[WRITE], packet, 1 + 263 * INT_SIZE) != (1 + 263 * INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the occurences packet\n");
    }

    return returnCode;
}

// Used to send a new file name to report in three cases:
// - unique packet if data can fit
// - packet pt1 and packet pt2 if file path can't fit in a single packet
int _internal_newFileNameToReportPacket(int packetType, int fd[], pid_t pidAnalyzer,
                int fileId, string filePath, int filePathLength){
    int returnCode = 0, offset = 0;
    int packetSize = 2 + 3*INT_SIZE + filePathLength;
    byte packet[packetSize];

    // header
    packet[offset++] = packetType;
    fromIntToBytes(packetSize - 1 - INT_SIZE, packet + 1);
    offset += INT_SIZE;

    // data section
    fromIntToBytes(pidAnalyzer, packet + offset);
    offset += INT_SIZE;
    fromIntToBytes(fileId, packet + offset);
    offset += INT_SIZE;
    memcpy(packet + offset, filePath, filePathLength);
    packet[packetSize - 1] = '\0';

    if (write(fd[WRITE], packet, packetSize) != packetSize){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the new name file packet\n");
    }

    return returnCode;
}

int newFileNameToReportPacket(int fd[], pid_t pidAnalyzer, int fileId, const string filePath){
    int returnCode = 0;
    int filePathLength = strlen(filePath);
    int freeSpaceInFirstPacket = 4096 - 2 - 3*INT_SIZE - filePathLength;

    if (freeSpaceInFirstPacket >= 0){
        returnCode = _internal_newFileNameToReportPacket(8, fd, pidAnalyzer, fileId, filePath, filePathLength);
    } else {
        int lenFirstPartOfPath = 4096 - 2 - 3*INT_SIZE;
        returnCode =  _internal_newFileNameToReportPacket(9,  fd, pidAnalyzer, fileId, filePath, lenFirstPartOfPath);

        // send the second packet only if we were able to send the first one
        if (returnCode == 0){
            returnCode = _internal_newFileNameToReportPacket(10, fd, pidAnalyzer, fileId, filePath + lenFirstPartOfPath, filePathLength - lenFirstPartOfPath);
        }
    }

    return returnCode;
}

int reportErrorOnFilePacket(int fd[], pid_t pidAnalyzer, int fileId){
    int returnCode = 0;
    byte packet[1 + 3*INT_SIZE];

    packet[0] = 11;
    fromIntToBytes(INT_SIZE, packet + 1);
    fromIntToBytes(pidAnalyzer, packet + 1 + INT_SIZE);
    fromIntToBytes(fileId, packet + 1 + 2*INT_SIZE);

    if (write(fd[WRITE], packet, 1 + 3*INT_SIZE) != (1 + 3*INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the report error file packet\n");
    }

    return returnCode;
}

// Used to delete a folder from report in three cases:
// - unique packet if data can fit
// - packet pt1 and packet pt2 if file path can't fit in a single packet
int _internal_deleteFolderFromReportPacket(int packetType, int fd[], pid_t pidAnalyzer, string folderPath, int folderPathLength){
    int returnCode = 0, offset = 0;
    int packetSize = 2 + 2*INT_SIZE + folderPathLength;
    byte packet[packetSize];

    // header
    packet[offset++] = packetType;
    fromIntToBytes(packetSize - 1 - INT_SIZE, packet + 1);
    offset += INT_SIZE;

    // data section
    fromIntToBytes(pidAnalyzer, packet + offset);
    offset += INT_SIZE;
    memcpy(packet + offset, folderPath, folderPathLength);
    packet[packetSize - 1] = '\0';

    if (write(fd[WRITE], packet, packetSize) != packetSize){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending delete folder packet\n");
    }

    return returnCode;
}

int deleteFolderFromReportPacket(int fd[], pid_t pidAnalyzer, string folderPath){
    int returnCode = 0;
    int folderPathLength = strlen(folderPath);
    int freeSpaceInFirstPacket = 4096 - 2 - 2*INT_SIZE - folderPathLength;

    if (freeSpaceInFirstPacket >= 0){
        returnCode = _internal_deleteFolderFromReportPacket(12, fd, pidAnalyzer, folderPath, folderPathLength);
    } else {
        int lenFirstPartOfPath = 4096 - 2 - 2*INT_SIZE;
        returnCode =  _internal_deleteFolderFromReportPacket(13,  fd, pidAnalyzer, folderPath, lenFirstPartOfPath);

        // send the second packet only if we were able to send the first one
        if (returnCode == 0){
            returnCode = _internal_deleteFolderFromReportPacket(14, fd, pidAnalyzer, folderPath + lenFirstPartOfPath, folderPathLength - lenFirstPartOfPath);
        }
    }

    return returnCode;
}

// Send finished analisys packet
// Error codes:
// 1 - Error with fd sending the packet
int sendFinishedAnalysisPacket(int fd[]){
    int returnCode = 0;
    byte finAnalisysPacket[1 + INT_SIZE];

    finAnalisysPacket[0] = 16;
    fromIntToBytes(0, finAnalisysPacket + 1);

    if (write(fd[WRITE], finAnalisysPacket, 1 + INT_SIZE) != (1 + INT_SIZE)){
        returnCode = 1;
        fprintf(stderr, "Error with fd sending the finished analisys packet\n");
    }

    return returnCode;
}

#endif