#include "parser.h"

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
                                arguments[j] = malloc(strlen(argv[i])+1);
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