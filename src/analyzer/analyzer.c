#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>
#include <fcntl.h>
#include  <ctype.h>
#include "../common/utils.h"
#include "../common/parser.h"
#include "../common/packets.h"
#include "crawler.h"
#include "controller.h"
#include "instances.h"

#define BUFFER_SIZE 4096
char buf[BUFFER_SIZE], command[BUFFER_SIZE];

/**
 * Command:
 
 gcc -Wall -std=gnu90 analyzer.c ../common/parser.c ../common/mymath.c ../common/packets.c ../common/datastructures/fileList.c ../common/datastructures/miniQlist.c ../common/datastructures/namesList.c ../common/utils.c q.c p.c crawler.c miniQ.c controller.c  -o main -lm
 
 */

/**** Globals ****/
analyzerInstance instanceOfMySelf;
controllerInstance *cInstance;
NamesList *filePaths;

static struct termios oldt, newt;

// Used for printing purposes
string statuses[] = {"Still not started", "Analysis is running", "Analysis finished"}; 

/*** Parameter for parser ****/
string argumentsAnalyzer[10];
int    numberPossibleFlagsAnalyzer = 10; 
string invalidPhraseAnalyzer    = "Wrong command syntax, try command '-h' for help.\n";
string possibleFlagsAnalyzer[]  = {"-analyze", "-i",  "-s", "-h",  "-show", "-rem", "-add", "-n", "-m", "-quit", "-main"};
bool   flagsWithArgsAnalyzer[]  = {false,      false, false, false, false,   true,   true,   true,  true, false,  false};
bool   settedFlagsAnalyzer[]    = {false,      false, false, false, false,   false,  false,  false, false,false,  false};


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
bool checkArgumentsValidity(char **arguments);


/**
 * Function that initializes all the resources of the Analyzer.
 * No returns.
 */
void initialize(){
    filePaths = constructorNamesList();
    instanceOfMySelf.completedFiles = 0;
    instanceOfMySelf.totalFiles = 0;
    instanceOfMySelf.statusAnalysis = 0;
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
    signal(SIGTERM, sig_handler_A);
    signal(SIGQUIT, sig_handler_A);
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

    /*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
    tcgetattr( STDIN_FILENO, &oldt);
    /*now the settings will be copied*/
    newt = oldt;

    cleanArguments();
    bool validCall = checkArguments(argc-1, argv+1, possibleFlagsAnalyzer, flagsWithArgsAnalyzer, numberPossibleFlagsAnalyzer +1, settedFlagsAnalyzer, argumentsAnalyzer, invalidPhraseAnalyzer, true);
    bool validArguments = checkArgumentsValidity(argumentsAnalyzer);
    bool modeSet = false;
    returnCode = generateNewControllerInstance();

    int i;      
    if(returnCode == 0){
        if (validCall && argc > 1 && validArguments){
            // For the first call we check all flags (which means also "-main" flag)
            for (i = numberPossibleFlagsAnalyzer - 1; i>=0; i--){
                if (settedFlagsAnalyzer[i]){
                    // If command is set, retrieve its arguments and then switch it 
                    char commandToPrint[BUFFER_SIZE];
                    strcpy(commandToPrint, possibleFlagsAnalyzer[i]);
                    char *listOfArguments[BUFFER_SIZE];
                    // char stringWithArguments[BUFFER_SIZE];
                    int  numArguments;
                    if (flagsWithArgsAnalyzer[i]){
                        strcat(commandToPrint, " "); 
                        strcat(commandToPrint, argumentsAnalyzer[i]); 
                        parser(argumentsAnalyzer[i], &numArguments, listOfArguments);
                    } else {
                        numArguments = 0;
                    }
                    updateHistory(commandToPrint);
                    if (!instanceOfMySelf.hasMainOption ){
                        // printf("Processing command: '%s' ", possibleFlagsAnalyzer[i]);
                    }
                    int j;
                    for (j=0; j<numArguments; j++){
                        if (!instanceOfMySelf.hasMainOption ){
                            // printf("argument %d : '%s' ", j, listOfArguments[j]);
                        }
                    }
                    if (!instanceOfMySelf.hasMainOption ){
                        // waitEnter();
                    }
                    settedFlagsAnalyzer[i] = false;
                    
                    switchCommand(i, numArguments, listOfArguments);
                }
            }
        } else {
            char invalidCommand[BUFFER_SIZE];
            strcpy(invalidCommand,"Invalid command '");
            for(i=1; i<argc; i++){
                strcat(invalidCommand, " ");
                strcat(invalidCommand, argv[i]);
            }
            strcat(invalidCommand, " '");
            updateHistory(invalidCommand);
        }
    }
    
    if (returnCode == 0 && !modeSet){
        returnCode = inputReader();
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
                          "-quit: o quit from the process\n\n";

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
    instanceOfMySelf.statusAnalysis = 1;
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
        char mess1[] = "\nError: specify numeric non-zero positive values for n\n";
        if(!instanceOfMySelf.hasMainOption){
            fprintf(stderr, "%s", mess1);
            waitEnter();
        } else {
            sendTextMessageToReport(cInstance->pipeAC, mess1);
        }
        returnValue = false;
    }
    if(instanceOfMySelf.m <=0 ) {
        char mess2[] = "\nError: specify numeric non-zero positive values for m\n";
        if(!instanceOfMySelf.hasMainOption){
            fprintf(stderr, "%s", mess2);
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
    cInstance = (controllerInstance*) malloc(sizeof(cInstance));
    checkNotNull(cInstance);

    cInstance->pidAnalyzer = getpid();
    
    if (instanceOfMySelf.hasMainOption){
        cInstance->hasMainOption = true;
    } else {
        cInstance->hasMainOption = false;
    }

    if ((pipe(cInstance->pipeAC) != -1) && (pipe(cInstance->pipeCA) != -1)){
        // make the pipes non blocking
        fcntl(cInstance->pipeAC[READ], F_SETFL, O_NONBLOCK);
        fcntl(cInstance->pipeCA[READ], F_SETFL, O_NONBLOCK);

        controllerInstance newInstance = *cInstance;
        cInstance->pid = fork();

        if (cInstance->pid < 0){
            if(!instanceOfMySelf.hasMainOption){
                fprintf(stderr, "Found an error creating The Controller\n");
                waitEnter();
            }
            returnCode = 1;
        } else if (cInstance->pid == 0){
            // child: new instance of Controller
            close(newInstance.pipeAC[WRITE]);
            close(newInstance.pipeCA[READ]);
            newInstance.pid = getpid();
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
            waitEnter();
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
    // restore termios
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    // Start the waterfall effect
    if(sendDeathPacket(cInstance->pipeAC) != 0){
        kill(cInstance->pid, SIGKILL);
    }

    // free occupied memory:
    free(cInstance);
    deleteNamesList(filePaths);

    if (!instanceOfMySelf.hasMainOption){
        clear();
        printf("Cleanup complete, see you next time!\n");
    }
    exit(9); // to exit from infinite loop
}

/**
 * Function that animates the waiting for static analysis to end.
 */
void waitAnalisysEnd(){
    int status = 3;
    while(status!=0){
        status = waitForMessagesInAFromC();
    }
    if(!instanceOfMySelf.hasMainOption){
        printf("Analisys finished!\n");
    }
}


/**
 * Function to print the header of the termios screen.
 */
void printScreen() {
    printf("================================================\n");
    printf("Analysis mode: %s\n", instanceOfMySelf.mode);
    // printf("Analysis status: %s\n", statuses[instanceOfMySelf.statusAnalysis]);
    printf("===================Processing===================\n");
    // if(instanceOfMySelf.statusAnalysis == 1){
    // printf("Completed files: %2d over %2d\n", instanceOfMySelf.completedFiles, instanceOfMySelf.totalFiles);
    // printf("================================================\n");
    // printf("============Messages from controller============\n");
    // int i;
    // for(i=0; i<MESSAGES; i++){
    //     printf("Previous message -%d: %s\n", i, instanceOfMySelf.lastMessages[i]);
    // }
    // printf("================================================\n");
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
                bool validCall = checkArguments(numCommands, listOfCommands, possibleFlagsAnalyzer, flagsWithArgsAnalyzer, numberPossibleFlagsAnalyzer, settedFlagsAnalyzer, argumentsAnalyzer, invalidPhraseAnalyzer, true);
                // Check contstraints on arguments
                bool validArguments = checkArgumentsValidity(argumentsAnalyzer);

                if(validCall && numCommands > 0 && validArguments){
                    for(i=numberPossibleFlagsAnalyzer-1; i>=0; i--){
                        // If command is set, then execute it
                        if(settedFlagsAnalyzer[i]){
                            settedFlagsAnalyzer[i] = false; 
                            char commandToPrint[BUFFER_SIZE];
                            strcpy(commandToPrint, possibleFlagsAnalyzer[i]);
                            int numArguments = 0;
                            string listOfArguments[BUFFER_SIZE];
                            if(flagsWithArgsAnalyzer[i]){
                                if(argumentsAnalyzer[i]!=NULL){
                                    strcat(commandToPrint, " ");
                                    strcat(commandToPrint, argumentsAnalyzer[i]); 
                                }
                                parser(argumentsAnalyzer[i], &numArguments, listOfArguments);
                            }
                            if(!instanceOfMySelf.hasMainOption){
                                // printf("Processing command: '%s' ", possibleFlagsAnalyzer[i]);
                            }
                            int j;
                            if(!instanceOfMySelf.hasMainOption){
                                for(j=0; j<numArguments; j++){
                                    // printf("argument %d : '%s' ", j, listOfArguments[j]);
                                }
                                // waitEnter();
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
                    // if(!instanceOfMySelf.hasMainOption){
                    //     printf("\nYou are not in interactive mode!");
                    //     waitEnter();
                    // }
                } else {
                    instanceOfMySelf.statusAnalysis = 1;
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
            sendNewNPacket(cInstance->pipeAC, instanceOfMySelf.n);
            break;
        case flag_m:
            instanceOfMySelf.m = atoi(arguments[0]);
            sendNewMPacket(cInstance->pipeAC, instanceOfMySelf.m);
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
    // printf("number of files wrote = %d\n\n", instanceOfMySelf.totalFiles);
    printf("\n\tvalue of n = %d\n\n", instanceOfMySelf.n);
    printf("\tvalue of m = %d\n\n", instanceOfMySelf.m);
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
            char actualName[BUFFER_SIZE];
            absolutePath = realpath(fileNames[i], actualName);
            if(absolutePath != NULL){
                int appended = appendNameToNamesList(filePaths, absolutePath);
                if(appended == 0){
                    // char message[BUFFER_SIZE] = "File ";
                    // char copy[BUFFER_SIZE];
                    // strcpy(copy, absolutePath);
                    // trimStringToLength(copy, 50);
                    // strcat(message, absolutePath);
                    // strcat(message, " added\n");
                    // if(!instanceOfMySelf.hasMainOption){
                    //     // printf("%s", message);
                    // } else {
                    //     // sendTextMessageToReport(cInstance->pipeAC, message);
                    // }
                }
            } else {
                // if(!instanceOfMySelf.hasMainOption){
                //     printf("Well, this is embarrassing... it seems we had problems checking file %s\n", fileNames[i]);
                // }
            }
        } else if (pathType == 1){
            // it's an existing folder
            int check = crawler(fileNames[i], filePaths, &numOfFilesInFolder);
            if(check!=0){
                // if(!instanceOfMySelf.hasMainOption){
                //     printf("Well, this is embarrassing... it seems we had problems checking folder %s\n", fileNames[i]);
                // }
            } else {
                // char message[BUFFER_SIZE] = "Folder ";
                // char copy[BUFFER_SIZE];
                // strcpy(copy, fileNames[i]);
                // trimStringToLength(copy, 50);
                // strcat(message, copy);
                // strcat(message, "  added\n");
                // if(!instanceOfMySelf.hasMainOption){
                //     printf("%s", message);
                // } else {
                //     sendTextMessageToReport(cInstance->pipeAC, message);
                // }
            }
        } else {
            // invalid file/folder
            char message[BUFFER_SIZE] = "File/folder ";
            char copy[BUFFER_SIZE];
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
        char actualPath[BUFFER_SIZE];
        absolutePath = realpath(fileNames[i], actualPath);

        if(absolutePath != NULL){
            // int numOfFilesInFolder; // used in case it's a folder
            int pathType = inspectPath(absolutePath);

            if (pathType == 0){
                // it's an existing file

                if (removeFileByNamePacket(cInstance->pipeAC, absolutePath) != -1){
                    char message[BUFFER_SIZE] = "Removing file ";
                    char copy[BUFFER_SIZE];
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
                    char copy[BUFFER_SIZE];
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
                // printf("About to send remove folder %s\n", absolutePath);
                // waitEnter();
                NamesList *containedFiles = constructorNamesList();
                int numFilesContained;
                crawler(absolutePath, containedFiles, &numFilesContained);
                // printf("Folder contains %d files\nGoin' to delete them\n", numFilesContained);
                string lista[numFilesContained];
                NodeName *elm = containedFiles->first;
                int i=0;
                while(elm != NULL){
                    lista[i] = (string)malloc(strlen(elm->name)+1);
                    strcpy(lista[i], elm->name);
                    lista[i][strlen(elm->name)]='\0';

                    elm = elm->next;
                    i++;
                }
                removeFiles(numFilesContained, lista);
            } else {
                // invalid file/folder
                if(!instanceOfMySelf.hasMainOption){
                    fprintf(stderr, "File/folder inserted to remove doesn't exist!\n");
                } else {
                    char message[BUFFER_SIZE] = "File/folder ";
                    char copy[BUFFER_SIZE];;
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
    if(settedFlagsAnalyzer[flag_n]){
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
    if(settedFlagsAnalyzer[flag_m]){
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

    static struct termios enter;
    tcgetattr(STDIN_FILENO, &enter);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\npress enter to continue...");
    while (getchar() != '\n');
    tcsetattr(STDIN_FILENO, TCSANOW, &enter);
}

/**
 * Function to clan variables arguments and settedFlagsAnalyzer.
 */
void cleanArguments(){
    int i;
    for(i=0; i<numberPossibleFlagsAnalyzer; i++){
        free(argumentsAnalyzer[i]);
        settedFlagsAnalyzer[i] = false;
        argumentsAnalyzer[i] = NULL;
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
    int numBytesRead, ret=4;
    byte packetHeader[1 + INT_SIZE];
    numBytesRead = read(cInstance->pipeCA[READ], packetHeader, 1 + INT_SIZE);
    if (numBytesRead > 0){
        if (packetHeader[0] == 16){
            ret = 0;
        } else if (packetHeader[0] == 17){
            // nuovo file completato
            int messageSize = fromBytesToInt(packetHeader+1); 
            byte packetData[messageSize];
            numBytesRead = read(cInstance->pipeCA[READ], packetData, messageSize);
            if (numBytesRead == 2*INT_SIZE){
                int completed  = fromBytesToInt(packetData );
                int total      = fromBytesToInt(packetData + INT_SIZE);
                instanceOfMySelf.totalFiles = total;
                instanceOfMySelf.completedFiles = completed;
                if(!instanceOfMySelf.hasMainOption){
                    staticAnalisysScreen();
                }
                ret = 1;
            } else {
                // if(!instanceOfMySelf.hasMainOption){
                //     printf("Something has gone wrong with a completedFile packet!\n");
                // }
                ret = 2;
            }
        } else if(packetHeader[0] == 18) {
            int messageSize = fromBytesToInt(packetHeader+1); 
            byte packetData[messageSize];
            int bytesRead = read(cInstance->pipeCA[READ], packetData, messageSize);
            if (bytesRead == messageSize){
                
                char message[messageSize];
                memcpy(message, packetData, messageSize);
                // message[messageSize] = '\0';
                updateMessages(message);
                if(!instanceOfMySelf.hasMainOption){
                    staticAnalisysScreen();
                }
                ret = 1;
            } else {
                ret = 2;
            }
        } else {
            // if(!instanceOfMySelf.hasMainOption){
            //     printf("Analyzer received wrong packet code from controller! %c\n", packetHeader[0]);
            // }
            ret = 2;
        }
    }   
    return ret;
}