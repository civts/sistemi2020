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


#define help 0
#define verbose 1
#define tab 2
#define compact 3
#define only 4
#define extended 5
#define force 6
#define quit 7
#define dumps_idx 8


// Flag for telling the report to show help dialog
char helpFlag[] = "-h";
// Flag for telling the report to show verbose stats
char verboseFlag[] = "-v";
// Flag for tab mode
char tabFlag[] = "-t";
// Flag for telling the report to show compact
char compactFlag[] = "-c";
// Flag for telling the report to give you only the stats for a given file, after that specify le list of arguments "--only /home/a.txt  /home/abac/ ", ? MAYBE THIS PART if a pid is specified in the same line, it will appied only to an analyzer with that pid
char onlyFlag[] = "--only";
// Flag for telling the report to show extended stats (a,b,c... instead of a-z)
char extendedFlag[] = "-e";
// Flag for telling the report to force another analysis. Discards all previous data
char forceReAnalysisFlag[] = "-r";
// Flag for telling the report to quit
char quitFlag[] = "-q";
// Flag for telling analyzers to start dumping to files
char dumpFlag[] = "--dump";

void clearScreen(){
//  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
//  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
    printf("\e[1;1H\e[2J");
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

//controlla se la combinazione di argomenti è valid
bool optionCombinationValid(bool *settedFlags){
    bool valid = true;
    // controllo non ci siano combinazioni invalide
    if(settedFlags[help] + settedFlags[tab]+ settedFlags[compact]+ settedFlags[only] + settedFlags[verbose]>= 2 ){
        valid = false;
    }
    // non è possibile utilizzare extened e tab assieme
    if(settedFlags[extended] && settedFlags[tab]){
        valid = false;
    }
    // non posso avere solo -e
    if(settedFlags[extended] && (settedFlags[help] + settedFlags[tab]+ settedFlags[compact]+ settedFlags[only] + settedFlags[verbose] == 0) )
        valid = false;
    // non posso avere -e e help
    if( settedFlags[extended] && settedFlags[help] )
        valid = false;
    return valid;
}
//funzione che fa il parsing dei file passati ad arguments trasformandoli in una lista di stringhe in resolvedPaths, lunghezza della lista in numArgs
bool parseArguments(char * arguments, int * numArgs,char ** resolvedPaths){
    char * unresolvedPaths[PATH_MAX];
    parser(arguments,numArgs,unresolvedPaths);
    bool valid = true;
    int i=0; int j=0;
    // for(i=0;i<*numArgs;i++){
    //     printf("%s ",unresolvedPaths[i]);
    // }
    //printf("\n");
    while(j<*numArgs){
        //printf("sto cercando di risolvere : %s \n",unresolvedPaths[j]);
        resolvedPaths[i]=realpath(unresolvedPaths[j],resolvedPaths[i]);
        //printf("risolto con  : %s \n",resolvedPaths[i]);
        if(inspectPath(resolvedPaths[i])!=-1){
            //printf("path valido : %s \n",resolvedPaths[i]);
            i++;
        }else{
            valid = false;
            //printf("path %s non valido\n",unresolvedPaths[j]);
        }
        j++;
    }
    for(j=0;j<*numArgs;j++){
        free(unresolvedPaths[j]);
    }
    //free(unresolvedPaths);

    // for(j=0;j<i;j++){
    //     printf("%s ",resolvedPaths[j]);
    // }
    // printf("\n");
    *numArgs = i;
    // unresolvedPaths = getArgumentsList(arguments[9],numFilesLog,unresolvedPaths);
    // i=0; j=0;
    // while(j<*numFilesLog){
    //     resolvedPaths[i]=realpath(unresolvedPaths[j],resolvedPaths[i]);
    //     if(inspectPath(resolvedPaths[i])!=-1){
    //         i++;
    //     }else{
    //         valid = false;
    //         printf("path %s non valido\n",unresolvedPaths[i]);
    //     }
    //     j++;
    // }
    return valid;
}
int main(int argc, char * argv[]){
    int i=0;
    int retCode = 0;
    char * possibleFlags[] = {helpFlag,verboseFlag,tabFlag,compactFlag,onlyFlag,extendedFlag,forceReAnalysisFlag,quitFlag, dumpFlag};
    // SPECIFICARE LA DIMENSIONE
    int numberPossibleFlags =  9;
    // SPECIFICARE QUALI FLAG ACCETTANO ARGOMENTI, da passare in una stringa "adasda dasdas asdad". Esempio: "--only "patate ecmpa cobp""
    bool flagsWithArguments[numberPossibleFlags];
    for(i = 0; i < numberPossibleFlags; i++){
        flagsWithArguments[i] = false;
    }
    flagsWithArguments[only] = true;
    // QUI RITORNO GLI ARGOMENTI PASSATI AL FLAG CHE HA ARGOMENTO
    char *arguments[numberPossibleFlags];
    for(i = 0; i < numberPossibleFlags; i++){
        arguments[i] = NULL;
    }
    //inizializzare i flag coi loro valori di default
    bool settedFlags[numberPossibleFlags];
    for(i = 0; i < numberPossibleFlags; i++){
        settedFlags[i] = false;
    }

    char* resolvedPaths[PATH_MAX];
    for(i=0;i<PATH_MAX;i++){
        resolvedPaths[i]=NULL;
    }

    int currentFilesCount = 0;
    bool valid = false;
    bool pathValid = true;
    //controllo gli argomenti siano validi
    if (checkArguments(argc-1,argv+1,possibleFlags,flagsWithArguments,numberPossibleFlags,settedFlags,arguments,NULL,false)){
        // controllo la combinazione sia valida
        if( optionCombinationValid(settedFlags)){
            //faccio ulteriore parsing solo se devo lavorare col flag --only
            if(settedFlags[only]){
                // provo ad effettuare il parsing
                if(parseArguments(arguments[only],&currentFilesCount,resolvedPaths)){
                    //parsing riuscito
                    valid=true; pathValid=true;
                }else{
                    valid=false; settedFlags[only]=false; pathValid=false;
                }
            }else{
                valid=true;
            }
        }
    }

    // for(i=0;i<currentFilesCount;i++){
    //         printf("%s ",resolvedPaths[i]);
    // }
    // printf("\n");
    // printf("%d\n",true);
    // printf("%d\n",false);
    if(valid){
        // default view is tab
        if(settedFlags[help] + settedFlags[tab]+ settedFlags[compact]+ settedFlags[only] + settedFlags[verbose]==0 ){
            settedFlags[tab] = true;
        }

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
        analyzers->dumps = settedFlags[dumps_idx];
        // PIPE opening
 
        int pipe = open(PATH_TO_PIPE, O_RDONLY );
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

                    int numCommands=0;
                    int i=0;
                    // resetto valid, potrebbe non essere valido
                    valid = false;
                    pathValid = true;
                    // copia dei flag su cui lavorare
                    bool copyFlags[numberPossibleFlags];
                    for(i = 0; i < numberPossibleFlags; i++){
                        copyFlags[i] = false;
                    }
                    // reset degli argomenti
                    for(i=0;i<numberPossibleFlags;i++){ if(arguments[i]!=NULL){free(arguments[i]);arguments[i]=NULL;}}
                    // buffer temporaneo
                    int tmpFilesCount = 0;
                    // buffer temporaneo dove inserire i percorsi
                    char* tmpResolvedPaths[PATH_MAX];
                    for(i=0;i<PATH_MAX;i++){
                        tmpResolvedPaths[i]=NULL;
                    }
                    // spezzo i comandi in stringhe
                    char* spliced[PATH_MAX];

                    parser(command,&numCommands,spliced);
                    // for(i=0;i<numCommands;i++){
                    //     printf("%s\n,",spliced[i]);
                    // }
                    //controllo se siano validi
                    if(checkArguments(numCommands,spliced,possibleFlags,flagsWithArguments,numberPossibleFlags,copyFlags,arguments,NULL,false)){
                        //controllo se la specifica combinazione sia valida
                        if(optionCombinationValid(copyFlags)){
                            //faccio ulteriore parsing solo se devo lavorare col flag --only
                           if(copyFlags[only]){
                                // provo ad effettuare il parsing
                                
                                if(parseArguments(arguments[only],&tmpFilesCount,tmpResolvedPaths)){
                                    //parsing riuscito
                                    valid=true; pathValid=true;
                                }else{
                                    valid=false; copyFlags[only]=false; pathValid=false;
                                }
                            }else{
                                valid=true;
                            }
                        }
                    }
                    //parsing riuscito, argomenti validi e altre cose
                    if(valid){
                        // se valido, copio i dati
                        for(i=0;i<numberPossibleFlags;i++){
                            settedFlags[i] = copyFlags[i];
                        }
                        currentFilesCount = tmpFilesCount;
                        for(i=0;i<currentFilesCount;i++){
                            resolvedPaths[i] = malloc(strlen(tmpResolvedPaths[i])+1);
                            strcpy(resolvedPaths[i],tmpResolvedPaths[i]);
                            //printf("%s ",resolvedPaths[i]);
                        }
                        //Check if I need to update the --dump flag
                        if(settedFlags[dumps_idx])
                            analyzers->dumps = true;
                    }

                } else {

                }
            }
            if (pipe == -1) {
                perror("No pipe");
                pipe = open(PATH_TO_PIPE, O_RDONLY);
            }else{
                //lettura di 1 pacchetto
                reportReadOnePacket(pipe, analyzers);
            }
            clear();
            //clearScreen();
            //printErrors(analyzers);
            if(settedFlags[tab])
                printRecapTabela(analyzers);
            if(settedFlags[verbose])
                printRecapVerbose(analyzers,!settedFlags[extended]);
            if(settedFlags[compact])
                printRecapCompact(analyzers);
            if(settedFlags[only]){
                //prints only the files in 
                int i=0;
                for(i=0;i<currentFilesCount;i++){
                    printf("%s \n",resolvedPaths[i]);
                }
                printFilesOrFolder(analyzers,currentFilesCount,resolvedPaths,!settedFlags[extended]);
            }
            if(settedFlags[help])
                printHelp();
            if(settedFlags[quit]){
                byte l[0];
                int ex = read(pipe,l,1);
                while (ex!=0){ ex = read(pipe,l,1);};
                destructoraAnalyzerList(analyzers);
                exit(0);
            }
            if(settedFlags[force]){
                destructoraAnalyzerList(analyzers);
                analyzers = constructorAnalyzerListEmpty();
            }
            if(!pathValid)
                printf("Path non valido\n");
            if(!valid && pathValid)
                printf("Comando non valido, -h per vedere la sintassi\n");


            //NON ELIMINARE QUESTE DUE STAMPE BUF E FFLUSH, SONO FONDAMENTALI PER IL CORRETTO FUNZIONAMENTO
            printf("> %s", buf);
            fflush(stdout);
            sleep(1);   
        }
        // restore the old settings
        tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    }else{
        printf("%s\n","Arguments not valid");
    }


    return 0;
}