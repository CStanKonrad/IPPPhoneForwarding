/** @file
 * Struktura tablicy dynamicznie dopasowującej
 * swój rozmiar w miarę potrzeb (Vectora)
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.04.2018
 */

#ifndef TELEFONY_VECTOR_H
#define TELEFONY_VECTOR_H

#include <stddef.h>

/**
 * @brief Typ elementów przechowywanych w Vectorze.
 */
#define VECTOR_ELEMENT_TYPE int

/**
 * @brief Kod zwracany przez operację zakończoną sukcesem.
 */
#define VECTOR_SUCCES 1

/**
 * @brief Kod zwracany przez operację zakończoną błedem allokacji.
 */
#define VECTOR_MEMORY_ERROR 0

/**
 * @brief Kod zwracany przez operację zakończoną innym błedem.
 */
#define VECTOR_OPERATION_ERROR 2

/**
 * @brief Wskaźnik na strukturę Vectora.
 * @see struct Vector.
 */
typedef struct Vector *Vector;

/**
 * @brief Struktura reprezentująca tablice o dynamicznym rozmiarze.
 */
struct Vector {
    /**
     * @brief Aktualny rozmiar Vectora.
     * Liczba używanych komórek tablicy array.
     */
    size_t size;

    /**
     * @brief Zadeklarowany rozmiar tablicy array.
     * Liczba komórek tablicy array.
     * @see array
     */
    size_t allocatedSize;

    /**
     * @brief Tablica przechowująca wstawiane elementy.
     */
    VECTOR_ELEMENT_TYPE *array;
};

/**
 * @brief Tworzy nowy Vector.
 * ####Złożoność
 * O(1)
 * @return Wskaźnik na nowo stoworzony i zainicjowany egzemplarz
 * Vectora. W przypadku problemów z przydzieleniem pamięci NULL.
 */
Vector vectorCreate();

/**
 * @brief Podaje liczbę przechowywanych elementów.
 * ####Złożoność
 * O(1)
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @return Liczba przechowywanych elementów w @p vector.
 */
size_t vectorSize(Vector vector);

/**
 * @brief Zwraca liczbę elementów na które jest zarezerwowane miejsce.
 * ####Złożoność
 * O(1)
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @return Podaje zarezerwowany rozmiar Vectora @p vector
 * (możliwą do przechowywania liczbę
 * elementów bez wykonania doatkowej allokacji).
 */
size_t vectorReservedSize(Vector vector);

/**
 * @brief Rezerwuje pamięć.
 * Jeżeli ilość pamięci zarezerwowanej na elementy Vectora @p vector
 * jest mniejsza niż @p toReserve to rezerwuje nowy spójny blok pamięci na
 * >= @p toReserve elementów i przepisuje do niego przechowywane elementy.
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @param[in] toReserve - rozmiar do zarezerwowania.
 * @return W przypadku problemów z przydzieleniem pamięci VECTOR_MEMORY_ERROR,
 *         w przeciwnym wypadku VECTOR_SUCCESS.
 */
int vectorReserve(Vector vector, size_t toReserve);

/**
 * @brief Zmienia rozmiar unikając allokacji.
 * Zmienia rozmiar Vectora @p vector na @p size.
 * Tylko w przypadku niedoboru zarezerwowanej pamięci
 * rezerwuje nowy spójny blok, przepisuje do niego elementy Vectora @p vector
 * i usuwa stary blok.
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @param[in] size      - rozmiar do ustawienia.
 * @return W przypadku problemów z przydzieleniem pamięci
 *         VECTOR_MEMORY_ERROR.
 *         W przeciwnym przypadku VECTOR_SUCCESS.
 */
int vectorSoftResize(Vector vector, size_t size);

/**
 * @brief Opróżnia Vector @p vector.
 * Usuwa wszystkie elementy z @p vector.
 * Zwalania całą pamięć zarezerwowaną na nowe elementy.
 * ####Złożoność
 * O(1)
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 */
void vectorClear(Vector vector);

/**
 * @brief Odpowiednik vectorSoftResize(vector, 0).
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @see vectorSoftResize
 */
void vectorSoftClear(Vector vector);

/**
 * @brief Usuwa Vector.
 * Zwalnia pamięć zarezerwowaną przez @p vector,
 * wraz z pamięcią zarezerwowaną na strukturę struct Vector.
 * ####Złożoność
 * O(1)
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 */
void vectorDelete(Vector vector);

/**
 * @brief Wstawia element na koniec Vectora.
 * Wstawia element na koniec Vectora @p vector,
 * w przypadku niedoboru zarezerwowanej pamięci
 * wywołuje vectorReserve(vector, vectorSize(vector) + 1).
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @param[in] element   - element do wstawienia..
 * @return W przypadku problemów z przydzieleniem pamięci VECTOR_MEMORY_ERROR,
 *         w przeciwnym wypadku VECTOR_SUCCESS.
 */
int vectorPushBack(Vector vector, VECTOR_ELEMENT_TYPE element);

/**
 * @brief Usuwa z końca Vectora.
 * Jeżeli vectorSize(vector) jest różne od 0 to
 * usuwa ostatni element z Vectora @p vector, zmniejsza jego rozmiar
 * o 1. W przypadku znaczącego spadnięcia rozmiaru Vectora względem
 * rozmiaru zarezerwowanego kopiuje Vector do nowego mniejszego spójnego bloku
 * pamięci i usuwa stary blok.
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @return Jeżeli vectorSize(vector) jest równe 0 to VECTOR_OPERATION_ERROR.
 *         W przypadku problemów z przydzieleniem pamięci VECTOR_MEMORY_ERROR.
 *         W przeciwnym wypadku VECTOR_SUCCESS.*/
int vectorPopBack(Vector vector);

/**
 * @brief Wskaźnik na początek tablicy elementów.
 * ####Złożoność
 * O(1)
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @return Wskaźnik na początek tablicy przechowującej elementy Vectora
 *         @p vector - spójny blok
 *         pamięci.
 */
VECTOR_ELEMENT_TYPE *vectorBegin(Vector vector);

/**
 * @brief Wskaźnik na zaostatni element tablicy elementów.
 * ####Złożoność
 * O(1)
 * @param[in] vector    - wskaźnik na strukturę Vectora.
 * @return Zwraca wskaźnik na zaostatni element tablicy przechowującej
 *         elementy Vectora @p vector.
 */
VECTOR_ELEMENT_TYPE *vectorEnd(Vector vector);

/**
 * @brief Zamienia dwa Vectory.
 * W efekcie wektor a posiada elementy b.
 * Natomiast b elementy które uprzednio posiadało a.
 * ####Złożoność
 * O(1)
 * @param[in] a    - wskaźnik na strukturę Vectora.
 * @param[in] b    - wskaźnik na strukturę Vectora.
 */
void vectorSwap(Vector a, Vector b);

#endif //TELEFONY_VECTOR_H
