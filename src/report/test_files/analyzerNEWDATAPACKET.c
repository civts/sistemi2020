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
  int fd;

  // FIFO file path
  char *myfifo = "../myfifo";

  // Creating the named file(FIFO)
  // mkfifo(<pathname>, <permission>)
  mkfifo(myfifo, 0666);

  //fd = open(myfifo, O_WRONLY |  O_NONBLOCK);

  fd = open(myfifo, O_WRONLY);

  byte header[INT_SIZE+1] = {'\0','\0','\0','\0','\0'};
  header[0] = Q_NEW_DATA_CODE;
  fromIntToBytes(INT_SIZE * (ASCII_LENGTH+6),header+1);

  printf("CODE: %u\n",*(header));
  printf("DataSize: %u\n",(uint)fromBytesToInt(header+1));
  
  write(fd,header,INT_SIZE+1);

  byte dati[INT_SIZE * (ASCII_LENGTH+6)];
  fromIntToBytes(*pid,dati);
  fromIntToBytes(*idFile,dati+INT_SIZE);
  uint m = 0;
  fromIntToBytes(m,dati+2*INT_SIZE);
  uint i = 0;
  fromIntToBytes(i,dati+3*INT_SIZE);
  uint d = 500;
  fromIntToBytes(d,dati+4*INT_SIZE);
  uint charLetti = 256;
  fromIntToBytes(charLetti,dati+5*INT_SIZE);
  int j=0;
  for(j=0;j<ASCII_LENGTH;j++){
    fromIntToBytes(1,dati+(6+j)*INT_SIZE);
  }
  
  printf("pid: %u\n",fromBytesToInt(dati));
  printf("idFil: %u\n",fromBytesToInt(dati+INT_SIZE));
  printf("m: %u\n",fromBytesToInt(dati+2*INT_SIZE));
  printf("i: %u\n",fromBytesToInt(dati+3*INT_SIZE));
  printf("totCharFile: %u\n",fromBytesToInt(dati+4*INT_SIZE));
  printf("charQuestoPacchetto: %u\n",fromBytesToInt(dati+5*INT_SIZE));
  for(j=0;j<ASCII_LENGTH;j++){
    printf("char: '%c' , n: %u\n",j,fromBytesToInt(dati+(6+j)*INT_SIZE));
  }
  
  write(fd,dati,INT_SIZE * (ASCII_LENGTH+6));
  
  close(fd);
  
  return 0;
}
