#ifndef __CRAWLER_H__
#define __CRAWLER_H__

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../common/utils.h"
#include "../common/datastructures/namesList.h"

#define SIZE_OF_BUFFER_TO_READ_PIPE 4096

// Parse the output of the command find <folder> -type f
int parseFileListFromFind(int readDescriptor, NamesList *fileList);

// Get all the file path inside a folder
// Error codes:
// 1: it was not possible to fork a child
// 2: it was not possibile to inspect the folder
int crawler(string folder, NamesList *fileList, int* outNumFilesFound);

#endif