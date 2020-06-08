#include "../../packet_codes.h"
#include "../../utils.c"

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
  int fd;
  // FIFO file path
  char *myfifo = "../myfifo";
  // Creating the named file(FIFO)
  // mkfifo(<pathname>, <permission>)
  mkfifo(myfifo, 0666);
  // fd = open(myfifo, O_WRONLY |  O_NONBLOCK);
  fd = open(myfifo, O_WRONLY);
  
  byte header[INT_SIZE+1] = {'\0','\0','\0','\0','\0'};
  header[0] = A_DELETE_FILE_CODE;
  fromIntToBytes(INT_SIZE*2,header+1);
  printf("CODE: %u\n",*(header));
  printf("DataSize: %u\n",(uint)fromBytesToInt(header+1));
  write(fd,header,INT_SIZE+1);

  byte dati[INT_SIZE*2];
  fromIntToBytes(*pid,dati);
  fromIntToBytes(*idFile,dati+INT_SIZE);
  printf("pid: %u\n",(uint)fromBytesToInt(dati));
  printf("idFile: %u\n",(uint)fromBytesToInt(dati+INT_SIZE));
  write(fd,dati,INT_SIZE*2);
  close(fd);
  return 0;
}
