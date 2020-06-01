#include "../../utils.c"
#include "../../packet_codes.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


typedef char *string;

int main(int argc, char **argv) {

  printf("Write the PID\n");
  uint *pid = malloc(sizeof(uint));
  scanf("%u",pid);
  printf("Write the FILEID\n");
  uint *idFile = malloc(sizeof(uint));
  scanf("%u",idFile);  
  printf("Write the PATH\n");
  char *path = malloc(1024);
  scanf("%s",path);
  
  int fd;
  // FIFO file path
  char *myfifo = "../myfifo";
  // Creating the named file(FIFO)
  // mkfifo(<pathname>, <permission>)
  mkfifo(myfifo, 0666);
  // fd = open(myfifo, O_WRONLY |  O_NONBLOCK);
  fd = open(myfifo, O_WRONLY);

  byte header[INT_SIZE+1] = {'\0','\0','\0','\0','\0'};
  header[0] = A_NEW_FILE_COMPLETE;
  fromIntToBytes(INT_SIZE*2+strlen(path)+1,header+1);
  printf("CODE: %u\n",*(header));
  printf("DataSize: %u\n",(uint)fromBytesToInt(header+1));
  write(fd,header,INT_SIZE+1);

  byte dati[INT_SIZE*3+strlen(path)+1];
  fromIntToBytes(*pid,dati);
  fromIntToBytes(*idFile,dati+INT_SIZE);
  strcpy(dati+2*INT_SIZE,path);
  printf("pid: %u\n",(uint)fromBytesToInt(dati));
  printf("idFile: %u\n",(uint)fromBytesToInt(dati+INT_SIZE));
  printf("path: %s\n",dati+(2*INT_SIZE));
  write(fd,dati,INT_SIZE*2+strlen(path)+1);
  close(fd);
  return 0;
}
