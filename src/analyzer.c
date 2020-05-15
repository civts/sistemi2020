#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.c"
#include "crawler.c"
// #include "controller.c"

// TODO : ELIMINA
void controller(string mode){
    printf("Controller avviato in modalità: %s\n", mode);
}


// Usages:
// -i: interactive mode
// -s: static mode
// -h: help mode

// Error codes:
// 1: missing arguments
// 2: n and m are not numeric non-zero values
// 3: usage mode not supported

NamesList *filePaths;
int numOfFiles = 0, n = 0, m = 0;

int  modeSwitcher(char, int, char**);
void helpMode();
void interactiveMode();
void staticMode(int, int, int, NamesList *);

bool isValidMode(string);
int getFilePathsFromArgv(string[], NamesList*, int);

// check if the mode is a two char string, with the
// first char being '-'
bool isValidMode(string mode){
    return (strlen(mode) == 2) && (mode[0] == '-');
}

// extract file paths from argv array. In case of folder,
// it uses the crawler to inspect inner files and folders.
// It returns the number of scanned files.
int getFilePathsFromArgv(string argv[], NamesList *fileList, int numPaths){
    const int padding = 4;    // index inside argv from which filenames occur
    unsigned long numFiles=0; // number of files recognized
    int out;
    fileList = constructorNamesList();

    int i;
    for (i = 0; i < numPaths; i++){
        if (isDirectory(argv[i + padding], '/', &out) && out == 0){
            int outNewFiles = 0;
            crawler(argv[i + padding], fileList, &outNewFiles);
            numFiles += outNewFiles;
        } else if ( isValidFile(argv[i + padding]) && out == 0 ){
            appendName(fileList, argv[i + padding]);
            numFiles++;
        }
    }

    return (int)numFiles;
}

int main(int argc, char *argv[]){
    int returnCode = 0;
    filePaths = constructorNamesList();

    if (argc == 1){
        printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
        returnCode = 1;
    } else if ( isValidMode(argv[1]) ){
        modeSwitcher(argv[1][1], argc, argv);
    } else {
        printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
        returnCode = 1;
    }
    
    // else if (argc <= 4){
    //     if (isValidMode(argv[1]) && argv[1][1] == 'h'){
    //         // Case -h
    //         helpMode();
    //     } else if((isValidMode(argv[1]) && argv[1][1] == 'i')){
    //         // Case -i
    //         interactiveMode();
    //     } else {
    //         printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
    //         returnCode = 1;
    //     }
    // } else {
    //     // Case -s
    //     if (!isValidMode(argv[1])){
    //         printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
    //         returnCode = 1;
    //     } else {
    //         char mode = argv[1][1]; // -i, -s, -h
    //         returnCode = modeSwitcher(mode, argc, argv);
    //     }
    // }

    return returnCode;
}

// Switch mode of the analyzer (interactive, static, help)
// Error codes:
// 3 - mode not supported
int modeSwitcher(char mode, int argc, char *argv[]){
    int returnCode = 0;

    switch (mode){

        case 'h':
            helpMode();
            break;
        case 'i':            
            interactiveMode();
            break;
        case 's':
            if(argc <= 4){
                printf("?Error: specify a valid mode, n, m and at least one file/folder\n");
                returnCode = 1;
            } else {
                
                // TODO use strol/stroll for parsing integer values
                n = atoi(argv[2]);
                m = atoi(argv[3]);

                // get file paths with the crawler
                numOfFiles = getFilePathsFromArgv(argv, filePaths, argc-4);

                if (n==0 || m==0){
                    printf("Error: specify numeric non-zero values for n and m\n");
                    returnCode = 2;
                } else {
                    staticMode(n, m, numOfFiles, filePaths);
                }
            }
            break;

        default:
            printf("Error: mode not supported\n");
            returnCode = 3;
    }

    return returnCode;
}

void helpMode(){
    string help_message =   "Help mode\n\n"
                            "Usages:\n"
                            "-i: interactive mode\n"
                            "\tInteractive mode commands:\n\n"
                            "\t+_name_fi/fo_ to add a file/folder to the list\n"
                            "\t-_name_file_  to remoove a file from the list\n"
                            "\tn=_value_     to set the value of n\n"
                            "\tm=_value_     to set the value of m\n"
                            "\ts             to see the list of files at the current moment\n"
                            "\tanalyze       to start the analisys\n"
                            "\texit          to exit from the process\n\n"
                            "-s: static mode\n"
                            "\tmust insert arguments: n, m and at least one file/folder\n\n"
                            "-h: help mode\n\n"
                            "Error codes:\n"
                            "1: missing arguments\n"
                            "2: n and m are not numeric non-zero values\n"
                            "3: usage mode not supported\n\n";

    printf("%s", help_message);
}

// TODO - this should be executed inside a child
void interactiveMode(){
    const char analyzeString[] = "analyze";
    const char exitString[] = "exit";

    printf("Interactive mode\n");

    char command[MINIQ_MAX_BUFFER_SIZE];

    printf("> ");
    scanf("%s", command);
    while (strcmp(command, exitString) != 0){
        if (strcmp(command, analyzeString) == 0){
            // start analyzing process
            // controller(n, m, filePaths, numOfFiles);
            
            controller("interactive");
            printf("Start analysis\n");

        } else if (command[0] == '+'){
            
            int *j = malloc(sizeof(int));
            
            if(isDirectory(command+1, '/', j)){
                crawler(command+1, filePaths, j);
            } else if(isValidFile(command+1)) {
                appendName(filePaths, command+1);
            } else {
                printf("File/folder inserted desn't exist!\n");
            }
            printf("Now the file-list is:\n");
            printNamesList(filePaths);

        } else if (command[0] == '-'){
            printf("Remove file %s\n", command + 1);

            removeByName(filePaths, command+1);

            printf("Now the file-list is:\n");
            printNamesList(filePaths);
        } else if (command[0] == 'n'){
            printf("Change n to %s\n", command + 2);

            n=atoi(command+2);
            printf("Now n=%d\n", n);

        } else if (command[0] == 'm'){
            printf("Change m to %s\n", command + 2);

            m=atoi(command+2);
            printf("Now m=%d\n", m);

        } else if (command[0] == 's'){
            printNamesList(filePaths);
        } else {
            // command not supported
        }
        printf("> ");
        scanf("%s", command);
    }    
}

void staticMode(int numOfP, int numOfQ, int numOfFiles, NamesList * listFilePaths){
    printf("Static mode\n");
    // printf("Files to process:\n");
    // printFileList(listFilePaths, numOfFiles);
    // controller(numOfP, numOfQ, listFilePaths, true);
}