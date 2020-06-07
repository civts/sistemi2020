#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    char *sossole = NULL;
    char *ptr;
    sossole =  realpath("./q.dwefc", sossole) ;

    if(sossole == NULL){
        printf("Sossole non inizializzato\n");
    } else {
        printf("Sossole: %s\n", sossole);
    }



    int i=0;
    while(1){
        wait_a_bit();
        i++;
        char* random = rand_string_alloc(10);
        if(i%15 == 0)sendTextMessageToReport(instanceOfMySelf->pipeCA, random);
        sendFinishedFilePacket(instanceOfMySelf->pipeCA, rand() % 100, rand() % 100);
    }
    

    return 0;
}