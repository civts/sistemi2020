#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../utils.c"
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
// Flag for telling the report to give you only the stats for a given file, after that specify le list of arguments "--only /home/a.txt  /home/abac/ "
char onlyFlag[] = "--only";
bool only = false;
// flag to clear the filter
char clearFlag[] = "-clear";
bool clear = false;
// Flag for telling the report to quit
char quitFlag[] = "-q";
bool quit = false;

int main (int argc, char *argv[]){
    // QUI LA PARTE DA MODIFICARE EVENTUALMENTE
    // INSERIRE QUI TUTTI I POSSIBILI FLAG
    char * possibleFlags[] = {helpFlag,groupFlag,verboseFlag,tabFlag,compactFlag,forceReAnalysisFlag,onlyFlag,quitFlag};
    // SPECIFICARE LA DIMENSIONE
    int numberPossibleFlags =  7;
    // SPECIFICARE QUALI FLAG ACCETTANO ARGOMENTI, da passare in una stringa "adasda dasdas asdad". Esempio: "--only "patate ecmpa cobp""
    bool flagsWithArguments[7] = {false,false,false,false,false,true,true};
    // QUI RITORNO GLI ARGOMENTI PASSATI AL FLAG CHE HA ARGOMENTO
    char *arguments[7] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    //inizializzare i flag coi loro valori di default
    bool settedFlags[7] = {false,true,false,false,false,false,false};
    //inserire qui il testo in caso gli argomenti non siano validi
    char * invalid = "argomenti non validi";
    bool ok = checkArguments(argc,argv,possibleFlags,flagsWithArguments,numberPossibleFlags,settedFlags,arguments,invalid,true);
    int j=0;
    for(j=0;j<numberPossibleFlags;j++){
        if(settedFlags[j]){
            printf("%s ",possibleFlags[j]);
            if(arguments[j]!=NULL){
                printf("%s ",arguments[j]);
            }
        }
    }
    char* buff[4096];
    if(arguments[6]!=NULL){
        realpath(arguments[6],buff);
    }
    printf("%s ",buff);
    // printf(argv[1][0]!='-'?"true":"false");
  return 0;
}