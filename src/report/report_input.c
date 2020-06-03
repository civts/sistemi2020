#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <stdlib.h>
#include <string.h>
#include "./report.h"
#include <limits.h>
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
// Flag for telling the report to show extended stats (a,b,c... instead of a-z)
char extendedFlag[] = "-e";
bool extended = false;
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
// Flag for telling the report to give you only the stats for a given file, after that specify le list of arguments "--only /home/a.txt  /home/abac/ ", ? MAYBE THIS PART if a pid is specified in the same line, it will appied only to an analyzer with that pid
char onlyFlag[] = "--only";
bool only = false;
// Flag for telling the report to quit
char quitFlag[] = "-q";
bool quit = false;

void clearScreen(){
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

void parseArguments(bool *valid,bool *help, bool * group, bool * extended, bool * verbose, bool * tab, bool * compact, bool * force, bool * only, bool * quit, bool * settedFlags,char ** arguments,char ** resolvedPaths){
    if(*valid){
        *help = settedFlags[0]; *group = settedFlags[1]; *extended=settedFlags[2]; *verbose = settedFlags[3]; *tab= settedFlags[4]; *compact=settedFlags[5]; *force=settedFlags[6]; *only=settedFlags[7]; *quit=settedFlags[8];
    }
    // invalid combinatons
    if(*verbose + *compact + *tab + *only >= 2){
        *valid = false;
    }
    if(*group && *extended){
        *valid = false;
    }
    // default group is on
    if(!*group && !*extended){
        *group = true;
    }
    // default view is tab
    if(*verbose + *compact + *tab == 0){
       *tab = true;
    }
    int numArgs;
    string* unresolvedPaths = getArgumentsList(arguments[7],&numArgs,unresolvedPaths);
    int i=0; int j=0;
    while(j<numArgs){
        resolvedPaths[i]=realpath(unresolvedPaths[j],resolvedPaths[i]);
        if(inspectPath(resolvedPaths[i])!=-1){
            i++;
        }else{
            *valid = false;
            printf("path %s non valido\n",unresolvedPaths[i]);
        }
        j++;
    }
}
int main(int argc, char ** argv){
    int retCode = 0;
    char * possibleFlags[] = {helpFlag,groupFlag,extendedFlag,verboseFlag,tabFlag,compactFlag,forceReAnalysisFlag,onlyFlag,quitFlag};
    // SPECIFICARE LA DIMENSIONE
    int numberPossibleFlags =  9;
    // SPECIFICARE QUALI FLAG ACCETTANO ARGOMENTI, da passare in una stringa "adasda dasdas asdad". Esempio: "--only "patate ecmpa cobp""
    bool flagsWithArguments[] = {false,false,false,false,false,false,false,true,false};
    // QUI RITORNO GLI ARGOMENTI PASSATI AL FLAG CHE HA ARGOMENTO
    char *arguments[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    //inizializzare i flag coi loro valori di default
    bool settedFlags[] = {false,false,false,false,false,false,false,false,false};
    char* invalidtext = "argomenti non validi\n";
    bool valid = checkArguments(argc-1,argv+1,possibleFlags,flagsWithArguments,numberPossibleFlags,settedFlags,arguments,invalidtext,false);
    char* resolvedPaths[PATH_MAX];

    if(valid){
        parseArguments(&valid,&help,&group,&extended,&verbose,&tab,&compact,&force,&only,&quit,settedFlags,arguments,resolvedPaths);

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
                    int numCommands;
                    string* spliced = getArgumentsList(command,&numCommands,spliced);
                    if(checkArguments(numCommands,spliced,possibleFlags,flagsWithArguments,numberPossibleFlags,settedFlags,arguments,invalidtext,false))
                        parseArguments(&valid,&help,&group,&extended,&verbose,&tab,&compact,&force,&only,&quit,settedFlags,arguments,resolvedPaths);
                } else {

                }
            }
            if (pipe == -1) {
                perror("No pipe");
                pipe = open(PATH_TO_PIPE, O_RDONLY);
            }else{
                //lettura di 1 pacchetto
                reportReadOnePacket(pipe,analyzers);
            }
            clearScreen();
            //printErrors(analyzers);
            if(tab)
                printRecapTabela(analyzers,group);
            if(verbose)
                printRecapVerbose(analyzers,group);
            if(compact)
                printRecapCompact(analyzers);
            if(help)
                printf("AIUTO\n");
            if(quit)
                exit(1);
            if(force){
                destructoraAnalyzerList(analyzers);
                analyzers = constructorAnalyzerListEmpty();
            }
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