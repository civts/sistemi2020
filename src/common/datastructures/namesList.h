#ifndef __NAMES_LIST__
#define __NAMES_LIST__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "../utils.h"

typedef struct NodeName {
    string name;
    struct NodeName *next;
    struct NodeName *prev;
} NodeName;

typedef struct {
    struct NodeName *first;
    struct NodeName *last;
    int counter;
} NamesList;

NodeName *constructorNodeName(string fileName);

void printNodeName(NodeName *node);

void deleteNodeName(NodeName *node);

/**
 * Returns a pointer to an empty list of NodeName (fileName) elements
 */
NamesList *constructorNamesList();

void printNamesList(NamesList *list);

void deleteNamesList(NamesList *list);

/**
 * Append a NodeName to the list 
 */
void appendToNamesList(NamesList *list, struct NodeName *newNode);

/**
 * Checks if it exists already in the list a node with that name, if it doesen't creates a new
 * NodeName with that name and appends it to the list.
 * Returns 0 if Name appended, 1 if not. 
 */
int appendNameToNamesList(NamesList *list, string name);

/**
 * Returns the NodeName given the fileName of the file assigned
 * returns NULL if there's no NodeName with that fileName
 */
NodeName *getNodeNameByName(NamesList *list, string fileName);

/**
 * Removes a NodeName given the fileName of the file it contains
 * return:
 * 0 if NodeName removed correctly
 * 1 if NodeName not found
 */
int removeNodeNameByName(NamesList *list, string fileName);

/**
 * Prende in input il nome di una cartella e due NamesList, toglie dalla prima tutti i 
 * nodi che contengono file che si trovano nella cartella e li aggiunge alla seconda.
 * Il nome della cartella deve essere in percorso assoluto.
 */
void deleteFolderNamesList(string folder, NamesList *existentList, NamesList *deletedList);

#endif