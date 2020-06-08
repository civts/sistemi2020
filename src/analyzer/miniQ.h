#ifndef __MINIQ_H__
#define __MINIQ_H__

// #include <stdio.h>
#include <stdlib.h>
// #include "../common/mymath.h"
// #include "../common/packets.h"
#include "../common/utils.h"
#include "../common/datastructures/miniQlist.h"
#include "instances.h"

// IMPORTANT: compile this with -lm to make ceil works...

// Principal core of a miniQ: it's goal is to detect char occurences
// and send them back to its parent Q
void miniQ(string fileName, miniQinfo *instanceOfMySelf);

int sendErrorOnFileToReport(miniQinfo *instanceOfMySelf);

// Get occurences of a portion of file given starting and ending position inside the file.
// If we get an error opening a file: return -1
int getBigOccurences(string fileName, long startPosition, long endPosition, ull outOccurences[NUM_OCCURENCES]);

// Get file length using stat syscall
// If it fails return -1.
long getFileLength(string fileName);

void sig_handler_miniQ();

#endif