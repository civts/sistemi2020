#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include "../common/utils.h"
#include "../common/packets.h"
#include "crawler.c"
#include "parser.c"
#include "controller.c"
#include "instances.h"

#define BUFFER_SIZE 4096
char buf[BUFFER_SIZE], command[BUFFER_SIZE];

/**
 * Command:
 
 ../common/mymath.c ../common/packets.c ../common/datastructures/fileList.c ../common/datastructures/miniQlist.c ../common/datastructures/namesList.c ../common/utils.c -o main -lm
 
 */


// Used for printing purposes
string statuses[] = {"Still not started", "Analysis is running", "Analysis finished"};

analyzerInstance instanceOfMySelf;
controllerInstance *cInstance;
NamesList *filePaths;

void initialize();
void updateHistory(string);
void updateMessages(string);
void helpMode();
int  interactiveMode();
int  staticMode(NamesList*);
bool checkParameters();
int  generateNewControllerInstance();
void sendAllFiles();
int  processExit();
void waitAnalisysEnd();
int  inputReader();
int  switchCommand(int, int, string*);
void printState();
void addFiles(int, string*);
void removeFiles(int, string*);
void sig_handler_A();
void waitEnter();
void cleanArguments();
void staticAnalisysScreen();
int  waitForMessagesInAFromC();
void printMessages();


/**
 * Function that initializes all the resources of the Analyzer.
 * No returns.
 */
void initialize(){
    filePaths = constructorNamesList();
    instanceOfMySelf.completedFiles = 0;
    instanceOfMySelf.totalFiles = 0;
    instanceOfMySelf.statusAnalisys = 0;
    instanceOfMySelf.n = instanceOfMySelf.m = 0;
    instanceOfMySelf.hasMainOption = false;
    int i;
    for(i=0; i<HISTORY; i++){
        instanceOfMySelf.lastCommands[i] = (string)malloc(BUFFER_SIZE);
        strcpy(instanceOfMySelf.lastCommands[i], "No command yet");
    }
    for(i=0; i<MESSAGES; i++){
        instanceOfMySelf.lastMessages[i] = (string)malloc(BUFFER_SIZE);
        strcpy(instanceOfMySelf.lastMessages[i], "No message yet");
    }
    strcpy(instanceOfMySelf.mode, "Not set yet");
    signal(SIGINT, sig_handler_A);
    signal(SIGKILL, sig_handler_A);
}

/**
 * Takes newCommand and puts it first in the history vector, moves previous commands down.
 * No returns.
 */
void updateHistory(string newCommand){
    int i;
    for(i=HISTORY-1; i>0; i--){
        strcpy(instanceOfMySelf.lastCommands[i], instanceOfMySelf.lastCommands[i-1]);
    }
    strcpy(instanceOfMySelf.lastCommands[0], newCommand);
}

/**
 * Takes newMessage and puts it first in the history of messages vector, moves previous messages down.
 * No returns
 */
void updateMessages(string newMessage){
    int i;
    for(i=MESSAGES-1; i>0; i--){
        strcpy(instanceOfMySelf.lastMessages[i], instanceOfMySelf.lastMessages[i-1]);
    }
    strcpy(instanceOfMySelf.lastMessages[0], newMessage);
}

/**
 * Reads the first commands from the terminal, then switches to inputReader cycle.
 * Return values:
 * 9 -> all good
 * 1 -> error generating Controller
 * 2 -> errors with messages in pipes
 */ 
int main(int argc, char *argv[]){
    int returnCode = 0;
    initialize();

    cleanArguments();
    bool validCall = checkArguments(argc-1, argv+1, possibleFlags, flagsWithArgs, numberPossibleFlags, settedFlags, arguments, invalidPhrase, true);
    bool validArguments = checkArgumentsValidity(arguments);
    
    int i;
    if(validCall && argc > 1 && validArguments){
        // For the first call we check all flags (which means also "-main" flag)
        for(i=numberPossibleFlags; i>=0; i--){
            if(settedFlags[i]){
                // If command is set, retrieve its arguments and then switch it 
                char commandToPrint[BUFFER_SIZE];
                strcpy(commandToPrint, possibleFlags[i]);
                char *listOfArguments[BUFFER_SIZE];
                char stringWithArguments[BUFFER_SIZE];
                int  numArguments;
                if(flagsWithArgs[i]){
                    strcat(commandToPrint, " "); 
                    strcat(commandToPrint, arguments[i]); 
                    parser(arguments[i], &numArguments, listOfArguments);
                } else {
                    numArguments = 0;
                }
                updateHistory(commandToPrint);
                if(!instanceOfMySelf.hasMainOption ){
                    printf("Processing command: '%s' ", possibleFlags[i]);
                }
                int j;
                for(j=0; j<numArguments; j++){
                    if(!instanceOfMySelf.hasMainOption ){
                        printf("argument %d : '%s' ", j, listOfArguments[j]);
                    }
                }
                if(!instanceOfMySelf.hasMainOption ){
                    waitEnter();
                }
                settedFlags[i] = false;
                switchCommand(i, numArguments, listOfArguments);
            }
        }
    } else {
        char invalidCommand[BUFFER_SIZE];
        strcpy(invalidCommand,"Invalid commnad '");
        for(i=1; i<argc; i++){
            strcat(invalidCommand, " ");
            strcat(invalidCommand, argv[i]);
        }
        strcat(invalidCommand, " '");
        updateHistory(invalidCommand);
    }
    
    returnCode = generateNewControllerInstance();
    
    if(returnCode == 0){
        inputReader();
    }
    
    return returnCode;
}


/**
 * Prints the helpscreen.
 */
void helpMode(){
    clear();
    string help_message = "Help mode\n\n"
                          "Usages:\n"
                          "-i: interactive mode\n"
                          "-s: static mode\n"
                          "\tto start the analisys must insert arguments:\n"
                          "\tn, m and at least one file/folder\n"
                          "-h: help mode\n\n"
                          "-add _names_  to add a files/folders to the list\n"
                          "-rem _names_  to remove a files/folders from the list\n"
                          "-n   _value_  to set the value of n\n"
                          "-m   _value_  to set the value of m\n"
                          "-show         to see info of the current status of settings\n"
                          "-analyze      to start the analysis if in interactive mode\n"
                          "-quit: o quit from the process\n\n"
                        //   "Error codes:\n"
                        //   "1: missing arguments\n"
                        //   "2: n and m are not numeric non-zero values\n"
                        //   "3: usage mode not supported\n"
                          ;

    printf("%s\n", help_message);
    waitEnter();
}


/**
 * Sets mode to interactive, sends all present files to the controller, then returns to readInput cycle. 
 */
int interactiveMode(){
    int returnCode = 0;
    strcpy(instanceOfMySelf.mode, "Interactive");
    sendAllFiles();
    returnCode = inputReader();
    return returnCode;    
}

/**
 * Starts the analisys in static mode. 
 */
int staticMode(NamesList *listFilePaths){
    int returnCode = 0;
    strcpy(instanceOfMySelf.mode, "Static");
    instanceOfMySelf.statusAnalisys = 1;
    sendNewNPacket(cInstance->pipeAC, instanceOfMySelf.n);
    sendNewMPacket(cInstance->pipeAC, instanceOfMySelf.m);
    sendAllFiles();
    
    pid_t myPid = getpid();
    sendStartAnalysisPacket(cInstance->pipeAC, myPid);    

    waitAnalisysEnd();

    returnCode = processExit();

    return returnCode;
}

/**
 * Returns true if n, m and at least one file/folder are set with valid values
 */
bool checkParameters(){
    int returnValue = true;
    if (instanceOfMySelf.n <= 0){
        char mess1[BUFFER_SIZE] = "\nError: specify numeric non-zero positive values for n\n";
        if(!instanceOfMySelf.hasMainOption){
            fprintf(stderr, mess1);
            waitEnter();
        } else {
            sendTextMessageToReport(cInstance->pipeAC, mess1);
        }
        returnValue = false;
    }
    if(instanceOfMySelf.m <=0 ) {
        char mess2[BUFFER_SIZE] = "\nError: specify numeric non-zero positive values for m\n";
        if(!instanceOfMySelf.hasMainOption){
            fprintf(stderr, mess2);
            waitEnter();
        } else {
            sendTextMessageToReport(cInstance->pipeAC, mess2);
        }
        returnValue = false;
    }
    if(instanceOfMySelf.totalFiles == 0){
        char mess2[BUFFER_SIZE] = "\nError: Cannot start analisys without a file or folder\n";
        if(!instanceOfMySelf.hasMainOption){
            fprintf(stderr, mess2);
            waitEnter();
        } else {
            sendTextMessageToReport(cInstance->pipeAC, mess2);
        }
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
    
    if(instanceOfMySelf.hasMainOption){
        cInstance->hasMainOption = true;
    } else {
        cInstance->hasMainOption = false;
    }

    if (pipe(cInstance->pipeAC) != -1 && pipe(cInstance->pipeCA) != -1){
        // TODO: check for error -1 for fcntl
        // make the pipes non blocking
        fcntl(cInstance->pipeAC[READ], F_SETFL, O_NONBLOCK);
        fcntl(cInstance->pipeCA[READ], F_SETFL, O_NONBLOCK);

        controllerInstance newInstance = *cInstance;

        cInstance->pid = fork();

        if (cInstance->pid < 0){
            if(!instanceOfMySelf.hasMainOption){
                fprintf(stderr, "Found an error creating the controllerInstance\n");
            }
            returnCode = 1;
        } else if (cInstance->pid == 0){
            // child: new instance of Controller
            if(!instanceOfMySelf.hasMainOption){
                fprintf(stderr, "controllerInstance created\n");
            }
            close(newInstance.pipeAC[WRITE]);
            close(newInstance.pipeCA[READ]);
            // while(true);
            controller(&newInstance);
            exit(0);
        } else {
            // parent
            close(cInstance->pipeAC[READ]);
            close(cInstance->pipeCA[WRITE]);
        }
    } else {
        if(!instanceOfMySelf.hasMainOption){
            fprintf(stderr, "Found an error creting pipes to Controller\n");
        }
        returnCode = 1;
    }

    return returnCode;
}


/**
 * Function called when the user adds a folder to the list of files.
 * It iterates trough the new files (last added in filePaths) and
 * sends each of them to the controller.
 * At the end deletes the filePaths list and creates it from zero as empty list.
 */
void sendAllFiles(){
    int numberOFfiles = filePaths->counter;
    NodeName *file = filePaths->first;

    int i;

    for (i = 0; i < numberOFfiles; i++){
        if (sendNewFilePacket(cInstance->pipeAC, file->name) != 0){
            if(!instanceOfMySelf.hasMainOption){
                fprintf(stderr, "Could not send file %s from A to C\n", file->name);
                waitEnter();
            }
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
    kill(cInstance->pid, SIGKILL);
    // free occupied memory:
    free(cInstance);
    deleteNamesList(filePaths);

    if(!instanceOfMySelf.hasMainOption){
        clear();
        printf("Cleanup complete, see you next time!\n");
    }
    sleep(1);
    return 9; // to exit from infinite loop
}

/**
 * Function that animates the waiting for static analysis to end.
 */
void waitAnalisysEnd(){
    int status = 3;
    while(status!=0){
        status = waitForMessagesInAFromC();
    }
    // TODO declare analisys finished
    if(!instanceOfMySelf.hasMainOption){
        printf("Analisys finished!\n");
        waitEnter();
    }
}


/**
 * Function to print the header of the termios screen.
 */
void printScreen() {
    printf("================================================\n");
    printf("Analisys mode: %s\n", instanceOfMySelf.mode);
    printf("Analisys status: %s\n", statuses[instanceOfMySelf.statusAnalisys]);
    printf("===================Processing===================\n");
    if(instanceOfMySelf.statusAnalisys == 1){
        printf("Completed files: %2d over %2d\n", instanceOfMySelf.completedFiles, instanceOfMySelf.totalFiles);
        printf("================================================\n");
        printf("============Messages from controller============\n");
        int i;
        for(i=0; i<MESSAGES; i++){
            printf("Previous message -%d: %s\n", i, instanceOfMySelf.lastMessages[i]);
        }
        printf("================================================\n");
    }
    int i;
    for(i=0; i<HISTORY; i++){
        printf("Previous command -%d: %s\n", i, instanceOfMySelf.lastCommands[i]);
    }
    printf("================================================\n");
    printf("> %s", buf);
    fflush(stdout);
}


// Reset input buffer
void resetBuffer(char buffer[], int size){
    int i;
    for (i = 0; i < size; i++){
        buffer[i] = '\0';
    }
}

/**
 * Termios input and command parser.
 */
int inputReader(){
    int returnCode = 0;
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


                char *listOfCommands[BUFFER_SIZE];
                int  numCommands=0;
                parser(command, &numCommands, listOfCommands);
                int i;
                cleanArguments();
                // Obtain arguments if valid
                bool validCall = checkArguments(numCommands, listOfCommands, possibleFlags, flagsWithArgs, numberPossibleFlags, settedFlags, arguments, invalidPhrase, true);
                // Check contstraints on arguments
                bool validArguments = checkArgumentsValidity(arguments);

                if(validCall && numCommands > 0 && validArguments){
                    for(i=numberPossibleFlags-1; i>=0; i--){
                        // If command is set, then execute it
                        if(settedFlags[i]){
                            settedFlags[i] = false; 
                            char commandToPrint[BUFFER_SIZE];
                            strcpy(commandToPrint, possibleFlags[i]);
                            int numArguments = 0;
                            string listOfArguments[BUFFER_SIZE];
                            if(flagsWithArgs[i]){
                                if(arguments[i]!=NULL){
                                    strcat(commandToPrint, " ");
                                    strcat(commandToPrint, arguments[i]); 
                                }
                                parser(arguments[i], &numArguments, listOfArguments);
                            }
                            if(!instanceOfMySelf.hasMainOption){
                                printf("Processing command: '%s' ", possibleFlags[i]);
                            }
                            int j;
                            if(!instanceOfMySelf.hasMainOption){
                                for(j=0; j<numArguments; j++){
                                    printf("argument %d : '%s' ", j, listOfArguments[j]);
                                }
                                waitEnter();
                            }
                            updateHistory(commandToPrint);
                            returnCode = switchCommand(i, numArguments, listOfArguments);
                        }
                    }
                } else {
                    // If command invalid, copy it on history and do nothing else
                    char invalidCommand[BUFFER_SIZE];
                    strcpy(invalidCommand,"invalid command '");
                    strcat(invalidCommand, command);
                    strcat(invalidCommand, "'");
                    
                    updateHistory(invalidCommand);
                    
                    if(!instanceOfMySelf.hasMainOption){
                        waitEnter();
                    }
                }
            } else {
                // wait for user to end command
            }
        }
        
        // One round of checking for the messages
        waitForMessagesInAFromC();

        if(!instanceOfMySelf.hasMainOption){
            clear();
            printScreen();
        }
        
        if(returnCode == 9){
                break;
        }
    }
    resetBuffer(buf, BUFFER_SIZE);
    resetBuffer(command, BUFFER_SIZE);                

    // restore the old settings
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);


    return returnCode;
}

/**
 * Function to switch the command code and execute it.
 */
int switchCommand(int commandCode, int numArgs, string *arguments){
    int returnCode = 0;
    switch(commandCode){
        case flag_analyze:
            if(checkParameters()){
                if(strcmp(instanceOfMySelf.mode, "Interactive") != 0){
                    if(!instanceOfMySelf.hasMainOption){
                        printf("\nYou are not in interactive mode!");
                        waitEnter();
                    } else {
                        char message[BUFFER_SIZE] = "\nYou are not in interactive mode!";
                        sendTextMessageToReport(cInstance->pipeAC, message);
                    }
                } else {
                    instanceOfMySelf.statusAnalisys = 1;
                    pid_t myPid = getpid();
                    sendStartAnalysisPacket(cInstance->pipeAC, myPid);
                }
            }
            break;
        case flag_i:
            returnCode = interactiveMode();
            break;
        case flag_s:
            if(checkParameters()){
                returnCode = staticMode(filePaths);
            }
            break;
        case flag_h:
            if(!instanceOfMySelf.hasMainOption ){
                helpMode();
            }
            break;
        case flag_show:
            if(!instanceOfMySelf.hasMainOption){
                printState();
            }
            break;
        case flag_rem:
            removeFiles(numArgs, arguments);
            break;
        case flag_add:
            addFiles(numArgs, arguments);
            break;
        case flag_n:
            instanceOfMySelf.n = atoi(arguments[0]);
            break;
        case flag_m:
            instanceOfMySelf.m = atoi(arguments[0]);
            break;
        case flag_quit:
            returnCode = processExit();
            break;
        case flag_main:
            instanceOfMySelf.hasMainOption = true;
            break;
    }
    return returnCode;
}

/**
 * Function that prints n, m and the number of files added 'till now.
 */
void printState(){
    clear();
    printf("number of files wrote = %d\n\n", instanceOfMySelf.totalFiles);
    printf("value of n = %d\n\n", instanceOfMySelf.n);
    printf("value of m = %d\n\n", instanceOfMySelf.m);
    waitEnter();
}

/**
 * Function that adds all files/folders in fileNames to the filePaths list.
 * fileNames can contain both a file name or a folder, in the second case
 * all file of the folder will be added to filePaths.
 * The files are sent to the controller at the end of the function.
 */
void addFiles(int numFiles, string *fileNames){
    int i;
    for(i=0; i<numFiles; i++){
        int numOfFilesInFolder = 0; // used in case it's a folder
        int pathType = inspectPath(fileNames[i]);
        if (pathType == 0){
            // it's an existing file
            char *absolutePath;
            absolutePath = realpath(fileNames[i], absolutePath);
            if(absolutePath != NULL){
                if(!instanceOfMySelf.hasMainOption){
                    printf("Adding file %s\n", absolutePath);
                }
                int appended = appendNameToNamesList(filePaths, absolutePath);
                if(appended == 0){
                    char message[BUFFER_SIZE] = "File ";
                    string copy;
                    strcpy(copy, absolutePath);
                    trimStringToLength(copy, 50);
                    strcat(message, absolutePath);
                    strcat(message, " added\n");
                    if(!instanceOfMySelf.hasMainOption){
                        printf("%s", message);
                    } else {
                        sendTextMessageToReport(cInstance->pipeAC, message);
                    }
                }
            } else {
                if(!instanceOfMySelf.hasMainOption){
                    printf("Well, this is embarrassing... it seems we had problems checking file %s\n", fileNames[i]);
                }
            }
        } else if (pathType == 1){
            // it's an existing folder
            if(!instanceOfMySelf.hasMainOption){
                printf("Adding folder %s\n", fileNames[i]);
            }
            int check = crawler(fileNames[i], filePaths, &numOfFilesInFolder);
            if(check!=0){
                if(!instanceOfMySelf.hasMainOption){
                    printf("Well, this is embarrassing... it seems we had problems checking folder %s\n", fileNames[i]);
                }
            } else {
                char message[BUFFER_SIZE] = "Folder ";
                string copy;
                strcpy(copy, fileNames[i]);
                trimStringToLength(copy, 50);
                strcat(message, copy);
                strcat(message, "  added\n");
                if(!instanceOfMySelf.hasMainOption){
                    printf("%s", message);
                } else {
                    sendTextMessageToReport(cInstance->pipeAC, message);
                }
            }
        } else {
            // invalid file/folder
            char message[BUFFER_SIZE] = "File/folder ";
            string copy;
            strcpy(copy, fileNames[i]);
            trimStringToLength(copy, 50);
            strcat(message, "  doesn't exist!\n");
            if(!instanceOfMySelf.hasMainOption){
                fprintf(stderr, "%s", message);
            } else {
                sendTextMessageToReport(cInstance->pipeAC, message);
            }   
        }
        
        sendAllFiles();
    }
}

/**
 * Function that removes files/folders contained in fileNames.
 */
void removeFiles(int numFiles, string *fileNames){
    int i;
    for(i=0; i<numFiles; i++){
        // Management of removal of file or folder
        char *absolutePath;
        absolutePath = realpath(fileNames[i], absolutePath);

        if(absolutePath != NULL){
            int numOfFilesInFolder; // used in case it's a folder
            int pathType = inspectPath(absolutePath);

            if (pathType == 0){
                // it's an existing file
                if (removeFileByNamePacket(cInstance->pipeAC, absolutePath) != -1){
                    char message[BUFFER_SIZE] = "Removing file ";
                    string copy;
                    strcpy(copy, absolutePath);
                    trimStringToLength(copy, 50);
                    strcat(message, copy);
                    strcat(message, "\n");
                    if(!instanceOfMySelf.hasMainOption){
                        printf("%s", message);
                    } else {
                        sendTextMessageToReport(cInstance->pipeAC, message);
                    }
                } else {
                    char message[BUFFER_SIZE] = "Error trying to remove ";
                    string copy;
                    strcpy(copy, absolutePath);
                    trimStringToLength(copy, 50);
                    strcat(message, copy);
                    strcat(message, "\n");
                    if(!instanceOfMySelf.hasMainOption){
                        fprintf(stderr, "%s", message);
                    }
                }                
            } else if (pathType == 1){
                // it's an existing folder
                if (removeFileByNamePacket(cInstance->pipeAC, absolutePath) != -1){
                    char message[BUFFER_SIZE] = "Removing folder ";
                    string copy;
                    strcpy(copy, absolutePath);
                    trimStringToLength(copy, 50);
                    strcat(message, copy);
                    strcat(message, "\n");
                    if(!instanceOfMySelf.hasMainOption){
                        printf("%s", message);
                    } else {
                        sendTextMessageToReport(cInstance->pipeAC, message);
                    }
                }
            } else {
                // invalid file/folder
                if(!instanceOfMySelf.hasMainOption){
                    fprintf(stderr, "File/folder inserted to remove doesn't exist!\n");
                } else {
                    char message[BUFFER_SIZE] = "File/folder ";
                    string copy;
                    strcpy(copy, absolutePath);
                    trimStringToLength(copy, 50);
                    strcat(message, copy);
                    strcat(message,"inserted to remove doesn't exist!\n");
                    sendTextMessageToReport(cInstance->pipeAC, message);
                }
            }
        }
    }
}

/**
 * Updates numbers of completed and total files;
 */
void updateCompleted(int completed, int total){
    instanceOfMySelf.completedFiles = completed;
    instanceOfMySelf.totalFiles = total;
    if(!instanceOfMySelf.hasMainOption){
        clear();
        printScreen();
    }
}

/**
 * Handler for kill and interrupt signals.
 */
void sig_handler_A(){
    processExit();
}

/**
 * Checks if arguments for commands n and m are numbers.
 */
bool checkArgumentsValidity(char **arguments){
    int j;
    bool ret = true;
    if(settedFlags[flag_n]){
        if(arguments[flag_n]!=NULL){
            for(j=0; j<strlen(arguments[flag_n]); j++){
                if(!isdigit(arguments[flag_n][j])){
                    // l'argomento non contiene solo cifre
                    ret=false;
                }
            }
        } else {
            // flag settato senza argomenti
            ret = false;
        }
    }
    if(settedFlags[flag_m]){
        if(arguments[flag_m]!=NULL){
            for(j=0; j<strlen(arguments[flag_m]); j++){
                if(!isdigit(arguments[flag_m][j])){
                    // l'argomento non contiene solo cifre
                    ret=false;
                }
            }
        } else {
            // flag settato senza argomenti
            ret = false;
        }
    }
    return ret;
}

/**
 * Waits for user to press enter.
 */
void waitEnter(){
    printf("\npress enter to continue...");
    while(getchar()!='\n');
}

/**
 * Function to clan variables arguments and settedFlags.
 */
void cleanArguments(){
    int i;
    for(i=0; i<numberPossibleFlags; i++){
        free(arguments[i]);
        settedFlags[i] = false;
        arguments[i] = NULL;
    }
}

/**
 * Function that prints the current state for a static analisys.
 */
void staticAnalisysScreen(){
    clear();
    printf("========================Static analisys running========================\n");
    printf("Files completed: %d\n", instanceOfMySelf.completedFiles);
    printf("Total files: %d\n", instanceOfMySelf.totalFiles);
    printMessages();
}

void printMessages(){
    printf("========================Messages from controller=======================\n");
    int i;
    for(i=0; i<MESSAGES; i++){
        printf("Previous message -%d: %s\n", i, instanceOfMySelf.lastMessages[i]);
    }
}

/**
 * Returns  0 if analisys ended
 *          1 if message reciieved succesfully
 *          2 if something went wrong with the message
 */
int waitForMessagesInAFromC(){
    int numBytesRead, dataSectionSize, offset, ret;
    byte packetHeader[1 + INT_SIZE];
    numBytesRead = read(cInstance->pipeCA[READ], packetHeader, 1 + INT_SIZE);
    if(numBytesRead > 0){
        if(packetHeader[0] == 16){
            ret = 0;
        } else if(packetHeader[0] == 17){
            // nuovo file completato
            int messageSize = fromBytesToInt(packetHeader+1); 
            byte packetData[messageSize];
            numBytesRead = read(cInstance->pipeCA[READ], packetData, messageSize);
            if(numBytesRead == 2*INT_SIZE){
                int completed  = fromBytesToInt(packetData );
                int total      = fromBytesToInt(packetData + INT_SIZE);
                instanceOfMySelf.totalFiles = total;
                instanceOfMySelf.completedFiles = completed;
                if(!instanceOfMySelf.hasMainOption){
                    staticAnalisysScreen();
                }
                ret = 1;
            } else {
                if(!instanceOfMySelf.hasMainOption){
                    printf("Something has gone wrong with a completedFile packet!\n");
                }
                ret = 2;
            }
        } else if(packetHeader[0] == 18) {
            int messageSize = fromBytesToInt(packetHeader+1); 
            byte packetData[messageSize];
            int bytesRead = read(cInstance->pipeCA[READ], packetData, messageSize);
            if(bytesRead == messageSize){
                // TODO check if read has read all the bytes? (chek for non atomicity);
                char message[messageSize+1];
                strcpy(message, packetData);
                message[messageSize] = '\0';
                updateMessages(message);
                if(!instanceOfMySelf.hasMainOption){
                    staticAnalisysScreen();
                }
                ret = 1;
            } else {
                ret = 2;
            }
        } else {
            if(!instanceOfMySelf.hasMainOption){
                printf("Analyzer recieved wrong packet code from controller! %c\n", packetHeader[0]);
            }
            ret = 2;
        }
    }   
    return ret;
}
