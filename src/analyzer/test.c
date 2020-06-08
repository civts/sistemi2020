#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>
#include <fcntl.h>
#include "../common/utils.h"
#include "../common/packets.h"
#include "instances.h"
#include "crawler.h"
#include "controller.h"

controllerInstance *cInstance;

int generateNewControllerInstance();

int m = 8;
int n = 1;

int main(int argc, char *argv[]){
    int returnCode = generateNewControllerInstance();
    sendNewNPacket(cInstance->pipeAC, n);
    sendNewMPacket(cInstance->pipeAC, m);

    sendNewFilePacket(cInstance->pipeAC, "q.c");
    // sendNewFilePacket(cInstance->pipeAC, "file2.txt");
    sendStartAnalysisPacket(cInstance->pipeAC, -1);
    // sendNewNPacket(cInstance->pipeAC, 2);
    sleep(4);

    sendDeathPacket(cInstance->pipeAC);
    sleep(2);

    return returnCode;
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

        controllerInstance cInstanceCopy = *cInstance;
        cInstanceCopy.pid = fork();
        cInstance->pid = cInstanceCopy.pid;
        if (cInstance->pid < 0){
            fprintf(stderr, "Found an error creating the controllerInstance\n");
            returnCode = 2;
        } else if (cInstance->pid == 0){
            // child: new instance of Controller
            printf("controllerInstance created\n");
            close(cInstanceCopy.pipeAC[WRITE]);
            close(cInstanceCopy.pipeCA[READ]);
            // while(true);
            controller(&cInstanceCopy);
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