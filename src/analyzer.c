#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include "utils.c"
#include "crawler.c"
#include "packets.h"
#include "controller.c"

#define BUFFER_SIZE 4096
char buf[BUFFER_SIZE], command[BUFFER_SIZE];
int counter = 0;

/**
 * TODO: Insert possibility to remove an entire folder? (un casin)
 * NOTE: processExit() è dichiarata come funzione (invece che essere scritta all'interno del codice
 *       perché dovrà essere richiamata anche alla ricezione di SIGKILL)
 * TODO: Decidere cosa fare mentre l'analisi statica si sta completando (una mezza soluzione è
 *       già implementata, dai un'occhiata)
 */

// da spostare in utils?
int numberPossibleFlags = 4; 
string possibleFlags[]  = {"-i", "-s", "-h", "-main"};
bool   flagsWithArgs[]  = {true, true, false, true};
bool   settedFlags[]    = {false, false, false, false};
string arguments[4];
string invalidPhrase    = "Wrong command syntax\n"; 

string statuses[] = {"Still not started", "Analysis is running", "Analysis finished"};

analyzerInstance instanceOfMySelf;
controllerInstance *cInstance;
NamesList *filePaths;
int numOfFiles = 0, n = 0, m = 0;

int  modeSwitcher(string, int, char**);
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
int inputReader(void);

int checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char ** arguments, char* invalid,bool printOnFailure){
    bool validity = true;
    int j=0;
    int i=1;
    while (i<argc ){
        bool valid =false;
        for(j=0;j<numberPossibleFlags && i<argc ;j++){
            if(!strcmp(argv[i],possibleFlags[j])){
                if(flagsWithArguments[j]){
                    bool serving = true;
                    i++;
                    while(i<argc && serving ){
                        if(argv[i][0]!='-'){
                            if(arguments[j]==NULL){
                                arguments[j] = malloc(strlen(argv[i]+1));
                                strcpy(arguments[j],argv[i]);
                                settedFlags[j]=true;
                                valid = true;
                            }else{
                                char* tmp = malloc(strlen(arguments[j])+strlen(argv[i])+2);
                                strcpy(tmp,arguments[j]);
                                strcat(tmp," ");
                                strcat(tmp,argv[i]);
                                free(arguments[j]);
                                arguments[j]=tmp;
                            }
                            i++;
                        }else{
                            serving=false;
                            i--;
                        }
                    }
                }else{
                    settedFlags[j]=true;
                    valid=true;
                }
            }
        }
        if(!valid){
            validity = false;
        }
        i++;
    }   
    if(printOnFailure && !validity)
        printf("%s",invalid);
    if(!validity){
        for(j=0;j<numberPossibleFlags;j++){
            settedFlags[j]=false;
            if(arguments[j]!=NULL){
                free(arguments[j]);
            }
        }
    }
    return validity;
}

string *getArgumentsList(char *arguments, int *numArgs, string *argumentsList){
    argumentsList = NULL;
    *numArgs = 0;
    if(arguments == NULL){
        // nothing
    } else {
        NamesList *list = constructorNamesList();    
        int   offset  = 0;
        char* oldPointer = arguments;
        char* pointer = strstr(arguments, " ");
        while(pointer != NULL){
            *numArgs = *numArgs+1;
            int argLength = pointer - oldPointer; 
            string argument = malloc(argLength+1);
            memcpy(argument, arguments+offset, argLength);
            argument[argLength] = '\0';
            oldPointer = pointer;
            offset = pointer - arguments + 1;
            appendNameToNamesList(list, argument);
            pointer = NULL;
            pointer = strstr(oldPointer+1, " ");
        }
        *numArgs = *numArgs+1;
        int argLength = strlen(arguments) - offset; 
        string argument = (string)malloc(argLength+1);
        // printf("Arglength: %d\n", argLength);
        memcpy(argument, arguments+offset, argLength);
        argument[argLength] = '\0';

        appendNameToNamesList(list, argument);

        argumentsList = (string *)malloc(*numArgs);
        int i=0;
        NodeName *node = list->first;
        // printf("num args: %d\n", *numArgs);
        while(node != NULL){
            argumentsList[i] = (string)malloc(strlen(node->name)+1);
            strcpy(argumentsList[i], node->name);
            argumentsList[i][strlen(node->name)] = '\0';
            // printf("name: %s\n", argumentsList[i]);
            node = node->next;
            i++;
        }
    }
    return argumentsList;
}

// check if the mode is a two char string, with the
// first char being '-'
bool isValidMode(string mode){
    return (strlen(mode) == 2) && (mode[0] == '-');
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
    filePaths = constructorNamesList();
    instanceOfMySelf.completedFiles = 0;
    instanceOfMySelf.totalFiles = 0;
    strcpy(instanceOfMySelf.lastCommand, "No commands yet");
    instanceOfMySelf.statusAnalisys = 0;

    bool validCall = checkArguments(argc, argv, possibleFlags, flagsWithArgs, numberPossibleFlags, settedFlags, arguments, invalidPhrase, true);

    int command = -1;
    int i;
    if(validCall && argc > 1){
        for(i=0; i<numberPossibleFlags; i++){
            if(settedFlags[i]){
                command = i;
            }
            // printf("Flag: %s\n", possibleFlags[i]);
            // if(settedFlags[i] && flagsWithArgs[i]){
            //     printf("Arguments: %s\n", arguments[i]);
            // } else {
            //     printf("No arguments\n");
            // }
        } 
        strcpy(instanceOfMySelf.lastCommand, possibleFlags[command]);
        strcat(instanceOfMySelf.lastCommand, " "); 
        strcat(instanceOfMySelf.lastCommand, arguments[command]); 
        int numArgs;
        string *argumentList;
        argumentList = getArgumentsList(arguments[command], &numArgs, argumentList);
        printf("num args: %d\n", numArgs);
        for(i=0; i<numArgs; i++){
            printf("argument #%d: %s\n", i, argumentList[i]);
        }
        modeSwitcher(possibleFlags[command], numArgs, argumentList);
    } else {
        printf("Invalid call\n");
        returnCode = 1;
    }
    while(1){
        inputReader();
    }

    return returnCode;
}

// Switch mode of the analyzer (interactive, static, help)
// Error codes:
// 1 - not enough args for static mode
// 2 - n, m and files are not valid
// 3 - mode not supported
int modeSwitcher(string mode, int argc, char *argv[]){
    int returnCode = 0;

    if(!strcmp(mode, "-h")){
            helpMode();
    } else if(!strcmp(mode, "-i")) {
        interactiveMode();
    } else if(!strcmp(mode, "-s")){
        if (argc < 3){
            fprintf(stderr, "?Error: specify a valid mode, n, m and at least one file/folder\n");
            returnCode = 1;
        } else {
            n = atoi(argv[0]);
            m = atoi(argv[1]);

            // Get file paths with the crawler
            numOfFiles = getFilePathsFromArgv(argv, argc - 2);

            if (!checkParameters()){
                returnCode = 2;
            } else {
                staticMode(n, m, numOfFiles, filePaths);
            }
        }
    } else {
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
    }  else if (filePaths->counter == 0){
        fprintf(stderr, "Error: all the files or folders specified are inexistent\n");
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

int inputReader(void){
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
                if (strcmp(command, "q") == 0){
                    break;
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