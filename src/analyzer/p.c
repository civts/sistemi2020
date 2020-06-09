#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "p.h"
#include "q.h"
#include "instances.h"
#include "../common/packets.h"
#include "../common/utils.h"

qInstance *qInstances = NULL; // Q processes associated to this P
int currM;

int p(pInstance *instanceOfMySelf, int _currM){
    int i, returnCode = 0;
    qInstances = (qInstance*) realloc(qInstances, sizeof(qInstance) * _currM);

    if (qInstances == NULL){
        // fprintf(stderr, "Error allocating Q table inside P\n");
        returnCode = 1;
    } else {
        for (i = 0; i < _currM; i++){
            generateNewQInstance(qInstances + i, i, _currM);
        }
    }

    signal(SIGINT, sig_handler_P);
    signal(SIGKILL, sig_handler_P);
    signal(SIGTERM, sig_handler_P);
    signal(SIGQUIT, sig_handler_P);


    currM = _currM;
    waitForMessagesInP(instanceOfMySelf);

    return returnCode;
}

int generateNewQInstance(qInstance *newQ, int index, int mValue){
    int returnCode = 0;

    if (pipe(newQ->pipePQ) != -1 && pipe(newQ->pipeQP) != -1){
        // TODO check for error code
        // make the pipes non blocking
        fcntl(newQ->pipePQ[READ], F_SETFL, O_NONBLOCK);
        fcntl(newQ->pipeQP[READ], F_SETFL, O_NONBLOCK);

        qInstance qChild = *newQ;
        newQ->pid = fork();
        newQ->currM = mValue;
        newQ->index = index;

        qChild.pid = newQ->pid;
        qChild.currM = mValue;
        qChild.index = index;

        if (newQ->pid < 0){
            // fprintf(stderr, "Found an error creating Q%d\n", index);
            returnCode = 2;
        } else if (newQ->pid == 0){
            // child: new instance of Q
            
            // fprintf(stderr, "New Q%d created\n", index);
            // close(newQ->pipePQ[WRITE]);
            // close(newQ->pipeQP[READ]);
            // q(newQ);
            close(qChild.pipePQ[WRITE]);
            close(qChild.pipeQP[READ]);

            qChild.pid = getpid();
            q(&qChild);
            exit(0); // just to be sure... it should not be necessary
        } else {
            // parent
            close(newQ->pipePQ[READ]);
            close(newQ->pipeQP[WRITE]);
        }
    } else {
        // fprintf(stderr, "Found an error creting pipes to Q%d\n", index);
        returnCode = 1;
    }

    return returnCode;
}

// main worker: infinite loop to get messages
void waitForMessagesInP(pInstance *instanceOfMySelf){

    while (true){
        waitForMessagesInPFromQ(instanceOfMySelf);
        waitForMessagesInPFromController(instanceOfMySelf);
    }
}

// here the messages arrives always atomically, so we don't
// need to check if the message is complete
void waitForMessagesInPFromQ(pInstance *instanceOfMySelf){
    int numBytesRead, dataSectionSize;
    byte packetHeader[1 + INT_SIZE];

    int i;
    for (i = 0; i < currM; i++){
        numBytesRead = read(qInstances[i].pipeQP[READ], packetHeader, 1 + INT_SIZE);

        if (numBytesRead == (1 + INT_SIZE)){
            dataSectionSize = fromBytesToInt(packetHeader + 1);
            byte packetData[dataSectionSize];

            numBytesRead = read(qInstances[i].pipeQP[READ], packetData, dataSectionSize);
            // printf("Got packet %d in P from Q\n", packetHeader[0]);
            processMessageInPFromQ(packetHeader[0], packetData, dataSectionSize, instanceOfMySelf);
        }
    }
}

// Here the messages can not be sent or received atomically
// since the new file packet contains the full file path as string
void waitForMessagesInPFromController(pInstance *instanceOfMySelf){
    int numBytesRead, dataSectionSize, offset;
    byte packetHeader[1 + INT_SIZE];

    numBytesRead = read(instanceOfMySelf->pipeCP[READ], packetHeader, 1 + INT_SIZE);

    if (numBytesRead == (1 + INT_SIZE)){
        dataSectionSize = fromBytesToInt(packetHeader + 1);

        offset = 0;
        byte packetData[dataSectionSize];

        // if we get a header then block and wait the whole message
        while (offset != dataSectionSize){
            numBytesRead = read(instanceOfMySelf->pipeCP[READ], packetData + offset, dataSectionSize - offset);
            if (numBytesRead > 0){
                offset += numBytesRead;
            } else if (numBytesRead < 0){
                // fprintf(stderr, "Error reading from pipe C->P\n");
            }
        }

        // printf("Got packet %d in P from C\n", packetHeader[0]);
        processMessageInPFromController(packetHeader[0], packetData, dataSectionSize, instanceOfMySelf);
    }
}

int processMessageInPFromQ(byte packetCode, byte *packetData, int packetDataSize, pInstance *instanceOfMySelf){
    int returnCode;
    switch (packetCode){
        case 6:
            returnCode = processPFileResults(packetData, packetDataSize, instanceOfMySelf);
            break;
        case 11:
            returnCode = processPErrorOnFilePacket(packetData, packetDataSize, instanceOfMySelf);
            break;
        default:
            // fprintf(stderr, "Error, P received from C an unknown packet type %d\n", packetCode);
            returnCode = 1;
    }

    return returnCode;
}

int processMessageInPFromController(byte packetCode, byte *packetData, int packetDataSize, pInstance *instanceOfMySelf){
    int returnCode;
    switch (packetCode){
        case 15:
            returnCode = processPNewFilePacket(packetData, packetDataSize);
            break;
        case 7:
            returnCode = processPRemoveFilePacket(packetData, packetDataSize);
            break;
        case 2:
            returnCode = processPDeathPacket();
            break;
        case 3:
            returnCode = processPNewValueForM(packetData, instanceOfMySelf);
            break;
        default:
            // fprintf(stderr, "Error, P received from C an unknown packet type %d\n", packetCode);
            returnCode = 1;
    }

    return returnCode;
}

// Only rebuilds the header and forward with no modifications
int processPNewFilePacket(byte packetData[], int packetDataSize){
    int i, returnCode = 0;
    for (i = 0; i < currM; i++){
        returnCode = forwardPacket(qInstances[i].pipePQ, 15, packetDataSize, packetData);
        if (returnCode < 0){
            // fprintf(stderr, "Could not forward file packet to Q\n");
        }
    }

    return 0;
}

// Forward the message to delete a file to all its Qs
int processPRemoveFilePacket(byte packetData[], int packetDataSize){
    int returnCode = 0;
    
    int i;
    for (i = 0; i < currM; i++){
        if (forwardPacket(qInstances[i].pipePQ, 7, packetDataSize, packetData) < 0){
            returnCode = 1;
            // fprintf(stderr, "Error trying to remove file from P to Q\n");
        }
    }

    return returnCode;
}

// Free resources and send a kill message to all its Qs
int processPDeathPacket(){
    int i, returnCode = 0;
    for (i = 0; i < currM; i++){
        // printf("Pipe %d %d\n", qInstances[i].pipePQ[0], qInstances[i].pipePQ[1]);
        
        if (sendDeathPacket(qInstances[i].pipePQ) != 0){
            
            // if pipes are not working than kill with a signal
            if (kill(qInstances[i].pid, SIGKILL) != 0){
                returnCode = 1;
            }
        } 

    }

    free(qInstances);
    // printf("P is dead\n");

    exit(returnCode); // to exit from infinite loop in waitForMessagesInP()
}

// Forward back occurences packet if its M value is up to date
int processPFileResults(byte packetData[], int packetDataSize, pInstance *instanceOfMySelf){
    int returnCode = 0;
    int m = fromBytesToInt(packetData + 2 * INT_SIZE);

    if (currM == m){
        if (forwardPacket(instanceOfMySelf->pipePC, 6, packetDataSize, packetData) < 0){
            returnCode = 1;
        }
    } else {
        returnCode = 2;
    }   

    return returnCode;
}

int processPErrorOnFilePacket(byte packetData[], int packetDataSize, pInstance *instanceOfMySelf){
    int returnCode = 0;

    if (forwardPacket(instanceOfMySelf->pipePC, 11, packetDataSize, packetData) < 0){
        returnCode = 1;
    }

    return returnCode;
}

int processPNewValueForM(byte packetData[], pInstance *instanceOfMySelf){
    int i, returnCode = 0;
    int newM = fromBytesToInt(packetData);

    // remove all exceeding Qs
    for (i = newM; i < currM; i++){
        if (sendDeathPacket(qInstances[i].pipePQ) != 0){
            kill(qInstances[i].pid, SIGKILL);
        }
        
    }

    // reallocate space for next Qs
    qInstances = (qInstance*) realloc(qInstances, sizeof(qInstance) * newM);

    // create new Q instances if newM > currM
    for (i = 0; i <currM; i++){
        sendNewMPacket(qInstances[i].pipePQ, newM);
    }
    
    for (i = currM; i < newM; i++){
        // printf("Seeee %d %d %d\n", i, currM, newM);
        generateNewQInstance(qInstances + i, i, newM);
    }
    
    currM = newM;
    // Nota: qui non serve riassegnare i file perché C invia subito dopo il pacchetto di cambio M anche
    // i pacchetti di tutti i file che non erano stati completati

    return returnCode;
}

void sig_handler_P(){
    // printf("\nP killed with signal\n");
    processPDeathPacket();
    exit(0);
}