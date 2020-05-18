#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.c"
#include "crawler.c"
#include "packets.h"
#include "controller.c"

#define READ 0
#define WRITE 1

/**
 * TODO: Insert possibility to remove an entire folder? (un casin)
 * NOTE: processExit() è dichiarata come funzione (invece che essere scritta all'interno del codice
 *       perché dovrà essere richiamata anche alla ricezione di SIGKILL)
 * TODO: Impostare tutti i vari controlli sui comandi in ingresso in modalità dinamica
 * TODO: Decidere cosa fare mentre l'analisi statica si sta completando (una mezza soluzione è
 *       già implementata, dai un'occhiata)
 */

typedef struct{
    pid_t pid;
    int pipeAC[2];
    int pipeCA[2];
} ControllerInstance;

ControllerInstance *controllerInstance;
NamesList *filePaths;
int numOfFiles = 0, n = 0, m = 0;

int  modeSwitcher(char, int, char**);
void helpMode();
void interactiveMode();
void staticMode(int, int, int, NamesList *);

bool isValidMode(string);
int getFilePathsFromArgv(string[], NamesList*, int);
bool checkParameters();
int generateNewControllerInstance();
void sendNewFolder(int);
int processExit();
void waitAnalisysEnd();

// check if the mode is a two char string, with the
// first char being '-'
bool isValidMode(string mode){
    return (strlen(mode) == 2) && (mode[0] == '-');
}

// extract file paths from argv array. In case of folder,
// it uses the crawler to inspect inner files and folders.
// It returns the number of scanned files.
int getFilePathsFromArgv(string argv[], NamesList *fileList, int numPaths){
    const int padding = 4;      // index inside argv from which filenames occur
    unsigned long numFiles = 0; // number of files recognized
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
                numOfFiles = getFilePathsFromArgv(argv, filePaths, argc - 4);

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
    const char analyzeString[] = "analyze";
    const char exitString[] = "exit";
    const char showString[] = "show";

    printf("Interactive mode\n");

    char command[MINIQ_MAX_BUFFER_SIZE];

    printf("> ");
    scanf("%s", command);
    while (strcmp(command, exitString) != 0){
        if (strcmp(command, analyzeString) == 0){
            // start analyzing process (if parameters are good)
            if(checkParameters()){
                printf("Start analysis\n");
                sendStartAnalysisPacket(controllerInstance->pipeAC);
            }

        } else if (strcmp(command, showString) == 0){
            printNamesList(filePaths); // Print-show file list

        } else if (command[0] == '+'){
            // Managment of addition of file or folder
            // int *j = malloc(sizeof(int));
            int j;
            int added;
            int oldNumberOfFiles = filePaths->counter;
            if(isDirectory(command + 1, '/', &j)){
                crawler(command + 1, filePaths, &j);
                added = 1;
            } else if (isValidFile(command + 1)) {
                appendName(filePaths, command + 1);
                added = 2;
            } else {
                fprintf(stderr, "File/folder inserted doesn't exist!\n");
                added = 3;
            }
            // TODO per Sam: perché esiste added? Non si poteva inserire
            // tutto negli if qui sopra?
            if (added == 1){
                sendNewFolder(oldNumberOfFiles);
                printf("Added folder %s\n", command + 1);
            } else if(added == 2){
                sendNewFilePacket(controllerInstance->pipeAC, command);
                printf("Added file %s\n", command + 1);
            }
            // free(j);
        } else if (command[0] == '-'){
            // Management of removal of file or folder
            printf("Remove file %s\n", command + 1);
            if (removeByName(filePaths, command + 1) == 0){
                removeFileByNamePacket(controllerInstance->pipeAC, command + 1);
            }
        // TODO per Sam: ATTENZIONE: stai supponendo che command sia di almeno tre caratteri!
        // rischiamo un out of bound! (sia per n che per m)
        } else if (command[0] == 'n'){
            // Update the value of N
            printf("Change n to %s\n", command + 2);
            n = atoi(command + 2);
            sendNewNPacket(controllerInstance->pipeAC, n);
            printf("Now n=%d\n", n);

        } else if (command[0] == 'm'){
            // Update the value of M
            printf("Change m to %s\n", command + 2);
            m = atoi(command + 2);
            sendNewMPacket(controllerInstance->pipeAC, m);
            printf("Now m=%d\n", m);

        } else {
            // Command not supported
            fprintf(stderr, "This command is not supported.\n");

        }
        printf("\n> ");
        scanf("%s", command);
    }
    // Management of exit command
    processExit();    
}

void staticMode(int numOfP, int numOfQ, int numOfFiles, NamesList * listFilePaths){
    int returnCode = generateNewControllerInstance();
    printf("Static mode\n");
    sendNewNPacket(controllerInstance->pipeAC, n);
    sendNewMPacket(controllerInstance->pipeAC, m);
    // Trick: to send all files in the list call sendNewFolder with oldNumberOfFiles=0
    sendNewFolder(0);
    sendStartAnalysisPacket(controllerInstance->pipeAC);

    // TODO: what do we do when static analisys is started?
    // da FRA: io direi niente... è statica per qualche motivo
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
    controllerInstance = (ControllerInstance*) malloc(sizeof(ControllerInstance));

    if (pipe(controllerInstance->pipeAC) != -1 && pipe(controllerInstance->pipeCA) != -1){
        // TODO check for error -1 for fcntl
        // make the pipes non blocking
        fcntl(controllerInstance->pipeAC[READ], F_SETFL, O_NONBLOCK);
        fcntl(controllerInstance->pipeCA[READ], F_SETFL, O_NONBLOCK);

        controllerInstance->pid = fork();

        if (controllerInstance->pid < 0){
            fprintf(stderr, "Found an error creating the controllerInstance\n");
            returnCode = 2;
        } else if (controllerInstance->pid == 0){
            // child: new instance of Controller
            fprintf(stderr, "controllerInstance created\n");
            close(controllerInstance->pipeAC[WRITE]);
            close(controllerInstance->pipeCA[READ]);

            while (true);
            // TODO: ricreare il costruttore di controller.
            // TODO: inserire l'istanza di se stesso come parametro
            // TODO: creare i metodi per aggiornare n, m e la lista di files
            // controller(controllerInstance);
            exit(0);
        } else {
            // parent
            close(controllerInstance->pipeAC[READ]);
            close(controllerInstance->pipeCA[WRITE]);
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
 * TODO: inert controls on errors
 */
void sendNewFolder(int oldNumberOfFiles){
    int newNumberOFfiles = filePaths->counter - oldNumberOfFiles;

    Node *firstNewFile = filePaths->first;
    int i;
    // This cycle brings firstNewFile to the pointer of the first new file
    for(i = oldNumberOfFiles; i > 0; i--){
        firstNewFile = firstNewFile->next;
    }

    // this cycle calls sendNewFilePacket for each new packet
    for(i = newNumberOFfiles; i > 0; i--){
        sendNewFilePacket(controllerInstance->pipeAC, firstNewFile->name);
        firstNewFile = firstNewFile->next;
    }
}

/**
 * Notifies the controller that exit command has been pressed.
 * Provokes a waterfall effect tht kills every process.
 */
int processExit(){
    // Start the waterfall effect
    sendDeathPacket(controllerInstance->pipeAC);

    // free occupied memory:
    free(controllerInstance);
    deleteNamesList(filePaths);

    printf("Cleanup complete, see you next time!\n");
    exit(0); // to exit from infinite loop
}

// TODO: this function should wait a packet from the Controller
// Function that animates the waiting for static analisys to end.
void waitAnalisysEnd(){
    int numberOfPoints = 10;
    int i;
    while (true){
        // wait untill reading the analisys-ended packet from controller
        for (i = numberOfPoints; i>0; i--){
            sleep(1);
            printf(".");
        }
        printf("\n");
    }
    printf("\nAnalisys ended\n");
}