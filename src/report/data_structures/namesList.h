#ifndef __NAMES_LIST__
#define __NAMES_LIST__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "../../utils.c"

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


NodeName*  constructorNodeName(string a);
void       printNodeName(NodeName* a);
void       deleteNodeName(NodeName* a);
NamesList* constructorNamesList();
void       printNamesList(NamesList* a) ;
void       deleteNamesList(NamesList* a);
void       appendToNamesList(NamesList* a, struct NodeName* b);
void       appendNameToNamesList(NamesList* a, string b);
NodeName*  getNodeNameByName(NamesList* a, string b);
int        removeNodeNameByName(NamesList*a, string b);
void       emptyNameList(NamesList*a);


NodeName *constructorNodeName(string fileName){
    NodeName *node = (NodeName*) malloc(sizeof(NodeName));
    node->name = (string) malloc(strlen(fileName) + 1);
    strcpy(node->name, fileName);
    node->next = node->prev = NULL;

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
    NamesList *list = (NamesList*) malloc(sizeof(NamesList));
    list->first   = NULL;
    list->last    = NULL;
    list->counter = 0;

    return list;
}

void printNamesList(NamesList *list){
    if(list->counter == 0){
        printf("namesList is empty!\n");
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
    if(list != NULL){
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
 */
void appendNameToNamesList(NamesList *list, string name){
    NodeName *tmp = getNodeNameByName(list, name);
    if(tmp == NULL){
        NodeName *newNode = constructorNodeName(name);
        appendToNamesList(list, newNode);
    } else {
        printf("File gia' presente nella lista!\n");
    }
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

void emptyNameList(NamesList *list){
    deleteNamesList(list);
    list = constructorNamesList();
}

/**
 * Controlla se la parte iniziale della stringa fileName corrisponde con 
 * la stringa folderName. Se sì ritrona true.
 */
bool isInFolder(string fileName, string folderName){
    bool ret = false;
    int folderLen = strlen(folderName);
    string toCompare = (string)malloc(folderLen+1);
    memcpy(toCompare, fileName, folderLen);
    toCompare[folderLen]='\0';
    
    string result = strstr(toCompare, folderName);
    
    
    if(result != NULL){
        ret = true;
    }
    return ret;
}

/**
 * Prende in input il nome di una cartella e due NamesList, toglie dalla prima tutti i 
 * nodi che contengono file che si trovano nella cartella e li aggiunge alla seconda.
 * Il nome della cartella deve essere in percorso assoluto.
 */
void deleteFolderNamesList(string folder, NamesList *existentList, NamesList *deletedList){
    NodeName *tempNode = existentList->first;
    NodeName *nextNode;
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
            appendToNamesList(deletedList, tempNode);
        }
        tempNode = nextNode;
    }
}
string *getArgumentsList(char *arguments, int *numArgs, string *argumentsList){
    argumentsList = NULL;
    *numArgs = 0;
    if(arguments == NULL){
        // nothing
    } else {
        NamesList *list = constructorNamesList();    
        int   offset  = 0;
        char* oldPointer = arguments;
        char* pointer = strstr(arguments, " ");
        while(pointer != NULL){
            *numArgs = *numArgs+1;
            int argLength = pointer - oldPointer; 
            string argument = malloc(argLength+1);
            memcpy(argument, arguments+offset, argLength);
            argument[argLength] = '\0';
            oldPointer = pointer;
            offset = pointer - arguments + 1;
            appendNameToNamesList(list, argument);
            pointer = NULL;
            pointer = strstr(oldPointer+1, " ");
        }
        *numArgs = *numArgs+1;
        int argLength = strlen(arguments) - offset; 
        string argument = (string)malloc(argLength+1);
        // printf("Arglength: %d\n", argLength);
        memcpy(argument, arguments+offset, argLength);
        argument[argLength] = '\0';

        appendNameToNamesList(list, argument);

        argumentsList = (string *)malloc(*numArgs);
        int i=0;
        NodeName *node = list->first;
        // printf("num args: %d\n", *numArgs);
        while(node != NULL){
            argumentsList[i] = (string)malloc(strlen(node->name)+1);
            strcpy(argumentsList[i], node->name);
            argumentsList[i][strlen(node->name)] = '\0';
            // printf("name: %s\n", argumentsList[i]);
            node = node->next;
            i++;
        }
    }
    return argumentsList;
}
// int main(){

//     NodeName *node1 = constructorNodeName("Marciello.txt");
//     NodeName *node2 = constructorNodeName("foldera/folder/Giovanni.txt");
//     NodeName *node3 = constructorNodeName("folder/Juan.txt");
//     // printNodeName(node1);
//     // printNodeName(node2);

//     NamesList *list = constructorNamesList();
//     appendToNamesList(list, node1);
//     appendNameToNamesList(list, "Marciello");
//     appendToNamesList(list, node2);
//     appendToNamesList(list, node3);

//     printNamesList(list);
//     // removeNodeNameByName(list, "Marciello");
//     // removeNodeNameByName(list, "Marciello");

//     // deleteNamesList(list);
//     // deleteNamesList(list);

//     NamesList *deletedOnes = constructorNamesList();
//     printf("Delete time\nList:\n");
//     deleteFolderNamesList("folder/", list, deletedOnes);
//     printNamesList(list);
//     printf("Deleted list:\n");
//     printNamesList(deletedOnes);

//     return 0;
// }

#endif