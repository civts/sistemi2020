
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#ifndef UTILS_H
#define UTILS_H
#define INT_SIZE 4
// How many characters we are considering
#define ASCII_LENGTH 256
#define NUM_OCCURENCES 256

//#ifndef bool
typedef int bool;
typedef unsigned long long ull;
#define false 0
#define true 1
//#endif

#define DEBUGGING true
typedef unsigned int uint;
typedef unsigned char byte;
typedef char* string;

// #define false 0
// #define true 1
// typedef int bool;

uint fromBytesToInt(byte *);
void fromIntToBytes(uint, byte[]);

// convert 4 bytes in unsigned int (little endian)
uint fromBytesToInt(byte *bytes) {
  uint result = 0, base = 1;
  int i;
  for (i = INT_SIZE - 1; i >= 0; i--) {
    result += bytes[i] * base;
    base *= 256;
  }

  return result;
}

// convert an unsigned int in 4 bytes (little endian)
void fromIntToBytes(uint value, byte out[]) {
  uint result = 0, base = 0;
  int i;
  for (i = INT_SIZE - 1; i >= 0; i--) {
    out[i] = ((value >> base) & 0xFF);
    base += 8;
  }
}

// Util function for knowing if two strings are the same TESTED✔️
bool streq(const char *p1, const char *p2) { return !strcmp(p1, p2); }

// Returns if the array of strings [pool] contains the given option.
// TESTED✔️
// (useful for checking that params are correct)
bool contains(int poolLength, const char *pool[], const char *option) {
  int i;
  bool found = false;
  for (i = 0; i < poolLength; i++)
    if (streq(option, pool[i])) {
      found = true;
      break;
    }
  return found;
}

// Ensures the given string is long at most maxLen. If it is longer the middle
// part is cut out and three dots are inserted instead.
// MODIFIES the original string
// Please use maxLen >= 6 (else it returns src). TESTED✔️
void trimStringToLength(char *src, int maxLen) {
  int pathLen = strlen(src);
  if (pathLen > maxLen && maxLen >= 6) {
    char tp[maxLen + 1];
    int i, split = maxLen / 2 - 3;
    for (i = 0; i < split; i++) {
      tp[i] = src[i];
    }
    for (i = split; i < split + 3; i++) {
      tp[i] = '.';
    }
    for (i = split + 3; i < maxLen; i++) {
      tp[i] = src[pathLen - (maxLen - i)];
    }
    tp[maxLen] = '\0';
    strcpy(src, tp);
  }
}

// Struct for keeping how many char of each group are present in a given
// file/folder, the total number of chars and how many we have read
typedef struct {
  uint az, AZ, digits, punctuation, spaces, otherChars;
  uint totalCharsRead, totalChars;
} charGroupStats;

// // FUNZIONE CHE GESTISCE IL PARSING
// //DOCUMENTAZIONE
// // argc parametri del main. Da chiamare con 1
// // argv parametri del main. Da chiamare con +1
// // possibleFlags lista di char* contenente i valori ammissibili come argomenti. DEVONO NECESSARIAMENTE INIZIARE CON "-"
// // flagsWithArguments vettore di booleani, settare a true la posizione in cui il corrispondente flag richiede un altro parametro
// // numberPossibleFlags lunghezza della lista dei possibili parametri
// // settedFlags vettore di booleani, sono settati a true è presente l'argomento della corrispondente posizione. E' possibile inizializzarli a valori diversi da false per far finta che un argomento sia sempre implicito
// // arguments lista dove in posizione i si trova l'altro parametro inserito dall'utente per l'argomento i. Dovete fare la free!. Nel caso di argomenti multipli "-i ciao patate" l'argomento viene concatenato con lo spazio. Dunque "ciao patate"
// // invalid è la stringa di testo da mostrare in caso vi siano argomenti invalidi
// // printOnFailure specifica se mostare la  stringa di testo invalid se un argomento è invalido
// // return true se gli argomenti sono tutti validi, false altrimenti. Se gli argomenti sono invalidi, tutti i flag sono messi a false e i parametri degli argomenti sono disallocati

// Given a path to a file/folder it returns:
// -1 : if it does not exist
//  0 : if it is a file and it exists
//  1 : if it is is a folder and it exists
int inspectPath(const char *path){
    struct stat path_stat;
    int returnCode = -1;
    if (path != NULL && stat(path, &path_stat) == 0){
        if (S_ISREG(path_stat.st_mode)){
            returnCode = 0;
        } else if (S_ISDIR(path_stat.st_mode)){
            returnCode = 1;
        }
    }
    return returnCode;
}


bool checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char ** arguments, char* invalid,bool printOnFailure){
    bool validity = true;
    int j=0;
    int i=0;
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
#endif
