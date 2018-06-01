/** @file
 * Funkcje do parsowania wejścia.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */

#ifndef TELEFONY_PARSER_H
#define TELEFONY_PARSER_H

#include <stdbool.h>
#include <stddef.h>

#include "vector.h"
#include "stdfunc.h"

/**
 * @brief Ciąg znaków rozpoczynający i kończący komentarz.
 */
#define PARSER_COMMENT_SEQUENCE "$$"

/**
 * @brief Ciąg znaków reprezentujący operator ?.
 */
#define PARSER_OPERATOR_QM_STRING "?"

/**
 * @brief Znak odpowiadający operatorowi ?.
 */
#define PARSER_OPERATOR_QM (STRING_TO_CHAR(PARSER_OPERATOR_QM_STRING))

/**
 * @brief Ciąg znaków reprezentujący operator przekierowania.
 */
#define PARSER_OPERATOR_REDIRECT_STRING ">"

/**
 * @brief Znak odpowiadający operatorowi przekierowania.
 */
#define PARSER_OPERATOR_REDIRECT (STRING_TO_CHAR(PARSER_OPERATOR_REDIRECT_STRING))

/**
 * @brief Ciąg znaków reprezentujący operator liczby nietrywialnych numerów.
 */
#define PARSER_OPERATOR_NONTRIVIAL_STRING "@"

/**
 * @brief Znak reprezentujący operator liczby nietrywialnych numerów.
 */
#define PARSER_OPERATOR_NONTRIVIAL (STRING_TO_CHAR(PARSER_OPERATOR_NONTRIVIAL_STRING))

/**
 * @brief Ciąg znaków odpowiadający operatorowi stworzenia nowej bazy.
 */
#define PARSER_OPERATOR_NEW "NEW"

/**
 * @brief Ciąg znaków odpowiadający operatorowi usunięcia bazy.
 */
#define PARSER_OPERATOR_DELETE "DEL"


/**
 * @see parserNextType
 * @see parserReadOperator
 */
#define PARSER_FAIL 0

/**
 * @see parserNextType
 */
#define PARSER_ELEMENT_TYPE_NUMBER 1

/**
 * @brief parserNextType
 */
#define PARSER_ELEMENT_TYPE_WORD 2

/**
 * @brief parserNextType
 */
#define PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR 3


/**
 * @see parserReadOperator
 */
#define PARSER_ELEMENT_TYPE_OPERATOR_QM 4

/**
 * @see parserReadOperator
 */
#define PARSER_ELEMENT_TYPE_OPERATOR_REDIRECT 5

/**
 * @see parserReadOperator
 */
#define PARSER_ELEMENT_TYPE_OPERATOR_NEW 6

/**
 * @see parserReadOperator
 */
#define PARSER_ELEMENT_TYPE_OPERATOR_DELETE 7

/**
 * @see parserReadOperator
 */
#define PARSER_ELEMENT_TYPE_OPERATOR_NONTRIVIAL 8



/**
 * @see struct Parser
 */
typedef struct Parser *Parser;

/**
 * @brief Struktura reprezentująca stan parsowania wejścia.
 */
struct Parser {
    /**
     * @brief Liczba wczytanych bajtów.
     */
    size_t readBytes;

    /**
     * @brief Czy wystąpił błąd.
     */
    bool isError;

    /**
     * @brief Czy nie znaleziono końca komentarza.
     */
    bool isCommentEofError;
};

/**
 * @brief Tworzy nową strukturę reprezentującą stan parsowania.
 * @return Zainicjowana struktura reprezentująca stan parsowania.
 */
struct Parser parserCreateNew();

/**
 * @brief Pomija komentarze i białe znaki.
 * @param[in, out] parser - uaktualnia stan parsowania.
 */
void parserSkipSkipable(Parser parser);

/**
 * @param[in] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 * @return true jeżeli @p parser zgłasza wystąpienie błędu,
 *         w przeciwnym przypadku zwraca false.
 */
bool parserError(Parser parser);

/**
 * @param[in] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 * @return true jeżeli wejście zostało przetworzone (napotkano EOF) lub
 *         parser zgłasza błąd,
 *         w przeciwnym przypadku false.
 */
bool parserFinished(Parser parser);

/**
 * @brief Podaje typ następnego słowa na wejściu.
 * @param[in, out] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 *       Po wykonaniu @p parser zostaje uaktualniony.
 * @return Przewidywany typ następnego słowa, wydedukowany
 *         na podstawie następnego znaku z wejścia. Możliwe wyniki:
 *         PARSER_ELEMENT_TYPE_NUMBER (numer),
 *         PARSER_ELEMENT_TYPE_WORD (identyfikator lub PARSER_OPERATOR_NEW
 *         lub PARSER_OPERATOR_DELETE),
 *         PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR
 *         (PARSER_OPERATOR_QM lub PARSER_OPERATOR_REDIRECT,
 *         lub PARSER_OPERATOR_NONTRIVIAL),
 *         PARSER_FAIL (coś innego).
 * @remarks W przypadku PARSER_FAIL następny znak z wejścia zostaje wczytany
 *          (ustawiona zostaje także flaga dotycząca błędu w @p parser),
 *          w przeciwnym przypadku nie.
 */
int parserNextType(Parser parser);

/**
 * @brief Wczytuje operator.
 * @param[in, out] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 *       Po wykonaniu @p parser zostaje uaktualniony.
 * @return Typ operatora:
 *         PARSER_ELEMENT_TYPE_OPERATOR_QM (PARSER_OPERATOR_QM),
 *         PARSER_ELEMENT_TYPE_OPERATOR_REDIRECT (PARSER_OPERATOR_REDIRECT),
 *         PARSER_ELEMENT_TYPE_OPERATOR_NEW (PARSER_OPERATOR_NEW),
 *         PARSER_ELEMENT_TYPE_OPERATOR_DELETE (PARSER_OPERATOR_DELETE)
 *         PARSER_ELEMENT_TYPE_OPERATOR_NONTRIVIAL (PARSER_OPERATOR_NONTRIVIAL)
 *         PARSER_FAIL (Nieznany operator
 *         lub @p parserFinished(parser) zwraca true).
 */
int parserReadOperator(Parser parser);

/**
 * @brief Wczytuje identyfikator.
 * @param[in, out] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 *       Po wykonaniu @p parser zostaje uaktualniony.
 * @param[out] destination - do podanego wektora zapisuje identyfikator.
 * @return false w przypadku problemów z pamięcią,
 *         true w przeciwnym wypadku.
 */
bool parserReadIdentificator(Parser parser, Vector destination);

/**
 * @brief Wczytuje numer.
 * @param[in, out] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 *       Po wykonaniu @p parser zostaje uaktualniony.
 * @param[out] destination - do podanego wektora zapisuje numer.
 * @return false w przypadku problemów z pamięcią,
 *         true w przeciwnym wypadku.
 */
bool parserReadNumber(Parser parser, Vector destination);

/**
 * @brief Zwraca liczbę wczytanych bajtów.
 * @param[in] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 * @return Liczba wczytanych bajtów.
 */
size_t parserGetReadBytes(Parser parser);
/**
 * @brief Sprawdza czy podjęto próbę wczytania niezamkniętego komentarza.
 * @param[in] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 * @return true jeżeli podjęto taką próbę, false w przeciwnym przypadku.
 */
bool parserIsCommentEofError(Parser parser);

#endif //TELEFONY_PARSER_H
