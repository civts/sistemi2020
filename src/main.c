#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "analyzer.c" 
#include "common/utils.h" 
#include "common/parser.h" 
// #include "report.c"
#define BUFFER_SIZE 4096
#define READ  0
#define WRITE 1

/*** Parameter for parser ****/
string argumentsAnalyzer[10];
int    numberPossibleFlagsAnalyzer = 10; 
string invalidPhraseAnalyzer    = "Wrong command syntax, try command '-h' for help.\n";
string possibleFlagsAnalyzer[]  = {"-analyze", "-i",  "-s", "-h",  "-show", "-rem", "-add", "-n", "-m", "-quit", "-main"};
bool   flagsWithArgsAnalyzer[]  = {false,      false, false, false, false,   true,   true,   true,  true, false,  false};
bool   settedFlagsAnalyzer[]    = {false,      false, false, false, false,   false,  false,  false, false,false,  false};



char buf[BUFFER_SIZE], command[BUFFER_SIZE];
int counter = 0;

void resetBuffer(char buffer[], int size){
    int i;
    for (i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = 0;
    }
}
void clear(){
    system("clear");
}
void printScreenMain(int carattere) {
    printf("================================================\n");
    printf("%d\n", counter++);
    printf("===================Processing===================\n");
    printf("%s\n", command);
    printf("================================================\n");
    printf("> %s", buf);
    fflush(stdout);
}


// char helpFlag[] = "-h";
// // Flag for telling the report to show verbose stats
// char verboseFlag[] = "-v";
// // Flag for tab mode
// char tabFlag[] = "-t";
// // Flag for telling the report to show compact
// char compactFlag[] = "-c";
// // Flag for telling the report to give you only the stats for a given file, after that specify le list of arguments "--only /home/a.txt  /home/abac/ ", ? MAYBE THIS PART if a pid is specified in the same line, it will appied only to an analyzer with that pid
// char onlyFlag[] = "--only";
// // Flag for telling the report to show extended stats (a,b,c... instead of a-z)
// char extendedFlag[] = "-e";
// // Flag for telling the report to force another analysis. Discards all previous data
// char forceReAnalysisFlag[] = "-r";
// // Flag for telling the report to quit
// char quitFlag[] = "-q";
// // Flag for telling analyzers to start dumping to files
// char dumpFlag[] = "--dump";

int main(void){
    int returnCode;

    char analyzerPath[PATH_MAX]="";
    char reportPath[PATH_MAX]="";

    int report;
    int pipeToReport;
    int pipeFromReport;
    int analyzer;
    int pipeToAnalyzer;
    int pipeFromAnalyzer;

    int parent;
    
    report = fork();

    if (report < 0){
        fprintf(stderr, "Found an error creating the Report\n");
        returnCode = 2;
    } else if (report == 0){
        // child: new instance of Controller;
        char reportExecutablePath[PATH_MAX];
        strcpy(reportExecutablePath, reportPath);
        strcat(reportExecutablePath, "r");
        //close(pipeToReport[WRITE]);
        dup2(pipeFromReport, 0); //STDIN
        // Inserire metodo report() qui
        int numArgs=0;
        char* args[1] = {"report"};
        execv("report.o",args); 
    } else {
        // // parent;
        analyzer = fork();
        if(analyzer < 0){
            fprintf(stderr, "Found an error creating the Analyzer\n");
            returnCode = 2;
        } else if (analyzer == 0){
            // child: new instance of Controller;
            char analyzerExecutablePath[PATH_MAX];
            strcpy(analyzerExecutablePath, analyzerPath);
            strcat(analyzerExecutablePath, "a");
            fprintf(stderr, "analyzer created\n");
            close(pipeToSon[WRITE]);
            close(pipeFromSon[READ]);
            int numArgs=0;
            char* args[4] = {"analyzer","-add ./", "-n 2", "-m 3"};
            analyzer_main(numArgs, args);
       }
    }
    // close(pipeToSon[READ]);
    // close(pipeFromSon[WRITE]);


    int lenBuffer = 0, numReadCharacters = 0;
    char *endCommandPosition;
    resetBuffer(buf, BUFFER_SIZE);
    resetBuffer(command, BUFFER_SIZE);

    bool exit = false;
    char c;
    while (!exit){
        c = getchar();
        if(c == "\n"){
            // gestione comando (switch su analyzer se necessario)
        }
        // Printare carattere sulla pipe verso il report
    }


    return 0;
}