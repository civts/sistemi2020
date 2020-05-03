#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "utils.c"
#include "q.c"

#define READ 0
#define WRITE 1

void p(int m, int *pipeFromController){
    pid_t *pids = (pid_t*) malloc(m*sizeof(pid_t));
    // Here I am building 2 pipes for each Q, one for the communication P->Q
    // and the other for the communication Q->P
    int **pipeListToQ = (int **) malloc(m*sizeof(int*));
    int **pipeListFromQ = (int **) malloc(m*sizeof(int*));

    int i, f=1;
    for (i = 0; i < m && f != 0; i++){
        int pipeToQ[2];
        int pipeFromQ[2];
        // TODO: check syscall return
        pipe(pipeToQ);
        pipe(pipeFromQ);
        pipeListToQ[i] = pipeToQ;
        pipeListFromQ[i] = pipeFromQ;

        f = fork();

        if (f < 0){
            printf("Error creating Q\n");
            i--;
        } else if (f == 0){
            // child
            printf("Q%d created\n", i);
            close(pipeToQ[READ]);
            close(pipeFromQ[WRITE]);
            q(i, m, pipeToQ, pipeFromQ);
        } else {
            // parent
            close(pipeToQ[READ]);
            close(pipeFromQ[WRITE]);
            pids[i] = f;
        }
    }
    
    while (true){
        // wait for messagge from controller
    }
}

int main(){
    return 0;
}