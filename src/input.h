/** @file
 * Interfejs modułu służącego do parsowania wejścia.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */

#ifndef MARATONFILMOWY_INPUT_H
#define MARATONFILMOWY_INPUT_H

#include <stddef.h>
#include "vector.h"

/**
 * @brief Kod pomyślnego zakończenia wczytywania.
 */
#define INPUT_READ_SUCCESS 3

/**
 * @see inputIgnoreLine
 */
#define INPUT_READ_TRASH_DETECTED 2

/**
 * @brief Kod błędu wczytywania.
 */
#define INPUT_READ_FAIL 0


/**
 * @brief Następny oczekujący znak.
 * @return Następny znak oczekujący na wczytanie
 *         ze standardowego wejścia.
 */
int inputPeekCharacter();

/**
 * @brief Następny znak.
 * Pobiera następny znak ze standardowego wejścia
 * i zwraca go.
 * @return Następny znak ze standardowego wejścia.
 */
int inputGetCharacter();

/**
 * @brief Pomija dopóki.
 * Pomija kolejne znaki ze standardowego wejścia
 * tak długo, jak długo funkcja (*predicate)(następny_znak)
 * zwraca wartość równą 0. Pierwszy znak dla którego,
 * predykat jest spełniony nie zostaje wczytany.
 * @param[in] predicate - predykat.
 * @return Liczba pominiętych znaków.
 */
size_t inputIgnoreUntil(int (*predicate)(int));

/**
 * @brief Pomija podczas gdy.
 * Pomija kolejne znaki ze standardowego wejścia
 * tak długo, jak długo funkcja (*predicate)(następny_znak)
 * zwraca wartość różną od 0. Pierwszy znak dla którego,
 * predykat nie jest spełniony nie zostaje wczytany.
 * @param[in] predicate - predykat.
 * @return Liczba pominiętych znaków.
 */
size_t inputIgnoreWhile(int (*predicate)(int));

/**
 * Pomija znaki ze standardowego wejścia tak długo jak długo predykat
 * characterIsBlank(znak) pozostaje spełniony.
 */
void inputIgnoreBlank();

/**
 * @brief Pomija białe znaki.
 * Pomija znaki ze standardowego wejścia tak długo jak długo predykat
 * characterIsWhite(znak) pozostaje spełniony.
 */
void inputIgnoreWhite();

/**
 * Pomija znak/znaki ze standardowego wejścia składające się na nową linię
 * w systemach (Windows, Unix-like, MacOs).
 */
void inputIgnoreNewLine();

/**
 * Pomija pojedynczy znak nowej uniksowej lini
 * @return W przypadku gdy oczekujący na wczytanie znak nie jest znakiem
 *         spełniającym predykat characterIsUnixNewLine INPUT_READ_FAIL,
 *         w przeciwnym wypadku INPUT_READ_SUCCESS.
 */
int inputIgnoreUnixNewLine();

/**
 * Pomija kolejne znaki ze standardowego wejścia
 * składające się na aktualnie przetwarzaną linię
 * (z wyjątkiem znaku nowej lini).
 * @return INPUT_READ_TRASH_DETECTED w przypadku wystąpienia w lini znaku
 *         nie będącego terminatorem (characterIsTerminator),
 *         w przeciwnym wypadku INPUT_READ_SUCCESS.
 */
int inputIgnoreLine();

/**
 * Pomija kolejne znaki ze standardowego wejścia
 * składające się na aktualnie przetwarzaną linię
 * (z wyjątkiem znaku uniksowej nowej lini).
 * @return INPUT_READ_TRASH_DETECTED w przypadku wystąpienia w lini znaku
 *         nie spełniającego predykatu characterIsUnixTerminator,
 *         w przeciwnym wypadku zwraca INPUT_READ_SUCCESS.
 */
int inputIgnoreUnixLine();

/**
 * Wczytuje kolejne znaki ze standardowego wejścia do kontenera
 * @p destionation tak długo, jak długo funkcja @p (*predicate)(następny_znak)
 * zwraca wartość różną od 0
 * oraz liczba wczytanych znaków nie przekroczyła maxLength.
 * Pierwszy znak dla którego,
 * predykat nie jest spełniony nie zostaje wczytany.
 * @param[in] predicate - predykat.
 * @param[in] maxLength - maksymalna długość.
 * @param[out] destination - miejsce docelowe.
 * @return INPUT_READ_SUCCESS, lub w przypadku problemów z pamięcią
 *         INPUT_READ_FAIL.
 */
int inputReadWhile(int (*predicate)(int),
                   size_t maxLength, Vector destination);

/**
 * Wczytuje kolejne znaki ze standardowego wejścia do kontenera
 * @p destionation tak długo, jak długo funkcja @p (*predicate)(następny_znak)
 * zwraca wartość równą 0
 * oraz liczba wczytanych znaków nie przekroczyła maxLength.
 * Pierwszy znak dla którego,
 * predykat nie jest spełniony nie zostaje wczytany.
 * @param[in] predicate - predykat.
 * @param[in] maxLength - maksymalna długość.
 * @param[out] destination - miejsce docelowe.
 * @return INPUT_READ_SUCCESS, lub w przypadku problemów z pamięcią
 *         INPUT_READ_FAIL.
 */
int inputReadUntil(int (*predicate)(int),
                   size_t maxLength, Vector destination);


/**
 * @return Niezerowa wartość jeżeli znak końca wejścia został napotkany,
 *         lub jest następnym w kolei znakiem.
 */
int inputIsEOF();

#endif //MARATONFILMOWY_INPUT_H
