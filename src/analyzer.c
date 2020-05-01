#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.c"
#include "crawler.c"

#define MAX_FILES 50

// Usages:
// -i: interactive mode
// -s: static mode
// -h: help mode

// Error codes:
// 1: missing arguments
// 2: n and m are not numeric non-zero values
// 3: usage mode not supported

int  modeSwitcher(int, int, int, string[], char);
void helpMode();
void interactiveMode();
void staticMode(int, int, int, string[]);

bool isValidMode(string);
int getFilePathsFromArgv(string[], string[], int);

// check if the mode is a two char string, with the
// first char being a -
bool isValidMode(string mode){
    return (strlen(mode) == 2) && (mode[0] == '-');
}

// extract file paths from argv array. In case of folder,
// it uses the crawler to inspect inner files and folders.
// It returns the number of scanned files.
int getFilePathsFromArgv(string argv[], string fileLists[], int numPaths){
    const int padding = 4;    // index inside argv from which filenames occur
    unsigned long numFiles=0; // number of files recognized

    int i;
    for (i=0; i<numPaths; i++){
        if (isDirectory(argv[i+padding], '/')){
            int outNewFiles = 0;
            crawler(argv[i+padding], fileLists+numFiles, &outNewFiles);
            numFiles += outNewFiles;
        } else {
            fileLists[numFiles] = (string) malloc(strlen(argv[i+padding]) + 1);
            strcpy(fileLists[numFiles], argv[i+padding]);
            numFiles++;
        }
    }

    return (int)numFiles;
}

int main(int argc, char *argv[]){
    int returnCode = 0;
    int numOfP = 0, numOfQ = 0, numOfFiles = 0;

    // TODO implement this as a list with pointers
    string filePaths[MAX_FILES]; // list of files to scan

    if (argc == 1){
        printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
        returnCode = 1;
    } else if (argc <= 4){
        if (isValidMode(argv[1]) && argv[1][1] == 'h'){
            helpMode();
        } else {
            printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
            returnCode = 1;
        }
    } else {
        // parsing argv
        if (!isValidMode(argv[1])){
            printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
            returnCode = 1;
        } else {
            char mode = argv[1][1]; // -i, -s, -h

            // TODO use strol/stroll for parsing integer values
            numOfP = atoi(argv[2]);
            numOfQ = atoi(argv[3]);

            // get file paths with the crawler
            numOfFiles = getFilePathsFromArgv(argv, filePaths, argc-4);

            if (numOfP==0 || numOfQ==0){
                printf("Error: specify numeric non-zero values for n and m\n");
                returnCode = 2;
            } else {
                returnCode = modeSwitcher(numOfP, numOfQ, numOfFiles, filePaths, mode);
            }
        }
    }

    return returnCode;
}

// Switch mode of the analyzer (interactive, static, help)
// Error codes:
// 3 - mode not supported
int modeSwitcher(int numOfP, int numOfQ, int numOfFiles, string filePaths[], char mode){
    int returnCode = 0;

    switch (mode){
        case 'h':
            helpMode();
            break;
        case 'i':
            interactiveMode();
            break;
        case 's':
            staticMode(numOfP, numOfQ, numOfFiles, filePaths);
            break;
        default:
            printf("Error: mode not supported\n");
            returnCode = 3;
    }

    return returnCode;
}

void helpMode(){
    printf("Help mode\n\n");
    printf("Usages:\n");
    printf("-i: interactive mode\n");
    printf("-s: static mode\n");
    printf("-h: help mode\n\n");

    printf("Error codes:\n");
    printf("1: missing arguments\n");
    printf("2: n and m are not numeric non-zero values\n");
    printf("3: usage mode not supported\n\n");
}

void interactiveMode(){
    printf("Interactive mode\n");
}

void staticMode(int numOfP, int numOfQ, int numOfFiles, string listFilePaths[]){
    printf("Static mode\n");
    printf("Files to process:\n");
    printFileList(listFilePaths, numOfFiles);
}