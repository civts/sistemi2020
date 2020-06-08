#ifndef __MINIQLIST__
#define __MINIQLIST__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

typedef struct{
  pid_t pid;
  int   fileId;
  int   pipeToQ[2];
  int   currM;
  int   index;
} miniQinfo;

typedef struct NodeMiniQ {
    miniQinfo        *data;
    struct NodeMiniQ *next;
    struct NodeMiniQ *prev;
} NodeMiniQ;

typedef struct {
    int counter;
    struct NodeMiniQ *first;
    struct NodeMiniQ *last;
} miniQlist;

/**
 * Returns a pointer to a new instance of miniQinfo
 */
miniQinfo* constructorMiniQinfo(pid_t thisPid, int fileIdAssigned, int pipe[2], int currentM, int index);

void printMiniQinfo(miniQinfo *miniQ);

void deleteMiniQinfo(miniQinfo *miniQ);

/**
 * Returns a pointer to a Node with the miniQ inside, it has next and prev fields
 * initialized to NULL;
 */
NodeMiniQ *constructorNodeMiniQ(miniQinfo *miniQ);

void printNodeMiniQ(NodeMiniQ *node);

void deleteNodeMiniQ(NodeMiniQ *node);

/**
 * Returns a pointer to an empty list of miniQinfo elements
 */
miniQlist* constructorMiniQlist();

void printMiniQlist(miniQlist *list);

void deleteMiniQlist(miniQlist *list);

/**
 * Append a NodeMiniQ to the list 
 */
void appendMiniQ(miniQlist *list, struct NodeMiniQ *newNode);

/**
 * Returns the NodeMiniQ given the pid of miniQ
 * returns NULL if there's no miniQinfo with that pid
 */
NodeMiniQ* getNodeMiniQByPid(miniQlist *list, pid_t pid);

/**
 * Returns the NodeMiniQ given the id of the file assigned
 * returns NULL if there's no miniQinfo with that fileId
 */
NodeMiniQ* getNodeMiniQByFileId(miniQlist *list, int fileId);

/**
 * Removes a Node given the pid of the miniQ it contains
 */
void removeMiniQByPid(miniQlist *list, pid_t pid);

/**
 * Removes a Node given the fileId of the miniQinfo it contains
 * it returns the pid of the miniQinfo deleted or -1 if there
 * was not a miniQinfo eith that fileId assigned
 */
pid_t removeMiniQByFileId(miniQlist *list, int fileId);

#endif