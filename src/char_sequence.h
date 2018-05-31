/** @file
 * Interfejs modułu reprezentującego
 * ciąg znaków.
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 13.05.2018
 */

#ifndef TELEFONY_CAHR_SEQUENCE_H
#define TELEFONY_CAHR_SEQUENCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * @see struct CharSequenceIterator
 */
typedef struct CharSequenceIterator CharSequenceIterator;

/**
 * @brief Wskaźnik na strukturę strukturę reprezentującą ciąg znaków.
 */
typedef struct CharSequence *CharSequence;

/**
 * @brief Struktura reprezentująca ciąg znaków.
 */
struct CharSequence;

/**
 * @brief Struktura reprezentująca iterator ciągu znaków.
 */
struct CharSequenceIterator {
    /**
     * @brief Wskaźnik na element ciągu znaków (blok).
     */
    CharSequence sequenceBlockPtr;

    /**
     * @brief Index litery z bloku.
     */
    size_t charId;

    /**
     * @brief Czy przejrzano wszystko.
     */
    bool isEnd;
};

/**
 * @brief Tworzy iterator dla ciągu @p sequence.
 * @param[in] sequence - wskaźnik na ciąg dla którego tworzony jest iterator.
 * @return Iterator dla @p sequence.
 */
CharSequenceIterator charSequenceGetIterator(CharSequence sequence);

/**
 * @brief Sprawdza czy dwa iteratory reprezentują to samo miejsce.
 * @param[in] a - iterator
 * @param[in] b - iterator
 * @return Jeżeli reprezentują to samo miejsce w tym samym ciągu to true,
 *         w przeciwnym wypadku false.
 */
bool charSequenceIteratorsEqual(CharSequenceIterator *a,
                                CharSequenceIterator *b);

/**
 * @brief Łączy ciągi.
 * Poszerza ciąg @p a o ciąg @p b, wskaźnik @p b
 * ciąg @p b przestaje istnieć.
 * @param[in, out] a - wskaźnik na ciąg, aktualny po operacji.
 * @param[in, out] b - wskaźnik na ciąg, nieaktualny po operacji.
 */
void charSequenceMerge(CharSequence a, CharSequence b);

/**
 * @brief Tnie Ciąg @p sequence w punkcie @p it.
 * @param[in, out] sequence - wskaźnik na cięty ciąg, po operacji
 *       odpowiada ciągowi [sequence; it).
 * @param[in, out] it - wskaźnik na punkt rozcięcia.
 * @return Wskaźnik na ciąg [it; ..],
 *         w przypadku problemów z pamięcią NULL.
 */
CharSequence charSequenceSplitByIterator(CharSequence sequence,
                                         CharSequenceIterator *it);

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
 * @param[in] node - wskaźnik na początek ciągu znaków.
 */
void charSequenceDelete(CharSequence node);

/**
 * @brief Pobiera znak.
 * @param[in] it - wskaźnik na pozycję w ciągu.
 * @return Znak znajdujący się na pozycji @p it, jeżeli
 *         @p sequence doszedł do końca to zwraca '\0'.
 */
char charSequenceGetChar(CharSequenceIterator *it);

/**
 * @brief Pobiera Znak i przestawia @p *it na znak następny.
 * @param[in] it - wskaźnik na element ciągu znaków.
 * @param[out] ch - znak znajdujący się w elemencie @p *it, jeżeli
 *       brak znaków to '\0'.
 * @return Wartość true jeżeli udało się pobrać znak, false
 *         w przeciwnym wypadku.
 */
bool charSequenceNextChar(CharSequenceIterator *it, char *ch);

/**
 * @brief Liczba elementów w ciągu @p sequence.
 * @param sequence - wskaźnik na ciąg znaków.
 * @return Liczba elementów w ciągu @p sequence.
 */
size_t charSequenceLength(CharSequence sequence);


/**
 * @brief Liczba elementów w ciągu @p sequence.
 * @param[in] sequence - wskaźnik na ciąg znaków.
 * @param[in] limit - limit liczenia długości.
 * @param[out] greater - true jeżeli ciąg jest dłuższy niż limit
 *       false w przeciwnym przypadku.
 * @return min(Liczba elementów w ciągu @p sequence, limit)
 */
size_t charSequenceLengthLimited(CharSequence sequence, size_t limit,
                                 bool *greater);

/**
 * @brief Ciąg znaków w stylu C.
 * @remarks Wynik należy zwolnić przy pomocy free.
 * @param[in] sequence - wskaźnik na ciąg znaków.
 * @return Ciąg znaków w stylu C, NULL w przypadku
 *         problemów z pamięcią.
 */
const char *charSequenceToCString(CharSequence sequence);

/**
 * @brief Sprawdza czy @p sequence odpowiada @p str.
 * @param[in] sequence - wskaźnik na ciąg znaków.
 * @param[in] str - wskaźnik na ciąg znaków w stylu c.
 * @return Jeżeli odpowiada to true, w przeciwnym wypadku false.
 */
bool charSequenceEqualToString(CharSequence sequence, const char *str);

/**
 * @brief Zwraca iterator reprezentujący koniec @p sequence.
 * @param[in] sequence - wskaźnik na ciąg znaków.
 * @return Wskaźnik reprezentujący koniec @p sequence.
 */
CharSequenceIterator charSequenceSequenceEnd(CharSequence sequence);


#endif //TELEFONY_CAHR_SEQUENCE_H
