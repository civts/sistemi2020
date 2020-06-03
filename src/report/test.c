#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../utils.c"

// QUESTE SONO COSE CHE USIAMO NOI, POTETE CAMBIARE LIBERAMENTE
// Flag for telling the report to show help dialog
char helpFlag[] = "-h";
bool help = false;
// Flag for telling the report to show grouped stats (a-z instead of a,b,c...)
// implies verbose
char groupFlag[] = "-g";
bool group = true;
// Flag for telling the report to show verbose stats
char verboseFlag[] = "-v";
bool verbose = false;
// Flag for tab mode
char tabFlag[] = "-t";
bool tab = false;
// Flag for telling the report to show verbose stats
char compactFlag[] = "-c";
bool compact =false;
// Flag for telling the report to force another analysis. Discards all previous data
char forceReAnalysisFlag[] = "-r";
bool force = false;
// Flag for telling the report to give you only the stats for a given file, after that specify le list of arguments "--only /home/a.txt  /home/abac/ "
char onlyFlag[] = "--only";
bool only = false;



// //DOCUMENTAZIONE
// // argc parametri del main
// // argv parametri del main
// // possibleFlags lista di char* contenente i valori ammissibili come argomenti
// // flagsWithArguments vettore di booleani, settare a true la posizione in cui il corrispondente flag richiede un altro parametro
// // numberPossibleFlags lunghezza della lista dei possibili parametri
// // settedFlags vettore di booleani, sono settati a true è presente l'argomento della corrispondente posizione. E' possibile inizializzarli a valori diversi da false per far finta che un argomento sia sempre implicito
// // arguments lista dove in posizione i si trova l'altro parametro inserito dall'utente per l'argomento i. Dovete fare la free!
// // invalid è la stringa di testo da mostrare in caso vi siano argomenti invalidi
// // printOnFailure specifica se mostare la  stringa di testo invalid se un argomento è invalido
// // return true se gli argomenti sono tutti validi, false altrimenti
// int checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char ** arguments, char* invalid,bool printOnFailure){
//     bool validity = true;
//     bool wait[numberPossibleFlags];
//     int j=0;
//     for(j=0;j<numberPossibleFlags;j++){
//         wait[j]=false;
//     }
//     // concateno due argomenti che non iniziano con - che sono vicini
//     int count = argc;
//     for(j=1;j<argc-1;j++){
//         if(argv[j][0]!='-' && argv[j+1][0]!='-'){
//             //concat the two
//             count--;
//             j++;
//         }
//     }
//     printf("count%d:\n",count);
//     char ** argvNoDouble = malloc(sizeof(char*)*count);
//     count=0;

//     argvNoDouble[0] = malloc(strlen(argv[0])+1);
//     strcpy(argvNoDouble[0],argv[0]);
//     count++;
//     for(j=1;j<argc-1;j++){
//         if(argv[j][0]!='-' && argv[j+1][0]!='-'){
//             argvNoDouble[count] = malloc(strlen(argv[j])+strlen(argv[j+1])+2);
//             stpcpy(argvNoDouble[count],argv[j]);
//             strcat(argvNoDouble[count]," ");
//             strcat(argvNoDouble[count],argv[j+1]);
//             j++;
//         }else{
//             argvNoDouble[count] = malloc(strlen(argv[j])+1);
//             strcpy(argvNoDouble[count],argv[j]);
//         }
//         count++;
//     }
//     argvNoDouble[count] = malloc(strlen(argv[j])+1);
//     strcpy(argvNoDouble[count],argv[j]);

//     for(j=0;j<count;j++){
//         printf("%s \n",argvNoDouble[j]);
//     }
//     int i=1;
//     while (i<count){
//         j = 0;
//         bool valid = false;
//         for(j=0;j<numberPossibleFlags;j++){
//             if(wait[j]){
//                 if(arguments[j]==NULL){
//                     arguments[j] = malloc(strlen(argvNoDouble[i]+1));
//                     strcpy(arguments[j],argvNoDouble[i]);
//                     wait[j]= false;
//                     i++;
//                     valid=true;
//                 }else{
//                      validity=false;
//                 }
//             }
//         }
//         if(i>=count)
//             break;
//         for(j=0;j<numberPossibleFlags;j++){
//             if(streq(argvNoDouble[i],possibleFlags[j])){
//                 settedFlags[j]=true;
//                 valid=true;
//                 if(flagsWithArguments[j]){
//                     wait[j]=true;
//                 }
//             }
//         }
//         if(!valid){
//             validity = false;
//         }
//         i++;
//     }   
//     for(j=0;j<numberPossibleFlags;j++){
//         if(wait[j]){
//             validity = false;
//         }
//     }
//     if(!validity){
//         for(j=0;j<numberPossibleFlags;j++){
//             settedFlags[j]=false;
//             if(arguments[j]!=NULL){
//                 free(arguments[j]);
//             }
//         }
//     }
//     if(printOnFailure)
//         printf("%s",invalid);
//     return validity;
// }

int checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char ** arguments, char* invalid,bool printOnFailure){
    bool validity = true;
    int j=0;
    int i=1;
    while (i<argc ){
        bool valid =false;
        for(j=0;j<numberPossibleFlags && i<argc ;j++){
            if(streq(argv[i],possibleFlags[j])){
                if(flagsWithArguments[j]){
                    bool serving = true;
                    i++;
                    while(i<argc && serving ){
                        if(argv[i][0]!='-'){
                            if(arguments[j]==NULL){
                                arguments[j] = malloc(strlen(argv[i]+1));
                                strcpy(arguments[j],argv[i]);
                                settedFlags[j]=true;
                                valid = true;
                            }else{
                                char* tmp = malloc(strlen(arguments[j])+strlen(argv[i])+2);
                                strcpy(tmp,arguments[j]);
                                strcat(tmp," ");
                                strcat(tmp,argv[i]);
                                free(arguments[j]);
                                arguments[j]=tmp;
                            }
                            i++;
                        }else{
                            serving=false;
                            i--;
                        }
                    }
                }else{
                    settedFlags[j]=true;
                    valid=true;
                }
            }
        }
        if(!valid){
            validity = false;
        }
        i++;
    }   
    if(printOnFailure && !validity)
        printf("%s",invalid);
    if(!validity){
        for(j=0;j<numberPossibleFlags;j++){
            settedFlags[j]=false;
            if(arguments[j]!=NULL){
                free(arguments[j]);
            }
        }
    }
    return validity;
}
int main (int argc, char *argv[]){
    // QUI LA PARTE DA MODIFICARE EVENTUALMENTE
    // INSERIRE QUI TUTTI I POSSIBILI FLAG
    char * possibleFlags[] = {helpFlag,groupFlag,verboseFlag,tabFlag,compactFlag,forceReAnalysisFlag,onlyFlag};
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
    // printf(argv[1][0]!='-'?"true":"false");
  return 0;
}