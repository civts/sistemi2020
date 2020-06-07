#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Analyzer/src/analyzer.c" 
#include "Report/src/report/report_input.c"
#define BUFFER_SIZE 4096
#define READ  0
#define WRITE 1

char buf[BUFFER_SIZE], command[BUFFER_SIZE];
int counter = 0;

void printScreenMain(int carattere) {
    printf("================================================\n");
    printf("%d\n", counter++);
    printf("===================Processing===================\n");
    printf("%s\n", command);
    printf("================================================\n");
    printf("> %s", buf);
    fflush(stdout);
}

int main(void){
    int returnCode;

    char analyzerPath[PATH_MAX]="./Analyzer/src/";
    char reportPath[PATH_MAX]="./Report/src/report/";

    int daughter;
    int pipeToDaughter[2];
    int pipeFromDaughter[2];

    int son;
    int pipeToSon[2];
    int pipeFromSon[2];

    int parent;
    
    daughter = fork();

    if (daughter < 0){
        fprintf(stderr, "Found an error creating the Report\n");
        returnCode = 2;
    } else if (daughter == 0){
        // child: new instance of Controller;
        char reportExecutablePath[PATH_MAX];
        strcpy(reportExecutablePath, reportPath);
        strcat(reportExecutablePath, "r");
        close(pipeToDaughter[WRITE]);
        close(pipeFromDaughter[READ]);
        // Inserire metodo report() qui
        int numArgs=0;
        string args[3] = {"-add ./", "-n 2", "-m 3"};
        report_main(numArgs, args);
    } else {
        // parent;
        son = fork();
        if(son < 0){
            fprintf(stderr, "Found an error creating the Analyzer\n");
            returnCode = 2;
        } else if (son == 0){
            // child: new instance of Controller;
            char analyzerExecutablePath[PATH_MAX];
            strcpy(analyzerExecutablePath, analyzerPath);
            strcat(analyzerExecutablePath, "a");
            fprintf(stderr, "analyzer created\n");
            close(pipeToSon[WRITE]);
            close(pipeFromSon[READ]);
            int numArgs=0;
            string args[3] = {"-add ./", "-n 2", "-m 3"};
            analyzer_main(numArgs, args);
        }
    }
    close(pipeToSon[READ]);
    close(pipeFromSon[WRITE]);


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
    newt.c_cc[VTIME] = 0;

    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /*This is your part:
    Notice that EOF is also turned off
    in the non-canonical mode*/
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
                if (strcmp(command, "q") == 0){
                    break;
                }
            } else {

            }
        }
         
        clear();
        printScreenMain('a');
        sleep(1);
    }                

    // restore the old settings
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);


    return 0;
}