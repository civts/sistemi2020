#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.c"
#include "datastructures/namesList.c"

#define READ 0
#define WRITE 1
#define SIZE_OF_BUFFER_TO_READ_PIPE 4096
// maximum file name length: 255
// maximum file path length: 4096 (is the file name included?)
#define MAX_SIZE_OF_FOLDER_NAME 4096


void addFolderToFileName(string out, string folder, string fileName){
    strcpy(out, folder);
    strcat(out, fileName);
}

int getOutputFromLSRec(int readDescriptor, NamesList *fileList){
    FILE *pipeToRead = fdopen(readDescriptor, "r");
    int numOfFileNamesProcessed = 0;
    
    char buffer[SIZE_OF_BUFFER_TO_READ_PIPE];
    char currentFolder[MAX_SIZE_OF_FOLDER_NAME];
    int out;

    while(fgets(buffer, SIZE_OF_BUFFER_TO_READ_PIPE, pipeToRead) != NULL){
        string stringRead = strtok(buffer, "\n");
        // check if the string ends with ":"
        if (isDirectory(stringRead, ':', &out) && out==0){
            // we found a new folder
            if(stringRead[strlen(stringRead) - 2] != '/'){
                // we must add / in place of :
                stringRead[strlen(stringRead) - 1] = '/';
            } else {
                // if the folder is ./ we must not add /, but only delete :
                stringRead[strlen(stringRead) - 1] = '\0';
            }
            strcpy(currentFolder, stringRead);  
        
        } else {
            // if it ends with '/' it's a folder, then we'll inspect it later 
            if(!isDirectory(stringRead, '/', &out) && out == 0){
                // that's a file
                string completeName = (string) malloc(strlen(stringRead) + strlen(currentFolder) + 1);

                if (completeName != NULL){
                    addFolderToFileName(completeName, currentFolder, stringRead);

                    Node *newNode = constructorNode(completeName);
                    append(fileList, newNode);

                    numOfFileNamesProcessed++;
                }
            }
        }
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
    string lsArgs[] = {"ls", folder, "-p", "-R", NULL};
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
        execvp("ls", lsArgs);
        fprintf(stderr, "Error: it is not possibile to inspect folder: %s\n", folder);
        returnCode = 2; // should never be here if exec works fine
    } else {
        // parent
        close(fds[WRITE]);
        
        // TODO check out == 0
        int out; // return code from ls
        pid_t pid = waitpid(f, &out, 0);
        if (pid != -1){
            *outNumFilesFound = getOutputFromLSRec(fds[READ], fileList);
        } else {
            fprintf(stderr, "Error waiting for ls syscall termination\n");
            returnCode = 3;
        }
    }

    return returnCode;
}

// int main(){
//     NamesList *list = constructorNamesList();
//     int *n = malloc(sizeof(int));
//     crawler("./", list, n);
//     printNamesList(list);
//     Node *no = list->first;
//     // while(no!=NULL){
//     //     printf("node: %s\n", (no->name) );
//     //     no = no->next;
//     // }

//     return 0;
// }