#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "types.h"
#include "tools.h"
#include "parser.h"
#include "map.h"

extern int32_t ERROR;
extern int32_t IGNORE;
extern int32_t ADD;
extern int32_t REPAIR;
extern int32_t DESCR;
extern int32_t CREATE;

void free_ptrs(Info *info){
  free(info->args);
  info->args = NULL;
  free(info->beg);
  info->beg = NULL;
}

void callError(Info *info, int32_t line){
  free_ptrs(info);
  fprintf(stderr, "ERROR %d\n", line);
}

int32_t main(){
  Info *info = createInfo();
  if(info == NULL){
    exit(1);
  }

  Map *m = newMap();
  if(m == NULL){
    free(info);
    exit(1);
  }

  int32_t line = 0;
  while(1){
    line++;

    char *s = NULL;
    if(!readLine(&s)){
      free(info);
      deleteMap(m);
      exit(1);
    }

    if(!whatToDo(s, info)){
      free(s);
      free(info);
      deleteMap(m);
      exit(1);
    }

    if(feof(stdin)){
      if(info->code != IGNORE) {
        fprintf(stderr, "ERROR %d\n", line);
      }
      free_ptrs(info);
      break;
    }

    if(info->code == ERROR){
      callError(info, line);
      continue;
    }

    if(info->code == IGNORE){
      free_ptrs(info);
      continue;
    }

    if(info->code == ADD){
      uint32_t length;
      int32_t year;
      toUnsigned(info->args[3], &length);
      toSigned(info->args[4], &year);

      if(addRoad(m, info->args[1], info->args[2], length, year)){
        free_ptrs(info);
      } else callError(info, line);
      continue;
    }

    if(info->code == REPAIR){
      int32_t year;
      toSigned(info->args[3], &year);
      if(repairRoad(m, info->args[1], info->args[2], year)){
        free_ptrs(info);
      } else callError(info, line);
      continue;
    }

    if(info->code == DESCR){
      uint32_t routeId;
      toUnsigned(info->args[1], &routeId);

      char const *s = getRouteDescription(m, routeId);
      if(s != NULL){
        printf("%s\n", s);
        free_ptrs(info);
	      free((char*)s);
      } else callError(info, line);
      continue;
    }

    if(info->code == CREATE){
      TreapNode *treap = NULL;
      uint32_t routeId;
      toUnsigned(info->args[0], &routeId);

      if(routeId == 0 || routeId > 999 || routeExists(m, routeId)){
        callError(info, line);
        continue;
      }

      int32_t roads_count = (info->size - 2)/3;
      int32_t *roads_info = (int32_t*)malloc(roads_count * sizeof(int32_t));
      if(roads_info == NULL){
        callError(info, line);
        continue;
      }

      if(!insert(&treap, (char*)info->args[1], 4)){
        free(roads_info);
        callError(info, line);
        continue;
      }

      int32_t i = 1;
      int32_t road_i = 0;
      bool isBad = false;

      while(i < info->size - 1){
        char const *search_res = (char const *)search(treap, (char*)info->args[i+3], 4);
        if(search_res != NULL){
          isBad = true;
          break;
        }

        uint32_t length;
        int32_t year;
        toUnsigned(info->args[i+1], &length);
        toSigned(info->args[i+2], &year);

        if(!checkRoad(m, info->args[i], info->args[i+3], length, year, &roads_info[road_i])){
          isBad = true;
          break;
        }
        if(!insert(&treap, (char*)info->args[i+3], 4)){
          isBad = true;
          break;
        }

        i += 3;
        road_i++;
      }
      flat_deleteTreap(treap);

      if(isBad){
        free(roads_info);
        callError(info, line);
        continue;
      }

      ListNode *list = NULL;
      ListNode **ptr = &list;

      i = 1;
      road_i = 0;

      while(i < info->size - 1){
        uint32_t length;
        int32_t year;
        toUnsigned(info->args[i+1], &length);
        toSigned(info->args[i+2], &year);

        if(roads_info[road_i] == 1){
          addRoad(m, info->args[i], info->args[i+3], length, year);
        }
        if(roads_info[road_i] == 2){
          repairRoad(m, info->args[i], info->args[i+3], year);
        }

        Neigh *road = NULL;
        searchRoad(m, info->args[i], info->args[i+3], &road);

        ListNode *newPtr = createListNode(road);
        *ptr = newPtr;
        ptr = &(newPtr->next);

        i += 3;
        road_i++;
      }

      setRoute(m, routeId, list);
      free(roads_info);
      free_ptrs(info);
      continue;
    }

    free_ptrs(info);
  }

  free(info);
  deleteMap(m);
  m = NULL;

  return 0;
}
