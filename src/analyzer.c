#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include "utils.c"
#include "crawler.c"
#include "packets.h"
#include "controller.c"
#include "parser.c"

#define BUFFER_SIZE 4096
char buf[BUFFER_SIZE], command[BUFFER_SIZE];


/**
 * TODO: Insert possibility to remove an entire folder? (un casin)
 * NOTE: processExit() è dichiarata come funzione (invece che essere scritta all'interno del codice
 *       perché dovrà essere richiamata anche alla ricezione di SIGKILL)
 * TODO: Decidere cosa fare mentre l'analisi statica si sta completando (una mezza soluzione è
 *       già implementata, dai un'occhiata)
 */

// da spostare in utils?
int numberPossibleFlags = 10; 
string possibleFlags[]  = {"-analyze", "-i",  "-s", "-h",  "-show", "-rem", "-add", "-n", "-m", "-quit"};
bool   flagsWithArgs[]  = {false,      false, true, false, false,   true,   true,   true,  true, false};
bool   settedFlags[]    = {false, false, false, false, false, false, false, false, false, false};
string arguments[10];
string invalidPhrase    = "Wrong command syntax\n"; 

string statuses[] = {"Still not started", "Analisys is running", "Analysis finished"};

analyzerInstance instanceOfMySelf;
controllerInstance *cInstance;
NamesList *filePaths;

void initialize();
void helpMode();
void interactiveMode();
void staticMode(NamesList*);
bool isValidMode(string);
int  getFilePathsFromArgv(string[], int);
bool checkParameters();
int  generateNewControllerInstance();
void sendAllFiles();
int  processExit();
void waitAnalisysEnd();
int  inputReader();
void switchCommand(int, int, string*);
void printState();
void addFiles(int, string*);
void removeFiles(int, string*);
void clear();

// check if the mode is a two char string, with the
// first char being '-'
bool isValidMode(string mode){
    return (strlen(mode) == 2) && (mode[0] == '-');
}

void initialize(){
    filePaths = constructorNamesList();
    instanceOfMySelf.completedFiles = 0;
    instanceOfMySelf.totalFiles = 0;
    instanceOfMySelf.statusAnalisys = 0;
    instanceOfMySelf.n = instanceOfMySelf.m = 0;
    strcpy(instanceOfMySelf.lastCommand, "No commands yet");
    int returnCode = generateNewControllerInstance();
}

// extract file paths from argv array. In case of folder,
// it uses the crawler to inspect inner files and folders.
// It returns the number of scanned files.
int getFilePathsFromArgv(string argv[], int numPaths){
    const int padding = 2;      // index inside argv from which filenames occur
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
    initialize();

    bool validCall = checkArguments(argc-1, argv+1, possibleFlags, flagsWithArgs, numberPossibleFlags, settedFlags, arguments, invalidPhrase, true);
    int i;
    if(validCall && argc > 1){
        for(i=numberPossibleFlags-1; i>=0; i--){
            if(settedFlags[i]){
                strcpy(instanceOfMySelf.lastCommand, possibleFlags[i]);
                int numArgs;
                string *argumentList;
                if(flagsWithArgs[i]){
                    strcat(instanceOfMySelf.lastCommand, " "); 
                    strcat(instanceOfMySelf.lastCommand, arguments[i]); 
                    argumentList = getArgumentsList(arguments[i], &numArgs, argumentList);
                } else {
                    numArgs = 0;
                    argumentList = NULL;
                }
                settedFlags[i] = false;
                switchCommand(i, numArgs, argumentList);
            }
        } 

        for(i=0; i<numberPossibleFlags; i++){
            free(arguments[i]);
            arguments[i] = NULL;
        }
        // printf("num args: %d\n", numArgs);
        // for(i=0; i<numArgs; i++){
        //     printf("argument #%d: %s\n", i, argumentList[i]);
        // }
        inputReader();
    } else {
        char invalidCommand[BUFFER_SIZE];
        strcpy(invalidCommand,"Invalid commnad '");
        for(i=1; i<argc; i++){
            strcat(invalidCommand, argv[i]);
            strcat(invalidCommand, " ");
        }
        strcat(invalidCommand, "'");
        strcpy(instanceOfMySelf.lastCommand, invalidCommand);
        inputReader(instanceOfMySelf);
        returnCode = 1;
    }

    return returnCode;
}

void helpMode(){
    clear();
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
    printf("\npress enter to continue...");
    while(getchar()!='\n');
    inputReader();
}

// TODO - check all error codes from sys calls and from out functions
void interactiveMode(){
    sendAllFiles();
    inputReader();    
}

void staticMode(NamesList *listFilePaths){
    int returnCode = generateNewControllerInstance();
    printf("Static mode\n");
    sendNewNPacket(cInstance->pipeAC, instanceOfMySelf.n);
    sendNewMPacket(cInstance->pipeAC, instanceOfMySelf.m);
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
    if (instanceOfMySelf.n <= 0){
        fprintf(stderr, "Error: specify numeric non-zero positive values for n\n");
        returnValue = false;
        printf("\npress enter to continue...");
        while(getchar()!='\n');
    } else if(instanceOfMySelf.m <=0 ) {
        fprintf(stderr, "Error: specify numeric non-zero positive values for m\n");
        returnValue = false;
        printf("\npress enter to continue...");
        while(getchar()!='\n');
    } else if (instanceOfMySelf.totalFiles == 0){
        fprintf(stderr, "Error: all the files or folders specified are inexistent\n");
        returnValue = false;
        printf("\npress enter to continue...");
        while(getchar()!='\n');
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
            printf("\npress enter to continue...");
            while(getchar()!='\n');
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

    clear();
    printf("Cleanup complete, see you next time!\n");
    sleep(3);
    exit(0); // to exit from infinite loop
}

// Function that animates the waiting for static analysis to end.
/**
 * TODO: rewrite package reading
 */
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


void printScreen() {
    printf("================================================\n");
    printf("Analisys status: %s\n", statuses[instanceOfMySelf.statusAnalisys]);
    printf("===================Processing===================\n");
    printf("Last command: %s\n", instanceOfMySelf.lastCommand);
    printf("================================================\n");
    printf("> %s", buf);
    fflush(stdout);
}

void clear(){
    system("clear");
    // printf("\e[1;1H\e[2J");
}

void resetBuffer(char buffer[], int size){
    int i;
    for (i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = 0;
    }
}

int inputReader(){
    int lenBuffer = 0, numReadCharacters = 0;
    char *endCommandPosition;
    resetBuffer(buf, BUFFER_SIZE);
    resetBuffer(command, BUFFER_SIZE);

    static struct termios oldt, newt;

    /*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
    tcgetattr( STDIN_FILENO, &oldt);
    /*now the settings will be copied*/
    newt = oldt;

    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    newt.c_lflag &= ~(ICANON);
    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 1;

    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /*This is your part:
    Notice that EOF is also turned off
    in the non-canonical mode*/
    while(1){
        if ((numReadCharacters = read(0, buf + lenBuffer, BUFFER_SIZE - lenBuffer)) > 0){
            lenBuffer += numReadCharacters;
            endCommandPosition = strrchr(buf, '\n');

            if (endCommandPosition != NULL){
                int commandLength = endCommandPosition - buf + 1;
                memcpy(command, buf, commandLength);
                command[commandLength - 1] = '\0';
                resetBuffer(buf, BUFFER_SIZE);
                lenBuffer = 0;

                
                // make switch on command
                string *argsList;
                int numCommands;
                argsList = getArgumentsList(command, &numCommands, argsList);
                bool validCall = checkArguments(numCommands, argsList, possibleFlags, flagsWithArgs, numberPossibleFlags, settedFlags, arguments, invalidPhrase, true);
                // -s richiamata qui può essere invocata senza parametri
                if(strcmp(argsList[0], "-s") == 0){
                    validCall = true;
                    settedFlags[2] = true;
                    printf("special case\n");
                    sleep(2);
                }   
                // lista con comandi e relativi argomenti
                int i;
                if(validCall && numCommands > 0){
                    for(i=numberPossibleFlags-1; i>=0; i--){
                        if(settedFlags[i]){
                            printf("Comando %s, flag settato a true\n", possibleFlags[i]);
                            sleep(1);
                            strcpy(instanceOfMySelf.lastCommand, possibleFlags[i]);
                            int numArgs;
                            string *argumentList;
                            if(flagsWithArgs[i]){
                                if(arguments[i]!=NULL){
                                    strcat(instanceOfMySelf.lastCommand, " ");
                                    strcat(instanceOfMySelf.lastCommand, arguments[i]); 
                                } 
                                argumentList = getArgumentsList(arguments[i], &numArgs, argumentList);
                            } else {
                                numArgs = 0;
                                argumentList = NULL;
                            }
                            switchCommand(i, numArgs, argumentList);
                            settedFlags[i] = false; 
                            free(argumentList);
                        }
                    }
                } else {
                    char invalidCommand[BUFFER_SIZE];
                    strcpy(invalidCommand,"Invalid commnad '");
                    strcat(invalidCommand, command);
                    strcat(invalidCommand, "'");
                    strcpy(instanceOfMySelf.lastCommand, invalidCommand);
                }
                free(argsList);
                for(i=0; i<numberPossibleFlags; i++){
                    free(arguments[i]);
                    arguments[i] = NULL;
                }
            } else {
                
            }
        }
        //lettura C->A
        clear();
        printScreen();
        // sleep(1);
    }                

    // restore the old settings
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);


    return 0;
}

void switchCommand(int commandCode, int numArgs, string *arguments){
    printf("switching command %d with %d arguments\n", commandCode, numArgs);
    sleep(2);
    switch(commandCode){
        case 0:
            if(checkParameters()){
                sendStartAnalysisPacket(cInstance->pipeAC);
                instanceOfMySelf.statusAnalisys = 1;
            }
            break;
        case 1:
            interactiveMode();
            break;
        case 2:
            if(checkParameters()){
                staticMode(filePaths);
                instanceOfMySelf.statusAnalisys = 1;
            }
            break;
        case 3:
            helpMode();
            break;
        case 4:
            printState();
            break;
        case 5:
            removeFiles(numArgs, arguments);
            break;
        case 6:
            addFiles(numArgs, arguments);
            break;
        case 7:
            instanceOfMySelf.n = atoi(arguments[0]);
            break;
        case 8:
            instanceOfMySelf.m = atoi(arguments[0]);
            break;
        case 9:
            processExit();
            break;
    }
    return;
}

void printState(){
    printf("number of files = %d\n", instanceOfMySelf.totalFiles);
    printf("n = %d\n", instanceOfMySelf.n);
    printf("m = %d\n", instanceOfMySelf.m);
    printf("\npress enter to continue...");
    while(getchar()!='\n');
}

void addFiles(int numFiles, string *fileNames){
    int i;
    for(i=0; i<numFiles; i++){
        int numOfFilesInFolder; // used in case it's a folder
        int pathType = inspectPath(fileNames[i]);
        if (pathType == 0){
            // it's an existing file
            strcpy( absolutePath, realpath(fileNames[i], absolutePath) );
            int appended = appendNameToNamesList(filePaths, absolutePath);
            if(appended == 0){
                instanceOfMySelf.totalFiles+=1;
            }
            sendAllFiles();
            printf("Added file %s\n", fileNames[i]);
        } else if (pathType == 1){
            // it's an existing folder
            crawler(fileNames[i], filePaths, &numOfFilesInFolder);
            instanceOfMySelf.totalFiles+=numOfFilesInFolder;
            sendAllFiles();
            printf("Added folder %s\n", fileNames[i]);
        } else {
            // invalid file/folder
            fprintf(stderr, "File/folder %s doesn't exist!\n", fileNames[i]);
        }
    }
}

void removeFiles(int numFiles, string *fileNames){
    int i;
    for(i=0; i<numFiles; i++){
        // Management of removal of file or folder
        strcpy( absolutePath, realpath(fileNames[i], absolutePath) );

        int numOfFilesInFolder; // used in case it's a folder
        int pathType = inspectPath(absolutePath);

        if (pathType == 0){
            // it's an existing file
            if (removeFileByNamePacket(cInstance->pipeAC, absolutePath) != -1){
                printf("Remove file %s\n", absolutePath);
                instanceOfMySelf.totalFiles-=1;
            } else {
                fprintf(stderr, "?Error trying to remove %s\n", absolutePath);
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
    }
}