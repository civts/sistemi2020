#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> // stat() to get file length
#include <unistd.h>   // lseek
#include <math.h>     // for ceil()
#include <fcntl.h>    // for file descriptors
#include <string.h>   // for strlen()
#include <signal.h>   // for signals
#include "packets.h"
#include "utils.c"
#include "datastructures/miniQlist.c"

// IMPORTANT: compile this with -lm to make ceil works...

void  miniQ(string, miniQinfo*);
// int   getOccurences(string, long, long, ull[NUM_OCCURENCES]);
int  getBigOccurences(string, long, long, ull[NUM_OCCURENCES]);
long getFileLength(string fileName);
void sig_handler_miniQ();


// Principal core of a miniQ: it's goal is to detect char occurences
// and sent them back to its parent Q
void miniQ(string fileName, miniQinfo *instanceOfMySelf){
    int exitCode = 0;

    signal(SIGINT, sig_handler_miniQ);
    signal(SIGKILL, sig_handler_miniQ);
    if (instanceOfMySelf->index >= instanceOfMySelf->currM){
        // should never come here: index of Q must always be less than M
        fprintf(stderr, "Error, index of miniQ bigger than its M value\n");
        exitCode = sendErrorOnFileToReport(instanceOfMySelf);
    } else {
        // check if file exists and it has some data
        long fileLength = getFileLength(fileName);
        if (fileLength > 0){
            // we shall read the interval [startPosition, endPosition[
            long lenPortion    = ceil(fileLength / (double)instanceOfMySelf->currM);
            long startPosition = lenPortion * instanceOfMySelf->index;
            long endPosition   = min_l(fileLength, startPosition + lenPortion);
            
            // get character occurences from the file
            ull occurences[256];
            int numCharsInPortion = getBigOccurences(fileName, startPosition, endPosition, occurences);
            
            if (numCharsInPortion >= 0){
                exitCode = sendOccurencesPacketToReport(instanceOfMySelf->pipeToQ, -1, instanceOfMySelf->fileId,
                                         instanceOfMySelf->currM, instanceOfMySelf->index, fileLength,
                                         endPosition-startPosition, occurences);
                printf("I've analyzed %d chars in %s\n", numCharsInPortion, fileName);
            } else {
                exitCode = sendErrorOnFileToReport(instanceOfMySelf);
            }
        } else {
            exitCode = sendErrorOnFileToReport(instanceOfMySelf);
        }
    }

    exit(exitCode);
}

int sendErrorOnFileToReport(miniQinfo *instanceOfMySelf){
    return reportErrorOnFilePacket(instanceOfMySelf->pipeToQ, -1, instanceOfMySelf->fileId);
}

// Giving the starting and ending offset in the file, it gets the number of
// occurences for each char. It returns the number of byte read.
// int getOccurences(string fileName, long startPosition, long endPosition, ull outOccurences[NUM_OCCURENCES]){
//     // TODO check for NULL pointer -> maybe we should allocate less memory and read more times
//     int bufferSize = endPosition - startPosition;
//     byte *buffer = (byte*) malloc(bufferSize * sizeof(byte));

//     int i;
//     for (i = 0; i<NUM_OCCURENCES; i++){
//         outOccurences[i] = 0;
//     }
    
//     int fd;
//     fd = open(fileName, O_RDONLY);

//     if (fd == -1){
//         fprintf(stderr, "Error, can't open the file %s\n", fileName);
//     } else {
//         lseek(fd, startPosition, SEEK_SET);

//         int r = read(fd, buffer, bufferSize);
//         bufferSize = min_l(r, bufferSize);

//         for (i = 0; i < bufferSize; i++){
//             outOccurences[buffer[i]]++;
//         }
//     }

//     // free resources
//     close(fd);
//     free(buffer);

//     return bufferSize;
// }

// Get occurences of a portion of file given starting and ending position inside the file.
// If we get an error opening a file: return -1
int getBigOccurences(string fileName, long startPosition, long endPosition, ull outOccurences[NUM_OCCURENCES]){
    byte buffer[MINIQ_MAX_BUFFER_SIZE];
    int i, fd, numCharsRead = 0, returnCode = 0;

    for (i = 0; i<NUM_OCCURENCES; i++){
        outOccurences[i] = 0;
    }

    fd = open(fileName, O_RDONLY);
    if (fd == -1){
        fprintf(stderr, "Error, can't open the file %s\n", fileName);
        returnCode = -1;
    } else {
        lseek(fd, startPosition, SEEK_SET);

        while (startPosition < endPosition){
            long bufferSize = min_l(endPosition - startPosition, MINIQ_MAX_BUFFER_SIZE);
            int r = read(fd, buffer, bufferSize);

            for (i = 0; i < bufferSize; i++){
                outOccurences[buffer[i]]++;
            }

            startPosition += MINIQ_MAX_BUFFER_SIZE;
            numCharsRead += r;
        }

        // free resources
        close(fd);
    }
    
    if (returnCode == 0){
        returnCode = numCharsRead;
    }

    return returnCode;
}

// Get file length using stat syscall
// If it fails return -1.
long getFileLength(string fileName){
    struct stat stbuf;
    long fileLength = -1;
    
    if (stat(fileName, &stbuf) == 0){
        fileLength = stbuf.st_size;
    }
    
    return fileLength;
}

void sig_handler_miniQ(){
    printf("\nMiniQ killed with signal\n");
    exit(0);
}