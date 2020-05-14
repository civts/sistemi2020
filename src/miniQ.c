#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> // stat() to get file length
#include <unistd.h>   // lseek
#include <math.h>     // for ceil()
#include <fcntl.h>    // for file descriptors
#include <string.h>   // for strlen()
#include "packets.h"
#include "utils.c"

#define NUM_OCCURENCES 256 // this should be un utils...

// IMPORTANT: compile this with -lm to make ceil works...

void  miniQ(string, bool, int, int);
void  sendOccurencesToReport(string, bool, int, int [NUM_OCCURENCES]);
byte* encodePacketForReport(string, bool, int, int [NUM_OCCURENCES], int*);
int   getOccurences(string, long, long, uint[NUM_OCCURENCES]);
long  getFileLength(string fileName);
void  printOccurencesTemp(string, int[NUM_OCCURENCES]);

// principal core of a miniQ: it's goal is to detect the char occurences
// for a single file of his parent Qij process
void miniQ(string fileName, bool isInsideFolder, int numOfPortions, int portionOfFileToRead){
    if (portionOfFileToRead >= numOfPortions){
        // should never come here
        fprintf(stderr, "Error, what's that portion?");
    } else {
        // check if file exists and it has some data
        long fileLength = getFileLength(fileName);
        if (fileLength > 0){
            // we shall read the interval [startPosition, endPosition[
            long lenPortion    = ceil(fileLength / (double)numOfPortions);
            long startPosition = lenPortion * portionOfFileToRead;
            long endPosition   = min_l(fileLength, startPosition + lenPortion);
            
            // TODO change to uint
            // get character occurences from the file
            uint occurences[256];

            // bufferOccurecesSize should be endPosition-startPosition
            int numCharsInPortion = getOccurences(fileName, startPosition, endPosition, occurences);
            // sendOccurencesToReport(fileName, isInsideFolder, numCharsInPortion, occurences);
            printf("I've analyzed %d chars in %s\n", numCharsInPortion, fileName);

        }        

        exit(0);
    }
}

// TODO atm I'm doing output on a normal file, change the it to the nominal pipe according to report.c
// TODO should we include a list of possibile nominal pipes to try if the first does not work?
// send char occureces to the report through a nominal pipe
void sendOccurencesToReport(string fileName, bool isInsideFolder, int numCharInPortion, int occurences[NUM_OCCURENCES]){
    int fd = open ("file.txt", O_WRONLY|O_CREAT, 0644);

    if (fd == -1){
        // TODO if errors try again after a delay (set max number of attemps)
        fprintf(stderr, "Could not write packet in the pipe\n");
    } else {
        // write encoded stream of bytes to the pipe
        int bufferSize;
        byte *buffer = encodePacketForReport(fileName, isInsideFolder, numCharInPortion, occurences, &bufferSize);
        write(fd, buffer, bufferSize);

        // free resources
        close(fd);
        free(buffer);
    }
}

// TODO write the packet format in a doc
// encode an occurences packet to send to the record through the nominal
// return the number of bytes in the packet
byte* encodePacketForReport(string fileName, bool isInsideFolder, int numCharInPortion, int occurences[NUM_OCCURENCES], int* outBufferSize){
    int lenFileName = strlen(fileName);
    const int bufferSize = 1 + INT_SIZE + lenFileName + 1 + INT_SIZE + NUM_OCCURENCES*INT_SIZE; // see docs

    byte* outBuffer = (byte*) malloc(sizeof(byte) * bufferSize);
    byte tempInteger[INT_SIZE]; // used for integer to bytes conversion

    int offset = 0;

    // packet type: occurences
    outBuffer[offset] = 0;
    offset++;

    // length of pathname
    fromIntToBytes(lenFileName, tempInteger);
    memcpy(outBuffer + offset, tempInteger, INT_SIZE);
    offset += INT_SIZE;

    // filename (without ending \0)
    memcpy(outBuffer + offset, fileName, lenFileName);
    offset += lenFileName;

    // 1 the file is inside a folder, 0 otherwise
    outBuffer[offset] = isInsideFolder;
    offset++;

    // TODO change to long
    // total number of chars in the current file portion
    fromIntToBytes(numCharInPortion, tempInteger);
    memcpy(outBuffer + offset, tempInteger, INT_SIZE);
    offset += INT_SIZE;

    // print the chars occurences
    int i;
    for (i = 0; i < NUM_OCCURENCES; i++){
        fromIntToBytes(occurences[i], tempInteger);
        memcpy(outBuffer + offset, tempInteger, INT_SIZE);
        offset += INT_SIZE;
    }
    
    *outBufferSize = bufferSize;
    return outBuffer;
}

// giving the starting and ending offset in the file, it gets the number of
// occurences for each char. It returns the number of byte read.
int getOccurences(string fileName, long startPosition, long endPosition, uint outOccurences[NUM_OCCURENCES]){
    // TODO check for NULL pointer -> maybe we should allocate less memory and read more times
    int bufferSize = endPosition - startPosition;
    byte *buffer = (byte*) malloc (bufferSize * sizeof(byte));

    int i;
    for (i = 0; i<NUM_OCCURENCES; i++){
        outOccurences[i] = 0;
    }
    
    int fd;
    fd = open(fileName, O_RDONLY);

    if (fd == -1){
        fprintf(stderr, "Error, can't open the file %s\n", fileName);
    } else {
        lseek(fd, startPosition, SEEK_SET);

        int r = read(fd, buffer, bufferSize);
        bufferSize = min_l(r, bufferSize);

        for (i = 0; i < bufferSize; i++){
            outOccurences[buffer[i]]++;
        }
    }

    // free resources
    close(fd);
    free(buffer);

    return bufferSize;
}

// it gets file length using stat syscall
long getFileLength(string fileName){
    struct stat stbuf;

    long fileLength = -1;
    
    if (stat(fileName, &stbuf) == 0){
        fileLength = stbuf.st_size;
    }
    
    return fileLength;
}