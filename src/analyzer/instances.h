#ifndef __INSTANCES_H__
#define __INSTANCES_H__

#include <sys/types.h> // for pid_t
#include "../common/utils.h"
#include "../common/datastructures/namesList.h"
#include "../common/datastructures/fileList.h"

#define HISTORY 5  // number of last commands to display in analyzer
#define MESSAGES 5 // number of last text messages to display in analyzer (unused)

typedef struct{
    pid_t pid;
    int pipeCP[2];
    int pipePC[2];
    int workload;
} pInstance;

typedef struct{
    pid_t pid;
    int pipePQ[2];
    int pipeQP[2];
    int currM; // number of parts in which to split the file
    int index; // index of the file part this Q needs to analyze
} qInstance;

typedef struct{
    pid_t pid;
    int pipeMiniQQ[2];
    int currM; // number of parts in which to split the file
    int index; // index of the file part this Q needs to analyze
} miniQInstance;

typedef struct{
    pid_t pid;
    pid_t pidAnalyzer;
    int   pipeAC[2];
    int   pipeCA[2];
    int   pipeToReport[2];
    int   currN;
    int   currM;
    int   tempN; // used in interactive mode, before starting the analysis
    int   tempM; // used in interactive mode, before starting the analysis
    int   nextFileID;
    int   filesFinished;
    bool  isAnalysing;
    bool  hasMainOption;
    pInstance **pInstances; // P processes associated to C
    NamesList *fileNameList;
    NamesList *removedFileNames;
    NamesList *foldersToRemove;
    FileList  *fileList;
} controllerInstance;

typedef struct{
    int  statusAnalysis;
    int  completedFiles;
    int  totalFiles;
    int  n;
    int  m;
    bool  hasMainOption; // has been called through -main option (unused)
    string lastCommands[HISTORY]; // list of the last commands in analyzer
    string lastMessages[MESSAGES]; // list of the last messages in analyzer (unused)
    char mode[50];
} analyzerInstance;

#endif