#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define BUFFER_SIZE 4096
char buf[BUFFER_SIZE], command[BUFFER_SIZE];
int counter = 0;
char altbuf[BUFFER_SIZE];

void printScreen(int carattere) {
    printf("================================================\n");
    printf("%d\n", counter++);
    printf("===================Processing===================\n");
    printf("%s\n", command);
    printf("================================================\n");
    printf("> %s", buf);
    fflush(stdout);
}

void printChild(int pipeOUT[2],int pipeIN[2]){
    int n=1; int quit=0;
    int i=0;
    while(  n == 1 && quit==0){
        //printf("sto aspettando in read parent\n");
        n = read(pipeOUT[0], altbuf+i, 1);   
        if(altbuf[i]=='*') quit=1;
        i++;
    }
    printf(altbuf);
    fflush(stdout);
    // close read pipe
    //close(pipeOUT[0]); 
    
    // // // waits for child process with pid 'cpid' to
    // // // return and stores the exit code in cstatus
    // // waitpid(cpid, &cstatus, 0); 
    
    // // printf("Child exit status was: %d\n", cstatus);
    
    // // terminate parent
    // exit(EXIT_SUCCESS);    
}
void clear(){
    system("clear");
}

void resetBuffer(char buffer[], int size){
    int i;
    for (i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = 0;
    }
}

void child_process(int pipeOUT[2],int pipeIN[2],char* path,char**argv){
    if (dup2(pipeOUT[1], STDOUT_FILENO) == -1){ 
        perror("dup2"); 
        exit(EXIT_FAILURE); 
    }   
    // if (dup2(pipeIN[0], STDIN_FILENO) == -1){ 
    //     perror("dup2"); 
    //     exit(EXIT_FAILURE); 
    // } 
    // duplicated by dup2 above, no longer needed
    //close(pipeOUT[1]); 
    //close(pipeOUT[0]); 
    //close(pipeIN[1]); 
    //close(pipeIN[0]);
    execv(path,argv);
    //printf("patate*\n");
    // make sure the write buffer is flushed before we exit
    fflush(stdout); 
    
    // close to make sure read() returns 0 in the parent
    close(STDOUT_FILENO); 
    
    // child exits
    exit(EXIT_SUCCESS); 
}
int main(void){
    int lenBuffer = 0, numReadCharacters = 0;
    char *endCommandPosition;
    resetBuffer(buf, BUFFER_SIZE);
    resetBuffer(command, BUFFER_SIZE);

    static struct termios oldt, newt;

    /*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
    tcgetattr( STDIN_FILENO, &oldt);
    /*now the settings will be copied*/
    newt = oldt;

    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    newt.c_lflag &= ~(ICANON);          
    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 1;

    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /*This is your part:
    Notice that EOF is also turned off
    in the non-canonical mode*/

    int returnCode;
    // creo i figli
    int report;    
    char reportPath[] = "./report.o";
    int analyzer;    
    char analyzerPath[] = "./analyzer.o";
    int pipeReportOUT [2];
    int pipeAnalyzerOUT[2];
    int pipeReportIN [2];
    int pipeAnalyzerIN [2];
    // pipe2(pipeReportOUT,O_NONBLOCK);
    // pipe2(pipeReportIN,O_NONBLOCK);
    // pipe2(pipeAnalyzerOUT,O_NONBLOCK);
    // pipe2(pipeAnalyzerIN,O_NONBLOCK);

    pipe(pipeReportOUT);
    pipe(pipeReportIN);
    pipe(pipeAnalyzerOUT);
    pipe(pipeAnalyzerIN);
    report = fork();
    if (report < 0){
        fprintf(stderr, "Found an error creating the Report\n");
        returnCode = 2;
    } else if (report == 0){
        //figlio
        child_process(pipeReportOUT,pipeReportIN,reportPath,NULL);
    } else {
        // // parent;
        analyzer = fork();
        if(analyzer < 0){
            fprintf(stderr, "Found an error creating the Analyzer\n");
            returnCode = 2;
        } else if (analyzer == 0){
            child_process(pipeAnalyzerOUT,pipeAnalyzerIN,analyzerPath,NULL);
        }else{
            int visualizzazione = 0; // 0 report, 1 analyzer
            while(1){
                if ((numReadCharacters = read(0, buf + lenBuffer, BUFFER_SIZE - lenBuffer)) > 0){
                    lenBuffer += numReadCharacters;
                    endCommandPosition = strrchr(buf, '\n');

                    if (endCommandPosition != NULL){
                        int commandLength = endCommandPosition - buf + 1;
                        memcpy(command, buf, commandLength);
                        command[commandLength - 1] = '\0';
                        resetBuffer(buf, BUFFER_SIZE);
                        lenBuffer = 0;

                        // make switch on command
                        if (strcmp(command, "--report") == 0){
                            visualizzazione=0; 
                        }else
                        // make switch on command
                        if (strcmp(command, "--analyzer") == 0){
                            visualizzazione=1;
                        }else{
                           
                        }
                    }
                    else{
                    }

                }
                clear();
                //printChild(pipeReportOUT,pipeReportIN);
                switch(visualizzazione){
                    case 0:
                        printChild(pipeReportOUT,pipeReportIN);
                        write(pipeReportIN,buf,4096);
                        break;
                    case 1:
                        printChild(pipeAnalyzerOUT,pipeAnalyzerIN);
                        //write(pipeAnalyzerIN,command,strlen(command)+1);
                        break;
                }
                //   printChild(pipeAnalyzerOUT,pipeAnalyzerIN);
                printf(">MAIN %s", buf);
                fflush(stdout);
                //sleep(1);
            }                

            // restore the old settings
            tcsetattr( STDIN_FILENO, TCSANOW, &oldt);


            return 0;
        }
    }
}