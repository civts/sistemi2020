#include "../common/utils.h"
#include "./data_structures/file_with_stats_data_structure.h"
#include "./data_structures/file_with_stats_list.h"
#include "./data_structures/analyzer_data_structure.h"
#include "./data_structures/analyzer_list.h"
#include "../common/datastructures/namesList.h"
#include "./packet_handler.h"
#include "./report_print_functions.h"

#include <fcntl.h>
#include <limits.h> //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h> //termios, TCSANOW, ECHO, ICANON
#include <unistd.h> //STDIN_FILENO

#ifndef REPORT_H
#define REPORT_H

#define BUFFER_SIZE 4096
char buf[BUFFER_SIZE], command[BUFFER_SIZE];
int counter = 0;


#define help 0
#define verbose 1
#define tab 2
#define compact 3
#define only 4
#define extended 5
#define force 6
#define quit 7
#define dumps_idx 8


// Flag for telling the report to show help dialog
char helpFlag[] = "-h";
// Flag for telling the report to show verbose stats
char verboseFlag[] = "-v";
// Flag for tab mode
char tabFlag[] = "-t";
// Flag for telling the report to show compact
char compactFlag[] = "-c";
// Flag for telling the report to give you only the stats for a given file, after that specify le list of arguments "--only /home/a.txt  /home/abac/ ", ? MAYBE THIS PART if a pid is specified in the same line, it will appied only to an analyzer with that pid
char onlyFlag[] = "--only";
// Flag for telling the report to show extended stats (a,b,c... instead of a-z)
char extendedFlag[] = "-e";
// Flag for telling the report to force another analysis. Discards all previous data
char forceReAnalysisFlag[] = "-r";
// Flag for telling the report to quit
char quitFlag[] = "-q";
// Flag for telling analyzers to start dumping to files
char dumpFlag[] = "--dump";

// void clearScreen(){
// //  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
// //  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
//     printf("\e[1;1H\e[2J");
// }
// void clear(){
//     system("clear");
// }
void clear();
void resetBuffer(char buffer[], int size);
bool optionCombinationValid(bool *settedFlags);
bool parseArguments(char * arguments, int * numArgs,char ** resolvedPaths);
bool checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char **arguments, char* invalid,bool printOnFailure);
void parser(char real[], int * no,char **out);
#endif // !REPORT_H
