#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "../utils.c"

typedef struct Node {
    string       name;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct {
    struct Node *first;
    struct Node *last;
    int counter;
} NamesList;

void printNode(Node*);

Node *constructorNode(string fileName){
    Node *node = (Node *)malloc(sizeof(Node));
    node->name = (string)malloc(sizeof(fileName));
    strcpy(node->name, fileName);
    node->next = node->prev = NULL;
    // printf("constructed node %s\n", node->name);
    // printNode(node);
    return node;
}

void printNode(Node *node){
    printf("File: %s \n", node->name);
}

void deleteNode(Node *node){
    free(node->name);
    free(node);
}

/**
 * Returns a pointer to an empty list of Node (fileName) elements
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
        struct Node *element = list->first; 
        int i;
        while(element!=NULL){
            printNode(element);
            element = element->next;
        }
    }
}

void deleteNamesList(NamesList *list){
    if(list->counter > 0){
        struct Node *element = list->first; 
        int i;
        for(i=0; i<list->counter; i++){
            deleteNode(element);
            element = element->next;
        }
    }
    free(list);
}

/**
 * Append a Node to the list 
 */
void append(NamesList *list, struct Node *newNode){
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

void appendName(NamesList *list, string name){
    Node *newNode = constructorNode(name);
    append(list, newNode);
}

/**
 * Returns the Node given the fileName of the file assigned
 * returns NULL if there's no Node with that fileName
 */
Node *getNodeByName(NamesList *list, string fileName){
    struct Node *node = list->first;
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
 * Removes a Node given the fileName of the file it contains
 */
void removeByName(NamesList *list, string fileName){
    Node *toRemove = getNodeByName(list, fileName);
    
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
        deleteNode(toRemove);
        list->counter--;
        printf("%s deleted.\n", fileName);
    }
}

// int main(){

//     Node *node1 = constructorNode("Marciello");
//     Node *node2 = constructorNode("Giovanni");
//     // printNode(node1);
//     // printNode(node2);

//     NamesList *list = constructorNamesList();
//     append(list, node1);
//     append(list, node2);

//     printNameList(list);
//     removeByName(list, "Marciello");
//     removeByName(list, "Marciello");
//     printNameList(list);

//     return 0;
// }