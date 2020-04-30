#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.c"

#define MAX_FILES 10

// Usages:
// -i: interactive mode
// -s: static mode
// -h: help mode

// Error codes:
// 1: missing arguments
// 2: n and m are not numeric non-zero values
// 3: usage mode not supported

void helpMode();
void interactiveMode();
void staticMode(int, int, string[]);

int main(int argc, char *argv[]){
    int returnCode = 0;
    int numOfP = 0, numOfQ = 0;

    // TODO implement this as a list with pointers
    string filePaths[MAX_FILES]; // list of files to scan

    if (argc <= 4){
        // TODO check if argc >= 2 and argv[1] has 2 chars
        if (argv[1][1] == 'h'){
            helpMode();
        } else {
            printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
            returnCode = 1;
        }
    } else {
        // parsing argv
        // check if argv[1] has 2 chars
        char mode = argv[1][1]; // -i, -s, -h

        // TODO use strol/stroll for parsing integer values
        numOfP = atoi(argv[2]);
        numOfQ = atoi(argv[3]);

        // TODO input file list

        if (numOfP==0 || numOfQ==0){
            printf("Error: specify numeric non-zero values for n and m\n");
            returnCode = 2;
        } else {
            switch (mode){
                // TODO check for help mode before num of argument:
                case 'h':
                    helpMode();
                    break;
                case 'i':
                    interactiveMode();
                    break;
                case 's':
                    staticMode(numOfP, numOfQ, filePaths);
                    break;
                default:
                    printf("Error: mode non supported\n");
                    returnCode = 3;
            }
        }
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

void staticMode(int numOfP, int numOfQ, string filePaths[]){
    printf("Static mode\n");
}