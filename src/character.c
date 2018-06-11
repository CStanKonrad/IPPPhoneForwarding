/** @file
 * Implementacja modułu modułu do uzyskiwania informacji o znakach.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */


#include <ctype.h>
#include <libio.h>

#include "character.h"

int characterIsBlank(int characterCode) {
    return isblank(characterCode);
}

int characterIsUnixNewLine(int characterCode) {
    return characterCode == CHARACTER_UNIX_NEW_LINE;
}

int characterIsCarriageReturn(int characterCode) {
    return characterCode == CHARACTER_CARRIAGE_RETURN;
}

int characterIsNewLine(int characterCode) {
    return characterIsUnixNewLine(characterCode) ||
           characterIsCarriageReturn(characterCode);
}

int characterIsEOF(int characterCode) {
    return characterCode == EOF;
}

int characterIsTerminator(int characterCode) {
    return characterIsNewLine(characterCode) ||
           characterIsCarriageReturn(characterCode) ||
           characterIsEOF(characterCode);
}

int characterIsUnixTerminator(int characterCode) {
    return characterIsUnixNewLine(characterCode) ||
           characterIsEOF(characterCode);
}

int characterIsWhite(int characterCode) {
    return isspace(characterCode);
}

int characterIsGraph(int characterCode) {
    return isgraph(characterCode);
}

int characterIsDigit(int characterCode) {
    return isdigit(characterCode)
           || characterCode == ':'
           || characterCode == ';';
}

int characterIsLetter(int characterCode) {
    return isalpha(characterCode);
}

int characterIsMinus(int characterCode) {
    return characterCode == '-';
}

int characterIsPlus(int characterCode) {
    return characterCode == '+';
}

int characterIsZero(int characterCode) {
    return characterCode == '0';
}

int characterIsSpaceBar(int characterCode) {
    return characterCode == ' ';
}
