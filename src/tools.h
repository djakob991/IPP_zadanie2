/** @file
 * Zestaw narzędzi potrzebnych przy implementacji obsługi map dróg krajowych.
 *
 * @author Jakub Organa
 * @date 29.04.2019
 */

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include "types.h"

/** @brief Wyszukuje element w treapie.
 * Na podstawie wartości compareId wyszukuje podanym treapie element.
 * @param[in] root      - wskaźnik na korzeń treapa
 * @param[in] valPtr      - wskaźnik na strukturę o wartości, którą należy wyszukać.
 * @param[in] compareId      - informuje komparator jakiego typu są wartości w treapie.
 * @return wskaźnik na strukturę wyszukaną w treapie
 */
void *search(TreapNode *root, void *valPtr, int32_t compareId);

/** @brief Dodaje element do treapa.
 * Na podstawie wartości compareId dodaje do treapa nowy element zawierający
 * wskaźnik na wskazaną strukturę.
 * @param[in, out] treapPtr      - podwójny wskaźnik na korzeń treapa
 * @param[in] valPtr      - wskaźnik na strukturę, którą należy dodać
 * @param[in] compareId      - informuje komparator jakiego typu są wartości w treapie.
 * @return zwraca true jeśli udało się dodać element, lub false jeśli nie udało się zaalokować pamięci.
 */
bool insert(TreapNode **treapPtr, void *valPtr, int32_t compareId);

/** @brief Usuwa element z treapa.
 * Na podstawie wartości compareId usuwa z treapa element zawierający wskaźnik
 * na wskazaną strukturę.
 * @param[in, out] treapPtr      - podwójny wskaźnik na korzeń treapa
 * @param[in] valPtr      - wskaźnik na strukturę, którą należy usunąć
 * @param[in] compareId      - nformuje komparator jakiego typu są wartości w treapie.
 */
void removeNode(TreapNode **treapPtr, void *valPtr, int32_t compareId);

/** @brief Usuwa z pamięci treapa, nie usuwając jednak structów w nim przechowywanych.
 * @param[in, out] root      - wskaźnik na korzeń treapa
 */
void flat_deleteTreap(TreapNode *root);

/** @brief Usuwa miasto.
 * Usuwa z pamięci miasto, a w tym jego nazwę.
 * @param[in] cityPtr      - wkaźnik na miasto
 */
void deleteCity(City *cityPtr);

/** @brief Usuwa treap zawierający miasta.
 * Usuwa treap zawierający miasta, a także dla każdego miasta usuwa treap
 * zawierający sąsiadów.
 * @param[in, out] root      - wkaźnik na korzeń treapa miast
 */
void deleteCityTreap(TreapNode *root);

/** @brief Znajduje ścieżkę między podanymi miastami.
 * Znajduje najlepszą ścieżkę (o właściwościach opisanym w dokumentacji map.h),
 * nieprzechodzącą przez elementy wskazanej drogi krajowej, z pominięciem miasta
 * wskazanego przez valCity
 * @param[in] cityPtr1      - wskaznik na miasto startowe
 * @param[in] cityPtr2      - wskaznik na miasto docelowe
 * @param[in] route      - wskaznik na drogę krajową, przez którą ścieżka nie może przechodzić
 * @param[in] cities      - wskaźnik na korzeń treapa miast
 * @param[in] valCity      - wskaźnik na miasto należące do drogi krajowej, przez które droga może przechodzić
 * @param[out] target      - podwójny wskaźnik na listę, w której będzie zapisany wynik
 * @return Zwraca true jeśli udało się wyznaczyć ścieżkę, lub false jeśli nie udało się zaalokować pamięci.
 */
bool findShortestPath(City *cityPtr1, City *cityPtr2, ListNode *route, TreapNode *cities, City *valCity, ListNode **target);

/** @brief Dodaje char do wskazanego stringa.
 * Dodaje znak do wskazanego stringa, w razie potrzeby realokouje pamięć.
 * @param[in, out] str      - podwójny wkaźnik na stringa
 * @param[in, out] ptr      - podwójny wskaźnik na koniec stringa
 * @param[in] c      - znak, który należy dodać
 * @param[in, out] count      - wskaźnik na długość stringa
 * @param[in, out] bufferSize      - wskaźnik na ilość zaalokowanej pamięci
 * @return Zwraca true jeśli operacja się powiodła, lub false jeśli nie udało się zaalokować pamięci.
 */
bool addChar(char **str, char **ptr, char c, int32_t *count, int32_t *bufferSize);

/** @brief Dokleja stringa do wskazanego stringa.
 * Dokleja stinga do wskazanego stringa, w razie potrzeby realokouje pamięć.
 * @param[in, out] str      - podwójny wkaźnik na stringa
 * @param[in, out] ptr      - podwójny wskaźnik na koniec stringa
 * @param[in] name      - wskaźnik na stringa, który należy dokleić
 * @param[in, out] count      - wskaźnik na długość stringa
 * @param[in, out] bufferSize      - wskaźnik na ilość zaalokowanej pamięci
 * @return Zwraca true jeśli operacja się powiodła, lub false jeśli nie udało się zaalokować pamięci.
 */
bool appendCityName(char **str, char **ptr, char *name, int32_t *count, int32_t *bufferSize);

/** @brief Dokleja stringa reprezentującego liczbę do wskazanego stringa.
 * okleja stringa reprezentującego liczbę do wskazanego stringa, w razie potrzeby realokouje pamięć.
 * @param[in, out] str      - podwójny wkaźnik na stringa
 * @param[in, out] ptr      - podwójny wskaźnik na koniec stringa
 * @param[in] value      - liczba, której reprezentację w postaci stringa należy dokleić.
 * @param[in, out] count      - wskaźnik na długość stringa
 * @param[in, out] bufferSize      - wskaźnik na ilość zaalokowanej pamięci
 * @return Zwraca true jeśli operacja się powiodła, lub false jeśli nie udało się zaalokować pamięci.
 */
bool appendInteger(char **str, char **ptr, int64_t value, int32_t *count, int32_t *bufferSize);


#endif /* __TOOLS_H__ */
