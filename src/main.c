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

#include "parser.h"
#include "phone_bases_system.h"
#include "vector.h"

#define BASIC_ERROR_MESSAGE "ERROR"
#define EOF_ERROR_SUFFIX "eof"


#define ERROR_EXIT_CODE 1
#define SUCCESS_EXIT_CODE 0


static PhoneBases  bases = NULL;
Vector word1 = NULL, word2 = NULL;
struct PhoneForward *currentBase = NULL;
const struct PhoneNumbers *phoneNumbers = NULL;


void exit_and_clean(int exit_code) {

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

void initProgram() {
    bases = phoneBasesCreateNewPhoneBases();
    if (bases == NULL) {
        exit_and_clean(ERROR_EXIT_CODE);
    }

    word1 = vectorCreate();

    if (word1 == NULL) {
        exit_and_clean(ERROR_EXIT_CODE);
    }

    word2 = vectorCreate();

    if (word2 == NULL) {
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

void makeVectorCStringCompatible(Vector v) {
    if (!vectorPushBack(v, '\0')) {
        exit_and_clean(ERROR_EXIT_CODE);
    }
}

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
            //fprintf(stderr, "%lld\n", parser.readedBytes);
            fprintf(stderr, "%s %s", BASIC_ERROR_MESSAGE, EOF_ERROR_SUFFIX);
            return ERROR_EXIT_CODE;
        } else if (parserFinished(&parser)) {
            exit_and_clean(SUCCESS_EXIT_CODE);
        } else {
            int nextType = parserNextType(&parser);
            if (parserError(&parser)) {
                fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                exit_and_clean(ERROR_EXIT_CODE);
            } else {
                if (nextType == PARSER_ELEMENT_TYPE_WORD) {
                    int operator = parserReadOperator(&parser);
                    if (operator == PARSER_ELEMENT_TYPE_OPERATOR_NEW) {
                        parserSkipSkipable(&parser);
                        if (parserError(&parser) || !parserReadIdentificator(&parser, word1)) {
                            //todo memory error
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else if (parserError(&parser)) {
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else {
                            makeVectorCStringCompatible(word1);
                            currentBase = phoneBasesAddBase(bases, vectorBegin(word1));
                        }
                    } else if (operator == PARSER_ELEMENT_TYPE_OPERATOR_DELETE) {
                        //todo
                    } else {
                        fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                        exit_and_clean(ERROR_EXIT_CODE);
                    }
                } else if (nextType == PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR) {
                    int operator = parserReadOperator(&parser);
                    if (operator == PARSER_ELEMENT_TYPE_OPERATOR_QM) {

                        if (currentBase == NULL) {
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        }

                        parserSkipSkipable(&parser);
                        if (parserError(&parser) || !parserReadNumber(&parser, word1)) {
                            //todo memory error
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else if (parserError(&parser)) {
                            fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                            exit_and_clean(ERROR_EXIT_CODE);
                        } else {
                            makeVectorCStringCompatible(word1);
                            phoneNumbers = phfwdReverse(currentBase, vectorBegin(word1));

                            if (phoneNumbers == NULL) {
                                //todo memory error
                                fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                                exit_and_clean(ERROR_EXIT_CODE);
                            } else {
                                size_t i = 0;
                                for (i = 0; phnumGet(phoneNumbers, i) != NULL; ++i) {
                                    fprintf(stdout, "%s\n", phnumGet(phoneNumbers, i));
                                }
                            }

                        }
                    } else {
                        fprintf(stderr, "%s %zu", BASIC_ERROR_MESSAGE, parser.readedBytes);
                        exit_and_clean(ERROR_EXIT_CODE);
                    }
                }
            }
        }
    }
    return 0;
}

