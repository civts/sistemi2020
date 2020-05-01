#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.c"

#define READ 0
#define WRITE 1
#define SIZE_OF_BUFFER_TO_READ_PIPE 256
// TODO: check stinky lengths
#define MAX_SIZE_OF_FOLDER_NAME 256

void printFileList(string fileList[], int numFiles){
    int i;
    for (i=0; i<numFiles; i++){
        printf("File %d: %s\n", i+1, fileList[i]);
    }
}

void addFolderToFileName(string out, string folder, string fileName){
    strcpy(out, folder);
    strcat(out, fileName);
}

int getOutputFromLS(int readDescriptor, string fileList[], string folder){
    FILE *pipeToRead = fdopen(readDescriptor, "r");
    int numOfFileNamesProcessed = 0;

    // TODO: check if 256 as limit can be a problem for file names
    string buffer = (string) malloc(SIZE_OF_BUFFER_TO_READ_PIPE);
    
    while(fgets(buffer, SIZE_OF_BUFFER_TO_READ_PIPE, pipeToRead) != NULL){

        string fileName = strtok(buffer, "\n"); // delete ending \n in filename string
        
        /**
        Little fix below here, fileName can also be null, tehrefore we must check that condition, and in case it is
        NULL if it enters isDirectory() it causes segmentation fault
        **/
        // do not include subdirectories
        if (fileName!=NULL && !isDirectory(fileName, '/')){
            fileList[numOfFileNamesProcessed] = (string) malloc(strlen(fileName) + strlen(folder) + 1);
            addFolderToFileName(fileList[numOfFileNamesProcessed], folder, fileName);
            // strcpy(fileList[numOfFileNamesProcessed], buffer);
            numOfFileNamesProcessed++;
        }

    }
    return numOfFileNamesProcessed;
}

int getOutputFromLSRec(int readDescriptor, string fileList[]){
    FILE *pipeToRead = fdopen(readDescriptor, "r");
    int numOfFileNamesProcessed = 0;
    // TODO: check if 256 as limit can be a problem for file names
    string buffer = (string)malloc(SIZE_OF_BUFFER_TO_READ_PIPE);
    string currentFolder = (string)malloc(MAX_SIZE_OF_FOLDER_NAME);

    while(fgets(buffer, SIZE_OF_BUFFER_TO_READ_PIPE, pipeToRead) != NULL){
        string stringRead = strtok(buffer, "\n");
        // check if the string ends with ":"
        if (stringRead != NULL && isDirectory(stringRead, ':')){
            // that's a new foldah
            // printf("I found folder  '%s'\n", stringRead);
            if(stringRead[strlen(stringRead)-2]!='/'){
                // we must add / in place of :
                stringRead[strlen(stringRead)-1]='/';
            } else {
                // if the folder is ./ we must not add /, but only delete :
                stringRead[strlen(stringRead)-1]='\0';
            }
            strcpy(currentFolder, stringRead);  
        } else if (stringRead!=NULL) {
            // if it ends with '/' it's a folder, then we'll inspect it later 
            if(!isDirectory(stringRead, '/')){
                // that's a file
                fileList[numOfFileNamesProcessed] = (string) malloc(strlen(stringRead) + strlen(currentFolder) + 1);
                addFolderToFileName(fileList[numOfFileNamesProcessed], currentFolder, stringRead);
                numOfFileNamesProcessed++;
            }
            
        }
    }

    return numOfFileNamesProcessed;
}

// Get all the file path inside a folder
// Error codes:
// 1: it was not possible to fork a child
// 2: it was not possibile to inspect the folder
int crawler(string folder, string fileList[], int* outNumFilesFound){
    *outNumFilesFound = -1; // in case of error;
    int returnCode = 0;
    // string lsArgs[] = {"ls", "-p", folder, NULL}; // test is just a placeholder because we can't pass only NULL
    string lsArgs[] = {"ls", folder, "-p", "-R", NULL};
    int fds[2];
    pipe(fds);

    // TODO change type of f to pid
    int f = fork();
    if (f < 0){
        printf("\nError creating little brittle crawler-son\n");
        returnCode = 1;
    } else if (f == 0){
        // child
        close(fds[READ]);
        dup2(fds[WRITE], 1); // substitute stdout with fds[WRITE] for ls
        execvp("ls", lsArgs);
        // system("ls ../");
        printf("Error: it is not possibile to inspect folder: %s\n", folder);
        returnCode = 2; // should never be here if exec works fine
    } else {
        // parent
        close(fds[WRITE]);
        
        // TODO check out == 0
        int out; // return code from ls

        // TODO change to PID
        int pid = waitpid(f, &out, 0);
        
        *outNumFilesFound = getOutputFromLSRec(fds[READ], fileList);
    }

    return returnCode;
}


// TODO: delete main when crawler is completely tested
// int main(){
//     int numFilesFound = 0;
//     string folder = "../test/";
//     string fileList[50];
//     crawler(folder, fileList, &numFilesFound);
//     printf("num files2: %d\n", numFilesFound);
//     printFileList(fileList, numFilesFound);
//     return 0;
// }