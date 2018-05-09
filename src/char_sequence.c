/** @file
 * Implementacja modułu reprezentującego
 * ciąg znaków.
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 06.05.2018
 */

#include <assert.h>
#include <string.h>
#include "char_sequence.h"

/**
 * @brief Struktura opisująca element ciągu znaków.
 */
struct CharSequence {
    /**
     * @brief Litera w ciągu.
     */
    char letter;

    /**
     * @brief Następny element w ciągu.
     */
    CharSequence next;
};

/**
 * @brief inicjuje węzeł ciągu znaków.
 * @param node - wskaźnik na węzeł.
 * @param letter - znak który ma być przechowywany w węźle.
 */
static void charSequenceInitNewNode(CharSequence node, char letter) {
    assert(node != NULL);
    node->letter = letter;
    node->next = NULL;
}

/**
 * @brief Usuwa węzeł.
 * @param node - wskaźnik na węzeł.
 */
static void charSequenceDeleteNode(CharSequence node) {
    assert(node != NULL);
    free(node);
}

void charSequenceDelete(CharSequence node) {
    assert(node != NULL);
    CharSequence deletePtr = node, tmp;
    while (deletePtr != NULL) {
        tmp = deletePtr;
        deletePtr = deletePtr->next;
        charSequenceDeleteNode(tmp);
    }
}

CharSequence charSequenceFromCString(const char *str) {
    CharSequence result = malloc(sizeof(struct CharSequence));

    if (result == NULL) {
        return NULL;
    } else {
        charSequenceInitNewNode(result, str[0]);

        CharSequence insertPtr = result;
        size_t i;
        for (i = 1; str[i] != '\0'; i++) {
            insertPtr->next = malloc(sizeof(struct CharSequence));
            if (insertPtr->next == NULL) {
                charSequenceDelete(result);
                return NULL;
            } else {
                insertPtr = insertPtr->next;
                charSequenceInitNewNode(insertPtr, str[i]);
            }
        }
    }
    return result;
}

bool charSequenceNextChar(CharSequence *sequence, char *ch) {
    if (*sequence == NULL) {
        if (ch != NULL) {
            *ch = '\0';
        }
        return false;
    } else {
        if (ch != NULL) {
            *ch = (*sequence)->letter;
        }
        (*sequence) = (*sequence)->next;
        return true;
    }
}

size_t charSequenceLength(CharSequence sequence) {
    size_t result = 0;
    CharSequence ptr = sequence;
    while (charSequenceNextChar(&ptr, NULL)) {
        result++;
    }

    return result;
}


const char *charSequenceToCString(CharSequence sequence) {
    size_t length = charSequenceLength(sequence);
    char *result = malloc(length + (size_t) 1);

    if (result == NULL) {
        return NULL;
    } else {
        size_t i = 0;
        CharSequence j = sequence;
        char out;
        while (charSequenceNextChar(&j, &out)) {
            result[i] = out;
            i++;
        }
        result[length] = '\0';
    }
    return result;
}

bool charSequenceEqualToString(CharSequence sequence, const char *str) {

    size_t i;
    char let;
    CharSequence ptr = sequence;
    for (i = 0; str[i] != '\0'; i++) {
        if (!charSequenceNextChar(&ptr, &let)) {
            return false;
        } else if (let != str[i]) {
            return false;
        }

    }
    return !charSequenceNextChar(&ptr, &let);


}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

CharSequence charSequenceSequenceEnd(CharSequence sequence) {
    return NULL;
}

#pragma GCC diagnostic pop

char charSequenceGetChar(CharSequence sequence) {
    if (sequence == NULL) {
        return '\0';
    } else {
        return sequence->letter;
    }
}

CharSequence charSequenceLast(CharSequence sequence) {
    CharSequence ptr = sequence;
    CharSequence prev = ptr;
    while (charSequenceNextChar(&ptr, NULL)) {
        if (ptr != NULL) {
            prev = ptr;
        }
    }
    return prev;
}

void charSequenceSetNext(CharSequence a, CharSequence next) {
    a->next = next;
}
