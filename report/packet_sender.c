#include <stdio.h>
#include "packets.h"
#include <fcntl.h>
#include <stdio.h> 
#include <string.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 

int main(){
    int fd[2];
    const char *myfifo = "./myfifo";

    int occurences[256];
    int i;
    for (i=0; i<256; i++){
        occurences[i] = 0;
    }
    occurences['a'] = 5;
    mkfifo(myfifo, 0666);
    fd[WRITE] = open(myfifo, O_WRONLY);
    newFileNameToReportPacket(fd, 30, 0, "/tmp/file1.txt");
    sendOccurencesPacketToReport(fd, 30, 0, 4, 0, 40, 5, occurences);
    newFileNameToReportPacket(fd, 30, 1, "/tmp/file2.txt");
    newFileNameToReportPacket(fd, 30, 2, "/home/michele/Documents/SISTEMI/sistemi2020-report/src/report/test_files_2/patate.txt");
    newFileNameToReportPacket(fd, 30, 3, "/home/michele/Documents/SISTEMI/sistemi2020-report/src/report/test_files_2/formaggio.txt");
    //sendOccurencesPacketToReport(fd, 40, 0, 5, 0, 50, 10, occurences);
    sendOccurencesPacketToReport(fd, 30, 3, 3, 0, 40, 5, occurences);
    sendOccurencesPacketToReport(fd, 30, 3, 9, 0, 40, 6, occurences);
    reportErrorOnFilePacket(fd, 30, 0);
    deleteFolderFromReportPacket(fd, 30, "/tmp/");
    
    


    return 0;
}