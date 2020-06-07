#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// #include <limits.h>
// #include "utils.c"
#include "../../src/utils.c"
#include "datastructures/namesList.c"

// maximum file name length: 255
// maximum file path length: 4096 (is the file name included?)
#define SIZE_OF_BUFFER_TO_READ_PIPE 4096

char *absolutePath;

int parseFileListFromFind(int readDescriptor, NamesList *fileList){
    FILE *pipeToRead = fdopen(readDescriptor, "r");
    int numOfFileNamesProcessed = 0;
    char buffer[SIZE_OF_BUFFER_TO_READ_PIPE];
    string filePath;

    while(fgets(buffer, SIZE_OF_BUFFER_TO_READ_PIPE, pipeToRead) != NULL){
        filePath = strtok(buffer, "\n");
        absolutePath = realpath(filePath, absolutePath);
        if(absolutePath != NULL){
            appendNameToNamesList(fileList, absolutePath);
        }
        numOfFileNamesProcessed++;
    }

    return numOfFileNamesProcessed;
}

// Get all the file path inside a folder
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
        fprintf(stderr, "\nError creating little brittle crawler-son\n");
        returnCode = 1;
    } else if (f == 0){
        // child
        close(fds[READ]);
        dup2(fds[WRITE], 1); // substitute stdout with fds[WRITE] for ls
        execvp("find", lsArgs);
        fprintf(stderr, "Error: it is not possibile to inspect folder: %s\n", folder);
        returnCode = 2; // should never be here if exec works fine
    } else {
        // parent
        close(fds[WRITE]);
        
        // TODO check out == 0
        int out; // return code from ls
        pid_t pid = waitpid(f, &out, 0);
        if (pid != -1){
            *outNumFilesFound = parseFileListFromFind(fds[READ], fileList);
        } else {
            fprintf(stderr, "Error waiting for ls syscall termination\n");
            returnCode = 3;
        }
    }

    return returnCode;
}

// Given a path to a file/folder it returns:
// -1 : if it does not exist
//  0 : if it is a file and it exists
//  1 : if it is is a folder and it exists
int inspectPath(const char *path){
    struct stat path_stat;
    int returnCode = -1;
    if (path != NULL && stat(path, &path_stat) == 0){
        if (S_ISREG(path_stat.st_mode)){
            returnCode = 0;
        } else if (S_ISDIR(path_stat.st_mode)){
            returnCode = 1;
        }
    }
    return returnCode;
}

// int main(){
//     string folder = "./";
//     NamesList *list = constructorNamesList();
//     int filesFound;

//     crawler(folder, list, &filesFound);

//     printf("Found %d files:\n", filesFound);
//     printNamesList(list);

//     char buffer[SIZE_OF_BUFFER_TO_READ_PIPE];

//     string file = NULL;
//     realpath(file, buffer);
//     printf("isRegularFile: %d\n", inspectPath(file));
//     printf("realpath %s\n", buffer);
    
//     return 0;
// }