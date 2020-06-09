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
    printf("===================REPORT=======================\n");
    printf("%s\n", command);
    printf("================================================\n");
    printf(">REPORT %s*", buf);
    fflush(stdout);
}

int main(int argc, char ** argv){
    int returnCode;

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
         
        //clear();
        printScreenReport('a');
        // sleep(1);
    }                

    // restore the old settings
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);


    return 0;
}