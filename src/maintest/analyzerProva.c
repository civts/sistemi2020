#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define BUFFER_SIZE 4096
#define READ  0
#define WRITE 1

char buf[BUFFER_SIZE], command[BUFFER_SIZE];
int counterReport = 0;
void resetBuffer(char buffer[], int size);
void clear();
void resetBuffer(char buffer[], int size){
    int i;
    for (i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = 0;
    }
}
void clear(){
    system("clear");
    //printf("\e[h\e[2j");
}
void printScreenReport(int carattere) {
    printf("================================================\n");
    printf("%d\n", counterReport++);
    printf("===================ANALYZER======================\n");
    printf("%s\n", command);
    printf("================================================\n");
    printf(">Analyzer %s*", buf);
    fflush(stdout);
}

int main(int argc, char ** argv){
    while(1){
        printScreenReport('a');
    }

    return 0;
}