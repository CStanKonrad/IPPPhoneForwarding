/** @file
 * Główna pętla programu.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "phone_bases_system.h"
#include "vector.h"
#include "input.h"
#include "character.h"
#include "stdfunc.h"

/**
 * @brief Bazowy prefiks informacji o błędzie.
 */
#define BASIC_ERROR_MESSAGE "ERROR"

/**
 * @brief Domyślny infiks informacji o błędzie.
 */
#define BASIC_ERROR_INFIX " "

/**
 * @brief Sufiks informacji o błędzie end of file.
 */
#define EOF_ERROR_SUFFIX " EOF"

/**
 * @brief Infiks informacji o błędzie związanym z pamięcią.
 */
#define MEMORY_ERROR_INFIX " not enough memory "

/**
 * @brief Infiks informacji o błędzie operatora DEL.
 */
#define DEL_OPERATOR_ERROR_INFIX \
    (CONCAT(" ", PARSER_OPERATOR_DELETE, " "))

/**
 * @brief Infiks informacji o błędzie operatora ?.
 */
#define QM_OPERATOR_ERROR_INFIX \
    (CONCAT(" ", PARSER_OPERATOR_QM_STRING, " "))

/**
 * @brief Infiks informacji o błędzie operatora >.
 */
#define REDIRECT_OPERATOR_ERROR_SUFFIX \
    (CONCAT(" ", PARSER_OPERATOR_REDIRECT_STRING, " "))


/**
 * @brief Kod błędu zwracany przez program.
 */
#define ERROR_EXIT_CODE 1

/**
 * @brief Kod zwracany przez program w przypadku braku błędów.
 */
#define SUCCESS_EXIT_CODE 0

/**
 * Przesunięcie pozycji błędu operatora PARSER_OPERATOR_[NEW/DELETE]
 * przy użyciu nazwy zastrzeżonej (PARSER_OPERATOR_[NEW/DELETE]).
 */
#define OPERATOR_POSITION_OFFSET 2

/**
 * @brief Wskaźnik na strukturę przechowującą bazy przekierowań.
 */
static PhoneBases bases = NULL;

/**
 * @brief Wskaźniki na pomocniczy wektor do buforowania wejścia.
 */
static Vector word1 = NULL;

/**
 * @brief Wskaźniki na pomocniczy wektor do buforowania wejścia.
 */
static Vector word2 = NULL;

/**
 * @brief Wskaźnik na aktualnie aktywną bazę przekierowań.
 * NULL w przypadku braku.
 */
static struct PhoneForward *currentBase = NULL;

/**
 * @brief Struktura opisująca stan parsowania.
 */
static struct Parser parser;

/**
 * @brief Kończy program.
 * Zwalnia pamięć i kończy program kodem @p exit_code.
 * @param[in] exit_code - kod zakończenia programu.
 */
static void exit_and_clean(int exit_code) {

    if (bases != NULL) {
        phoneBasesCreateDestroyPhoneBases(bases);
    }

    if (word1 != NULL) {
        vectorDelete(word1);
    }

    if (word2 != NULL) {
        vectorDelete(word2);
    }


    exit(exit_code);
}

/**
 * @brief Wypisuje informację o błędzie.
 * @param[in] infix - infiks informacji
 * @param[in] bytes - liczba wczytanych bajtów.
 */
static void printErrorMessage(const char *infix, size_t bytes) {
    fprintf(stderr, "%s%s%zu", BASIC_ERROR_MESSAGE, infix,
            bytes);
}

/**
 * @brief Wypisuje informacje o błędzie związanym z nieoczekiwanym końcem pliku.
 */
static void printEofError() {
    fprintf(stderr, "%s%s", BASIC_ERROR_MESSAGE, EOF_ERROR_SUFFIX);
}

/**
 * @brief Inicjuje program.
 * Inicjuje struktury:
 * @ref parser
 * @ref bases
 * @ref word1
 * @ref word2
 * w przypadku problemów z pamięcią kończy program
 * i wypisuje informacje o błędzie.
 */
static void initProgram() {
    parser = parserCreateNew();
    bases = phoneBasesCreateNewPhoneBases();
    if (bases == NULL) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    word1 = vectorCreate();

    if (word1 == NULL) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    word2 = vectorCreate();

    if (word2 == NULL) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

/**
 * @brief Dodaje do Vectora '\0' na koniec.
 * W przypadku problemów z pamięcią kończy program
 * i wypisuje informacje o błędzie.
 * @param[in] v - dany Vector.
 */
static void makeVectorCStringCompatible(Vector v) {
    if (!vectorPushBack(v, '\0')) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

/**
 * @brief Czyści Vectory @p word1 @p word2.
 */
static void loopStepClear() {
    vectorSoftClear(word1);
    vectorSoftClear(word2);
}

/**
 * @brief Sprawdza czy wystąpił błąd parsowania.
 * Jeżeli wystąpił to wypisuje odpowiednią informację
 * i kończy program.
 */
static void checkParserError() {
    if (parserIsCommentEofError(&parser)) {
        printEofError();
        exit_and_clean(ERROR_EXIT_CODE);
    }
    if (parserError(&parser)) {
        printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
}


/**
 * @brief Sprawdza czy wystąpił błąd parsowania typu EOF.
 * Jeżeli wystąpił to wypisuje odpowiednią informację
 * i kończy program.
 */
static void checkEofError() {
    if (inputIsEOF()) {
        printEofError();
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

/**
 * @brief Sprawdza czy parsowanie się zakończyło.
 * Jeżeli tak to kończy program.
 */
static void checkParserFinished() {
    if (parserFinished(&parser)) {
        exit_and_clean(SUCCESS_EXIT_CODE);
    }
}

/**
 * @brief Pomija zbędne znaki.
 * Wywołuje @ref parserSkipSkipable,
 * oraz @ref checkParserError.
 */
static void skipSkipable() {
    parserSkipSkipable(&parser);
    checkParserError();
}

/**
 * @brief Obsługuje operację dodania nowej bazy.
 * Zakłada, że poprzednio wczytaną operacją jest PARSER_OPERATOR_NEW.
 * W przypadku problemów wypisuje odpowiedni komunikat
 * i kończy program.
 */
static void readOperationNew() {
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();


    if (nextType != PARSER_ELEMENT_TYPE_WORD) {
        printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (!parserReadIdentificator(&parser, word1)) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
    checkParserError();

    makeVectorCStringCompatible(word1);

    if (strcmp(vectorBegin(word1), PARSER_OPERATOR_DELETE) == 0
        || strcmp(vectorBegin(word1), PARSER_OPERATOR_NEW) == 0) {
        printErrorMessage(BASIC_ERROR_INFIX,
                          parserGetReadBytes(&parser) - OPERATOR_POSITION_OFFSET);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (vectorSize(word1) <= 1) {
        printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    currentBase = phoneBasesAddBase(bases, vectorBegin(word1));

    if (currentBase == NULL) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

}

/**
 * @brief Obsługuje operację phfwdRemove(numer).
 * Oczekuje, że poprzednio wczytano operator PARSER_OPERATOR_DELETE
 * oraz że na wczytanie według @p parserNextType oczekuje numer.
 * @param[in] operatorPos - pozycja operatora usunięcia.
 * W przypadku problemów wypisuje odpowiedni komunikat
 * i kończy program.
 */
static void readOperationDeleteNumber(size_t operatorPos) {
    if (!parserReadNumber(&parser, word1)) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
    checkParserError();

    if (currentBase == NULL) {
        printErrorMessage(DEL_OPERATOR_ERROR_INFIX, operatorPos);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    makeVectorCStringCompatible(word1);
    phfwdRemove(currentBase, vectorBegin(word1));
}

/**
 * @brief Obsługuje operację usunięcia bazy przekierowań.
 * Oczekuje, że poprzednio wczytano operator PARSER_OPERATOR_DELETE
 * oraz że na wczytanie według @p parserNextType oczekuje identyfikator.
 * @param[in] operatorPos - pozycja (nr bajtu) operatora usunięcia.
 * W przypadku problemów wypisuje odpowiedni komunikat
 * i kończy program.
 */
static void readOperationDeleteBase(size_t operatorPos) {
    if (!parserReadIdentificator(&parser, word1)) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
    checkParserError();

    makeVectorCStringCompatible(word1);

    if (strcmp(vectorBegin(word1), PARSER_OPERATOR_DELETE) == 0
        || strcmp(vectorBegin(word1), PARSER_OPERATOR_NEW) == 0) {
        printErrorMessage(BASIC_ERROR_INFIX,
                          parserGetReadBytes(&parser) - OPERATOR_POSITION_OFFSET);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    struct PhoneForward *toDel = phoneBasesGetBase(bases, vectorBegin(word1));

    if (toDel == NULL) {
        printErrorMessage(DEL_OPERATOR_ERROR_INFIX, operatorPos);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (toDel == currentBase) {
        currentBase = NULL;
    }

    phoneBasesDelBase(bases, vectorBegin(word1));

}

/**
 * @brief Obsługuje operację zadaną przez operator PARSER_OPERATOR_DELETE.
 * Oczekuje, że poprzednio wczytano operator PARSER_OPERATOR_DELETE.
 */
static void readOperationDelete() {
    size_t operatorPos =
            parserGetReadBytes(&parser) - strlen(PARSER_OPERATOR_DELETE) + 1;
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();

    if (nextType == PARSER_ELEMENT_TYPE_NUMBER) {
        readOperationDeleteNumber(operatorPos);
    } else if (nextType == PARSER_ELEMENT_TYPE_WORD) {
        readOperationDeleteBase(operatorPos);
    } else {
        printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }

}

/**
 * @brief Wypisuje numery.
 * @param[in] numbers - struktura przechowująca numery do wypisania.
 */
static void printNumbers(const struct PhoneNumbers *numbers) {
    size_t i;
    for (i = 0; phnumGet(numbers, i) != NULL; i++) {
        fprintf(stdout, "%s\n", phnumGet(numbers, i));
    }
}

/**
 * @brief Obsługuje operację phwfdReverse.
 * Oczekuje, że poprzednio wczytano PARSER_OPERATOR_QM.
 */
static void readOperationReverse() {
    size_t operatorPos = parserGetReadBytes(&parser);
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();

    if (nextType == PARSER_ELEMENT_TYPE_NUMBER) {
        if (!parserReadNumber(&parser, word1)) {
            printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }
        checkParserError();


        if (currentBase == NULL) {
            printErrorMessage(QM_OPERATOR_ERROR_INFIX, operatorPos);
            exit_and_clean(ERROR_EXIT_CODE);
        }
        makeVectorCStringCompatible(word1);
        const struct PhoneNumbers *numbers
                = phfwdReverse(currentBase, vectorBegin(word1));

        if (numbers == NULL) {
            printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }

        printNumbers(numbers);

        phnumDelete(numbers);


    } else {
        printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }

}

/**
 * @brief Obsługuje operację phfwdNonTrivialCount.
 * Oczekuje, że poprzednio wczytano PARSER_OPERATOR_NONTRIVIAL.
 */
static void readOperationNonTrivial() {
    //size_t operatorPos = parserGetReadBytes(&parser);
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();

    if (nextType == PARSER_ELEMENT_TYPE_NUMBER) {
        if (!parserReadNumber(&parser, word1)) {
            printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }
        checkParserError();


        /*if (currentBase == NULL) {
            printErrorMessage(QM_OPERATOR_ERROR_INFIX, operatorPos);
            exit_and_clean(ERROR_EXIT_CODE);
        }*/

        size_t len = vectorSize(word1);
        if (len <= 12) {
            len = 0;
        } else {
            len -= 12;
        }
        makeVectorCStringCompatible(word1);
        size_t result = phfwdNonTrivialCount(currentBase, vectorBegin(word1), len);

        fprintf(stdout, "%zu\n", result);


    } else {
        printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }
}


/**
 * @brief Obsługuje operację phfwdGet(word1).
 */
static void readOperatorGetFromWord1() {
    makeVectorCStringCompatible(word1);

    if (currentBase == NULL) {
        printErrorMessage(QM_OPERATOR_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    const struct PhoneNumbers *numbers = phfwdGet(currentBase, vectorBegin(word1));

    if (numbers == NULL) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    printNumbers(numbers);

    phnumDelete(numbers);

}

/**
 * @brief Obsługuje operację przekierowania numerów word1 > word2.
 * Oczekuje wczytania pierwszego numeru do word1
 * i wczytania operatora przekierowania.
 */
static void readOperatorRedirectWord1() {
    size_t operatorPos = parserGetReadBytes(&parser);
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();

    if (nextType != PARSER_ELEMENT_TYPE_NUMBER) {
        printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (!parserReadNumber(&parser, word2)) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
    checkParserError();

    if (currentBase == NULL) {
        printErrorMessage(REDIRECT_OPERATOR_ERROR_SUFFIX, operatorPos);
        exit_and_clean(ERROR_EXIT_CODE);
    }
    makeVectorCStringCompatible(word1);
    makeVectorCStringCompatible(word2);

    if (strcmp(vectorBegin(word1), vectorBegin(word2)) == 0) {
        printErrorMessage(REDIRECT_OPERATOR_ERROR_SUFFIX, operatorPos);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (!phfwdAdd(currentBase, vectorBegin(word1), vectorBegin(word2))) {
        printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

/**
 * @brief Wczytuje operację / jej fragment i obsługuje ją.
 * @param[in] nextType - oczekiwany typ wczytanych danych,
 *       pochodzący z wywołania @ref parserNextType.
 */
static void readOperation(int nextType) {
    if (nextType == PARSER_ELEMENT_TYPE_WORD) {
        int operator = parserReadOperator(&parser);
        checkParserError();
        checkEofError();

        if (operator == PARSER_ELEMENT_TYPE_OPERATOR_NEW) {
            readOperationNew();
        } else if (operator == PARSER_ELEMENT_TYPE_OPERATOR_DELETE) {
            readOperationDelete();
        } else {
            printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }

    } else if (nextType == PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR) {
        int operator = parserReadOperator(&parser);
        checkParserError();
        checkEofError();

        if (operator == PARSER_ELEMENT_TYPE_OPERATOR_QM) {
            readOperationReverse();
        } else if (operator == PARSER_ELEMENT_TYPE_OPERATOR_NONTRIVIAL) {
            readOperationNonTrivial();
        } else {
            printErrorMessage(BASIC_ERROR_INFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }

    } else if (nextType == PARSER_ELEMENT_TYPE_NUMBER) {
        if (!parserReadNumber(&parser, word1)) {
            printErrorMessage(MEMORY_ERROR_INFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }
        checkParserError();

        skipSkipable();
        checkEofError();

        int midType = parserNextType(&parser);
        checkParserError();

        if (midType == PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR) {
            int operator = parserReadOperator(&parser);
            checkParserError();

            if (operator == PARSER_ELEMENT_TYPE_OPERATOR_QM) {
                readOperatorGetFromWord1();
            } else if (operator == PARSER_ELEMENT_TYPE_OPERATOR_REDIRECT) {
                checkEofError();
                readOperatorRedirectWord1();
            } else {
                printErrorMessage(BASIC_ERROR_INFIX,
                                  parserGetReadBytes(&parser) + 1);
                exit_and_clean(ERROR_EXIT_CODE);
            }


        } else {
            printErrorMessage(BASIC_ERROR_INFIX,
                              parserGetReadBytes(&parser) + 1);
            exit_and_clean(ERROR_EXIT_CODE);
        }


    } else {
        printErrorMessage(BASIC_ERROR_INFIX,
                          parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

/**
 * @brief Główna pętla programu.
 * @return
 */
int main() {
    initProgram();
    //freopen("bigtest0.in", "r", stdin);
    while (true) {
        loopStepClear();
        skipSkipable();
        checkParserFinished();

        int nextType = parserNextType(&parser);
        checkParserError();

        readOperation(nextType);


    }

}
