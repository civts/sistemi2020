#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // pid_t
#include "utils.c"
#include "p.c"

void controller(int, int, string[], bool);
void generateTree(int, int);
void reshapeTree(int, int);

pid_t *pids;

void controller(int n, int m, string files[], bool isFirstReport){
    // if first report we need to allocate everything...
    isFirstReport ? generateTree(n, m) : reshapeTree(n, m);
}

void generateTree(int n, int m){
    pids = (pid_t*) malloc(n*sizeof(pid_t));

    int i, f=1;
    for (i=0; i<n && f!=0; i++){
        f = fork();
        if (f < 0){
            printf("Errore nella generazione di un figlio P");
            i--;
        } else if (f == 0){
            // child
            printf("P%d created\n", i);
            p(m);
        } else {
            // parent
            pids[i] = f;
        }
    }

    while (true);
}

void reshapeTree(int n, int m){

}