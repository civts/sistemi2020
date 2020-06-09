#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "crawler.h"
#include "../common/utils.h"
#include "../common/datastructures/namesList.h"

// Parse the output of the command find <folder> -type f
int parseFileListFromFind(int readDescriptor, NamesList *fileList){
    FILE *pipeToRead = fdopen(readDescriptor, "r");
    int numOfFileNamesProcessed = 0;
    char buffer[SIZE_OF_BUFFER_TO_READ_PIPE];
    string filePath;
    char *absolutePath;

    while (fgets(buffer, SIZE_OF_BUFFER_TO_READ_PIPE, pipeToRead) != NULL){
        filePath = strtok(buffer, "\n");
        absolutePath = realpath(filePath, absolutePath);
        if(absolutePath != NULL){
            appendNameToNamesList(fileList, absolutePath);
        }
        numOfFileNamesProcessed++;
    }

    return numOfFileNamesProcessed;
}

// Get all the file paths inside a folder, recursively
// Error codes:
// 1: it was not possible to fork a child
// 2: it was not possibile to inspect the folder
int crawler(string folder, NamesList *fileList, int* outNumFilesFound){
    int returnCode = 0;
    *outNumFilesFound = -1; // in case of error;
    string lsArgs[] = {"find", folder, "-type", "f", NULL};
    int fds[2];
    pipe(fds);

    pid_t f = fork();
    if (f < 0){
        returnCode = 1;
    } else if (f == 0){
        // child process
        close(fds[READ]);
        dup2(fds[WRITE], 1); // substitute stdout with fds[WRITE] for find
        execvp("find", lsArgs);
        returnCode = 2; // should never come here if exec works fine
    } else {
        // parent process
        close(fds[WRITE]);
        
        int out; // return code from ls
        pid_t pid = waitpid(f, &out, 0);
        if (pid != -1 && out == 0){
            *outNumFilesFound = parseFileListFromFind(fds[READ], fileList);
        } else {
            returnCode = 3;
        }
    }

    return returnCode;
}