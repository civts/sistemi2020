#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.c"
#include "crawler.c"
#include "controller.c"

#define MAX_FILES 50

// Usages:
// -i: interactive mode
// -s: static mode
// -h: help mode

// Error codes:
// 1: missing arguments
// 2: n and m are not numeric non-zero values
// 3: usage mode not supported

// TODO implement this as a list with pointers
string filePaths[MAX_FILES]; // list of files to scan
int numOfFiles = 0, n = 0, m = 0;

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
    int out;

    int i;
    for (i = 0; i < numPaths; i++){
        if (isDirectory(argv[i + padding], '/', &out) && out == 0){
            int outNewFiles = 0;
            crawler(argv[i + padding], fileLists + numFiles, &outNewFiles);
            numFiles += outNewFiles;
        } else if ( isValidFile(argv[i + padding]) && out == 0 ){
            fileLists[numFiles] = (string) malloc(strlen(argv[i + padding]) + 1);
            strcpy(fileLists[numFiles], argv[i + padding]);
            numFiles++;
        }
    }

    return (int)numFiles;
}

int main(int argc, char *argv[]){
    int returnCode = 0;

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
            n = atoi(argv[2]);
            m = atoi(argv[3]);

            // get file paths with the crawler
            numOfFiles = getFilePathsFromArgv(argv, filePaths, argc-4);

            if (n==0 || m==0){
                printf("Error: specify numeric non-zero values for n and m\n");
                returnCode = 2;
            } else {
                returnCode = modeSwitcher(n, m, numOfFiles, filePaths, mode);
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

// TODO - this should be executed inside a child
void interactiveMode(){
    const char analyzeString[] = "analyze";
    const char exitString[] = "exit";

    printf("Interactive mode\n");

    char command[5000]; // TODO change to fit max path length on linux
    printf("> ");
    scanf("%s", command);
    while (strcmp(command, exitString) != 0){
        if (strcmp(command, analyzeString) == 0){
            // start analyzing process
            // controller(n, m, filePaths, numOfFiles);
            printf("Start analysis\n");
        } else if (command[0] == '+'){
            printf("New file %s\n", command + 1);
        } else if (command[0] == '-'){
            printf("Remove file %s\n", command + 1);
        } else if (command[0] == 'n'){
            printf("Change n to %s\n", command + 2);
        } else if (command[0] == 'm'){
            printf("Change m to %s\n", command + 2);
        } else {
            // command not supported
        }
        printf("> ");
        scanf("%s", command);
    }    
}

void staticMode(int numOfP, int numOfQ, int numOfFiles, string listFilePaths[]){
    printf("Static mode\n");
    // printf("Files to process:\n");
    // printFileList(listFilePaths, numOfFiles);
    controller(numOfP, numOfQ, listFilePaths, true);
}