/** @file
 * Interfejs modułu reprezentującego
 * ciąg znaków.
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 06.05.2018
 */

#ifndef TELEFONY_CAHR_SEQUENCE_H
#define TELEFONY_CAHR_SEQUENCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * @brief Struktura reprezentująca ciąg znaków.
 */
struct CharSequence;


/**
 * @brief Wskaźnik na strukturę strukturę reprezentującą ciąg znaków.
 */
typedef struct CharSequence *CharSequence;

/**
 * @brief Tworzy ciąg znaków z cstringa.
 * @param[in] str - ciąg znaków w stylu c.
 * @return Wskaźnik na strukturę reprezentującą ciąg znaków,
 *         w przypadku problemów z pamięcią NULL.
 */
CharSequence charSequenceFromCString(const char *str);

/**
 * @brief Usuwa ciąg znaków.
 * @remarks node musi być wskaźnikiem na początek ciągu znaków.
 * @param node - wskaźnik na początek ciągu znaków.
 */
void charSequenceDelete(CharSequence node);

/**
 * @brief Pobiera znak.
 * @param[in] sequence - wskaźnik na pozycję w ciągu.
 * @return Znak znajdujący się na pozycji @p sequence, jeżeli
 *         @p sequence jest NULLem to zwraca '\0'.
 */
char charSequenceGetChar(CharSequence sequence);

/**
 * @brief Pobiera Znak i przestawia @p *sequence na znak następny.
 * @param[in] sequence - wskaźnik na element ciągu znaków.
 * @param[out] ch - znak znajdujący się w elemencie @p *sequence, jeżeli
 *       @p *sequence == NULL to '\0'.
 * @return Wartośc true jeżeli udało się pobrać znak, false
 *         w przeciwnym wypadku.
 */
bool charSequenceNextChar(CharSequence *sequence, char *ch);

/**
 * @brief Liczba elementów w ciągu @p sequence.
 * @param sequence - wskaźnik na ciąg znaków.
 * @return Liczba elementów w ciągu @p sequence.
 */
size_t charSequenceLength(CharSequence sequence);

/**
 * @brief Ciąg znaków w stylu C.
 * @param sequence - wskaźnik na ciąg znaków.
 * @return Ciąg znaków w stylu C.
 */
const char *charSequenceToCString(CharSequence sequence);

/**
 * @brief Sprawdza czy @p sequence odpowiada @p str.
 * @param sequence - wskaźnik na ciąg znaków.
 * @param str - wskaźnik na ciąg znaków w stylu c.
 * @return Jeżeli odpowiada to true, w przeciwnym wypadku false.
 */
bool charSequenceEqualToString(CharSequence sequence, const char *str);

/**
 * @brief Zwraca Wskaźnik reprezentujący koniec @p sequence.
 * @param sequence - wskaźnik na ciąg znaków.
 * @remarks Koniec oznacza tu wskaźnik na pozycję informującą, że ciąg się
 *         skończył.
 * @return Wskaźnik reprezentujący koniec @p sequence.
 */
CharSequence charSequenceSequenceEnd(CharSequence sequence);

/**
 * @brief Zwraca Wskaźnik reprezentujący ostatni element @p sequence.
 * @param sequence - wskaźnik na ciąg znaków.
 * @return Wskaźnik reprezentujący ostatni element @p sequence.
 */
CharSequence charSequenceLast(CharSequence sequence);

/**
 * @brief ustawia następnika elementu ciągu.
 * @param a - element ciągu.
 * @param next - następnik.
 */
void charSequenceSetNext(CharSequence a, CharSequence next);

#endif //TELEFONY_CAHR_SEQUENCE_H
