#include "../utils.c"
#include "./data_structures/file_with_stats_list.h"
#include "./data_structures/analyzer_list.h"
#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H

const char spaceChars[] = {' ', '\t', '\r', '\n', '\f', '\v'};
const char punctuationChars[] = {
    ',', ';', '.', ':', '-', '?', '!', '\'', '`', '"', '*', '(', ')', '_',
};

// Prints the line "Analyzed X files [in Y folders] [w/ Z analyzers]:\n"
// AGGIORNATA CON LE FOLDER
void printFirstInfoLine(analyzerList *aList);
// Prints a progress bar with the percentage of a/b*100 [###    ]
void printPercentage(uint done, uint total, int barWidth);

// Default print function (no additional argv). Should look ilke this:
//  Analyzed 50 files [in 2 folders] [w/ 6 analyzers]:
//  a-z: 2109
//  A-Z: 42
//  ....
//  total characters read: 16432 over 56600 (POTENZIALMENTE PROGRESSBAR)
// 50 % complete
// TESTED
void printRecapCompact(analyzerList *aList);
// Print function for the -v flag and possibly -g.
//
//---------------------------------------------------------------
// If shouldGroup is TRUE output should look like the following:
//
// Analyzed 50 files [in 2 folders] [w/ 6 analyzers]:
//      text1.txt
//      text2.txt
//      ....
//      textN.txt
//    ------------text1.txt------------
//    a-z: 2109
//    A-Z: 42
//    ....
//    total characters: 16432
//    ------------text2.txt------------
//    a-z: 912
//    ...
//---------------------------------------------------------------
// If shouldGroup is FALSE output should look like the following:
//
// Analyzed 50 files [in 2 folders] [w/ 6 analyzers]:
//      text1.txt
//      text2.txt
//      ....
//      textN.txt
//    ------------text1.txt------------
//    a: 2109
//    b: 42
//    ....
//    total characters: 16432
//    ------------text2.txt------------
//    a: 912
//    ...
void printRecapVerbose(analyzerList *aList, bool shouldGroup);
// Prints a single file (you need to specify if you want to have letters grouped
// or not). If group is false it prints occourrences of each letter, else
// clusters.
void printSingleFile(fileWithStats *f, bool group);
// Print function for when the user specifies the --only flag. Accepts the
// analyzers list + list of paths of the files and a bool to group or not.
void printSelectedFiles(analyzerList *analyzers, int pathsLen, char *paths[],
                        bool group);
// Prints all the files in a given folder.
// If the user has analyzed the folder with more than one analyzer all the
// analysis are printed sequentially
// void printFolder(analyzerList *analyzers, char *folderPath, bool group);
//prints selected file or folders
void printFilesOrFolder(analyzerList *analyzers,int pathsLen, char *paths[], bool group);
//prints the help Text
void printHelp();

void printRecapTabela(analyzerList *analyzers);

#endif