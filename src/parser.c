#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "parser.h"

int32_t ERROR = -2;
int32_t IGNORE = -1;
int32_t ADD = 0;
int32_t REPAIR = 1;
int32_t DESCR = 2;
int32_t CREATE = 3;

char const *_add = "addRoad";
char const *_repair = "repairRoad";
char const *_descr = "getRouteDescription";

uint64_t unsigned_MAX = 4294967295;
int64_t int_MAX = 2147483647;

Info *createInfo(){
  Info *new = (Info*)malloc(sizeof(Info));
  if(new == NULL) return NULL;
  new->code = -2;
  new->args = NULL;
  new->beg = NULL;
  new->size = 0;

  return new;
}

void writeInfo(int32_t code, char const **args, int32_t size, Info *dest, char *beg){
  dest->code = code;
  dest->args = args;
  dest->size = size;
  dest->beg = beg;
}

bool readLine(char **dest) {
  uint32_t buffer_size = 10;
  uint32_t counter = 0;

  char *str = (char*)malloc(buffer_size);
  if(str == NULL) exit(1);
  char *ptr = str;

  bool isBad = 0;
  bool isFirst = 1;
  bool ignore = 0;

  while (1) {
    if (counter == buffer_size - 1) {
      buffer_size *= 2;
      char *old = str;
      str = (char*)realloc(str, buffer_size);
      if(str == NULL){
        free(old);
        *dest = NULL;
        return false;
      }
      ptr = str + counter;
    }

    *ptr = fgetc(stdin);

    if (feof(stdin)) {
      break;
    }

    if (isFirst) {
      if (*ptr == '#') ignore = 1;
      isFirst = 0;
    }

    if (*ptr == 10) break;
    if (*ptr <= 31 && *ptr >= 0) isBad = 1;

    counter++;
    ptr++;
  }

  if (isBad && !ignore) {
    free(str);
    *dest = NULL;
    return true;
  }

  *ptr = 0;
  *dest = str;
  return true;
}

char const *extract(char *s, uint32_t *curr_dist, bool *isNumber, bool *alphCheck) {
  char *ptr = s;
  *curr_dist = 0;
  *isNumber = 1;
  *alphCheck = 1;
  bool isFirst = true;

  while (1) {
    if (*ptr == 0 || *ptr == ';') break;
    if (*ptr <= 31 && *ptr >= 0) *alphCheck = 0;
    if(isFirst){
      if ((*ptr < '0' || *ptr > '9') && *ptr != '-') *isNumber = 0;
      isFirst = false;
    }
    else if (*ptr < '0' || *ptr > '9') *isNumber = 0;
    (*curr_dist)++;
    ptr++;
  }

  *ptr = 0;
  return s;
}

bool toUnsigned(char const *s, uint32_t *dest){
  if(*s == '-' || *s == 0) return false;
  int32_t len = strlen(s);
  uint32_t result = 0;

  for(int32_t i = 0; i < len; i++){
    uint64_t new_val = 10 * (uint64_t)result + s[i] - '0';
    if(new_val > unsigned_MAX) return false;
    result = (uint32_t)new_val;
  }
  *dest = result;
  return true;
}

bool toSigned(char const *s, int32_t *dest){
  bool neg = false;
  if(*s == '-'){
    neg = true;
    s++;
  }
  if(*s == 0) return false;
  int32_t len = strlen(s);
  int32_t result = 0;

  for(int32_t i = 0; i < len; i++){
    int64_t new_val = 10 * (int64_t)result + s[i] - '0';
    if(!neg && new_val > int_MAX) return false;
    if(neg && new_val > int_MAX + 1) return false;
    result = (int32_t)new_val;
  }

  if(neg) result = -result;
  *dest = result;
  return true;
}

void free_args(void *tab1, void *tab2){
  free(tab1);
  free(tab2);
}

bool whatToDo(char *s, Info *dest){
  if(s == NULL){
    writeInfo(ERROR, NULL, 0, dest, s);
    return true;
  }

  if(*s == '#' || *s == 0){
    writeInfo(IGNORE, NULL, 0, dest, s);
    return true;
  }

  uint32_t buffer_size = 5;
  uint32_t size = 0;
  char const **args = (char const**)malloc(buffer_size * sizeof(char const*));
  if(args == NULL) return false;

  bool *num = (bool*)malloc(buffer_size * sizeof(bool));
  if(num == NULL){
    free(args);
    return false;
  }

  bool *alph = (bool*)malloc(buffer_size * sizeof(bool));
  if(alph == NULL){
    free(args);
    free(num);
    return false;
  }

  uint32_t curr_dist = 0;
  uint32_t curr_sum = 0;
  uint32_t len = strlen(s) + 1;

  while(curr_sum < len){
    if(size + 1 >= buffer_size){
      buffer_size *= 2;

      char const **old_args = args;
      args = (char const**)realloc(args, buffer_size * sizeof(char const*));
      if(args == NULL){
        free(old_args);
        free(num);
        free(alph);
        return false;
      }

      bool *old_num = num;
      num = (bool*)realloc(num, buffer_size * sizeof(bool));
      if(num == NULL){
        free(args);
        free(old_num);
        free(alph);
        return false;
      }

      bool *old_alph = alph;
      alph = (bool*)realloc(alph, buffer_size * sizeof(bool));
      if(alph == NULL){
        free(args);
        free(num);
        free(old_alph);
        return false;
      }
    }

    args[size] = extract(s + curr_sum, &curr_dist, &num[size], &alph[size]);
    size++;
    curr_sum += curr_dist + 1;
  }

  int32_t icheck = 0;
  uint32_t ucheck = 0;

  if(num[0]){
    if(size < 3 || !toUnsigned(args[0], &ucheck) || !alph[1] || (size-2)%3 != 0){
      free_args(num, alph);
      writeInfo(ERROR, args, size, dest, s);
      return true;
    }

    int32_t id = 2;
    int32_t routes_count = (size-2)/3;

    for(int32_t i=0; i<routes_count; i++){
      for(int32_t j=1; j<=3; j++){

        if(j == 1){
          if(!num[id] || !toUnsigned(args[id], &ucheck)){
            free_args(num, alph);
            writeInfo(ERROR, args, size, dest, s);
            return true;
          }
        }

        if(j == 2){
          if(!num[id] || !toSigned(args[id], &icheck)){
            free_args(num, alph);
            writeInfo(ERROR, args, size, dest, s);
            return true;
          }
        }

        if(j == 3){
          if(!alph[id]){
            free_args(num, alph);
            writeInfo(ERROR, args, size, dest, s);
            return true;
          }
        }

        id++;
      }
    }

    writeInfo(CREATE, args, size, dest, s);
    free_args(num, alph);
    return true;
  }

  else if(alph[0]){
    bool add_cmp = !strcmp(args[0], _add);
    bool repair_cmp = !strcmp(args[0], _repair);
    bool descr_cmp = !strcmp(args[0], _descr);

    if(add_cmp || repair_cmp){
      if(size < 4 || !alph[1] || !alph[2]){
        free_args(num, alph);
        writeInfo(ERROR, args, size, dest, s);
        return true;
      }

      if(repair_cmp){
        if(size != 4 || !num[3] || !toSigned(args[3], &icheck)){
          free_args(num, alph);
          writeInfo(ERROR, args, size, dest, s);
          return true;
        }

        writeInfo(REPAIR, args, size, dest, s);
        free_args(num, alph);
        return true;
      }

      if(add_cmp){
        if(size != 5 || !num[3] || !toUnsigned(args[3], &ucheck)){
          free_args(num, alph);
          writeInfo(ERROR, args, size, dest, s);
          return true;
        }

        if(!num[4] || !toSigned(args[4], &icheck)){
          free_args(num, alph);
          writeInfo(ERROR, args, size, dest, s);
          return true;
        }

        writeInfo(ADD, args, size, dest, s);
        free_args(num, alph);
        return true;
      }
    }

    if(descr_cmp){
      if(size != 2 || !num[1] || !toUnsigned(args[1], &ucheck)){
        free_args(num, alph);
        writeInfo(ERROR, args, size, dest, s);
        return true;
      }

      writeInfo(DESCR, args, size, dest, s);
      free_args(num, alph);
      return true;
    }
  }

  free_args(num, alph);
  writeInfo(ERROR, args, size, dest, s);
  return true;
}
