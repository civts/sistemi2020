#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <stdlib.h>
#include <string.h>
#include <limits.h>


void clear(){
    system("clear");
}

int main(int argc, char**argv){
    int returnCode=0;
    int analyzer;
    int pipeReportOUT [2];
    int pipeAnalyzerOUT[2];
    int pipeReportIN [2];
    int pipeAnalyzerIN [2];
    pipe2(pipeReportOUT,O_NONBLOCK);
    pipe2(pipeReportIN,O_NONBLOCK);
    pipe2(pipeAnalyzerOUT,O_NONBLOCK);
    pipe2(pipeAnalyzerIN,O_NONBLOCK);
    analyzer = fork();
    if(analyzer < 0){
        fprintf(stderr, "Found an error creating the Analyzer\n");
        returnCode = 2;
    } else if (analyzer == 0){
        if (dup2(pipeAnalyzerOUT[0], STDOUT_FILENO) == -1){ 
            perror("dup2"); 
            exit(EXIT_FAILURE); 
        }   
        if (dup2(pipeAnalyzerIN[1], STDIN_FILENO) == -1){ 
            perror("dup2"); 
            exit(EXIT_FAILURE); 
        } 
        execv("../../bin/analyzer",argv);
    }else{
        
        execv("../../bin/report",NULL);
        return 0;
    }
    
}