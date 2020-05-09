#include "../utils.c"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const int FILESTOGEN = 1;

typedef char *string;

int main(int argc, char **argv) {
  int fd;

  // FIFO file path
  char *myfifo = "/tmp/myfifo";

  // Creating the named file(FIFO)
  // mkfifo(<pathname>, <permission>)
  mkfifo(myfifo, 0666);

  // Open FIFO for write only
  int i;
  for (i = 0; i < FILESTOGEN; i++) {
    fd = open(myfifo, O_WRONLY);
    // packet code PRIMO BYTE FUNZIONANTE E VERIFICATO
    // fd, const void * buf, size_t count
    //const int * buff = malloc(INT_SIZE);
    //(*buff) = '0
    byte codeByte[1] = {'\0'};
    //NON PUOI USARE fromIntToBytes perché fa la conversione su 4 byte non 1
    //fromIntToBytes(code,codeBytes);
    write(fd, codeByte, 1);
    //write(1,codeBytes,1);

    //printf("statusCode %u",code);
    //printf("statusCode %u",codeBytes[0]);//
    char *path = "/abcd/file";
    byte bytes[INT_SIZE];
    // path length FUNZIONANTE E VERIFICATO

    fromIntToBytes((uint)strlen(path), bytes);
    write(fd, bytes, INT_SIZE);

    //printf("pathLength %u",(uint)strlen(path));
    //printf("pathLength %u",pathLength);
    // path FUNZIONANTE  E VERIFICATO

    write(fd, path, strlen(path)+1);
    //write(fd, path, strlen(path) );

    // from folder/not flag VERIFICATO E FUNZIONANTE
    byte folderFlag[1] = {'\0'+1};
    //fromIntToBytes((uint)1,folderFlag);
    write(fd, folderFlag, 1);
    //printf("folder %u",folderFlag[0]);
    // total chars VERIFICATO E FUNZIONANTE
    //uint totalChars = i;
    uint totalChars = ASCII_LENGTH;
    fromIntToBytes(totalChars, bytes);
    //printf("caratteri tot %u",totalChars);
    //printf("caratteri tot %u",bytes[3]);
    write(fd, bytes, INT_SIZE);
    int j;
    uint characters[ASCII_LENGTH];
    for (j = 0; j < ASCII_LENGTH; j++) {
        characters[j] = 1;

    }
    for (j = 0; j < ASCII_LENGTH; j++) {
        fromIntToBytes(characters[j],bytes);
        write(fd, bytes, INT_SIZE);

    }

    close(fd);
  }
  return 0;
}
