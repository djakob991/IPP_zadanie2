/** @file
 * Parser wejścia
 *
 * @author Jakub Organa
 * @date 13.05.2019
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 * Struktura przechowująca zparsowane dane z wejścia
 */
typedef struct Info {
  /*@{*/
  int32_t code; /**< kod, w zależności od danych ERROR, IGNORE, ADD, REPAIR, DESCR lub CREATE */
  int32_t size; /**< rozmiar tablicy stringów z wejscia */
  char const **args; /**< tablica stringów z wejścia */
  char *beg; /**< wskaźnik na zaalokowaną na daną linię przez readLine pamięć */
  /*@}*/
} Info;

/** @brief Tworzy nową strukturę "Info"
 * @return Zwraca wskaźnik na powstałą strukturę.
 */
Info *createInfo();

/** @brief Czyta linię ze standardowego wejścia i zapisuje ją w dest.
 * @param[out] dest      - wskaźnik na stringa, w którym zostanie zapisana wczytana linia
 * @return Zwraca true w przypadku sukcesu, lub false jeśli nie udało się zaalokować pamięci.
 */
bool readLine(char **dest);

/** @brief Zapisuje w dest reprezentację danego stringa jako int.
 * Założenie: string składa się z cyfr (na pierwszej pozycji dopuszczany '-')
 * @param[in] s      - string
 * @param[out] dest      - wskaźnik na inta, w którym zostanie zapisany rezultat
 * @return zwraca true w przypadku sukcesu, lub false gdy string nie był poprawny:
 * jego wartość była poza zakresem, posiadał zera wiodące.
 */
bool toSigned(char const *s, int32_t *dest);

/** @brief Zapisuje w dest reprezentację danego stringa jako unsigned.
 * Założenie: string składa się z cyfr
 * @param[in] s      - string
 * @param[out] dest      - wskaźnik na unsigned, w którym zostanie zapisany rezultat
 * @return zwraca true w przypadku sukcesu, lub false gdy string nie był poprawny:
 * jego wartość była poza zakresem, posiadał zera wiodące.
 */
bool toUnsigned(char const *s, uint32_t *dest);

/** @brief Na podstawie linii z wejścia, zapisuje zparsowane dane we wskazanej strukturze "Info"
 * @param[in] s      - linia z wejścia
 * @param[out] dest      - wskaźnik na strukturę Info, na której zostaną zapisane dane
 * @return Zwraca true w przypadku sukcesu, lub false jeśli nie udało się zaalokować pamięci.
 */
bool whatToDo(char *s, Info *dest);

#endif /* __PARSER_H__ */
