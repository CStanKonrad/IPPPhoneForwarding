/** @file
 * Interfejs systemu zarządzania bazami przekierowań.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */

#ifndef TELEFONY_PHONE_BASES_SYSTEM_H
#define TELEFONY_PHONE_BASES_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>

#include "phone_forward.h"


/**
 * @see struct PhoneBases
 */
typedef struct PhoneBases *PhoneBases;

/**
 * @brief Struktura przechowująca bazy przekierowań numerów telefonów.
 */
struct PhoneBases;

/**
 * @brief Tworzy nową strukturę przechowującą bazy przekierowań.
 * @return Wskaźnik na zainicjowaną strukturę, NULL
 *         w przypadku problemów z przydzieleniem pamięci.
 * @remarks Wynikowa struktura musi zostać usunięta przy pomocy
 *          @p phoneBasesCreateDestroyPhoneBases.
 */
PhoneBases phoneBasesCreateNewPhoneBases();

/**
 * @brief Usuwa strukturę przechowującą bazy przekierowań.
 * @param[in] pb - wskaźnik na strukturę.
 */
void phoneBasesDestroyPhoneBases(PhoneBases pb);

/**
 * @param[in] pb - wskaźnik na strukturę przechowującą bazy przekierowań.
 * @return Liczba przechowywanych baz.
 */
size_t phoneBasesHowManyBases(PhoneBases pb);

/**
 *
 * @param[in] pb - wskaźnik na strukturę przechowującą bazy przekierowań.
 * @param[in] id - identyfikator bazy.
 * @return Wskaźnik na bazę o identyfikatorze id, NULL w przypadku
 *         braku takiej bazy.
 */
struct PhoneForward *phoneBasesGetBase(PhoneBases pb, const char *id);

/**
 * @brief Dodaje bazę i zwraca wskaźnik do niej.
 * @param[in] pb - wskaźnik na strukturę przechowującą bazy przekierowań.
 * @param[in,out] id - identyfikator bazy.
 * @return Wskaźnik do dodanej bazy, w przypadku problemów z pamięcią
 *         NULL.
 */
struct PhoneForward *phoneBasesAddBase(PhoneBases pb, const char *id);

/**
 * @brief Usuwa bazę przekierowań.
 * @param[in] pb - wskaźnik na strukturę przechowującą bazy przekierowań.
 * @param[in] id - identyfikator bazy.
 * @return true jeżeli podana baza istniała, w przeciwnym przypadku false.
 */
bool phoneBasesDelBase(PhoneBases pb, const char *id);


#endif //TELEFONY_PHONE_BASES_SYSTEM_H
