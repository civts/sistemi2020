#ifndef __PARSER_H__
#define __PARSER_H__
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../common/utils.h"

#define flag_analyze 0
#define flag_i 1
#define flag_s 2
#define flag_h 3
#define flag_show 4
#define flag_rem 5
#define flag_add 6
#define flag_n 7
#define flag_m 8
#define flag_quit 9
#define flag_main 10

/*** Parameter for parser ****/
string argumentsAnalyzer[10];
int numberPossibleFlagsAnalyzer = 10; 
string invalidPhraseAnalyzer    = "Wrong command syntax, try command '-h' for help.\n";
string possibleFlagsAnalyzer[]  = {"-analyze", "-i",  "-s", "-h",  "-show", "-rem", "-add", "-n", "-m", "-quit", "-main"};
bool   flagsWithArgsAnalyzer[]  = {false,      false, false, false, false,   true,   true,   true,  true, false,  false};
bool   settedFlagsAnalyzer[]    = {false,      false, false, false, false,   false,  false,  false, false,false,  false};


bool checkArguments(int argc,char * argv[],char **possibleFlags,bool* flagsWithArguments, int numberPossibleFlags, bool* settedFlags,char **arguments, char* invalid,bool printOnFailure);
void parser(char real[], int * no,char **out);
void clear();

#endif