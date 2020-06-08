#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../common/utils.h"
#include "../common/datastructures/namesList.h"

#define flag_analyze 0
#define flag_i 1
#define flag_s 2
#define flag_h 3
#define flag_show 4
#define flag_rem 5
#define flag_add 6
#define flag_n 7
#define flag_m 8
#define flag_quit 9
#define flag_main 10

char* rand_string_alloc(int);

int numberPossibleFlags = 10; 
string possibleFlags[]  = {"-analyze", "-i",  "-s", "-h",  "-show", "-rem", "-add", "-n", "-m", "-quit", "-main"};
bool   flagsWithArgs[]  = {false,      false, false, false, false,   true,   true,   true,  true, false,  false};

bool   settedFlags[]    = {false,      false, false, false, false,   false,  false,  false, false,false,  false};
string arguments[10];
string invalidPhrase    = "Wrong command syntax, try command '-h' for help.\n";

void clear(){
    system("clear");
}

bool checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char **arguments, char* invalid,bool printOnFailure){
    bool validity = true;
    int j=0;
    int i=0;
    while (i<argc ){
        bool valid =false;
        for(j=0;j<numberPossibleFlags && i<argc ;j++){
            if(strcmp(argv[i],possibleFlags[j])==0){
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

string *getArgumentsList(char *arguments, int *numArgs){
    string *argumentsList = NULL;
    *numArgs = 0;
    if(arguments == NULL){
        // nothing
        // printf("No arguments\n");
    } else {
        NamesList *list = constructorNamesList();    
        int   offset  = 0;
        char* oldPointer = arguments;
        char* pointer = strstr(arguments, " ");
        while(pointer != NULL){
            int argLength = pointer - oldPointer;
            if(argLength>0){
                *numArgs = *numArgs+1;
                string argument = malloc(argLength+1);
                memcpy(argument, arguments+offset, argLength);
                argument[argLength] = '\0';
                offset = pointer - arguments + 1;
                appendNameToNamesList(list, argument);
            } 
            oldPointer = pointer+1;
            pointer = NULL;
            pointer = strstr(oldPointer, " ");
        }
        *numArgs = *numArgs+1;
        int argLength = strlen(arguments) - offset; 
        string argument = (string)malloc(argLength+1);
        // printf("Arglength: %d\n", argLength);
        memcpy(argument, arguments+offset, argLength);
        argument[argLength] = '\0';

        appendNameToNamesList(list, argument);

        argumentsList = (string *)malloc(*numArgs);
        int i=0;
        NodeName *node = list->first;
        // printf("num args: %d\n", *numArgs);
        while(node != NULL){
            argumentsList[i] = (string)malloc(strlen(node->name)+1);
            strcpy(argumentsList[i], node->name);
            // argumentsList[i][strlen(node->name)] = '\0';
            // printf("name: %s||\n", argumentsList[i]);
            // printf("strlen: %d\n", strlen(node->name));
            // printf("strlen: %d\n", strlen(argumentsList[i]));
            node = node->next;
            i++;
        }
    }
    return argumentsList;
}

void parser(char real[], int * no,char **out){
    char *str = strdup(real);

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

    free(str);
}

static char *rand_string(char *str, int size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    int n;
    if (size) {
        --size;
        for (n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

char* rand_string_alloc(int size)
{
     char *s = malloc(size + 1);
     if (s) {
         rand_string(s, size);
     }
     return s;
}

// void wait_a_bit(){
//     long long int i;
//     for (i=0; i<999999; i++){}
// }

// int main(){

//     while(1){
//         wait_a_bit();
//         char *str = rand_string_alloc(10);
//         printf("String: %s\n", str);
//     }

//     return 0;
// }