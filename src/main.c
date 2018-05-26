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

#define BASIC_ERROR_MESSAGE "ERROR"
#define BASIC_ERROR_SUFFIX " "
#define EOF_ERROR_SUFFIX " EOF"
#define MEMORY_ERROR_SUFFIX " not enough memory "
#define DEL_OPERATOR_ERROR_SUFFIX " DEL "
#define QM_OPERATOR_ERROR_SUFFIX " ? "
#define REDIRECT_OPERATOR_ERROR_SUFFIX " > "


#define ERROR_EXIT_CODE 1
#define SUCCESS_EXIT_CODE 0

//todo 2?
#define OPERATOR_POSITION_OFFSET 2


static PhoneBases  bases = NULL;
static Vector word1 = NULL, word2 = NULL;
static struct PhoneForward *currentBase = NULL;
static const struct PhoneNumbers *phoneNumbers = NULL;
static struct Parser parser;


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

static void printErrorMessage(const char *suffix, size_t bytes) {
    fprintf(stderr, "%s%s%zu", BASIC_ERROR_MESSAGE, suffix,
            bytes);
}

static void printEofError() {
    fprintf(stderr, "%s%s", BASIC_ERROR_MESSAGE, EOF_ERROR_SUFFIX);
}

static void initProgram() {
    parser = parserCreateNew();
    bases = phoneBasesCreateNewPhoneBases();
    if (bases == NULL) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    word1 = vectorCreate();

    if (word1 == NULL) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    word2 = vectorCreate();

    if (word2 == NULL) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

static void makeVectorCStringCompatible(Vector v) {
    if (!vectorPushBack(v, '\0')) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

static void loopStepClear() {
    vectorSoftClear(word1);
    vectorSoftClear(word2);
}

static void checkParserError() {
    if (parserIsCommentEofError(&parser)) {
        printEofError();
        exit_and_clean(ERROR_EXIT_CODE);
    }
    if (parserError(&parser)) {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

static void checkEofError() {
    if (inputIsEOF()) {
        printEofError();
        exit_and_clean(ERROR_EXIT_CODE);
    }
}
static void checkParserFinished() {
    if (parserFinished(&parser)) {
        exit_and_clean(SUCCESS_EXIT_CODE);
    }
}

static void skipSkipable() {
    parserSkipSkipable(&parser);
    checkParserError();
}

static void readOperationNew() {
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();


    if (nextType != PARSER_ELEMENT_TYPE_WORD) {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (!parserReadIdentificator(&parser, word1)) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
    checkParserError();

    makeVectorCStringCompatible(word1);

    if (strcmp(vectorBegin(word1), PARSER_OPERATOR_DELETE) == 0
        || strcmp(vectorBegin(word1), PARSER_OPERATOR_NEW) == 0) {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) - OPERATOR_POSITION_OFFSET);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (vectorSize(word1) <= 1) {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    currentBase = phoneBasesAddBase(bases, vectorBegin(word1));

    if (currentBase == NULL) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

}

static void readOperationDeleteNumber(size_t operatorPos) {
    if (!parserReadNumber(&parser, word1)) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
    checkParserError();

    if (currentBase == NULL) {
        printErrorMessage(DEL_OPERATOR_ERROR_SUFFIX, operatorPos);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    makeVectorCStringCompatible(word1);
    phfwdRemove(currentBase, vectorBegin(word1));
}

static void readOperationDeleteBase(size_t operatorPos) {
    if (!parserReadIdentificator(&parser, word1)) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
    checkParserError();

    makeVectorCStringCompatible(word1);

    if (strcmp(vectorBegin(word1), PARSER_OPERATOR_DELETE) == 0
            || strcmp(vectorBegin(word1), PARSER_OPERATOR_NEW) == 0) {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) - OPERATOR_POSITION_OFFSET);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    struct PhoneForward *toDel = phoneBasesGetBase(bases, vectorBegin(word1));

    if (toDel == NULL) {
        printErrorMessage(DEL_OPERATOR_ERROR_SUFFIX, operatorPos);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (toDel == currentBase) {
        currentBase = NULL;
    }

    phoneBasesDelBase(bases, vectorBegin(word1));

}


static void readOperationDelete() {
    size_t operatorPos = parserGetReadBytes(&parser) - strlen(PARSER_OPERATOR_DELETE) + 1;
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();

    if (nextType == PARSER_ELEMENT_TYPE_NUMBER) {
        readOperationDeleteNumber(operatorPos);
    } else if (nextType == PARSER_ELEMENT_TYPE_WORD) {
        readOperationDeleteBase(operatorPos);
    } else {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }

}

static void printNumbers(const struct PhoneNumbers *numbers ) {
    size_t i;
    for (i = 0; phnumGet(numbers, i) != NULL; i++) {
        fprintf(stdout, "%s\n", phnumGet(numbers, i));
    }
}

static void readOperationReverse() {
    size_t operatorPos = parserGetReadBytes(&parser);
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();

    if (nextType == PARSER_ELEMENT_TYPE_NUMBER) {
        if (!parserReadNumber(&parser, word1)) {
            printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }
        checkParserError();



        if (currentBase == NULL) {
            printErrorMessage(QM_OPERATOR_ERROR_SUFFIX, operatorPos);
            exit_and_clean(ERROR_EXIT_CODE);
        }
        makeVectorCStringCompatible(word1);
        const struct PhoneNumbers *numbers = phfwdReverse(currentBase, vectorBegin(word1));

        if (numbers == NULL) {
            printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }

        printNumbers(numbers);

        phnumDelete(numbers);



    } else {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }

}


static void readOperatorGetFromWord1() {
    makeVectorCStringCompatible(word1);

    if (currentBase == NULL) {
        printErrorMessage(QM_OPERATOR_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    const struct PhoneNumbers *numbers = phfwdGet(currentBase, vectorBegin(word1));

    if (numbers == NULL) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }

    printNumbers(numbers);

    phnumDelete(numbers);

}


static void readOperatorRedirectWord1() {
    size_t operatorPos = parserGetReadBytes(&parser);
    skipSkipable();
    checkEofError();

    int nextType = parserNextType(&parser);
    checkParserError();

    if (nextType != PARSER_ELEMENT_TYPE_NUMBER) {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }

    if (!parserReadNumber(&parser, word2)) {
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
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
        printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

static void readOperation(int nextType) {
    if (nextType == PARSER_ELEMENT_TYPE_WORD) {
        int operator = parserReadOperator(&parser);
        checkParserError();
        checkEofError();

        if (operator == PARSER_ELEMENT_TYPE_OPERATOR_NEW) {
            //fprintf(stderr, "%d\n", parserGetReadBytes(&parser));
            readOperationNew();
        } else if (operator == PARSER_ELEMENT_TYPE_OPERATOR_DELETE) {
            readOperationDelete();
        } else {
            printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }

    } else if (nextType == PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR) {
        int operator = parserReadOperator(&parser);
        checkParserError();
        checkEofError();

        if (operator == PARSER_ELEMENT_TYPE_OPERATOR_QM) {
            readOperationReverse();
        } else {
            printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser));
            exit_and_clean(ERROR_EXIT_CODE);
        }

    } else if (nextType == PARSER_ELEMENT_TYPE_NUMBER) {
        if (!parserReadNumber(&parser, word1)) {
            printErrorMessage(MEMORY_ERROR_SUFFIX, parserGetReadBytes(&parser));
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
                //todo
            } else {
                printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) + 1);
                exit_and_clean(ERROR_EXIT_CODE);
            }


        } else {
            printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) + 1);
            exit_and_clean(ERROR_EXIT_CODE);
        }


    } else {
        printErrorMessage(BASIC_ERROR_SUFFIX, parserGetReadBytes(&parser) + 1);
        exit_and_clean(ERROR_EXIT_CODE);
    }
}


int main() {
    initProgram();
    //freopen("in.txt","r",stdin);

    while (true) {
        loopStepClear();
        skipSkipable();
        checkParserFinished();

        int nextType = parserNextType(&parser);
        checkParserError();

        readOperation(nextType);


    }

}

/*
int main() {

    initProgram();
    struct Parser parser = parserCreateNew();
    //system("pwd");
    freopen("in.txt","r",stdin);
    while (true) {
        vectorSoftClear(word1);
        vectorSoftClear(word2);
        parserSkipSkipable(&parser);
        if (parserError(&parser)) {
            //fprintf(stderr, "%lld\n", parser.readBytes);
            fprintf(stderr, "%s %s", BASIC_ERROR_MESSAGE, EOF_ERROR_SUFFIX);
            return ERROR_EXIT_CODE;
        } else if (parserFinished(&parser)) {
            exit_and_clean(SUCCESS_EXIT_CODE);
        } else {
            int nextType = parserNextType(&parser);
            if (parserError(&parser)) {
                fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                exit_and_clean(ERROR_EXIT_CODE);
            } else {
                if (nextType == PARSER_ELEMENT_TYPE_WORD) {
                    int operator = parserReadOperator(&parser);
                    if (operator == PARSER_ELEMENT_TYPE_OPERATOR_NEW) {
                        parserSkipSkipable(&parser);
                        if (parserError(&parser)) {
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else if (!parserReadIdentificator(&parser, word1)) {
                            //todo memory error
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else if (parserError(&parser)) {
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else {
                            makeVectorCStringCompatible(word1);
                            currentBase = phoneBasesAddBase(bases, vectorBegin(word1));
                        }
                    } else if (operator == PARSER_ELEMENT_TYPE_OPERATOR_DELETE) {
                        parserSkipSkipable(&parser);
                        if (parserError(&parser)) {
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else {
                            nextType = parserNextType(&parser);

                            if (nextType == PARSER_ELEMENT_TYPE_NUMBER) {
                                if (currentBase == NULL) {
                                    fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                                    exit_and_clean(ERROR_EXIT_CODE);
                                }

                                if (!parserReadNumber(&parser, word1) || parserError(&parser)) {
                                    //todo memory error
                                    fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                                    exit_and_clean(ERROR_EXIT_CODE);
                                } else {
                                    makeVectorCStringCompatible(word1);
                                    phfwdRemove(currentBase, vectorBegin(word1));
                                }
                            } else if (nextType == PARSER_ELEMENT_TYPE_WORD) {
                                if (!parserReadIdentificator(&parser, word1) || parserError(&parser)) {
                                    //todo memory error
                                    fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                                    exit_and_clean(ERROR_EXIT_CODE);
                                } else {
                                    makeVectorCStringCompatible(word1);
                                    if (currentBase == phoneBasesGetBase(bases, vectorBegin(word1))) {
                                        currentBase = NULL;
                                    }
                                    if (!phoneBasesDelBase(bases, vectorBegin(word1))) {
                                        fprintf(stderr, "%s DEL %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                                        exit_and_clean(ERROR_EXIT_CODE);
                                    }
                                }
                            }
                        }


                    } else {
                        //todo
                        fprintf(stderr, "%s yyyy %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                        exit_and_clean(ERROR_EXIT_CODE);
                    }
                } else if (nextType == PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR) {
                    int operator = parserReadOperator(&parser);
                    if (operator == PARSER_ELEMENT_TYPE_OPERATOR_QM) {

                        if (currentBase == NULL) {
                            fprintf(stderr, "%s ? %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        }

                        parserSkipSkipable(&parser);
                        if (parserError(&parser)) {
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else if (!parserReadNumber(&parser, word1)) {
                            //todo memory error
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else if (parserError(&parser)) {
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else {
                            makeVectorCStringCompatible(word1);
                            phoneNumbers = phfwdReverse(currentBase, vectorBegin(word1));

                            if (phoneNumbers == NULL) {
                                //todo memory error
                                fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                                exit_and_clean(ERROR_EXIT_CODE);
                            } else {
                                size_t i = 0;
                                for (i = 0; phnumGet(phoneNumbers, i) != NULL; ++i) {
                                    fprintf(stdout, "%s\n", phnumGet(phoneNumbers, i));
                                }
                                phnumDelete(phoneNumbers);
                            }

                        }
                    } else {
                        fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readBytes);
                        exit_and_clean(ERROR_EXIT_CODE);
                    }
                }
            }
        }
    }
    return 0;
}

*/