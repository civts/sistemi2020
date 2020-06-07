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


int main(int argc, char * argv[]){
   char p1[] = "/ciao/";
   char p2 [] = "/ciao/sdfsdfs";
   if(pathIsContained(p1,p2)){
       printf("yessa\n");
   }
   fwsList * lista = constructorFwsListEmpty();
   fileWithStats *x = constructorFWS("/ciao/pasda",0,0,NULL)
   fwsListAppend(lista,x);
   fileWithStats *item = fwsListGetElementByPathContained(lista,"/ciao/");
   if(pathIsContained(x->path,)){
       printf("yessa\n");
   }
   if(item!=NULL){
       fwsPrint(item);
   }
    return 0;
}