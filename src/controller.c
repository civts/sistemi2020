#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // pid_t // in crawler I use pid_t without types.h
#include "utils.c"
#include "p.c"

#define READ 0
#define WRITE 1

void controller(int, int, string[], bool);
void generateTree(int, int);
void reshapeTree(int, int);

pid_t *pids;
int **pipes;

void controller(int n, int m, string files[], bool isFirstReport){
    // if first report we need to allocate everything...
    isFirstReport ? generateTree(n, m) : reshapeTree(n, m);
}

void generateTree(int n, int m){
    pids = (pid_t*) malloc(n*sizeof(pid_t));
    // atm we can communicate only from counter to p
    // to communicate from p to counter we need MORE PIPES
    pipes = (int **) malloc(n*sizeof(int*));

    int i, f=1;
    for (i=0; i<n && f!=0; i++){
        int fd[2];
        // TODO:check syscall return
        pipe(fd);
        pipes[i] = fd;
        f = fork();
        if (f < 0){
            printf("Errore nella generazione di un figlio P");
            i--;
        } else if (f == 0){
            // child
            printf("P%d created\n", i);
            close(fd[WRITE]);
            p(m, fd);
        } else {
            // parent
            close(pipes[i][READ]);
            pids[i] = f;
        }
    }

    while (true);
}

void reshapeTree(int n, int m){

}

