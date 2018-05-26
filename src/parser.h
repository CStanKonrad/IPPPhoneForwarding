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

#include "vector.h"

#define PARSER_COMMENT_SEQUENCE "$$"
#define PARSER_OPERATOR_QM '?'
#define PARSER_OPERATOR_REDIRECT '>'
#define PARSER_OPERATOR_NEW "NEW"
#define PARSER_OPERATOR_DELETE "DEL"

#define PARSER_FAIL 0

#define PARSER_ELEMENT_TYPE_NUMBER 1
#define PARSER_ELEMENT_TYPE_WORD 2
#define PARSER_ELEMENT_TYPE_SINGLE_CHARACTER_OPERATOR 3

#define PARSER_ELEMENT_TYPE_OPERATOR_QM 4
#define PARSER_ELEMENT_TYPE_OPERATOR_REDIRECT 5
#define PARSER_ELEMENT_TYPE_OPERATOR_NEW 6
#define PARSER_ELEMENT_TYPE_OPERATOR_DELETE 7


typedef struct Parser* Parser;

struct Parser {
    size_t readBytes;
    bool isError;
};

struct Parser parserCreateNew();

void parserSkipSkipable(Parser parser);

bool parserError(Parser parser);

bool parserFinished(Parser parser);


int parserNextType(Parser parser);

int parserReadOperator(Parser parser);

bool parserReadIdentificator(Parser parser, Vector destination);

bool parserReadNumber(Parser parser, Vector destination);

size_t parserGetReadBytes(Parser parser);

#endif //TELEFONY_PARSER_H
