#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.c"

#define READ 0
#define WRITE 1


void crawler(string folder, string *fileList){
    
    int fds[2];
    pipe(fds);

    int f;
    f = fork();

    int status = 0;
    if (f < 0){
        printf("\nError creating little brittle crawler-son\n");
    } else if (f == 0){
        close(fds[READ]);
        dup2(fds[WRITE], 1);     // substituting stdout with fds[WRITE]
        // string args[]={"-p ", "-R", NULL};
        string args[] ={"ciao", NULL};
        for (long long i=0; i<9999999999; i++){}
        // execvp("ls", args);
        // execvp("echo", args);
        system("echo ciao");
    }
    // char[0] = (char*)malloc(fine-inizio+1);
    // parent
    printf("test\n");
    wait(NULL);
    close(fds[WRITE]);
    char result[50];
    
    read(fds[READ], result, sizeof result);
    result[49]='\0';
    printf("I've found: %s\n", result);
}


int main(){
    string folder = "./";
    string *fileList;
    crawler(folder, fileList);
    return 0;
}