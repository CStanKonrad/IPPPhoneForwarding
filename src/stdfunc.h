/** @file
 * Pomocne makra.
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.04.2018
 */

#ifndef TELEFONY_STDFUNC_H
#define TELEFONY_STDFUNC_H

/**
 * @brief Zwraca minimum z dwóch wartości.
 */
#define MIN(a, b) (((a)<(b))?(a):(b))

/**
 * @brief Zwraca maksimum z dwóch wartości.
 */
#define MAX(a, b) (((a)>(b))?(a):(b))

/**
 * @brief Makro pomocnicze.
 * @see STR
 */
#define TO_STRING(x) #x

/**
 * @brief Zamienia napis na ciąg znaków.
 */
#define STR(x) TO_STRING(x)

void copyText(const char *src, char * dest, size_t length);

const char *stringEnd(const char *str);

#endif //TELEFONY_STDFUNC_H