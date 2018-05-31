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
 * @see struct PhoneBaseInfo
 */
typedef struct PhoneBaseInfo PhoneBaseInfo;


/**
 * @see struct PhoneBases
 */
typedef struct PhoneBases *PhoneBases;

/**
 * @see struct PhoneBasesNode
 */
typedef struct PhoneBasesNode *PhoneBasesNode;

/**
 * @brief Informacje o bazie przekierowań.
 */
struct PhoneBaseInfo {
    /**
     * @brief Wartość funkcji skrótu dla @p id.
     */
    size_t hash;

    /**
     * @brief Identyfikator bazy.
     */
    const char *id;

    /**
     * @brief Wskaźnik na bazę przekierowań.
     */
    struct PhoneForward *base;
};


/**
 *  @brief Węzeł listy baz przekierowań.
 *  @see PhoneBases.
 */
struct PhoneBasesNode {
    /**
     * @brief Informacje o przechowywanej bazie.
     */
    PhoneBaseInfo baseInfo;

    /**
     * @brief Następny węzeł.
     * Jeżeli nie ma następnego węzła to @p next
     * ma wartość NULL.
     */
    PhoneBasesNode next;
};


/**
 * @brief Struktura przechowująca bazy przekierowań numerów telefonów.
 */
struct PhoneBases {
    /**
     * @brief Pierwszy węzeł listy.
     * NULL w przypadku przechowywania zera baz.
     */
    PhoneBasesNode basesList;

    /**
     * @brief Liczba przechowywanych baz.
     */
    size_t numberOfBases;
};

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
void phoneBasesCreateDestroyPhoneBases(PhoneBases pb);

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
