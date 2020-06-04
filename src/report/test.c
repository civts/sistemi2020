//#include "../utils.h"
// #ifndef bool
// typedef unsigned char bool;
// typedef unsigned long long ull;
// #define false 0
// #define true 1
// #endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "./data_structures/namesList.h"
typedef int bool;
typedef unsigned long long ull;
#define false 0
#define true 1
//#include "prova.h"
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

// bool checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char ** arguments, char* invalid,bool printOnFailure){
//     bool validity = true;
//     int j=0;
//     int i=0;
//     while (i<argc ){
//         bool valid =false;
//         printf("%s argument\n",argv[i]);
//         for(j=0;j<numberPossibleFlags && i<argc ;j++){
//             if(strcmp(argv[i],possibleFlags[j])==0){
//                 if(flagsWithArguments[j]){
//                     bool serving = true;
//                     i++;
//                     while(i<argc && serving ){
//                         if(argv[i][0]!='-'){
//                             if(arguments[j]==NULL){
//                                 arguments[j] = malloc(strlen(argv[i]+1));
//                                 strcpy(arguments[j],argv[i]);
//                                 settedFlags[j]=true;
//                                 valid = true;
//                             }else{
//                                 char* tmp = malloc(strlen(arguments[j])+strlen(argv[i])+2);
//                                 strcpy(tmp,arguments[j]);
//                                 strcat(tmp," ");
//                                 strcat(tmp,argv[i]);
//                                 free(arguments[j]);
//                                 arguments[j]=tmp;
//                             }
//                             i++;
//                         }else{
//                             serving=false;
//                             i--;
//                         }
//                     }
//                 }else{
//                     settedFlags[j]=true;
//                     valid=true;
//                 }
//             }
//         }
//         if(!valid){
//             validity = false;
//         }
//         i++;
//     }   
//     if(printOnFailure && !validity)
//         printf("%s",invalid);
//     if(!validity){
//         for(j=0;j<numberPossibleFlags;j++){
//             settedFlags[j]=false;
//             if(arguments[j]!=NULL){
//                 free(arguments[j]);
//             }
//         }
//     }
//     return validity;
// }
void parser(char str[], int * no,char **out){
    //char str[] ="- This, a sample string.";
    char * pch;
    pch = strtok (str," \t\r\n\f\v");
    int i=0;
    while (pch != NULL)
    {
        out[i] = strdup(pch);
        pch = strtok (NULL, " \t\r\n\f\v");
        i++;
    }
    *no=i;
}
int main (int argc, char *argv[]){

    // char * possibleFlags[] = {helpFlag,verboseFlag,tabFlag,compactFlag,onlyFlag,extendedFlag,forceReAnalysisFlag,quitFlag};
    // // SPECIFICARE LA DIMENSIONE
    // int numberPossibleFlags =  8;
    // // SPECIFICARE QUALI FLAG ACCETTANO ARGOMENTI, da passare in una stringa "adasda dasdas asdad". Esempio: "--only "patate ecmpa cobp""
    // bool flagsWithArguments[] = {false,false,false,false,true,false,false,true};
    // // QUI RITORNO GLI ARGOMENTI PASSATI AL FLAG CHE HA ARGOMENTO
    // char *arguments[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    // //inizializzare i flag coi loro valori di default
    // bool settedFlags[] = {false,false,false,false,false,false,false,false};
    // bool valid = checkArguments(argc-1,argv+1,possibleFlags,flagsWithArguments,numberPossibleFlags,settedFlags,arguments,NULL,false);
    // if(valid==1){
    //   printf("validi true\n");
    // }else{
    //   printf("invalidi false\n");
    // }
    // int x = 3;
    // string * names = getArgumentsList("patate e cetrioli",&x,names);
    // int i=0;
    // for(i=0;i<x;i++){
    //     printf("%s\n",names[i]);
    // }
    // char * res[] = {NULL,NULL,NULL};
    // char * a = "patate e cetrioli";
    // //parser(a,&x,res);
    // char * tok;
    // tok = strtok(a,"e");
    // printf("%s\n",tok);


    // char str[] ="- This,     a   sample st  ring.";
    // char * pch;
    // printf ("Splitting string \"%s\" into tokens:\n",str);
    // pch = strtok (str," \t\r\n\f\v");
    // while (pch != NULL)
    // {
    //     printf ("%s\n",pch);
    //     pch = strtok (NULL, " ,.-");
    // }
    char  str[] = "asdad as das sada er vera";
    int x =0 ;
    //char * out[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    char *out[4096];
    // //char str[] ="- This, a sample string.";
    // char * pch;
    // printf ("Splitting string \"%s\" into tokens:\n",str);
    // pch = strtok (str," \t\r\n\f\v");
    // printf("%s,\n",pch);
    // int i=0;
    // while (pch != NULL)
    // {
    //     //out[i] = strdup(pch);
    //     pch = strtok (NULL, " \t\r\n\f\v");
    //     i++;
    // }
    int i;
    parser(str,&x,out);
    for(i=0;i<x;i++){
        printf("%s\n",out[i]);
    }
  return 0;

}