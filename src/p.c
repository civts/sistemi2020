#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "utils.c"
#include "q.c"

void p(int m){
    pid_t *pids = (pid_t*) malloc(m*sizeof(pid_t));

    int i, f=1;
    for (i = 0; i < m && f != 0; i++){
        f = fork();

        if (f < 0){
            printf("Error creating Q\n");
            i--;
        } else if (f == 0){
            // child
            printf("Q%d created\n", i);
            q(i, m);
        } else {
            // parent
            pids[i] = f;
        }
    }
    
    while (true){
        // wait for messagge from controller
    }
}

