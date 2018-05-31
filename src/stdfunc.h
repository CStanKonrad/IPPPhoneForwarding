/** @file
 * Pomocne makra.
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 31.05.2018
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
 * @brief Zamienia x w ciąg znaków w stylu c.
 * @see STR
 */
#define TO_STRING(x) #x

/**
 * @brief Zamienia wartość x w ciąg znaków w stylu c.
 */
#define TO_STRING2(x) TO_STRING(x)

/**
 * @brief Wyłuskuje pierwszy znak z ciągu znaków w stylu c.
 */
#define STRING_TO_CHAR(x) (*(x))

/**
 * @brief Zamienia napis na ciąg znaków.
 */
#define STR(x) TO_STRING(x)

/**
 * @brief Skleja ciągi znaków w stylu c.
 */
#define CONCAT(A, B, C)   (A B C)

#endif //TELEFONY_STDFUNC_H
