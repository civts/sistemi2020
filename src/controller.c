#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // pid_t // in crawler I use pid_t without types.h
#include "utils.c"
#include "p.c"

#define READ 0
#define WRITE 1

void controller(int, int, string[], int);
void generateTree(int, int);
void update_m(int, int);
void shapeTree(int, int);
void staccaStacca(int);

void sendPathNameToP(string, int, bool);

pid_t *pids;
int **pipesToP;

int curr_n = 0;
int curr_m = 0;

// TODO create a fd from analyzer to controller
// TODO send controller also the NUMBER OF FILES in files[]
void controller(int n, int m, string files[], int numFiles){
    // if first report we need to allocate everything...
    shapeTree(n, m);

    int i;
    for (i = 0; i < numFiles; i++){
        sendPathNameToP(files[i], i % curr_n, false);
    }
}

void staccaStacca(int pIndex){
    // TODO this will not work if we change INT_SIZE in utils
    printf("Stacca stacca: %d\n", pIndex);
    byte packet[5] = {2, 0, 0, 0, 0};

    write(pipesToP[pIndex][WRITE], packet, 5);
}

void update_m(int new_m, int new_n){
    printf("Update m to %d\n", new_m);

    byte packet[9] = {3, 0, 0, 0, INT_SIZE, 0, 0, 0, 0};
    fromIntToBytes(new_m, packet + 5);

    int i;

    if(new_n>curr_n){
        // updating only old Ps
        for (i = 0; i < curr_n; i++){
            write(pipesToP[i][WRITE], packet, 9);
        }
    } else {
        // updating all the Ps
        for (i = 0; i < new_n; i++){
           write(pipesToP[i][WRITE], packet, 9);
        }
    }
}

// TODO: gestire il caso di un reshape(0,0)
void shapeTree(int new_n, int new_m){
    int difference = new_n - curr_n;
    
    if( difference < 0){ 
        // We have to delete some Ps
        difference *= -1;  // making a positive difference
        pid_t *newPids = (pid_t *) malloc(sizeof(pid_t) * new_n);
        int **newPipes = (int **) malloc(sizeof(int*) * new_n);

        int i;
        for(i = 0; i < curr_n; i++){
            if (i < new_n){
                newPids[i] = pids[i];
                newPipes[i]= pipesToP[i];
            } else {
                staccaStacca(i);   // notify its child Qs to kill themselves
                free(pipesToP[i]); // free execeeding pipesToP
            }
        }

        // free old resources
        free(pids);
        free(pipesToP);
        
        // updating global pipesToP and pids
        pipesToP = newPipes;
        pids  = newPids;

    } else if (difference > 0){
        // This is the case in which we have to add new Ps
        pid_t *newPids = (pid_t*) malloc(new_n*sizeof(pid_t));
        int **newPipes = (int **) malloc(new_n*sizeof(int *));

        int i;
        for( i = 0; i < curr_n; i++){
            // copying good old Ps
            newPids[i]  = pids[i];
            newPipes[i] = pipesToP[i];
        }

        for (i = curr_n; i < new_n; i++){
            newPipes[i] = (int *) malloc(sizeof(int) * 2); // creating brand new Ps
            pipe(newPipes[i]); // TODO:check syscall return

            int f = fork();
            if (f < 0){
                printf("Errore nella generazione di un figlio P");
                i--;
            } else if (f == 0){
                // new child
                printf("New P%d created\n", i);
                close(newPipes[i][WRITE]);              
                p(new_m, newPipes[i]);
                exit(0); // just to be sure... it should not be necessary
            } else {
                // parent
                close(newPipes[i][READ]);
                newPids[i] = f;
            }
        }

        free(pids); // valgrind says it works, we need to investigate further

        pids = newPids;
        pipesToP = newPipes;
    }


    if(new_m != curr_m){
        update_m(new_m, new_n);
    }
    
    curr_n = new_n;
    curr_m = new_m;
}

void sendPathNameToP(string pathName, int indexOfP, bool isInsideFolder){
    int packetLength = 1 + INT_SIZE + 1 + strlen(pathName); // packet type, data length, isInFolder, pathName
    byte* packet = (byte*) malloc(packetLength * sizeof(byte));
    int offset = 0;

    // packet type: new file packet
    packet[offset] = 0;
    offset++;

    // data length
    fromIntToBytes(packetLength - 1 - INT_SIZE, packet + offset);
    offset += INT_SIZE;

    // isInsideFolder flag
    packet[offset] = (byte)isInsideFolder;
    offset++;

    // pathName
    memcpy(packet + offset, pathName, strlen(pathName));

    write(pipesToP[indexOfP][WRITE], packet, packetLength);
    free(packet);
}

void wait_a_bit(){
    long long int i;
    for (i=0; i<999999999; i++){}
}

int main(){
    string files[2]={"prova1.txt", "prova2.txt"};
    controller(2,2, files, 2);
    fflush(stdout);
    wait_a_bit(); 

    shapeTree(4, 2);
    wait_a_bit(); 

    int y;
    for (y=0; y<curr_n; y++){
        staccaStacca(y);
    }
    fflush(stdout);
    wait_a_bit();
    
    printf("Fine\n");
    free(pids);
    int z;
    for (z=0; z<curr_n; z++){
        free(pipesToP[z]);
    }
    free(pipesToP);
    return 0;
}