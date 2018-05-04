//
// Created by skonrad on 04.05.18.
//

#include <string.h>
#include "stdfunc.h"

void copyText(const char *src, char * dest, size_t length) {
    size_t i;
    for (i= 0; i < length; i++) {
        dest[i] = src[i];
    }
    dest[length] = '\0';
}

const char *stringEnd(const char *str) {
    return str + strlen(str);
}
