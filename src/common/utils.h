#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define INT_SIZE 4
#define DEBUGGING true

#define ASCII_LENGTH 256 // how many characters we are considering
#define NUM_OCCURENCES 256
#define READ 0
#define WRITE 1
#define MINIQ_MAX_BUFFER_SIZE 4096
#define MAX_STRING_SIZE 4096

typedef int bool;
#define false 0
#define true 1

typedef unsigned char byte;
typedef unsigned int uint;
typedef char *string;
typedef unsigned long long ull;

// Check if two strings are the same TESTED✔️
// true: p1 is equal to p2; false otherwise
bool streq(const char *p1, const char *p2);

// Ensures the given string is long at most maxLen. If it is longer the middle
// part is cut out and three dots are inserted instead.
// MODIFIES the original string
// Please use maxLen >= 6 (else it returns src). TESTED✔️
void trimStringToLength(char *src, int maxLen);

// Struct for keeping how many char of each group are present in a given
// file/folder, the total number of chars and how many we have read
typedef struct {
  uint az, AZ, digits, punctuation, spaces, otherChars;
  uint totalCharsRead, totalChars;
} charGroupStats;

// Check if malloc has failed
void checkNotNull(void *ptr);

/**
 * Controlla se la parte iniziale della stringa fileName corrisponde con
 * la stringa folderName. Se sì ritrona true.
 */
bool isInFolder(string fileName, string folderName);

// Given a path to a file/folder it returns:
// -1 : if it does not exist
//  0 : if it is a file and it exists
//  1 : if it is is a folder and it exists
int inspectPath(const char *path);

#endif