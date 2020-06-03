#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <stdlib.h>
#include <string.h>
#include "./report.h"
#define BUFFER_SIZE 4096
#define DEBUGGING true
char buf[BUFFER_SIZE], command[BUFFER_SIZE];
int counter = 0;

// Flag for telling the report to show help dialog
char helpFlag[] = "-h";
bool help = false;
// Flag for telling the report to show grouped stats (a-z instead of a,b,c...)
char groupFlag[] = "-g";
bool group = true;
// Flag for telling the report to show verbose stats
char verboseFlag[] = "-v";
bool verbose = false;
// Flag for tab mode
char tabFlag[] = "-t";
bool tab = false;
// Flag for telling the report to show compact
char compactFlag[] = "-c";
bool compact =false;
// Flag for telling the report to force another analysis. Discards all previous data
char forceReAnalysisFlag[] = "-r";
bool force = false;
// Flag for telling the report to give you only the stats for a given file, after that specify le list of arguments "--only /home/a.txt  /home/abac/ "
char onlyFlag[] = "--only";
bool only = false;
// Flag for telling the report to quit
char quitFlag[] = "-q";
bool quit = false;

void clear(){
//   const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
//   write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
    printf("\e[1;1H\e[2J");
}
// void clear(){
//     system("clear");
// }
void resetBuffer(char buffer[], int size){
    int i;
    for (i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = 0;
    }
}

int main(int argc, char ** argv){
    int retCode = 0;
    char * possibleFlags[] = {helpFlag,groupFlag,verboseFlag,tabFlag,compactFlag,forceReAnalysisFlag,onlyFlag,quitFlag};
    // SPECIFICARE LA DIMENSIONE
    int numberPossibleFlags =  8;
    // SPECIFICARE QUALI FLAG ACCETTANO ARGOMENTI, da passare in una stringa "adasda dasdas asdad". Esempio: "--only "patate ecmpa cobp""
    bool flagsWithArguments[] = {false,false,false,false,false,false,true};
    // QUI RITORNO GLI ARGOMENTI PASSATI AL FLAG CHE HA ARGOMENTO
    char *arguments[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    //inizializzare i flag coi loro valori di default
    bool settedFlags[] = {false,false,false,false,false,false,false,false};
    char* invalidtext = "argomenti non validi\n";
    bool valid = checkArguments(argc,argv,possibleFlags,flagsWithArguments,numberPossibleFlags,settedFlags,arguments,invalidtext,false);
    help = settedFlags[0]; group = settedFlags[1]; verbose = settedFlags[2]; tab= settedFlags[3];compact=settedFlags[4];force=settedFlags[5];only=settedFlags[6];quit=settedFlags[7];
    
    
    if(verbose + compact + tab >= 2){
        valid = false;
    }

    if(verbose + compact + tab == 0){
        tab = true;
    }
    if(only){
        if(arguments[6]!=NULL){
            ///parsing "/home  ../patetae/"->["/home", " ../patetae/"]
            //realPath per ogni path
        }
    } 
    if(valid){
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
        analyzerList *analyzers = constructorAnalyzerListEmpty();
        // PIPE opening
 
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
                    
                    // make switch on command
                    if (strcmp(command, "q") == 0){
                        break;
                    }
                } else {

                }
            }
            if (pipe == -1) {
                perror("No pipe");
                pipe = open(PATH_TO_PIPE, O_RDONLY);
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
                            gotErrorFilePacket(pipe,header,analyzers);
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
            //printErrors(analyzers);
            if(tab)
                printRecapTabela(analyzers,group);
            else if(verbose)
                printRecapVerbose(analyzers,group);
            else if(compact)
                printRecapCompact(analyzers);
            else if(help)
                printf("AIUTO\n");
            else if(quit)
                exit(0);
            
            //printRecapTabela(analyzers,group);

            //NON ELIMINARE QUESTE DUE STAMPE BUF E FFLUSH, SONO FONDAMENTALI PER IL CORRETTO FUNZIONAMENTO
            printf("> %s", buf);
            fflush(stdout);
            sleep(1);   
        }
        // restore the old settings
        tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    }else{
        printf("%s\n",invalidtext);
    }


    return 0;
}