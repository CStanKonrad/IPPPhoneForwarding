/** @file
 * Funkcje do parsowania wejścia.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */

#include <assert.h>
#include <stdint.h>
#include "character.h"
#include "input.h"
#include "parser.h"


struct Parser parserCreateNew() {
    struct Parser result;
    result.readBytes = 0;
    result.isError = false;
    result.isCommentEofError = false;
    return result;
}

/**
 * @param[in] characterCode - kod znaku
 * @return Niezerowa wartość jeżeli @p characterCode
 *         reprezentuje znak biały lub znak nowej lini.
 */
static int parserCharacterCanBeSkipped(int characterCode) {
    return characterIsWhite(characterCode)
           || characterIsNewLine(characterCode);
}

/**
 * @brief Pomija symbole spełniające predykat @p parserCharacterCanBeSkipped.
 * @see parserCharacterCanBeSkipped
 * @param[in, out] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 *       Po wykonaniu @p parser zostaje uaktualniony.
 * @return true jeżeli coś zostało pominięte false w przeciwnym przypadku.
 * @remarks Nie robi nic jeżeli funkcja @p parserFinished(parser) zwraca true.
 */
static bool parserSkipCharactersThatCanBeSkipped(Parser parser) {
    if (parserFinished(parser)) {
        return false;
    }

    size_t skipped = inputIgnoreWhile(parserCharacterCanBeSkipped);
    parser->readBytes += skipped;
    return skipped != 0;
}

/**
 * @brief Pomija komentarze.
 * @param[in, out] parser - wskaźnik na strukturę reprezentującą stan parsowania.
 *       Po wykonaniu @p parser zostaje uaktualniony.
 * @return true jeżeli coś zostało pominięte false w przeciwnym przypadku.
 * @remarks Nie robi nic jeżeli funkcja @p parserFinished(parser) zwraca true.
 */
static bool parserSkipComments(Parser parser) {
    if (parserFinished(parser)
        || inputPeekCharacter() != PARSER_COMMENT_SEQUENCE[0]) {
        return false;
    }

    inputGetCharacter();
    parser->readBytes++;

    if (inputPeekCharacter() != PARSER_COMMENT_SEQUENCE[1]) {
        parser->isError = true;
        return false;
    }

    inputGetCharacter();
    parser->readBytes++;


    while (true) {
        int r = inputGetCharacter();
        parser->readBytes++;
        if (parserFinished(parser)) {
            if (characterIsEOF(r)) {
                parser->readBytes--;
            }
            parser->isError = true;
            parser->isCommentEofError = true;
            return false;
        } else if (r == PARSER_COMMENT_SEQUENCE[0]
                   && inputPeekCharacter() == PARSER_COMMENT_SEQUENCE[1]) {
            inputGetCharacter();
            parser->readBytes++;

            return true;
        }
    }
}

void parserSkipSkipable(Parser parser) {
    while (parserSkipCharactersThatCanBeSkipped(parser)
           || parserSkipComments(parser));
}

bool parserError(Parser parser) {
    return parser->isError;
}

bool parserFinished(Parser parser) {
    return parserError(parser) || inputIsEOF();
}

/**
 * @param[in] characterCode - kod znaku.
 * @return true jeżeli jest operatorem składającym się z jednego
 *         symbolu,
 *         false w przeciwnym wypadku.
 */
static int parserIsSingleCharacterOperator(int characterCode) {
    return characterCode == PARSER_OPERATOR_QM
           || characterCode == PARSER_OPERATOR_REDIRECT
           || characterCode == PARSER_OPERATOR_NONTRIVIAL;
}

int parserNextType(Parser parser) {
    if (parserFinished(parser)) {
        return PARSER_FAIL;
    }

    int c = inputPeekCharacter();

    if (characterIsDigit(c)) {
        return PARSER_ELEMENT_TYPE_NUMBER;
    } else if (characterIsLetter(c)) {
        return PARSER_ELEMENT_TYPE_WORD;
    } else if (parserIsSingleCharacterOperator(c)) {
        return PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR;
    } else {
        inputGetCharacter();
        parser->readBytes++;
        parser->isError = true;

        return PARSER_FAIL;
    }
}

int parserReadOperator(Parser parser) {
    if (parserFinished(parser)) {
        return PARSER_FAIL;
    }

    int ch = inputGetCharacter();
    parser->readBytes++;
    if (parserIsSingleCharacterOperator(ch)) {
        if (ch == PARSER_OPERATOR_QM) {
            return PARSER_ELEMENT_TYPE_OPERATOR_QM;
        } else if (ch == PARSER_OPERATOR_REDIRECT) {
            return PARSER_ELEMENT_TYPE_OPERATOR_REDIRECT;
        } else if (ch == PARSER_OPERATOR_NONTRIVIAL) {
            return PARSER_ELEMENT_TYPE_OPERATOR_NONTRIVIAL;
        } else {
            parser->isError = true;
            return PARSER_FAIL;
        }
    } else {
        const char *toCmp;
        int result;
        if (ch == PARSER_OPERATOR_NEW[0]) {
            toCmp = PARSER_OPERATOR_NEW + 1;
            result = PARSER_ELEMENT_TYPE_OPERATOR_NEW;
        } else if (ch == PARSER_OPERATOR_DELETE[0]) {
            toCmp = PARSER_OPERATOR_DELETE + 1;
            result = PARSER_ELEMENT_TYPE_OPERATOR_DELETE;
        } else {
            parser->isError = true;
            return PARSER_FAIL;
        }
        size_t startPos = parser->readBytes;

        const char *ptr;
        for (ptr = toCmp; *ptr != '\0'; ptr++) {
            ch = inputGetCharacter();
            parser->readBytes++;
            if (ch != *ptr) {
                parser->isError = true;
                parser->readBytes = startPos;
                return PARSER_FAIL;
            }
        }

        if (!parserCharacterCanBeSkipped(inputPeekCharacter())
            && (inputPeekCharacter() != PARSER_COMMENT_SEQUENCE[0])
            && !(parserIsSingleCharacterOperator(inputPeekCharacter()))) {
            parser->isError = true;
            parser->readBytes = startPos;
            return PARSER_FAIL;
        }

        return result;

    }
}

/**
 * @param[in] characterCode - kod znaku
 * @return Niezerowa wartość jeżeli @p characterCode jest literą
 *         lub cyfrą.
 */
static int parserIsLetterOrDigit(int characterCode) {
    return characterIsLetter(characterCode) || characterIsDigit(characterCode);
}

bool parserReadIdentificator(Parser parser, Vector destination) {
    size_t prefSize = vectorSize(destination);
    int readStatus = inputReadWhile(parserIsLetterOrDigit, SIZE_MAX, destination);
    parser->readBytes += vectorSize(destination) - prefSize;

    return readStatus == INPUT_READ_SUCCESS;
}

bool parserReadNumber(Parser parser, Vector destination) {
    size_t prefSize = vectorSize(destination);
    int readStatus = inputReadWhile(characterIsDigit, SIZE_MAX, destination);
    parser->readBytes += vectorSize(destination) - prefSize;

    return readStatus == INPUT_READ_SUCCESS;
}

size_t parserGetReadBytes(Parser parser) {
    return parser->readBytes;
}

bool parserIsCommentEofError(Parser parser) {
    return parser->isCommentEofError;
}
