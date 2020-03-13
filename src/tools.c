#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <inttypes.h>
#include "types.h"

static const uint64_t INFINITY = 9223372036854775807;
static const int32_t NEG_INFINITY = -2147483648;
static const int32_t POS_INFINITY = 2147483647;

//TREAP

int32_t compare(void *ptrA, void *ptrB, int32_t compareId){
  // 1: City
  if(compareId == 1){
    return strcmp(((City*)ptrA)->name, ((City*)ptrB)->name);
  }

  // 2: Neigh
  if(compareId == 2){
    return strcmp(((Neigh*)ptrA)->dest->name, ((Neigh*)ptrB)->dest->name);
  }

  // 3: dijkVal
  if(compareId == 3){
    uint64_t distA = ((dijkVal*)ptrA)->actDist;
    uint64_t distB = ((dijkVal*)ptrB)->actDist;

    if(distA > distB) return 1;
    if(distA == distB) return 0;
    if(distA < distB) return -1;
  }

  // 4: strings
  if(compareId == 4){
    return strcmp((char*)ptrA, (char*)ptrB);
  }

  return 0;
}

void rightRotate(TreapNode **treapPtr){
  TreapNode *y = *treapPtr;
  TreapNode *x = y->left;
  TreapNode *T2 = x->right;

  x->right = y;
  y->left = T2;

  *treapPtr = x;
}

void leftRotate(TreapNode **treapPtr){
  TreapNode *x = *treapPtr;
  TreapNode *y = x->right;
  TreapNode *T2 = y->left;

  y->left = x;
  x->right = T2;

  *treapPtr = y;
}

void *search(TreapNode *root, void *valPtr, int32_t compareId){
  if(root == NULL || compare(valPtr, root->valPtr, compareId) == 0){
    if(root != NULL) return root->valPtr;
    return NULL;
  }

  if(compare(valPtr, root->valPtr, compareId) < 0){
    return search(root->left, valPtr, compareId);
  } else {
    return search(root->right, valPtr, compareId);
  }
}

void *removeMin(TreapNode **treapPtr){
  TreapNode *t = *treapPtr;
  if(t->left == NULL){
    *treapPtr = t->right;
    void* answ = t->valPtr;
    free(t);
    return answ;
  }
  else {
    return removeMin(&(t->left));
  }
}

bool insert(TreapNode **treapPtr, void *valPtr, int32_t compareId){
  if(*treapPtr == NULL){
    *treapPtr = createTreapNode(valPtr);
    if(treapPtr == NULL) return false;
    return true;
  }

  bool ret = true;
  if(compare(valPtr, (*treapPtr)->valPtr, compareId) < 0){
    ret = insert(&((*treapPtr)->left), valPtr, compareId);

    if((*treapPtr)->left->priority > (*treapPtr)->priority){
      rightRotate(treapPtr);
    }
  } else {
    ret = insert(&((*treapPtr)->right), valPtr, compareId);

    if((*treapPtr)->right->priority > (*treapPtr)->priority){
      leftRotate(treapPtr);
    }
  }

  return ret;
}

void removeNode(TreapNode **treapPtr, void *valPtr, int32_t compareId){
  if(*treapPtr == NULL){
    return;
  }

  if(compare(valPtr, (*treapPtr)->valPtr, compareId) < 0){
    removeNode(&((*treapPtr)->left), valPtr, compareId);
  }
  else if(compare(valPtr, (*treapPtr)->valPtr, compareId) > 0){
    removeNode(&((*treapPtr)->right), valPtr, compareId);
  }

  else if((*treapPtr)->left == NULL){
    TreapNode *rChild = (*treapPtr)->right;
    free(*treapPtr);
    *treapPtr = rChild;
  }
  else if((*treapPtr)->right == NULL){
    TreapNode *lChild = (*treapPtr)->left;
    free(*treapPtr);
    *treapPtr = lChild;
  }

  else if((*treapPtr)->left->priority < (*treapPtr)->right->priority){
    leftRotate(treapPtr);
    removeNode(&((*treapPtr)->left), valPtr, compareId);
  } else {
    rightRotate(treapPtr);
    removeNode(&((*treapPtr)->right), valPtr, compareId);
  }
}

void deleteTreap(TreapNode *root){
  if(root == NULL) return;

  deleteTreap(root->left);
  deleteTreap(root->right);

  free(root->valPtr);
  root->valPtr = NULL;
  root->left = NULL;
  root->right = NULL;

  free(root);
}

void flat_deleteTreap(TreapNode *root){
  if(root == NULL) return;
  flat_deleteTreap(root->left);
  flat_deleteTreap(root->right);
  root->valPtr = NULL;
  free(root);
}

void deleteNeighTreap(TreapNode *root){
  if(root == NULL) return;

  deleteNeighTreap(root->left);
  deleteNeighTreap(root->right);

  deleteNeigh((Neigh*)(root->valPtr));
  root->valPtr = NULL;
  root->left = NULL;
  root->right = NULL;

  free(root);
}

void deleteCity(City *cityPtr){
  deleteNeighTreap(cityPtr->neighbours);
  cityPtr->neighbours = NULL;
  free(cityPtr->name);
  cityPtr->name = NULL;

  free(cityPtr);
}

void deleteCityTreap(TreapNode *root){
  if(root == NULL) return;

  deleteCityTreap(root->right);
  deleteCityTreap(root->left);

  deleteCity((City*)(root->valPtr));
  root->valPtr = NULL;
  root->left = NULL;
  root->right = NULL;
  free(root);
}

//DIJKSTRA

void setToInfinity(TreapNode *treap){
  if(treap == NULL) return;
  setToInfinity(treap->left);
  setToInfinity(treap->right);

  City *cityPtr = (City*)(treap->valPtr);
  cityPtr->dijkDist = INFINITY;
  cityPtr->dijkYoungestOldest = NEG_INFINITY;
  cityPtr->isInRoute = false;
  cityPtr->dijkInCount = 0;
}

bool dijkProcessCity(TreapNode **dijkTreapPtr, TreapNode *neighRoot, int32_t oldest, uint64_t dist){
  if(neighRoot == NULL) return true;

  Neigh *rootVal = (Neigh*)(neighRoot->valPtr);
  City *destCity = rootVal->dest;

  uint64_t potDist = dist + rootVal->length;

  if(!(rootVal->forbid) && destCity->isInRoute == false && potDist <= destCity->dijkDist){
    uint64_t lastDijkDist = destCity->dijkDist;
    destCity->dijkDist = potDist;

    int32_t newOldest = min(oldest, rootVal->date);

    if(potDist == lastDijkDist){
      int32_t old = destCity->dijkYoungestOldest;
      destCity->dijkYoungestOldest = max(destCity->dijkYoungestOldest, newOldest);

      if(newOldest == old) destCity->dijkInCount++;
      if(newOldest > old) destCity->dijkInCount = 1;
    } else {
      destCity->dijkYoungestOldest = newOldest;
      destCity->dijkInCount = 1;
    }

    dijkVal *toInsert = createDijkVal(destCity, destCity->dijkDist, newOldest);
    if(toInsert == NULL) return false;
    if(!insert(dijkTreapPtr, toInsert, 3)) return false;
  }

  bool l = dijkProcessCity(dijkTreapPtr, neighRoot->left, oldest, dist);
  bool r = dijkProcessCity(dijkTreapPtr, neighRoot->right, oldest, dist);
  return (l && r);
}

void chooseNeighbour(City *cityPtr, Neigh **neighPtr, TreapNode *neighRoot, int32_t youngestOldest, int32_t actOldest){
  if(neighRoot == NULL) return;

  chooseNeighbour(cityPtr, neighPtr, neighRoot->left, youngestOldest, actOldest);
  chooseNeighbour(cityPtr, neighPtr, neighRoot->right, youngestOldest, actOldest);

  Neigh* rootVal = (Neigh*)(neighRoot->valPtr);
  City *actNeighPtr = rootVal->dest;

  if(!(rootVal->forbid) && cityPtr->dijkDist == actNeighPtr->dijkDist + rootVal->length){
    int32_t newOldest = min(actOldest, rootVal->date);

    if(min(newOldest, actNeighPtr->dijkYoungestOldest) == youngestOldest){
      *neighPtr = rootVal;
    }
  }
}

bool findShortestPath(City *cityPtr1, City *cityPtr2, ListNode *route, TreapNode *cities, City *valCity, ListNode **target){
  setToInfinity(cities);

  if(route != NULL){
    ((Neigh*)(route->valPtr))->reversed->dest->isInRoute = true;

    while(route != NULL){
      ((Neigh*)(route->valPtr))->dest->isInRoute = true;
      route = route->next;
   }
  }

  if(valCity != NULL) valCity->isInRoute = false;

  TreapNode *dijkTreap = NULL;
  dijkVal *startCity = createDijkVal(cityPtr1, 0, POS_INFINITY);
  if(startCity == NULL) return false;
  cityPtr1->dijkDist = 0;
  cityPtr1->dijkYoungestOldest = POS_INFINITY;
  if(!insert(&dijkTreap, startCity, 3)){
    free(startCity);
    return false;
  }

  while(dijkTreap != NULL){
    dijkVal *treapMin = (dijkVal*)removeMin(&dijkTreap);
    City *nearestCity = treapMin->cityPtr;
    int32_t oldest = treapMin->actOldest;
    uint64_t dist = treapMin->actDist;
    free(treapMin);

    if(!dijkProcessCity(&dijkTreap, nearestCity->neighbours, oldest, dist)){
      deleteTreap(dijkTreap);
      return false;
    }
  }

  deleteTreap(dijkTreap);
  if(cityPtr2->dijkDist == INFINITY){
    *target = NULL;
    return true;
  }
  if(cityPtr2->dijkInCount > 1){
    *target = NULL;
    return true;
  }

  ListNode *path = NULL;
  int32_t youngestOldest = cityPtr2->dijkYoungestOldest;
  int32_t oldest = POS_INFINITY;
  Neigh *actNeighPtr = NULL;
  City *actCityPtr = cityPtr2;

  while(true){
    if(actCityPtr == cityPtr1) break;

    actNeighPtr = NULL;
    chooseNeighbour(actCityPtr, &actNeighPtr, actCityPtr->neighbours, youngestOldest, oldest);
    oldest = min(oldest, actNeighPtr->date);

    ListNode *prevListNode = createListNode(actNeighPtr->reversed);

    if(prevListNode == NULL){
      freeList(path);
      return false;
    }

    prevListNode->next = path;
    path = prevListNode;
    actCityPtr = actNeighPtr->dest;

  }
  *target = path;
  return true;
}

// STRINGS

bool addChar(char **str, char **ptr, char c, int32_t *count, int32_t *bufferSize){
  if(*count + 1 >= *bufferSize){
    *bufferSize *= 2;
    char *relstr = (char*)realloc(*str, sizeof(char) * *bufferSize);
    if(relstr == NULL){
      free(*str);
      *str = NULL;
      return false;
    }
    *str = relstr;
    *ptr = *str + *count;
  }

  **ptr = c;
  (*count)++;
  (*ptr)++;
  **ptr = 0;
  return true;
}

bool appendCityName(char **str, char **ptr, char *name, int32_t *count, int32_t *bufferSize){
  while(*name != 0){
    if(!addChar(str, ptr, *name, count, bufferSize)) return false;
    name++;
  }
  return true;
}

bool appendInteger(char **str, char **ptr, int64_t value, int32_t *count, int32_t *bufferSize){
  int64_t pom = value;
  int32_t length = 0;
  while(pom != 0){
    length++;
    pom /= 10;
  }

  if(*count + length >= *bufferSize){
    *bufferSize *= 2;
    char *relstr = (char*)realloc(*str, sizeof(char) * *bufferSize);
    if(relstr == NULL){
      free(*str);
      *str = NULL;
      return false;
    }
    *str = relstr;
    *ptr = *str + *count;
  }

  char *tempPtr = *ptr + length - 1;
  while(tempPtr >= *ptr){
    char c = (char)(value % 10 + 48);
    *tempPtr = c;
    tempPtr--;
    value /= 10;
  }
  *ptr += length;
  *count += length;
  **ptr = 0;
  return true;
}
