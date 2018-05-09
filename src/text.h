/** @file
 * Pomocne funkcje do przetwarzania tekstu.
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 06.05.2018
 */

#ifndef TELEFONY_TEXT_H
#define TELEFONY_TEXT_H

#include <stddef.h>

/**
 * @brief Kopiuje @p length liter tekstu.
 * Kopiuje @p length liter tekstu @p src do @p dest
 * wstawiając na końcu '\0'.
 * @param[in] src - wskaźnik na tekst źródłowy.
 * @param[out] dest - wskaźnik na miejsce docelowe.
 * @param[in] length - długość tekstu do skopiowania.
 */
void copyText(const char *src, char *dest, size_t length);


/**
 * @brief Duplikuje text.
 * @param[in] txt - tekst do zduplikowania.
 * @return Wskaźnik do duplikatu tekstu @p txt,
 *         w przypadku problemów z pamięcią zwraca NULL.
 */
char *duplicateText(const char *txt);


/**
 * @brief Wskaźnik na koniec.
 * @param[in] str - tekst.
 * @return Wskaźnik do koniec tekstu @p str.
 */
const char *stringEnd(const char *str);


/**
 * @brief Łączy dwa teksty.
 * @param[in] a - wskaźnik na tekst.
 * @param[in] b - wskaźnik na tekst.
 * @return Wskaźnik na tekst będący połączaniem a i b,
 *         w przypadku problemów z pamięciom NULL.
 */
char *concatenate(const char *a, const char *b);

#endif //TELEFONY_TEXT_H
