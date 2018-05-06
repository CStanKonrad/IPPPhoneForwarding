/** @file
 * Pomocne funkcje do przetwarzania tekstu.
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 06.05.2018
 */

#include <string.h>
#include <stdlib.h>
#include "text.h"


void copyText(const char *src, char *dest, size_t length) {
    size_t i;
    for (i = 0; i < length; i++) {
        dest[i] = src[i];
    }
    dest[length] = '\0';
}

const char *stringEnd(const char *str) {
    return str + strlen(str);
}

char *concatenate(const char *a, const char *b) {
    size_t aLen = strlen(a);
    size_t bLen = strlen(b);
    char *result = malloc(aLen + bLen + (size_t) 1);
    if (result == NULL) {
        return NULL;
    } else {
        copyText(a, result, aLen);
        copyText(b, result + aLen, bLen);
        return result;
    }

}

char *duplicateText(const char *txt) {
    size_t txtLen = strlen(txt);
    char *result = malloc(txtLen + (size_t) 1);
    if (result == NULL) {
        return NULL;
    } else {
        copyText(txt, result, txtLen);
        return result;
    }
}
