#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <inttypes.h>
#include "types.h"
#include "tools.h"

static const uint64_t INFINITY = 9223372036854775807;

/**
  * Struktura reprezentująca mapę dróg
  */
typedef struct Map {
  /*@{*/
  TreapNode *cities; /**< Treap zawierający miasta */
  ListNode **routes; /**< Tablica dróg krajowych (reprezentowanych przez listy struktur Neigh) */
  /*}@*/
} Map;

bool routeExists(Map *map, uint32_t routeId){
  if(map->routes[routeId] == NULL) return false;
  return true;
}

void setRoute(Map *map, uint32_t routeId, ListNode *list){
  map->routes[routeId] = list;

  ListNode *ptr = list;
  while(ptr != NULL){
    Neigh *val = (Neigh*)(ptr->valPtr);
    val->localRoutes[routeId] = true;
    val->reversed->localRoutes[routeId] = true;
    ptr = ptr->next;
  }
}

Map *newMap(){
  Map *newMapPtr = (Map*)malloc(sizeof(Map));
  if(newMapPtr == NULL) return NULL;

  newMapPtr->routes = (ListNode**)malloc(sizeof(ListNode*) * 1000);
  if(newMapPtr->routes == NULL){
    free(newMapPtr);
    return NULL;
  }
  for(int32_t i = 0; i < 1000; i++) newMapPtr->routes[i] = NULL;

  newMapPtr->cities = NULL;

  return newMapPtr;
}

void deleteMap(Map *mapPtr){
  deleteCityTreap(mapPtr->cities);

  for(int32_t i=1; i<1000; i++){
     ListNode *listPtr = mapPtr->routes[i];

     while(listPtr != NULL){
       ListNode *l = listPtr->next;
       free(listPtr);
       listPtr = l;
     }
  }

  free(mapPtr->routes);
  mapPtr->routes = NULL;

  free(mapPtr);
}

bool searchCity(Map *map, char *city, City **target){
  City *temp = createCity(city);
  if(temp == NULL){
    return false;
  }
  City *cityPtr = (City*)search(map->cities, temp, 1);
  deleteCity(temp);
  *target = cityPtr;
  return true;
}

bool searchNeigh(TreapNode *neighs, City *cityPtr, Neigh **target){
  Neigh *temp = createNeigh(cityPtr, 0, 0);
  if(temp == NULL) return false;
  Neigh *result = (Neigh*)search(neighs, temp, 2);
  free(temp);
  *target = result;
  return true;
}

bool searchRoad(Map *map, const char *city1, const char *city2, Neigh **target){
  if(*city1 == 0 || *city2 == 0) return false;
  if(map == NULL || strcmp(city1, city2) == 0) return false;

  City *cityPtr1 = NULL;
  City *cityPtr2 = NULL;

  if(!searchCity(map, (char *)city1, &cityPtr1)) return false;
  if(!searchCity(map, (char *)city2, &cityPtr2)) return false;

  if(cityPtr1 == NULL || cityPtr2 == NULL) return false;

  Neigh *neighPtr = NULL;
  if(!searchNeigh(cityPtr1->neighbours, cityPtr2, &neighPtr)) return false;

  *target = neighPtr;
  return true;
}

bool addRoad(Map *map, const char *city1, const char *city2, unsigned length, int builtYear){
  if(*city1 == 0 || *city2 == 0) return false;

  if(map == NULL || strcmp(city1, city2) == 0 || length == 0 || builtYear == 0){
    return false;
  }

  City *cityPtr1 = NULL;
  City *cityPtr2 = NULL;

  if(!searchCity(map, (char *)city1, &cityPtr1)) return false;
  if(!searchCity(map, (char *)city2, &cityPtr2)) return false;

  bool wasAdded1 = false;
  bool wasAdded2 = false;

  if(cityPtr1 != NULL && cityPtr2 != NULL){
    Neigh *neighPtr = NULL;
    if(!searchNeigh(cityPtr1->neighbours, cityPtr2, &neighPtr)) return false;
    if(neighPtr != NULL) return false;
  }

  if(cityPtr1 == NULL){
    cityPtr1 = createCity((char *)city1);
    if(cityPtr1 == NULL) return false;
    if(!insert(&(map->cities), cityPtr1, 1)){
      free(cityPtr1);
      return false;
    }
    wasAdded1 = true;
  }

  if(cityPtr2 == NULL){
    cityPtr2 = createCity((char *)city2);
    if(cityPtr2 == NULL){
      if(wasAdded1){
        removeNode(&(map->cities), cityPtr1, 1);
        free(cityPtr1);
      }
      return false;
    }
    if(!insert(&(map->cities), cityPtr2, 1)){
      if(wasAdded1){
        removeNode(&(map->cities), cityPtr1, 1);
        free(cityPtr1);
      }
      free(cityPtr2);
      return false;
    }
    wasAdded2 = true;
  }

  Neigh *neighPtr1 = createNeigh(cityPtr2, length, builtYear);
  if(neighPtr1 == NULL){
    if(wasAdded1){
      removeNode(&(map->cities), cityPtr1, 1);
      free(cityPtr1);
    }
    if(wasAdded2){
      removeNode(&(map->cities), cityPtr2, 1);
      free(cityPtr2);
    }
    return false;
  }

  Neigh *neighPtr2 = createNeigh(cityPtr1, length, builtYear);
  if(neighPtr2 == NULL){
    if(wasAdded1){
      removeNode(&(map->cities), cityPtr1, 1);
      free(cityPtr1);
    }
    if(wasAdded2){
      removeNode(&(map->cities), cityPtr2, 1);
      free(cityPtr2);
    }
    free(neighPtr1);
    return false;
  }

  neighRoutesInit(neighPtr1);
  neighRoutesInit(neighPtr2);
  neighPtr1->reversed = neighPtr2;
  neighPtr2->reversed = neighPtr1;

  if(!insert(&(cityPtr1->neighbours), neighPtr1, 2)){
    if(wasAdded1){
      removeNode(&(map->cities), cityPtr1, 1);
      free(cityPtr1);
    }
    if(wasAdded2){
      removeNode(&(map->cities), cityPtr2, 1);
      free(cityPtr2);
    }
    free(neighPtr1);
    free(neighPtr2);
    return false;
  }

  if(!insert(&(cityPtr2->neighbours), neighPtr2, 2)){
    if(wasAdded1){
      removeNode(&(map->cities), cityPtr1, 1);
      free(cityPtr1);
    }
    if(wasAdded2){
      removeNode(&(map->cities), cityPtr2, 1);
      free(cityPtr2);
    }
    free(neighPtr1);
    free(neighPtr2);
    removeNode(&(cityPtr1->neighbours), neighPtr1, 2);
    return false;
  }

  return true;
}

bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear){
  if(*city1 == 0 || *city2 == 0) return false;
  if(map == NULL || strcmp(city1, city2) == 0 || repairYear == 0){
    return false;
  }

  City *cityPtr1 = NULL;
  City *cityPtr2 = NULL;

  if(!searchCity(map, (char *)city1, &cityPtr1)) return false;
  if(!searchCity(map, (char *)city2, &cityPtr2)) return false;

  if(cityPtr1 == NULL || cityPtr2 == NULL){
    return false;
  }

  Neigh *neighbour1 = NULL;
  Neigh *neighbour2 = NULL;
  if(!searchNeigh(cityPtr1->neighbours, cityPtr2, &neighbour1)) return false;
  if(neighbour1 == NULL) return false;
  neighbour2 = neighbour1->reversed;

  if(repairYear < neighbour1->date){
    return false;
  }

  neighbour1->date = repairYear;
  neighbour2->date = repairYear;

  return true;
}

bool newRoute(Map *map, unsigned routeId, const char *city1, const char *city2){
  if(*city1 == 0 || *city2 == 0) return false;
  if(map == NULL || routeId == 0 || routeId > 999 || strcmp(city1, city2) == 0){
    return false;
  }
  if(map->routes[routeId] != NULL){
    return false;
  }

  City *cityPtr1 = NULL;
  City *cityPtr2 = NULL;

  if(!searchCity(map, (char *)city1, &cityPtr1)) return false;
  if(!searchCity(map, (char *)city2, &cityPtr2)) return false;

  if(cityPtr1 == NULL || cityPtr2 == NULL){
    return false;
  }

  ListNode *shortestPath = NULL;
  if(!findShortestPath(cityPtr1, cityPtr2, NULL, map->cities, NULL, &shortestPath)){
    return false;
  }

  if(shortestPath == NULL) return false;

  map->routes[routeId] = shortestPath;

  ListNode *pathPtr = shortestPath;
  while(pathPtr != NULL){
    Neigh *neigh = (Neigh*)(pathPtr->valPtr);
    neigh->localRoutes[routeId] = true;
    neigh->reversed->localRoutes[routeId] = true;
    pathPtr = pathPtr->next;
  }

  return true;
}

bool extendRoute(Map *map, unsigned routeId, const char *city){
  if(*city == 0) return false;
  if(map == NULL || routeId == 0 || routeId > 999){
    return false;
  }
  if(map->routes[routeId] == NULL) return false;

  City *cityPtr = NULL;
  if(!searchCity(map, (char*)city, &cityPtr)) return false;
  if(cityPtr == NULL) return false;

  ListNode *listPtr = map->routes[routeId];

  if(((Neigh*)(listPtr->valPtr))->reversed->dest == cityPtr) return false;

  bool found = false;
  while(true){
    if(((Neigh*)(listPtr->valPtr))->dest == cityPtr) found = true;
    if(listPtr->next == NULL) break;
    listPtr = listPtr->next;
  }
  if(found) return false;

  City *begCity = ((Neigh*)(map->routes[routeId]->valPtr))->reversed->dest;
  City *endCity = ((Neigh*)(listPtr->valPtr))->dest;

  ListNode *begPath = NULL;
  if(!findShortestPath(begCity, cityPtr, map->routes[routeId], map->cities, NULL, &begPath)){
    return false;
  }

  ListNode *oldList = begPath;
  if(!reverseNeighList(begPath, &begPath)){
      freeList(begPath);
      return false;
  }
  freeList(oldList);

  uint64_t begDist = cityPtr->dijkDist;
  int32_t begYoungestOldest = cityPtr->dijkYoungestOldest;

  ListNode *endPath = NULL;
  if(!findShortestPath(endCity, cityPtr, map->routes[routeId], map->cities, NULL, &endPath)){
    freeList(begPath);
    return false;
  }
  uint64_t endDist = cityPtr->dijkDist;
  int32_t endYoungestOldest = cityPtr->dijkYoungestOldest;

  if(begDist < endDist){
    if(begPath != NULL){
      ListNode *begPathPtr = begPath;

      while(true){
        Neigh *neigh = (Neigh*)(begPathPtr->valPtr);
        neigh->localRoutes[routeId] = true;
        neigh->reversed->localRoutes[routeId] = true;
        if(begPathPtr->next == NULL) break;
        begPathPtr = begPathPtr->next;
      }
      begPathPtr->next = map->routes[routeId];
      map->routes[routeId] = begPath;

      freeList(endPath);
      return true;
    }
  }

  else if(endDist < begDist){
    if(endPath != NULL){
      listPtr->next = endPath;

      ListNode *endPathPtr = endPath;
      while(endPathPtr != NULL){
        Neigh *neigh = (Neigh*)(endPathPtr->valPtr);
        neigh->localRoutes[routeId] = true;
        neigh->reversed->localRoutes[routeId] = true;
        endPathPtr = endPathPtr->next;
      }

      freeList(begPath);
      return true;
    }
  }

  else if(begDist != INFINITY) {
    if(begYoungestOldest > endYoungestOldest){
      if(begPath != NULL){
        ListNode *begPathPtr = begPath;

        while(true){
          Neigh *neigh = (Neigh*)(begPathPtr->valPtr);
          neigh->localRoutes[routeId] = true;
          neigh->reversed->localRoutes[routeId] = true;
          if(begPathPtr->next == NULL) break;
          begPathPtr = begPathPtr->next;
        }
        begPathPtr->next = map->routes[routeId];
        map->routes[routeId] = begPath;

        freeList(endPath);
        return true;
      }
    }

    else if(endYoungestOldest > begYoungestOldest){
      if(endPath != NULL){
        listPtr->next = endPath;

        ListNode *endPathPtr = endPath;
        while(endPathPtr != NULL){
          Neigh *neigh = (Neigh*)(endPathPtr->valPtr);
          neigh->localRoutes[routeId] = true;
          neigh->reversed->localRoutes[routeId] = true;
          endPathPtr = endPathPtr->next;
        }

        freeList(begPath);
        return true;
      }
    }
  }

  freeList(begPath);
  freeList(endPath);
  return false;
}

bool removeRoad(Map *map, const char *city1, const char *city2){
  if(*city1 == 0 || *city2 == 0) return false;
  if(map == NULL || strcmp(city1, city2) == 0) return false;

  City *cityPtr1 = NULL;
  City *cityPtr2 = NULL;
  if(!searchCity(map, (char*)city1, &cityPtr1)) return false;
  if(!searchCity(map, (char*)city2, &cityPtr2)) return false;

  if(cityPtr1 == NULL || cityPtr2 == NULL){
    return false;
  }

  Neigh *neighbour2 = NULL;
  if(!searchNeigh(cityPtr1->neighbours, cityPtr2, &neighbour2)) return false;
  if(neighbour2 == NULL) return false;

  ListNode ***leftPointers = (ListNode***)malloc(1000 * sizeof(ListNode**));
  if(leftPointers == NULL) return false;

  ListNode **rightPointers = (ListNode**)malloc(1000 * sizeof(ListNode*));
  if(rightPointers == NULL){
    free(leftPointers);
    return false;
  }

  ListNode **paths = (ListNode* *)malloc(1000 * sizeof(ListNode*));
  if(paths == NULL){
    free(leftPointers);
    free(rightPointers);
    return false;
  }

  for(int i=0; i<1000; i++){
    leftPointers[i] = NULL;
    rightPointers[i] = NULL;
    paths[i] = NULL;
  }

  for(int routeId=1; routeId<1000; routeId++){
    if(neighbour2->localRoutes[routeId] == false) continue;

    Neigh *orientedRoad = NULL;
    ListNode *listPtr = map->routes[routeId];
    ListNode **leftPtr = &(map->routes[routeId]);
    ListNode *rightPtr = NULL;

    while(listPtr != NULL){
      rightPtr = listPtr->next;
      Neigh *road = (Neigh*)(listPtr->valPtr);
      Neigh *rev = road->reversed;
      City *first = rev->dest;
      City *sec = road->dest;

      if((first == cityPtr1 && sec == cityPtr2) || (first == cityPtr2 && sec == cityPtr1)){
        orientedRoad = road;
        break;
      }

      leftPtr = &(listPtr->next);
      listPtr = listPtr->next;
    }

    orientedRoad->forbid = true;
    orientedRoad->reversed->forbid = true;
    City *lCity = orientedRoad->reversed->dest;
    City *rCity = orientedRoad->dest;

    ListNode *path = NULL;
    if(!findShortestPath(lCity, rCity, map->routes[routeId], map->cities, rCity, &path)){
      free(leftPointers);
      free(rightPointers);

      for(int32_t i=0; i<1000; i++){
        freeList(paths[i]);
      }
      free(paths);
      orientedRoad->forbid = false;
      orientedRoad->reversed->forbid = false;
      return false;
    }
    orientedRoad->forbid = false;
    orientedRoad->reversed->forbid = false;

    if(path == NULL){
      free(leftPointers);
      free(rightPointers);

      for(int32_t i=0; i<1000; i++){
        freeList(paths[i]);
      }
      free(paths);
      return false;
    }

    leftPointers[routeId] = leftPtr;
    rightPointers[routeId] = rightPtr;
    paths[routeId] = path;
  }

  for(int routeId=1; routeId<1000; routeId++){
    if(neighbour2->localRoutes[routeId] == false) continue;

    ListNode *endOfPath = paths[routeId];
    while(true){
      Neigh *road = (Neigh*)(endOfPath->valPtr);
      road->localRoutes[routeId] = true;
      road->reversed->localRoutes[routeId] = true;
      if(endOfPath->next == NULL) break;
      endOfPath = endOfPath->next;
    }

    free(*(leftPointers[routeId]));
    *(leftPointers[routeId]) = paths[routeId];
    endOfPath->next = rightPointers[routeId];
  }

  Neigh *rev = neighbour2->reversed;
  removeNode(&(cityPtr1->neighbours), neighbour2, 2);
  removeNode(&(cityPtr2->neighbours), rev, 2);
  deleteNeigh(neighbour2);
  deleteNeigh(rev);
  free(leftPointers);
  free(rightPointers);
  free(paths);
  return true;
}

bool checkRoad(Map *map, char const *city1, char const *city2, uint32_t length, int32_t year, int32_t *result){
  if(*city1 == 0 || *city2 == 0) return false;
  if(map == NULL || strcmp(city1, city2) == 0 || length == 0 || year == 0){
    return false;
  }

  City *cityPtr1 = NULL;
  City *cityPtr2 = NULL;
  if(!searchCity(map, (char *)city1, &cityPtr1)) return false;
  if(!searchCity(map, (char *)city2, &cityPtr2)) return false;

  if(cityPtr1 == NULL || cityPtr2 == NULL){
    *result = 1;
    return true;
  }

  Neigh *neighPtr = NULL;
  if(!searchNeigh(cityPtr1->neighbours, cityPtr2, &neighPtr)) return false;

  if(neighPtr == NULL){
    *result = 1;
    return true;
  }

  if(neighPtr->length != length || neighPtr->date > year){
    return false;
  }

  if(neighPtr->date == year){
    *result = 3;
    return true;
  }

  *result = 2;
  return true;
}

char const *getRouteDescription(Map *map, unsigned routeId){
  char *result = (char*)malloc(sizeof(char) * 10);
  if(result == NULL) return NULL;
  *result = 0;

  if(map == NULL || routeId == 0 || routeId > 999) return result;

  if(map->routes[routeId] == NULL) return result;

  char *ptr = result;
  int32_t count = 0;
  int32_t bufferSize = 10;

  if(!appendInteger(&result, &ptr, routeId, &count, &bufferSize)){
    return NULL;
  }
  if(!addChar(&result, &ptr, ';', &count, &bufferSize)) return NULL;

  ListNode *listPtr = map->routes[routeId];
  char *headName = ((Neigh*)(listPtr->valPtr))->reversed->dest->name;
  if(!appendCityName(&result, &ptr, headName, &count, &bufferSize)) return NULL;
  if(!addChar(&result, &ptr, ';', &count, &bufferSize)) return NULL;

  while(listPtr != NULL){
    Neigh *actNeigh = (Neigh*)(listPtr->valPtr);
    if(!appendInteger(&result, &ptr, actNeigh->length, &count, &bufferSize)){
      return NULL;
    }
    if(!addChar(&result, &ptr, ';', &count, &bufferSize)) return NULL;

    int32_t date_pos = actNeigh->date;
    if(date_pos < 0){
      if(!addChar(&result, &ptr, '-', &count, &bufferSize)) return NULL;
      date_pos = -date_pos;
    }

    if(!appendInteger(&result, &ptr, date_pos, &count, &bufferSize)){
      return NULL;
    }
    if(!addChar(&result, &ptr, ';', &count, &bufferSize)) return NULL;

    if(!appendCityName(&result, &ptr, actNeigh->dest->name, &count, &bufferSize)){
      return NULL;
    }
    if(listPtr->next != NULL){
      if(!addChar(&result, &ptr, ';', &count, &bufferSize)) return NULL;
    }
    listPtr = listPtr->next;
  }
  return result;
}
