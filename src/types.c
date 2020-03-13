#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <inttypes.h>
#include "types.h"

static const uint64_t INFINITY = 9223372036854775807;
static const int32_t NEG_INFINITY = -2147483648;

int64_t min(int64_t a, int64_t b){
  if(a < b) return a;
  return b;
}

int64_t max(int64_t a, int64_t b){
  if(a > b) return a;
  return b;
}

ListNode *createListNode(void *valPtr){
  ListNode *newNode = (ListNode*)malloc(sizeof(ListNode));
  if(newNode == NULL) return NULL;

  newNode->valPtr = valPtr;
  newNode->next = NULL;

  return newNode;
}

dijkVal *createDijkVal(City *cityPtr, uint64_t actDist, int32_t actOldest){
  dijkVal *newDijkVal = (dijkVal*)malloc(sizeof(dijkVal));
  if(newDijkVal == NULL) return NULL;

  newDijkVal->cityPtr = cityPtr;
  newDijkVal->actDist = actDist;
  newDijkVal->actOldest = actOldest;

  return newDijkVal;
}

City *createCity(char *name){
  City *newCity = (City*)malloc(sizeof(City));
  if(newCity == NULL) return NULL;

  newCity->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
  if(newCity->name == NULL) return NULL;
  strcpy(newCity->name, name);
  newCity->neighbours = NULL;
  newCity->dijkDist = INFINITY;
  newCity->dijkYoungestOldest = NEG_INFINITY;
  newCity->isInRoute = false;
  newCity->dijkInCount = 0;

  return newCity;
}

Neigh *createNeigh(City *cityPtr, uint32_t length, int32_t date){
  Neigh *newNeigh = (Neigh*)malloc(sizeof(Neigh));
  if(newNeigh == NULL) return NULL;

  newNeigh->dest = cityPtr;
  newNeigh->length = length;
  newNeigh->date = date;
  newNeigh->reversed = NULL;
  newNeigh->localRoutes = NULL;
  newNeigh->forbid = false;

  return newNeigh;
}

TreapNode *createTreapNode(void *valPtr){
  TreapNode *newTreapNode = (TreapNode*)malloc(sizeof(TreapNode));
  if(newTreapNode == NULL) return NULL;

  newTreapNode->valPtr = valPtr;
  newTreapNode->priority = rand();
  newTreapNode->left = NULL;
  newTreapNode->right = NULL;

  return newTreapNode;
}

void freeList(ListNode *path){
  while(path != NULL){
    ListNode *p = path->next;
    free(path);
    path = p;
  }
}

bool reverseNeighList(ListNode *listPtr, ListNode **target){
  ListNode *newListPtr = NULL;
  while(listPtr != NULL){
    Neigh *rev = ((Neigh*)(listPtr->valPtr))->reversed;
    ListNode *prevListNode = createListNode(rev);
    if(prevListNode == NULL){
      freeList(newListPtr);
      return false;
    }
    prevListNode->next = newListPtr;
    newListPtr = prevListNode;
    listPtr = listPtr->next;
  }

  *target = newListPtr;
  return true;
}

void deleteNeigh(Neigh *neighPtr){
  free(neighPtr->localRoutes);
  neighPtr->localRoutes = NULL;

  free(neighPtr);
  neighPtr = NULL;
}

bool neighRoutesInit(Neigh *ptr){
  ptr->localRoutes = (bool*)malloc(sizeof(bool) * 1000);
  if(ptr->localRoutes == NULL) return false;

  for(int i=0; i<1000; i++) ptr->localRoutes[i] = false;
  return true;
}
