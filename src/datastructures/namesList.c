#ifndef __NAMES_LIST__
#define __NAMES_LIST__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "../utils.c"

typedef struct NodeName {
    string       name;
    struct NodeName *next;
    struct NodeName *prev;
} NodeName;

typedef struct {
    struct NodeName *first;
    struct NodeName *last;
    int counter;
} NamesList;

void printNodeName(NodeName*);

NodeName *constructorNodeName(string fileName){
    NodeName *node = (NodeName *)malloc(sizeof(NodeName));
    node->name = (string)malloc(sizeof(fileName));
    strcpy(node->name, fileName);
    node->next = node->prev = NULL;
    // printf("constructed node %s\n", node->name);
    // printNodeName(node);
    return node;
}

void printNodeName(NodeName *node){
    printf("File: %s \n", node->name);
}

void deleteNodeName(NodeName *node){
    free(node->name);
    free(node);
}

/**
 * Returns a pointer to an empty list of NodeName (fileName) elements
 */
NamesList *constructorNamesList(){
    NamesList *list = (NamesList *)malloc(sizeof(NamesList));
    list->first = NULL;
    list->last = NULL;
    list->counter = 0;

    return list;
}

void printNamesList(NamesList *list){
    if(list->counter == 0){
        printf("miniQlist is empty!\n");
    } else {
        struct NodeName *element = list->first; 
        int i;
        while(element!=NULL){
            printNodeName(element);
            element = element->next;
        }
    }
}

void deleteNamesList(NamesList *list){
    if(list->counter > 0){
        struct NodeName *element = list->first; 
        int i;
        for(i=0; i<list->counter; i++){
            deleteNodeName(element);
            element = element->next;
        }
    }
    free(list);
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

void appendNameToNamesList(NamesList *list, string name){
    NodeName *newNode = constructorNodeName(name);
    appendToNamesList(list, newNode);
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
        printf("There is no file %s in the list\n", fileName);
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
        deleteNodeName(toRemove);
        list->counter--;
        printf("%s deleted.\n", fileName);
        result = 0;
    }
    return result;
}

// int main(){

//     NodeName *node1 = constructorNodeName("Marciello");
//     NodeName *node2 = constructorNodeName("Giovanni");
//     // printNodeName(node1);
//     // printNodeName(node2);

//     NamesList *list = constructorNamesList();
//     appendToNamesList(list, node1);
//     appendToNamesList(list, node2);

//     printNameList(list);
//     removeNodeNameByName(list, "Marciello");
//     removeNodeNameByName(list, "Marciello");
//     printNameList(list);

//     return 0;
// }

#endif