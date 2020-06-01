#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <stdlib.h>
#include <string.h>
#include "./report.h"
#define BUFFER_SIZE 4096
char buf[BUFFER_SIZE], command[BUFFER_SIZE];
int counter = 0;


void clear(){
    system("clear");
}

void resetBuffer(char buffer[], int size){
    int i;
    for (i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = 0;
    }
}

int main(int argc, const char *argv[]){
    // SETTARE DEI FLAG GLOBALI CON GLI ARGOMENTI

    // MAGIE DI BOZZO
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

    int retCode = 0;
    // QUI METTIAMO LA LISTA DEGLI ANALIZER
    // This is where the state is stored: it contains the references to the
    // "objects" representing the files and their stats.
    analyzerList *analyzers = constructorAnalyzerListEmpty();
    //APRO LA PIPE. SI PUÒ FARE CHE RITENTA FINO A QUANDO NON FALLISCE
    int pipe = open(PATH_TO_PIPE, O_RDONLY);

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
                // QUI IN COMMAND C'E' LA STRINGA DI INPUT DELL'UTENTE!!!! CAMBIARE DEI FLAG GLOBALI A SECONDA DELL'INPUT
                // QUI GRANDISSIMO QUI
                // NOTAMI
                // make switch on command
                if (strcmp(command, "q") == 0){
                    break;
                }
                // qui mettiamo il flag sulla stampa di help
                if (strcmp(command, "--help") == 0){
                    break;
                }
            } else {

            }
        }
        if (pipe == -1) {
            perror("No pipe");
        }else{
            // AGGIUNGERE UN WHILE/FOR PER LEGGERE PIÙ PACCHETTI DI 1 AL SECONDO, TIPO CON UNA SYSCALL PER SAPERE IL TEMPO PASSATO ?
            // Reading new packet and taking appropriate action
            byte header[INT_SIZE + 1];
            int rdHeader = read(pipe, header, INT_SIZE + 1);
            if (rdHeader == INT_SIZE + 1) {
                if (DEBUGGING)
                    printf("Got new packet with code %d\n", header[0]);
                switch (header[0]) {
                    case Q_NEW_DATA_CODE:
                        gotNewDataPacket(pipe, header, analyzers);
                        break;
                    case Q_FILE_ERROR_CODE:
                        // TODO (What do we do on error? print to the user and forget about it?)
                        break;
                    case A_NEW_FILE_COMPLETE:
                        gotAddFilePacket(pipe, header, analyzers);
                        break;
                    case A_NEW_FILE_INCOMPLETE_PART1:
                        got1stPathPartPacket(pipe, header, analyzers);
                        break;
                    case A_NEW_FILE_INCOMPLETE_PART2:
                        got2ndPathPartPacket(pipe, header, analyzers);
                        break;
                    case A_DELETE_FILE_CODE:
                        gotDeleteFilePacket(pipe, header, analyzers);
                        break;
                    case A_DELETE_FOLDER:
                        gotDeleteFolderPacket(pipe, header, analyzers);
                        break;
                    case A_DELETE_FOLDER_INCOMPLETE_PART1:
                        got1stPathPartDeleteFolderPacket(pipe, header, analyzers);
                        break;
                    case A_DELETE_FOLDER_INCOMPLETE_PART2:
                        got2ndPathPartDeleteFolderPacket(pipe, header, analyzers);
                        break;
                }
            }
        }
        clear();
        // LA STAMPA DEVE ESSERE DIVERSA A SECONDA DEI FLAG GLOBALI
        printRecapCompact(analyzers);
        printf("> %s", buf);
        fflush(stdout);
        sleep(1);   
    }
              
    // restore the old settings
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;

}