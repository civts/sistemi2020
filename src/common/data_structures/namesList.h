#include "../../utils.c"
#ifndef NAMES_LIST_H
#define NAMES_LIST_H
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
#endif