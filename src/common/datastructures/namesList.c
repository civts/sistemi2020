#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "namesList.h"
#include "../utils.h"

NodeName *constructorNodeName(string fileName){
    NodeName *node = (NodeName*) malloc(sizeof(NodeName));
    node->name = (string) malloc(strlen(fileName) + 1);
    strcpy(node->name, fileName);
    node->next = node->prev = NULL;

    return node;
}

void printNodeName(NodeName *node){
    if(node == NULL){
        printf("Node is null!\n");
    } else if(node->name == NULL) {
        printf("Node->name is null!\n");
    } else {
        printf("File: %s, next %p \n", node->name, (void *) node->next);
    }
}

void deleteNodeName(NodeName *node){
    free(node->name);
    free(node);
}

/**
 * Returns a pointer to an empty list of NodeName (fileName) elements
 */
NamesList *constructorNamesList(){
    NamesList *list = (NamesList*) malloc(sizeof(NamesList));
    list->first     = NULL;
    list->last      = NULL;
    list->counter   = 0;

    return list;
}

void printNamesList(NamesList *list){
    if(list->counter == 0){
        printf("namesList is empty!\n");
    } else {
        struct NodeName *element = list->first; 
        while(element!=NULL){
            printNodeName(element);
            element = element->next;
        }
    }
}

void deleteNamesList(NamesList *list){
    if (list != NULL){
        if(list->counter > 0){
            struct NodeName *element = list->first; 
            int i;
            NodeName *temp;
            for(i=0; i<list->counter; i++){
                temp = element->next;
                deleteNodeName(element);
                element = temp;
            }
        }
        free(list);
    }
}

/**
 * Append a NodeName to the list 
 */
void appendToNamesList(NamesList *list, struct NodeName *newNode){
    newNode->prev = list->last;
    newNode->next = NULL;
    if(list->counter > 0){
        list->last->next = newNode;
    } else {
        list->first = newNode;
    }
    
    list->last = newNode;
    list->counter++;
}

/**
 * Checks if it exists already in the list a node with that name, if it doesen't creates a new
 * NodeName with that name and appends it to the list.
 * Returns 0 if Name appended, 1 if not. 
 */
int appendNameToNamesList(NamesList *list, string name){
    int ret = 1;
    NodeName *tmp = getNodeNameByName(list, name);
    if(tmp == NULL){
        NodeName *newNode = constructorNodeName(name);
        appendToNamesList(list, newNode);
        ret = 0;
    } else {
        printf("File gia' presente nella lista!\n");
    }
    return ret;
}

/**
 * Returns the NodeName given the fileName of the file assigned
 * returns NULL if there's no NodeName with that fileName
 */
NodeName *getNodeNameByName(NamesList *list, string fileName){
    struct NodeName *node = list->first;
    int i;
    
    for(i=0; i<list->counter; i++) {
        if( strcmp(node->name, fileName) == 0 ){
            return node;
        }
        node = node->next;
    }

    return NULL;
}

/**
 * Removes a NodeName given the fileName of the file it contains
 * return:
 * 0 if NodeName removed correctly
 * 1 if NodeName not found
 */
int removeNodeNameByName(NamesList *list, string fileName){
    NodeName *toRemove = getNodeNameByName(list, fileName);
    int result = 1;
    if(toRemove == NULL){
        // printf("There is no file %s in the list\n", fileName);
    } else {
        if(list->first == toRemove){
            list->first = toRemove->next;
            if(list->first != NULL)list->first->prev = NULL;
        } else if(list->last == toRemove) {
            list->last = toRemove->prev;
            if(list->last != NULL) list->last->next = NULL;
        } else {
            toRemove->prev->next = toRemove->next;
            toRemove->next->prev = toRemove->prev;
            
        }
        printf("%s deleted.\n", fileName);
        deleteNodeName(toRemove);
        list->counter--;
        result = 0;
    }
    return result;
}

/**
 * Prende in input il nome di una cartella e due NamesList, toglie dalla prima tutti i 
 * nodi che contengono file che si trovano nella cartella e li aggiunge alla seconda.
 * Il nome della cartella deve essere in percorso assoluto.
 */
void deleteFolderNamesList(string folder, NamesList *existentList, NamesList *deletedList){
    NodeName *tempNode = existentList->first;
    NodeName *nextNode;
    if(deletedList == NULL){
        deletedList = constructorNamesList();
    }
    while(tempNode != NULL){
        nextNode = tempNode->next;
        if(isInFolder(tempNode->name, folder)){
            // aggiorno nodo precedente
            if(tempNode->prev!=NULL){
                tempNode->prev->next = tempNode->next;
            } else {
                existentList->first = tempNode->next;
            }
            // aggiorno nodo successivo
            if(tempNode->next != NULL){
                tempNode->next->prev = tempNode->prev;
            } else {
                existentList->last = tempNode->prev;
            }
            // aggiorno contatore nodi
            existentList->counter--;
            // sposto il nodo nella lista degli eliminati
            tempNode->next = tempNode->prev = NULL;
            appendToNamesList(deletedList, tempNode);
        }
        tempNode = nextNode;
    }
}