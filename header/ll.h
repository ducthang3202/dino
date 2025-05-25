#pragma once

#include <SDL3/SDL.h>
#include <malloc.h>
// basic shell construct for Queue Nodes, that holds a pointer to whatever data
typedef struct Node{
    struct Node* next;
    struct Node* prev;
    // pointer to whatever data
    void* data;
}Node;

typedef struct LinkedList{
    Node* NIL, *curr;
    int size;

}LinkedList;

void LL_Add(LinkedList* ll, void* data);
bool LL_Clean(LinkedList* ll);
void* LL_Get(LinkedList* ll);
void LL_Remove(LinkedList* ll, void* data);
LinkedList* LL_Create();

