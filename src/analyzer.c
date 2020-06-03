#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "utils.c"
#include "crawler.c"
#include "packets.h"
#include "controller.c"

/**
 * TODO: Insert possibility to remove an entire folder? (un casin)
 * NOTE: processExit() è dichiarata come funzione (invece che essere scritta all'interno del codice
 *       perché dovrà essere richiamata anche alla ricezione di SIGKILL)
 * TODO: Decidere cosa fare mentre l'analisi statica si sta completando (una mezza soluzione è
 *       già implementata, dai un'occhiata)
 */

controllerInstance *cInstance;
NamesList *filePaths;
int numOfFiles = 0, n = 0, m = 0;

int  modeSwitcher(char, int, char**);
void helpMode();
void interactiveMode();
void staticMode(int, int, int, NamesList *);
bool isValidMode(string);
int  getFilePathsFromArgv(string[], int);
bool checkParameters();
int  generateNewControllerInstance();
void sendAllFiles();
int  processExit();
void waitAnalisysEnd();

// check if the mode is a two char string, with the
// first char being '-'
bool isValidMode(string mode){
    return (strlen(mode) == 2) && (mode[0] == '-');
}

// extract file paths from argv array. In case of folder,
// it uses the crawler to inspect inner files and folders.
// It returns the number of scanned files.
int getFilePathsFromArgv(string argv[], int numPaths){
    const int padding = 4;      // index inside argv from which filenames occur
    unsigned long numFiles = 0; // number of files recognized
    int out;
    filePaths = constructorNamesList();

    int i;
    for (i = 0; i < numPaths; i++){
        if (isDirectory(argv[i + padding], '/', &out) && out == 0){
            int outNewFiles = 0;
            crawler(argv[i + padding], filePaths, &outNewFiles);
            numFiles += outNewFiles;
        } else if ( isValidFile(argv[i + padding]) && out == 0 ){
            appendNameToNamesList(filePaths, argv[i + padding]);
            numFiles++;
        }
    }
    return (int)numFiles;
}

int main(int argc, char *argv[]){
    int returnCode = 0;
    filePaths = constructorNamesList();

    if (argc <= 1){
        fprintf(stderr, "?Error: specify a valid mode, n, m and at least one file/folder\n");
        returnCode = 1;
    } else if (isValidMode(argv[1])){
        modeSwitcher(argv[1][1], argc, argv);
    } else {
        fprintf(stderr, "?Error: specify a valid mode, n, m and at least one file/folder\n");
        returnCode = 1;
    }

    return returnCode;
}

// Switch mode of the analyzer (interactive, static, help)
// Error codes:
// 1 - not enough args for static mode
// 2 - n, m and files are not valid
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
            if (argc <= 4){
                fprintf(stderr, "?Error: specify a valid mode, n, m and at least one file/folder\n");
                returnCode = 1;
            } else {
                n = atoi(argv[2]);
                m = atoi(argv[3]);

                // Get file paths with the crawler
                numOfFiles = getFilePathsFromArgv(argv, argc - 4);

                if (!checkParameters()){
                    returnCode = 2;
                } else {
                    staticMode(n, m, numOfFiles, filePaths);
                }
            }
            break;
        default:
            fprintf(stderr, "Error: mode not supported\n");
            returnCode = 3;
    }

    return returnCode;
}

void helpMode(){
    string help_message = "Help mode\n\n"
                          "Usages:\n"
                          "-i: interactive mode\n"
                          "\tInteractive mode commands:\n\n"
                          "\t+_name_fi/fo_ to add a file/folder to the list\n"
                          "\t-_name_file_  to remove a file from the list\n"
                          "\tn=_value_     to set the value of n\n"
                          "\tm=_value_     to set the value of m\n"
                          "\tshow          to see the list of files at the current moment\n"
                          "\tanalyze       to start the analysis\n"
                          "\texit          to exit from the process\n\n"
                          "-s: static mode\n"
                          "\tmust insert arguments: n, m and at least one file/folder\n\n"
                          "-h: help mode\n\n"
                          "Error codes:\n"
                          "1: missing arguments\n"
                          "2: n and m are not numeric non-zero values\n"
                          "3: usage mode not supported\n";

    printf("%s\n", help_message);
}

// TODO - check all error codes from sys calls and from out functions
void interactiveMode(){
    int returnCode = generateNewControllerInstance();
    sendAllFiles();

    const char analyzeString[] = "analyze";
    const char exitString[] = "exit";

    printf("Interactive mode\n");

    char command[MINIQ_MAX_BUFFER_SIZE];
    char fileNameBuffer[MINIQ_MAX_BUFFER_SIZE];

    printf("> ");
    scanf("%s", command);
    while (strcmp(command, exitString) != 0){
        if (strcmp(command, analyzeString) == 0){
            // start analyzing process (if parameters are good)
            if (checkParameters()){
                printf("Start analysis\n");
                sendStartAnalysisPacket(cInstance->pipeAC);
            }

        } else if (command[0] == '+'){
            // Managment of addition of file or folder
            if (command + 1 != NULL){
                realpath(command + 1, fileNameBuffer);
            } else {
                fileNameBuffer[0] = '\0';
            }
            
            int numOfFilesInFolder; // used in case it's a folder
            int pathType = inspectPath(fileNameBuffer);

            if (pathType == 0){
                // it's an existing file
                appendNameToNamesList(filePaths, fileNameBuffer);
                sendAllFiles();
                printf("Added file %s\n", fileNameBuffer);
            } else if (pathType == 1){
                // it's an existing folder
                crawler(fileNameBuffer, filePaths, &numOfFilesInFolder);
                sendAllFiles();
                printf("Added folder %s\n", fileNameBuffer);
            } else {
                // invalid file/folder
                fprintf(stderr, "File/folder inserted doesn't exist!\n");
            }

        } else if (command[0] == '-'){
            // Management of removal of file or folder
            if (command + 1 != NULL){
                realpath(command + 1, fileNameBuffer);
            } else {
                fileNameBuffer[0] = '\0';
            }

            int numOfFilesInFolder; // used in case it's a folder
            int pathType = inspectPath(fileNameBuffer);

            if (pathType == 0){
                // it's an existing file
                if (removeFileByNamePacket(cInstance->pipeAC, fileNameBuffer) != -1){
                    printf("Remove file %s\n", fileNameBuffer);
                } else {
                    fprintf(stderr, "?Error trying to remove %s\n", fileNameBuffer);
                }                
            } else if (pathType == 1){
                // it's an existing folder
                // crawler(fileNameBuffer, filePaths, &numOfFilesInFolder);
                // sendAllFiles();
                printf("Not implemented yet\n");
                // printf("Removed folder %s\n", fileNameBuffer);
            } else {
                // invalid file/folder
                fprintf(stderr, "File/folder inserted to remove doesn't exist!\n");
            }

        // TODO per Sam: ATTENZIONE: stai supponendo che command sia di almeno tre caratteri!
        // rischiamo un out of bound! (sia per n che per m)
        } else if (command[0] == 'n'){
            // Update the value of N
            printf("Change n to %s\n", command + 2);
            n = atoi(command + 2);
            sendNewNPacket(cInstance->pipeAC, n);
            printf("Now n=%d\n", n);

        } else if (command[0] == 'm'){
            // Update the value of M
            printf("Change m to %s\n", command + 2);
            m = atoi(command + 2);
            sendNewMPacket(cInstance->pipeAC, m);
            printf("Now m=%d\n", m);

        } else {
            // Command not supported
            fprintf(stderr, "This command is not supported.\n");
        }

        // wait for next command
        printf("> ");
        scanf("%s", command);
    }
    // Management of exit command
    processExit();    
}

void staticMode(int numOfP, int numOfQ, int numOfFiles, NamesList *listFilePaths){
    int returnCode = generateNewControllerInstance();
    printf("Static mode\n");
    sendNewNPacket(cInstance->pipeAC, n);
    sendNewMPacket(cInstance->pipeAC, m);
    // Trick: to send all files in the list call sendNewFolder with oldNumberOfFiles=0
    sendAllFiles();
    sendStartAnalysisPacket(cInstance->pipeAC);    

    // TODO: what do we do when static analisys is started?
    // da FRA: io direi niente... è statica per qualche motivo
    // da Sam: e invece stampiamo l'avanzamento, infame! 
    waitAnalisysEnd();

    processExit();
}

// Returns true if n, m and at least one file/folder are set with valid values
bool checkParameters(){
    int returnValue = true;
    if (n <= 0 || m <= 0){
        fprintf(stderr, "Error: specify numeric non-zero positive values for n and m\n");
        returnValue = false;
    } else if(filePaths->counter == 0){
        fprintf(stderr, "Error: specify at least one file/folder\n");
        returnValue = false;
    }

    return returnValue;
}


// Generate an "empty" instance of controller, this method is to be used everytime.
int generateNewControllerInstance(){
    int returnCode = 0;
    // TODO: check for null return from malloc
    cInstance = (controllerInstance*) malloc(sizeof(cInstance));
    cInstance->pidAnalyzer = getpid();

    if (pipe(cInstance->pipeAC) != -1 && pipe(cInstance->pipeCA) != -1){
        // TODO: check for error -1 for fcntl
        // make the pipes non blocking
        fcntl(cInstance->pipeAC[READ], F_SETFL, O_NONBLOCK);
        fcntl(cInstance->pipeCA[READ], F_SETFL, O_NONBLOCK);

        cInstance->pid = fork();

        if (cInstance->pid < 0){
            fprintf(stderr, "Found an error creating the controllerInstance\n");
            returnCode = 2;
        } else if (cInstance->pid == 0){
            // child: new instance of Controller
            fprintf(stderr, "controllerInstance created\n");
            close(cInstance->pipeAC[WRITE]);
            close(cInstance->pipeCA[READ]);
            // while(true);
            controller(cInstance);
            exit(0);
        } else {
            // parent
            close(cInstance->pipeAC[READ]);
            close(cInstance->pipeCA[WRITE]);
        }
    } else {
        fprintf(stderr, "Found an error creting pipes to Controller\n");
        returnCode = 1;
    }

    return returnCode;
}


/**
 * Function called when the user adds a folder to the list of files.
 * It iterates trough the new files (last added in filePaths) and
 * sends each of them to the controller.
 */
void sendAllFiles(){
    int numberOFfiles = filePaths->counter;
    NodeName *file = filePaths->first;

    int i;
    for (i = 0; i < numberOFfiles; i++){
        if (sendNewFilePacket(cInstance->pipeAC, file->name) != 0){
            fprintf(stderr, "Could not send file %s from A to C\n", file->name);
        }
        file = file->next;
    }

    deleteNamesList(filePaths);
    filePaths = constructorNamesList();
}

/**
 * Notifies the controller that exit command has been pressed.
 * Provokes a waterfall effect tht kills every process.
 */
int processExit(){
    // Start the waterfall effect
    sendDeathPacket(cInstance->pipeAC);

    // free occupied memory:
    free(cInstance);
    deleteNamesList(filePaths);

    printf("Cleanup complete, see you next time!\n");
    exit(0); // to exit from infinite loop
}

// Function that animates the waiting for static analysis to end.
void waitAnalisysEnd(){
    int numBytesRead, dataSectionSize, offset;
    byte packetCode[1];
    byte packetData[INT_SIZE*2];

    while(true){
        numBytesRead = read(cInstance->pipeCA[READ], packetCode, 1);
        if(numBytesRead > 0){
            if(packetCode[0] == 16){
                break;
            } else if(packetCode[0] == 17){
                // nuovo file completato
                numBytesRead = read(cInstance->pipeCA[READ], packetData, 2*INT_SIZE);
                if(numBytesRead == 2*INT_SIZE){
                    int completed = fromBytesToInt(packetData + 0);
                    int total     = fromBytesToInt(packetData + INT_SIZE);
                    printf("Completed %d files over %d\n", completed, total);
                } else {
                    printf("Something has gone wrong with a completedFile packet!\n");
                }
            } else {
                printf("Analyzer recieved wrong packet code from controller!\n");
            }
        }
    }
    printf("\nAnalisys ended\n");
}