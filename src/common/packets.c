#include "packets.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// Convert 4 bytes in unsigned int (little endian)
uint fromBytesToInt(byte *bytes) {
  uint result = 0, base = 1;

  int i;
  for (i = INT_SIZE - 1; i >= 0; i--) {
    result += bytes[i] * base;
    base *= 256;
  }

  return result;
}

// Convert an unsigned int in 4 bytes (little endian)
void fromIntToBytes(uint value, byte out[]) {
  uint base = 0;
  int i;
  for (i = INT_SIZE - 1; i >= 0; i--) {
    out[i] = ((value >> base) & 0xFF);
    base += 8;
  }
}

// Forward a packet without looking inside it's content
int forwardPacket(int fd[], byte packetCode, int dataSectionSize,
                  byte *dataSection) {
  byte completePacket[1 + INT_SIZE + dataSectionSize];

  completePacket[0] = packetCode;
  fromIntToBytes(dataSectionSize, completePacket + 1);
  memcpy(completePacket + 1 + INT_SIZE, dataSection, dataSectionSize);

  return write(fd[WRITE], completePacket, 1 + INT_SIZE + dataSectionSize);
}

/**
 * This function sends the newFilePacket to the file descriptor
 * in the arguments.
 * Error codes:
 * 1 - Error with the fd sending name packet
 */
int sendNewFilePacket(int fd[], const string fileName) {
  int returnCode = 0;
  int fileNameLength = strlen(fileName);
  int packetSize = 1 + INT_SIZE + fileNameLength;
  byte newFilePacket[packetSize];

  newFilePacket[0] = 0;
  fromIntToBytes(fileNameLength, newFilePacket + 1);
  memcpy(newFilePacket + 1 + INT_SIZE, fileName,
         fileNameLength); // no \0 for fileName inside the packet

  if (write(fd[WRITE], newFilePacket, packetSize) != (packetSize)) {
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
int removeFileByNamePacket(int fd[], const string fileName) {
  int returnCode = 0;
  int fileNameLength = strlen(fileName);
  int packetSize = 1 + INT_SIZE + fileNameLength;
  byte newFilePacket[packetSize];

  newFilePacket[0] = 1;
  fromIntToBytes(fileNameLength, newFilePacket + 1);
  memcpy(newFilePacket + 1 + INT_SIZE, fileName, fileNameLength);

  if (write(fd[WRITE], newFilePacket, packetSize) != (packetSize)) {
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
int sendDeathPacket(int fd[]) {
  int returnCode = 0;
  byte deathPacket[1 + INT_SIZE];

  deathPacket[0] = 2;
  fromIntToBytes(0, deathPacket + 1);

  if (write(fd[WRITE], deathPacket, 1 + INT_SIZE) != (1 + INT_SIZE)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the death packet\n");
  }

  return returnCode;
}

// This function sends the packet with the new value of M.
int sendNewMPacket(int fd[], int newM) {
  int returnCode = 0;
  byte packet[1 + 2 * INT_SIZE];

  packet[0] = 3;
  fromIntToBytes(INT_SIZE, packet + 1);
  fromIntToBytes(newM, packet + 1 + INT_SIZE); // new value for m

  if (write(fd[WRITE], packet, 1 + 2 * INT_SIZE) != (1 + 2 * INT_SIZE)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the new M packet\n");
  }

  return returnCode;
}

// This function sends the packet with the new value of N.
int sendNewNPacket(int fd[], int newN) {
  int returnCode = 0;
  byte packet[1 + 2 * INT_SIZE];

  packet[0] = 4;
  fromIntToBytes(INT_SIZE, packet + 1);
  fromIntToBytes(newN, packet + 1 + INT_SIZE); // new value for n

  if (write(fd[WRITE], packet, 1 + 2 * INT_SIZE) != (1 + 2 * INT_SIZE)) {
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
int sendStartAnalysisPacket(int fd[], pid_t pidAnalyzer) {
  int returnCode = 0;
  byte packet[1 + 2 * INT_SIZE];

  packet[0] = 5;
  fromIntToBytes(INT_SIZE, packet + 1);
  fromIntToBytes(pidAnalyzer, packet + 1 + INT_SIZE);

  if (write(fd[WRITE], packet, 1 + 2 * INT_SIZE) != (1 + 2 * INT_SIZE)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the death packet\n");
  }

  return returnCode;
}

// Remove a file (from report or miniQlist) given it's ID
int removeFileByIdPacket(int fd[], pid_t pidAnalyzer, int fileId) {
  int returnCode = 0;
  byte packet[1 + 3 * INT_SIZE];

  packet[0] = 7;
  fromIntToBytes(2 * INT_SIZE, packet + 1);
  fromIntToBytes(pidAnalyzer, packet + 1 + INT_SIZE);
  fromIntToBytes(fileId, packet + 1 + 2 * INT_SIZE);

  if (write(fd[WRITE], packet, 1 + 3 * INT_SIZE) != (1 + 3 * INT_SIZE)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the remove file packet\n");
  }

  return returnCode;
}

int sendNewFilePacketWithID(int fd[], int idFile, string fileName) {
  int returnCode = 0, offset = 0;
  int fileNameLength = strlen(fileName);
  int packetSize = 1 + 2 * INT_SIZE + fileNameLength;
  byte newFilePacket[packetSize];

  // header
  newFilePacket[offset++] = 15;
  fromIntToBytes(packetSize - 1 - INT_SIZE, newFilePacket + offset);
  offset += INT_SIZE;

  // data section
  fromIntToBytes(idFile, newFilePacket + offset);
  offset += INT_SIZE;
  memcpy(newFilePacket + offset, fileName,
         fileNameLength); // no \0 for fileName inside the packet

  if (write(fd[WRITE], newFilePacket, packetSize) != (packetSize)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the new file with ID packet\n");
  }

  return returnCode;
}

// -------------------------------- PACKETS TO REPORT
// --------------------------------------

int sendOccurencesPacketToReport(int fd[], int pidAnalyzer, int idFile, int m,
                                 int index, ull totalFileSize,
                                 ull numCharsReadInThisSection,
                                 ull occurences[NUM_OCCURENCES]) {
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

  for (i = 0; i < NUM_OCCURENCES; i++) {
    fromIntToBytes(occurences[i], packet + offset);
    offset += INT_SIZE;
  }

  if (write(fd[WRITE], packet, 1 + 263 * INT_SIZE) != (1 + 263 * INT_SIZE)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the occurences packet\n");
  }

  return returnCode;
}

// Used to send a new file name to report in three cases:
// - unique packet if data can fit
// - packet pt1 and packet pt2 if file path can't fit in a single packet
int _internal_newFileNameToReportPacket(int packetType, int fd[],
                                        pid_t pidAnalyzer, int fileId,
                                        string filePath, int filePathLength) {
  int returnCode = 0, offset = 0;
  int packetSize = 2 + 3 * INT_SIZE + filePathLength;
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

  if (write(fd[WRITE], packet, packetSize) != packetSize) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the new name file packet\n");
  }

  return returnCode;
}

int newFileNameToReportPacket(int fd[], pid_t pidAnalyzer, int fileId,
                              string filePath) {
  int returnCode = 0;
  int filePathLength = strlen(filePath);
  int freeSpaceInFirstPacket = 4096 - 2 - 3 * INT_SIZE - filePathLength;

  if (freeSpaceInFirstPacket >= 0) {
    returnCode = _internal_newFileNameToReportPacket(8, fd, pidAnalyzer, fileId,
                                                     filePath, filePathLength);
  } else {
    int lenFirstPartOfPath = 4096 - 2 - 3 * INT_SIZE;
    returnCode = _internal_newFileNameToReportPacket(
        9, fd, pidAnalyzer, fileId, filePath, lenFirstPartOfPath);

    // send the second packet only if we were able to send the first one
    if (returnCode == 0) {
      returnCode = _internal_newFileNameToReportPacket(
          10, fd, pidAnalyzer, fileId, filePath + lenFirstPartOfPath,
          filePathLength - lenFirstPartOfPath);
    }
  }

  return returnCode;
}

int reportErrorOnFilePacket(int fd[], pid_t pidAnalyzer, int fileId) {
  int returnCode = 0;
  byte packet[1 + 3 * INT_SIZE];

  packet[0] = 11;
  fromIntToBytes(INT_SIZE * 2, packet + 1);
  fromIntToBytes(pidAnalyzer, packet + 1 + INT_SIZE);
  fromIntToBytes(fileId, packet + 1 + 2 * INT_SIZE);

  if (write(fd[WRITE], packet, 1 + 3 * INT_SIZE) != (1 + 3 * INT_SIZE)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the report error file packet\n");
  }

  return returnCode;
}

// Used to delete a folder from report in three cases:
// - unique packet if data can fit
// - packet pt1 and packet pt2 if file path can't fit in a single packet
int _internal_deleteFolderFromReportPacket(int packetType, int fd[],
                                           pid_t pidAnalyzer, string folderPath,
                                           int folderPathLength) {
  int returnCode = 0, offset = 0;
  int packetSize = 2 + 2 * INT_SIZE + folderPathLength;
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

  if (write(fd[WRITE], packet, packetSize) != packetSize) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending delete folder packet\n");
  }

  return returnCode;
}

int deleteFolderFromReportPacket(int fd[], pid_t pidAnalyzer,
                                 string folderPath) {
  int returnCode = 0;
  int folderPathLength = strlen(folderPath);
  int freeSpaceInFirstPacket = 4096 - 2 - 2 * INT_SIZE - folderPathLength;

  if (freeSpaceInFirstPacket >= 0) {
    returnCode = _internal_deleteFolderFromReportPacket(
        12, fd, pidAnalyzer, folderPath, folderPathLength);
  } else {
    int lenFirstPartOfPath = 4096 - 2 - 2 * INT_SIZE;
    returnCode = _internal_deleteFolderFromReportPacket(
        13, fd, pidAnalyzer, folderPath, lenFirstPartOfPath);

    // send the second packet only if we were able to send the first one
    if (returnCode == 0) {
      returnCode = _internal_deleteFolderFromReportPacket(
          14, fd, pidAnalyzer, folderPath + lenFirstPartOfPath,
          folderPathLength - lenFirstPartOfPath);
    }
  }

  return returnCode;
}

// Send finished analysis packet
// Error codes:
// 1 - Error with fd sending the packet
int sendFinishedAnalysisPacket(int fd[]) {
  int returnCode = 0;
  byte finAnalysisPacket[1 + INT_SIZE];

  finAnalysisPacket[0] = 16;
  fromIntToBytes(0, finAnalysisPacket + 1);

  if (write(fd[WRITE], finAnalysisPacket, 1 + INT_SIZE) != (1 + INT_SIZE)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the finished analysis packet\n");
  }

  return returnCode;
}

// Send finished file packet
// Error codes:
// 1 - Error with fd sending the packet
int sendFinishedFilePacket(int fd[], int yetFinished, int total) {
  int returnCode = 0, offset = 0;
  byte finishedFilePacket[1 + 3 * INT_SIZE];

  // header section
  finishedFilePacket[offset++] = 17;
  fromIntToBytes(2 * INT_SIZE, finishedFilePacket + offset);
  offset += INT_SIZE;

  // data section
  fromIntToBytes(yetFinished, finishedFilePacket + offset);
  offset += INT_SIZE;
  fromIntToBytes(total, finishedFilePacket + offset);

  if (write(fd[WRITE], finishedFilePacket, 1 + 3 * INT_SIZE) !=
      (1 + 3 * INT_SIZE)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending the finished analysis packet\n");
  }

  return returnCode;
}

int sendTextMessageToReport(int fd[], const string message) {
  int returnCode = 0;
  int messageLength = strlen(message);
  int packetSize = 1 + INT_SIZE + messageLength + 1;
  byte messagePacket[packetSize];

  messagePacket[0] = 18;
  fromIntToBytes(messageLength, messagePacket + 1);
  memcpy(messagePacket + 1 + INT_SIZE, message, messageLength);
  messagePacket[packetSize - 1] = '\0';

  if (write(fd[WRITE], messagePacket, packetSize) != (packetSize)) {
    returnCode = 1;
    fprintf(stderr, "Error with fd sending message packet to report\n");
  }

  return returnCode;
}
