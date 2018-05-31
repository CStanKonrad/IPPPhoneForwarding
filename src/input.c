/** @file
 * Implementacja modułu służącego do parsowania wejścia.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "input.h"
#include "character.h"
#include "vector.h"


/**
 * @brief Czy identyfikator końca pliku ustawiony.
 * @return Niezerowa wartość jeżeli tak zerowa w przeciwnym wypadku.
 */
static int inputIsStreamEnded() {
    return feof(stdin);
}

int inputPeekCharacter() {
    int characterCode = getc(stdin);

    ungetc(characterCode, stdin);

    return characterCode;
}

int inputGetCharacter() {
    return getc(stdin);
}

size_t inputIgnoreUntil(int (*predicate)(int)) {
    size_t abandoned = 0;
    while (!(*predicate)(inputPeekCharacter())) {
        inputGetCharacter();
        abandoned++;
    }
    return abandoned;
}

size_t inputIgnoreWhile(int (*predicate)(int)) {
    size_t abandoned = 0;
    while ((*predicate)(inputPeekCharacter())) {
        inputGetCharacter();
        abandoned++;
    }
    return abandoned;
}

void inputIgnoreBlank() {
    inputIgnoreWhile(characterIsBlank);
}

void inputIgnoreWhite() {
    inputIgnoreWhile(characterIsWhite);
}

void inputIgnoreNewLine() {
    if (characterIsNewLine(inputPeekCharacter())) {
        int c = inputGetCharacter();

        if (!inputIsStreamEnded()
            && characterIsCarriageReturn(inputPeekCharacter())
            && characterIsUnixNewLine(c)) {
            inputGetCharacter();
        }
    }
}

int inputIgnoreUnixNewLine() {
    if (characterIsUnixNewLine(inputPeekCharacter())) {
        inputGetCharacter();
        return INPUT_READ_SUCCESS;
    }
    return INPUT_READ_FAIL;
}

int inputIgnoreLine() {
    int returnCode = INPUT_READ_SUCCESS;

    if (!characterIsTerminator(inputPeekCharacter())) {
        returnCode = INPUT_READ_TRASH_DETECTED;
    }

    inputIgnoreUntil(characterIsTerminator);

    return returnCode;
}

int inputIgnoreUnixLine() {
    int returnCode = INPUT_READ_SUCCESS;

    if (!characterIsUnixTerminator(inputPeekCharacter())) {
        returnCode = INPUT_READ_TRASH_DETECTED;
    }

    inputIgnoreUntil(characterIsUnixTerminator);

    return returnCode;
}

int inputReadWhile(int (*predicate)(int),
                   size_t maxLength, Vector destination) {
    size_t readBytes = 0;

    while ((*predicate)(inputPeekCharacter())
           && readBytes < maxLength) {
        if (vectorPushBack(destination, (char) inputGetCharacter())
            != VECTOR_SUCCES) {
            return INPUT_READ_FAIL;
        }

        readBytes++;
    }
    return INPUT_READ_SUCCESS;
}

int inputReadUntil(int (*predicate)(int),
                   size_t maxLength, Vector destination) {
    size_t readedBytes = 0;

    while (!(*predicate)(inputPeekCharacter())
           && readedBytes < maxLength) {
        if (vectorPushBack(destination, (char) inputGetCharacter())
            != VECTOR_SUCCES) {
            return INPUT_READ_FAIL;
        }

        readedBytes++;
    }
    return INPUT_READ_SUCCESS;
}


int inputIsEOF() {
    return inputIsStreamEnded() || characterIsEOF(inputPeekCharacter());
}