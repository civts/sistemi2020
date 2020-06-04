#include <string.h>
#include "utils.c"
#include "datastructures/namesList.c"

int checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char ** arguments, char* invalid,bool printOnFailure){
    bool validity = true;
    int j=0;
    int i=0;
    while (i<argc ){
        bool valid =false;
        for(j=0;j<numberPossibleFlags && i<argc ;j++){
            if(!strcmp(argv[i],possibleFlags[j])){
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

string *getArgumentsList(char *arguments, int *numArgs, string *argumentsList){
    argumentsList = NULL;
    *numArgs = 0;
    if(arguments == NULL){
        // nothing
    } else {
        NamesList *list = constructorNamesList();    
        int   offset  = 0;
        char* oldPointer = arguments;
        char* pointer = strstr(arguments, " ");
        while(pointer != NULL){
            *numArgs = *numArgs+1;
            int argLength = pointer - oldPointer; 
            string argument = malloc(argLength+1);
            memcpy(argument, arguments+offset, argLength);
            argument[argLength] = '\0';
            oldPointer = pointer;
            offset = pointer - arguments + 1;
            appendNameToNamesList(list, argument);
            pointer = NULL;
            pointer = strstr(oldPointer+1, " ");
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
            argumentsList[i][strlen(node->name)] = '\0';
            // printf("name: %s\n", argumentsList[i]);
            node = node->next;
            i++;
        }
    }
    return argumentsList;
}
