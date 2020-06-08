#include "./report_print_functions.h"
#include "./help.h"

const char spaceChars[] = {' ', '\t', '\r', '\n', '\f', '\v'};
const char punctuationChars[] = {
    ',', ';', '.', ':', '-', '?', '!', '\'', '`', '"', '*', '(', ')', '_',
};
void printFirstInfoLine(analyzerList *aList) {
  uint totFiles = 0;
  int totAnalyzers = 0;
  analyzer *currentAnalyzer = aList->firstNode;
  while (currentAnalyzer != NULL) {
    totFiles += currentAnalyzer->files->count;
    currentAnalyzer = currentAnalyzer->nextNode;
  }
  printf("Analyzed %u files", totFiles);
  if (totAnalyzers > 1) {
    printf(" with %d analyzers", totAnalyzers);
  }
  printf(":\n");
}
void printPercentage(uint done, uint total, int barWidth,bool gotData) {
  float percentage;
  if(total == 0 && gotData)
    percentage=1;
  if(total == 0 && !gotData)
    percentage=0;
  if(total!=0)
    percentage = done/(float)total;
  printf("\n[");
  int i, pos = barWidth * percentage;
  for (i = 0; i < barWidth; i++) {
    if (i < pos)
      printf("=");
    else if (i == pos && percentage != 0) {
      printf(">");
    } else
      printf(" ");
  }
  printf("] %.2f%% complete\n", percentage * 100);
}

void printErrors(analyzer *a) {
  fileWithStats *fNode = a->errors->firstNode;
  if (a->errors->count != 0) {
    printf("ERRORI IN QUESTI FILE!\n");
    while (fNode != NULL) {
      char pathCopy[strlen(fNode->path)];
      strcpy(pathCopy, fNode->path);
      trimStringToLength(pathCopy, 80);
      printf("%s\n", pathCopy);
      fNode = fNode->nextNode;
    }
  }
}
void printRecapCompact(analyzerList *aList) {
  printFirstInfoLine(aList);
  analyzer *current = aList->firstNode;
  uint az, AZ, digits, spaces, punctuation, otherChars, totalCharsRead,
      totalChars;
  az = AZ = digits = spaces = punctuation = otherChars = totalChars =
      totalCharsRead = 0;
  bool gotData = false;
  while (current != NULL) {
    fileWithStats *cursor = current->files->firstNode;
    while(cursor!=NULL){
      charGroupStats fileStats = statsForFile(cursor);
      az += fileStats.az;
      AZ += fileStats.AZ;
      digits += fileStats.digits;
      punctuation += fileStats.punctuation;
      spaces += fileStats.spaces;
      otherChars += fileStats.otherChars;
      totalCharsRead += fileStats.totalCharsRead;
      totalChars += fileStats.totalChars;
      gotData = cursor->gotData || gotData;
      cursor = cursor->nextNode;
    }
    current = current->nextNode;
  }
  printf("a-z: %u\nA-Z: %u\ndigits: %u\npunctuation: %u\nspace: %u\nother: "
         "%u\n\nTotal characters read: %u over %u\n",
         az, AZ, digits, punctuation, spaces, otherChars, totalCharsRead,
         totalChars);
  printPercentage(totalCharsRead, totalChars, 30,gotData);
  current = aList->firstNode;
   while (current != NULL) {
    printErrors(current);
    analyzerPrintErrorMessages(current);
    current=current->nextNode;
   }
}

void printRecapVerbose(analyzerList *aList, bool shouldGroup) {
  printFirstInfoLine(aList);
  analyzer *current = aList->firstNode;
  // Print file paths
  while (current != NULL) {
    printErrors(current);
    fileWithStats *fNode = current->files->firstNode;
    while (fNode != NULL) {
      printf("%s\n", fNode->path);
      fNode = fNode->nextNode;
    }
    current = current->nextNode;
  }
  current = aList->firstNode;
  // Print stats of each file
  while (current != NULL) {
    fileWithStats *fNode = current->files->firstNode;
    while (fNode != NULL) {
      printSingleFile(fNode, shouldGroup);
      fNode = fNode->nextNode;
    }
     analyzerPrintErrorMessages(current);
    current = current->nextNode;
  }
}

void printSingleFile(fileWithStats *f, bool group) {
  char pathCopy[strlen(f->path)];
  strcpy(pathCopy, f->path);
  trimStringToLength(pathCopy, 70);
  printf("---------------%s---------------\n", pathCopy);
  short i;
  uint *oc = f->occorrenze;
  uint thisaz, thisAZ, thisDigits, thisSpaces, thisPunct, otherChars;
  thisaz = thisAZ = thisDigits = thisSpaces = thisPunct = 0;
  for (i = 'a'; i <= 'z'; i++) {
    if (!group) {
      printf("%c: %d\n", i, oc[i]);
    }
    thisaz += oc[i];
  }
  for (i = 'A'; i <= 'Z'; i++) {
    if (!group) {
      printf("%c: %d\n", i, oc[i]);
    }
    thisAZ += oc[i];
  }
  for (i = '0'; i <= '9'; i++) {
    if (!group) {
      printf("%c: %d\n", i, oc[i]);
    }
    thisDigits += oc[i];
  }
  for (i = 0; i < 6; i++) {
    if (!group) {
      printf("%c: %d\n", spaceChars[i], oc[(int)spaceChars[i]]);
    }
    thisSpaces += oc[(int)spaceChars[i]];
  }
  for (i = 0; i < 14; i++) {
    if (!group) {
      printf("%c: %d\n", punctuationChars[i], oc[(int)punctuationChars[i]]);
    }
    thisPunct += oc[(int)punctuationChars[i]];
  }
  uint totalCharsRead = f->readCharacters;
  uint totalChars = f->totalCharacters;
  otherChars =
      totalCharsRead - thisaz - thisAZ - thisDigits - thisPunct - thisSpaces;
  if (group) {
    printf("a-z: %u\nA-Z: %u\ndigits: %u\npunctuation: %u\nspace: %u\n", thisaz,
           thisAZ, thisDigits, thisPunct, thisSpaces);
  }
  printf("others: %d\n", otherChars);
  printf("\nTotal characters read: %u over %u\n", totalCharsRead, totalChars);
  printPercentage(totalCharsRead, totalChars, 30,f->gotData);
}

// void printSelectedFiles(analyzerList *analyzers, int pathsLen, char *paths[],
//                         bool group) {
//   int i;
//   // for each path I search it across the analyzers and print it if found
//   for (i = 0; i < pathsLen; i++) {
//     bool printed = false;
//     char *path = paths[i];
//     analyzer *analyzer = analyzers->firstNode;
//     while (analyzer != NULL && !printed) {
//       fileWithStats *fws = analyzer->files->firstNode;
//       while (fws != NULL) {
//         if (streq(fws->path, path)) {
//           printSingleFile(fws, group);
//           printed = true;
//         }
//         fws = fws->nextNode;
//       }
//       analyzer = analyzer->nextNode;
//     }
//     if (!printed) {
//       char *msg = "File with path ";
//       char nPath [strlen(path)];
//       strcpy(nPath,path);
//       trimStringToLength(nPath, 80);
//       msg = strcat(msg, nPath);
//       msg = strcat(msg, "was not found\n");
//       perror(msg);
//     }
//   }
// }

// dovremmo speficare il pid a mio avviso
void printSelectedFiles(analyzerList *analyzers, int pathsLen, char *paths[],
                        bool group) {
  analyzer *a = analyzers->firstNode;
  while (a != NULL) {
    int i = 0;
    for (i = 0; i < pathsLen; i++) {
      fileWithStats *item = fwsListGetElementByPath(a->files, paths[i]);
      if (item != NULL) {
        printSingleFile(item, group);
      }
    }
    a = a->nextNode;
  }
}
// void printFolder(analyzerList *analyzers, char *folderPath, bool group) {
//   analyzer *a = analyzers->firstNode;
//   while (a != NULL) {
//     fwsList *folder = fwsListGetFolder(a->files, folderPath);
//     // adesso dentro folder puoi farci quello che vuoi. Per adesso è ancora una
//     // copia. Nel caso
//     a = a->nextNode;
//   }
// }

void printFilesOrFolder(analyzerList *analyzers,int pathsLen, char *paths[], bool group){
  analyzer *a = analyzers->firstNode;
  while (a != NULL) {
    int i;
    for(i=0;i<pathsLen;i++){
        int val = inspectPath(paths[i]);
        switch (val){
          case 0:{
            fileWithStats * fws = fwsListGetElementByPath(a->files,paths[i]);
            if(fws!=NULL)
              printSingleFile(fws,group);
            break;
          }
          case 1:
          {
            fileWithStats *cursor = a->files->firstNode;
            while (cursor!= NULL){
              if(pathIsContained(paths[i],cursor->path)){
                printSingleFile(cursor,group);
              }
              cursor = cursor->nextNode;
            }
            break;
          }
          case -1: printf("file/cartella non esistente %s\n",paths[i]);
            break;
        }
    }
    a = a->nextNode;
  }
}
// void printFolder(analyzerList *analyzers, char *folderPath, bool group) {
//   analyzer *a = analyzers->firstNode;
//   bool foundFolder = false;
//   while (a != NULL) {
//     fileWithStats *f = a->files->firstNode;
//     int i;
//     bool foundInThisAnalyzer = false;
//     for (i = 0; i < a->files->count; i++) {
//       if (f == NULL)
//         break; // Does not happen, but if it happens…
//       if (strlen(folderPath) >= strlen(f->path))
//         continue;
//       int j;
//       bool isInTheFolder = true;
//       // check if file is in desired folder
//       for (j = 0; j < strlen(folderPath); j++) {
//         if (f->path[j] != folderPath[j]) {
//           isInTheFolder = false;
//           break;
//         }
//       }
//       if (isInTheFolder) {
//         foundFolder = true;
//         if (!foundInThisAnalyzer) {
//           foundInThisAnalyzer = true;
//           printf("Files in the folder %s from the analyzer with pid %d:\n",
//                  trimStringToLength(folderPath, 30), a->pid);
//         }
//         printSingleFile(f, group);
//       }
//       f = f->nextNode;
//     }
//     a = a->nextNode;
//   }
//   if (!foundFolder) {
//     printf("Requested folder was not found in any known analysis. Please
//     check "
//            "the input for typos\n");
//   }
// };

void printProgressBar(uint done, uint total, int barWidth, bool gotData) {
  float percentage;
  if(total == 0 && gotData)
    percentage=1;
  if(total == 0 && !gotData)
    percentage=0;
  if(total!=0)
    percentage = done/(float)total;
  int i, pos = (barWidth - 3) * percentage;
  for (i = 0; i < barWidth - 3; i++) {
    if (i < pos)
      printf("=");
    else if (i == pos && percentage != 0) {
      printf(">");
    } else
      printf(" ");
  }
  printf("%-3d\n", (int)(percentage * 100));
}

void printRecapTabela(analyzerList *aList) {
  analyzer *a = aList->firstNode;
  const int totWidth = 120;
  const int groupWidth = 6;
  const int barWidth = 15;
  if(aList->count==0){
    printf("Nessun dato al momento\n");
  }
  while (a != NULL) {        // forEach analyzer
    printErrors(a);
    char msg[totWidth + 1];  // table header text
    char leftOver[totWidth]; // se devo finire la print nella riga succ
    leftOver[0] = '\0';
    int i;
    if (aList->count > 1) {
      sprintf(msg, "\nAnalizzati %d files nell'analyzer con pid %d",
              a->files->count, a->pid);
    } else {
      sprintf(msg, "Analizzati %d files", a->files->count);
    }
    int firstColWidth = totWidth - (8 * groupWidth + barWidth + 1);
    int msgLen = strlen(msg);
    if (msgLen > firstColWidth) {
      strcpy(leftOver, msg + firstColWidth - 1);
    }
    for (i = msgLen; i < firstColWidth; i++) {
      msg[i] = ' ';
    }
    // la lunghezza di ogni pezzo dev'essere groupWidth-1:
    // strlen("|az  ") = 6 = groupWidth
    // si termini con |
    const string beforeBar = " |az   |AZ   |num  |puntg|spazi|altri|letti|total|";
    sprintf(msg + firstColWidth, beforeBar);
    const string barTxt = "progress bar";
    int positionNow = firstColWidth + strlen(beforeBar);
    for (i = 0; i < (barWidth - strlen(barTxt)) / 2; i++) {
      msg[positionNow + i] = ' ';
      positionNow++;
    }
    sprintf(msg + positionNow, barTxt);
    positionNow += strlen(barTxt);
    for (i = 0; i < (barWidth - strlen(barTxt)) / 2; i++) {
      msg[positionNow + i] = ' ';
      positionNow++;
    }
    msg[positionNow] = '\0';
    printf("%s\n", msg);
    if (strlen(leftOver) > 0) {
      printf("%s\n", leftOver);
    }

    fileWithStats *f = a->files->firstNode;
    for (i = 0; i < a->files->count; i++) {
      if (f == NULL)
        break;
      charGroupStats stats = statsForFile(f);
      char line[totWidth];
      char trimmedPath[strlen(f->path)];
      strcpy(trimmedPath, f->path);
      trimStringToLength(trimmedPath, firstColWidth);
      strcpy(line, trimmedPath);
      int j;
      for (j = 0; j < firstColWidth - strlen(trimmedPath); j++) {
        line[strlen(trimmedPath) + j] = ' ';
      }
      sprintf(line + firstColWidth, " |%-5d|%-5d|%-5d|%-5d|%-5d|%-5d|%-5d|%-5d|",
              stats.az, stats.AZ, stats.digits, stats.punctuation, stats.spaces,
              stats.otherChars, stats.totalCharsRead, stats.totalChars);
      printf("%s",line);
      printProgressBar(f->readCharacters, f->totalCharacters, barWidth,f->gotData);
      f = f->nextNode;
    }
    analyzerPrintErrorMessages(a);
    a = a->nextNode;
  }
}
void printHelp(){
  printf("%s", help);
}
void printLastErrorMessages(analyzer* a,int qty){
  int i=0;
  NodeName * current = a->errorMessages->last;
  if(a->errorMessages->counter!=0){
    printf("Ultimi %d messaggi di errore provenienti dall'analyzer PID:%d\n",qty,a->pid);
  }
  while(i<qty && current!=NULL){
    printf("%s\n",current->name);
    current = current->prev;
    i++;
  }

}
void printErrorMessages(analyzer* a){
  NodeName * current = a->errorMessages->last;
  while(current!=NULL){
    printf("%s\n",current->name);
    current = current->prev;
  }

}