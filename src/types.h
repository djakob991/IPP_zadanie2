/** @file
 * Zestaw typów (i funkcji z nimi związanych) potrzebnych przy implementacji obsługi map dróg krajowych.
 *
 * @author Jakub Organa
 * @date 29.04.2019
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

/** @brief Znajduje minimalną wartość
 * @param[in] a      - wartość a
 * @param[in] b      - wartość b
 * @return Zwraca minimum z wartości a i b
 */
int64_t min(int64_t a, int64_t b);

/** @brief Znajduje maksymalną wartość
 * @param[in] a      - wartość a
 * @param[in] b      - wartość b
 * @return Zwraca maksimum z wartości a i b
 */
int64_t max(int64_t a, int64_t b);

/**
 * Element listy wskaźnikowej
 */
typedef struct ListNode {
  /*@{*/
  void *valPtr; /**< wartość */
  struct ListNode *next; /**< wskaźnik na kolejny element */
  /*@}*/
} ListNode;

/**
 * Element treapa
 */
typedef struct TreapNode {
  /*@{*/
  void *valPtr; /**< wartość */
  int32_t priority; /**< priorytet */
  struct TreapNode *left; /**< wskaźnik na lewego syna */
  struct TreapNode *right; /**< wskaźnik na prawego syna */
  /*@}*/
} TreapNode;

/**
 * Struktura reprezentująca miasto
 */
typedef struct City {
  /*@{*/
  char *name; /**< nazwa miasta */
  struct TreapNode *neighbours; /**< treap sąsiadów */
  uint64_t dijkDist; /**< odległość na ścieżce (potrzebne do algorytmu Dijkstry) */
  int32_t dijkYoungestOldest; /**< najmłodszy z najstarszych na ścieżce (generowane w trakcie algorytmu Dijkstry) */
  bool isInRoute; /**< przechowuje informację, czy miasto znajduje się w aktualnie rozpatrywanej drodze krajowej */
  int32_t dijkInCount; /**< liczba ścieżek, które weszły do tego miasta w trakcie algorytmu Dijkstry */
  /*@}*/
} City;

/**
 * Struktura przechowująca zakończenia ścieżek w trakcie algorytmu Dijksty
 */
typedef struct dijkVal{
  /*@{*/
  struct City *cityPtr; /**< wskaźnik na miasto */
  uint64_t actDist; /**< aktualna odległość */
  int32_t actOldest; /**< najstarszy na ścieżce */
  /*@}*/
} dijkVal;

/**
 * Struktura reprezentująca odcinek drogi (skierowany)
 */
typedef struct Neigh {
  /*@{*/
  struct City *dest; /**< wskaźnik na miasto docelowe */
  uint32_t length; /**< długość */
  int32_t date; /**< rok budowy/ostatniego remontu */
  struct Neigh *reversed; /**< wkaźnik odpowiedni odcinek drogi skierowany przeciwnie */
  bool *localRoutes; /**< tablica przechowująca informacje o drogach krajowych, które przechodzą przez odcinek */
  bool forbid; /**< informacja dla algorytmu Dijksty, czy przejście przez dany odcinek jest zabronione */
  /*@{*/
} Neigh;


/** @brief Tworzy nowy element typu ListNode
 * @param[in] valPtr      - wskaźnik na wartość
 * @return Zwraca wskaźnik na utworzony element.
 */
ListNode *createListNode(void *valPtr);

/** @brief Tworzy nowy element typu dijkVal
 * @param[in] cityPtr      - wskaźnik na miasto
 * @param[in] actDist      - odległość
 * @param[in] actOldest      - najstarszy
 * @return Zwraca wskaźnik na utworzony element.
 */
dijkVal *createDijkVal(City *cityPtr, uint64_t actDist, int32_t actOldest);

/** @brief Tworzy nowe miasto
 * @param[in] name      - nazwa miasta
 * @return Zwraca wskaźnik na utworzony element.
 */
City *createCity(char *name);

/** @brief Tworzy nowy element typu Neigh
 * @param[in] cityPtr      - wskaźnik na miasto docelowe
 * @param[in] length      - długość
 * @param[in] date      - rok budowy
 * @return Zwraca wskaźnik na utworzony element.
 */
Neigh *createNeigh(City *cityPtr, uint32_t length, int32_t date);

/** @brief Tworzy nowy element typu TreapNode
 * @param[in] valPtr      - wskaźnik na wartość
 * @return Zwraca wskaźnik na utworzony element.
 */
TreapNode *createTreapNode(void *valPtr);

/** @brief Odwraca listę wskaźnikową odcinków drogowych.
 * @param[in] listPtr      - wskaźnik na początek listy
 * @param[out] target      - tu zostanie zapisany wskaźnik na nowo stworzoną listę
 * @return Zwraca true w przypadku powodzenia, lub false jeśli nie udało się zaalokować pamięci.
 */
bool reverseNeighList(ListNode *listPtr, ListNode **target);
/** @brief Usuwa listę.
 * @param[in] path      - wskaźnik na początek listy
 */
void freeList(ListNode *path);
/** @brief Usuwa element typu Neigh.
 * @param[in] neighPtr      - wskaźnik na Neigh
 */
void deleteNeigh(Neigh *neighPtr);

/** @brief Alokuje pamięć i tworzy tablicę localRoutes dla podanego Neigh
 * @param[in] ptr      - wskaźnik na Neigh
 * @return Zwraca true w przypadku powodzenia, lub false jeśli nie udało się zaalokować pamięci.
 */
bool neighRoutesInit(Neigh *ptr);


#endif /* __TYPES_H__ */
