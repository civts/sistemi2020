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

int curr_n;
int curr_m;

void controller(int n, int m, string files[], bool isFirstReport){
    // if first report we need to allocate everything...
    isFirstReport ? generateTree(n, m) : reshapeTree(n, m);
}

void generateTree(int n, int m){
    curr_n = n;
    curr_m = m;

    pids = (pid_t*) malloc(n*sizeof(pid_t));
    pipes = (int **) malloc(n*sizeof(int*));

    int i, f=1;
    for (i=0; i<n && f!=0; i++){
        pipes[i] = (int *) malloc(sizeof(int) * 2);
        // TODO:check syscall return
        pipe(pipes[i]);
        f = fork();
        if (f < 0){
            printf("Errore nella generazione di un figlio P");
            i--;
        } else if (f == 0){
            // child
            printf("P%d created\n", i);
            fflush(stdout);
            close(pipes[i][WRITE]);
            p(m, pipes[i]);
            exit(0);
        } else {
            // parent
            close(pipes[i][READ]);
            pids[i] = f;
        }
    }
}

void staccaStacca(int pIndex){
    // TODO: implement the communication to tell Ps to kill themselves
    printf("Stacca stacca: %d\n", pIndex);
}

void update_m(int new_m){
    // TODO: implement the communication to tell Ps to change m
    printf("Update m to %d\n", new_m);
}

// TODO tell p if m has changed
void reshapeTree(int new_n, int new_m){
    int difference = new_n - curr_n;
    
    if( difference < 0){   // We have to delete some Ps
        // The case when the new n is less than curr_n
        // here we have to stop and delete some Ps
        difference *= -1; // making a positive difference
        pid_t *newPids = (pid_t *) malloc(sizeof(pid_t) * new_n);
        int **newPipes = (int **) malloc(sizeof(int*) * new_n);

        int i;
        for(i = 0; i < curr_n; i++){
            if (i < new_n){
                newPids[i] = pids[i];
                newPipes[i]= pipes[i];
            } else {
                staccaStacca(i); // notify its child Qs to kill themselves
                free(pipes[i]);  // free exceeeeeeding pipes
            }
        }

        // free old resources
        free(pids);
        free(pipes);
        
        // updating global pipes and pids
        pipes = newPipes;
        pids  = newPids;

    } else if (difference > 0){
        // This is the case in which we have to add new Ps
        pid_t *newPids = (pid_t*) malloc(new_n*sizeof(pid_t));
        int **newPipes = (int **) malloc(new_n*sizeof(int *));

        int i;
        for( i = 0; i < curr_n; i++){
            // copying good old Ps
            newPids[i]=pids[i];
            newPipes[i]=pipes[i];
        }

        for (i = curr_n; i < new_n; i++){
            // creating brand new Ps
            newPipes[i] = (int *) malloc(sizeof(int) * 2);
            // TODO:check syscall return
            pipe(newPipes[i]);
            int f = fork();
            if (f < 0){
                printf("Errore nella generazione di un figlio P");
                i--;
            } else if (f == 0){
                // new child
                printf("New P%d created\n", i);
                close(newPipes[i][WRITE]);
                // TODO invio new_m o curr_m??                
                p(new_m, newPipes[i]);
                exit(0);
            } else {
                // parent
                close(newPipes[i][READ]);
                newPids[i] = f;
            }
        }

        // valgrind says it works, we need to investigate further
        free(pids);

        pids = newPids;
        pipes = newPipes;
    }

    // Once settled the number of Ps to new_n, update the curr_n value
    curr_n = new_n;
    
    int mDifference = new_m - curr_m;

    if(mDifference != 0){
        update_m(new_m);
    }

    // Once updated the number of m to new_m, update also curr_m value
    curr_m = new_m;
}

// int main(){
//     string files[2]={"lol", "lel"};
//     controller(8,2, files, true);
//     fflush(stdout);
//     controller(5,2, files, false);  
    
//     free(pids);
//     int i;
//     for (i=0; i<curr_n; i++){
//         free(pipes[i]);
//     }
//     free(pipes);
//     return 0;
// }