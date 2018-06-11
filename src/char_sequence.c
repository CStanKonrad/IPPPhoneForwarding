/** @file
 * Implementacja modułu reprezentującego
 * ciąg znaków.
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 01.06.2018
 */

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "char_sequence.h"
#include "stdfunc.h"
#include "text.h"
#include "character.h"

/**
 * @brief Maksymalna liczba znaków w bloku (węźle) ciągu znaków.
 */
#define CHAR_SEQUENCE_MAX_LETTERS_IN_BLOCK 256

/**
 * @brief Struktura opisująca element ciągu znaków (blok).
 */
struct CharSequence {
    /**
     * @brief Litery w bloku.
     */
    char *letters;

    /**
     * @brief Cyfry występujące w @p letters.
     * Jeżeli cyfra występuje to na bicie numer cyfra - '0' występuje 1,
     * w przeciwnym wypadku 0.
     */
    size_t availableDigits;

    /**
     * @brief Następny element (blok) w ciągu.
     */
    CharSequence next;
};

/**
 * @brief Sprawdza czy iterator nie ma już więcej elementów do przejrzenia.
 * @param[in] it - wskaźnik na iterator ciągu znaków.
 * @return true jeżeli brak elementów do przejrzenia, false w przeciwnym wypadku.
 */
static bool charSequenceIteratorEnd(CharSequenceIterator *it) {
    return it->isEnd;
}

/**
 * @brief Znak wskazywany przez iterator.
 * @param[in] it - wskaźnik na iterator ciągu znaków.
 * @return Znak wskazywany przez iterator, jeżeli @p charSequenceIteratorEnd
 *         to '\0'.
 */
static char charSequenceIteratorGetChar(CharSequenceIterator *it) {
    if (charSequenceIteratorEnd(it)) {
        return '\0';
    } else {
        return it->sequenceBlockPtr->letters[it->charId];
    }
}

/**
 * @brief Przesuwa iterator dalej.
 * @param[in, out] it  - wskaźnik na iterator ciągu znaków.
 */
static void charSequenceIteratorIncrement(CharSequenceIterator *it) {
    if (charSequenceIteratorEnd(it)) {
        return;
    } else {
        assert(it->charId < strlen(it->sequenceBlockPtr->letters));

        if (it->sequenceBlockPtr->letters[it->charId + 1] != '\0') {
            it->charId++;
        } else {
            it->charId = 0;
            it->sequenceBlockPtr = it->sequenceBlockPtr->next;
            if (it->sequenceBlockPtr == NULL) {
                it->isEnd = true;
            }
        }
    }
}

/**
 * @brief Resetuje informacje o występujących cyfrach.
 * Po wykonaniu @p availableDigits mówi, że nie wystąpiła żadna cyfra.
 * @param[out] availableDigits - wskaźnik na informacje o występujących cyfrach.
 */
static void charSequenceResetAvailableDigits(size_t *availableDigits) {
    *availableDigits = 0;
}

/**
 * @brief Ustawia informacje o występujących cyfrach.
 * Po wykonaniu @p availableDigits mówi, że cyfra digit wystąpiła.
 * @param[in, out] availableDigits - wskaźnik na informacje o występujących cyfrach.
 * @param[in] digit - cyfra.
 */
static void charSequenceSetDigitAvailable(size_t *availableDigits,
                                          char digit) {
    assert(characterIsDigit(digit));
    (*availableDigits) |= (((size_t) 1) << ((size_t) (digit - '0')));
}

/**
 * @brief Sprawdza czy według @p availableDigits cyfra @p digit wystąpiła.
 * @param[in] availableDigits - wskaźnik na informacje o występujących cyfrach.
 * @param[in] digit - cyfra.
 * @return true jeżeli tak, false w przeciwnym wypadku.
 */
static bool charSequenceIsDigitAvailable(const size_t *availableDigits,
                                         char digit) {
    assert(characterIsDigit(digit));
    return ((*availableDigits) & (((size_t) 1) << ((size_t) (digit - '0')))) != 0;
}


/**
 * @brief inicjuje węzeł ciągu znaków.
 * @param[in, out] node - wskaźnik na węzeł.
 * @param[in] letters - ciąg znaków który ma być przechowywany w węźle.
 */
static void charSequenceInitNewNode(CharSequence node, char *letters) {
    assert(node != NULL);
    node->letters = letters;
    node->next = NULL;

    charSequenceResetAvailableDigits(&node->availableDigits);
    if (letters != NULL) {
        const char *i;
        for (i = letters; *i != '\0'; i++) {
            if (characterIsDigit(*i)) {
                charSequenceSetDigitAvailable(&node->availableDigits, *i);
            }
        }
    }
}

/**
 * @brief Usuwa węzeł.
 * @param[in, out] node - wskaźnik na węzeł.
 */
static void charSequenceDeleteNode(CharSequence node) {
    assert(node != NULL);
    if (node->letters != NULL) {
        free(node->letters);
        node->letters = NULL;
    }
    free(node);
}

CharSequenceIterator charSequenceGetIterator(CharSequence sequence) {
    CharSequenceIterator result;
    result.isEnd = false;
    result.charId = 0;
    result.sequenceBlockPtr = sequence;

    return result;
}

bool charSequenceIteratorsEqual(CharSequenceIterator *a,
                                CharSequenceIterator *b) {
    return a->sequenceBlockPtr == b->sequenceBlockPtr
           && a->charId == b->charId
           && a->isEnd == b->isEnd;
}

void charSequenceMerge(CharSequence a, CharSequence b) {
    assert(a != NULL);
    assert(b != NULL);
    CharSequence ptr = a;

    while (ptr->next != NULL) {
        ptr = ptr->next;
    }

    size_t leftLen = strlen(ptr->letters);
    size_t rightLen = strlen(b->letters);

    if (leftLen + rightLen < CHAR_SEQUENCE_MAX_LETTERS_IN_BLOCK) {
        char *txt = concatenate(ptr->letters, b->letters);
        if (txt != NULL) {
            free(ptr->letters);
            ptr->letters = txt;
            ptr->availableDigits |= b->availableDigits;
            ptr->next = b->next;
            charSequenceDeleteNode(b);
        } else {
            ptr->next = b;
        }
    } else {
        ptr->next = b;
    }


}

CharSequence charSequenceSplitByIterator(CharSequence sequence,
                                         CharSequenceIterator *it) {
    assert(!charSequenceIteratorEnd(it));

    if (it->charId == 0) {
        CharSequence result = it->sequenceBlockPtr;

        CharSequence ptr = sequence;

        assert(ptr != it->sequenceBlockPtr);
        assert(ptr != NULL);

        while (ptr->next != it->sequenceBlockPtr) {
            ptr = ptr->next;
        }
        ptr->next = NULL;

        return result;
    } else {
        char *textA = malloc(sizeof(char) * (it->charId + (size_t) 1));
        if (textA == NULL) {
            return NULL;
        } else {
            size_t textLeftLen = strlen(it->sequenceBlockPtr->letters) - it->charId;
            char *textB = malloc(sizeof(char) * (textLeftLen + (size_t) 1));

            if (textB == NULL) {
                free(textA);
                return NULL;
            } else {
                copyText(it->sequenceBlockPtr->letters, textA, it->charId);
                copyText(it->sequenceBlockPtr->letters + it->charId, textB,
                         textLeftLen);


                CharSequence newBlock = malloc(sizeof(struct CharSequence));
                if (newBlock == NULL) {
                    free(textB);
                    free(textA);
                    return NULL;
                } else {
                    charSequenceInitNewNode(newBlock, textB);
                    newBlock->next = it->sequenceBlockPtr->next;

                    free(it->sequenceBlockPtr->letters);
                    charSequenceInitNewNode(it->sequenceBlockPtr, textA);
                    it->sequenceBlockPtr->next = NULL;

                    it->sequenceBlockPtr = newBlock;
                    it->charId = 0;

                    return it->sequenceBlockPtr;
                }
            }
        }
    }
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

/**
 * @brief Funkcja pomocnicza do czyszczenia pamięci dla @p charSequenceFromCString.
 * @param[in] blocks - bloki do wyczyszczenia.
 * @param[in] last - numer ostatniego bloku do wyczyszczenia.
 */
static void charSequenceFromCStringFreeBlocks(struct CharSequence **blocks,
                                              size_t last) {
    size_t i;
    for (i = 0; i <= last; i++) {
        if (blocks[i] != NULL) {
            charSequenceDeleteNode(blocks[i]);
        }
    }

    free(blocks);
}

CharSequence charSequenceFromCString(const char *str) {
    size_t strLength = strlen(str);
    size_t numberOfBlocks = strLength / CHAR_SEQUENCE_MAX_LETTERS_IN_BLOCK
                            + (strLength % CHAR_SEQUENCE_MAX_LETTERS_IN_BLOCK != 0);

    struct CharSequence **blocks = malloc(sizeof(struct CharSequence *)
                                          * numberOfBlocks);
    if (blocks == NULL) {
        return NULL;
    } else {
        size_t i;
        for (i = 0; i < numberOfBlocks; i++) {
            blocks[i] = malloc(sizeof(struct CharSequence));
            if (blocks[i] == NULL) {
                charSequenceFromCStringFreeBlocks(blocks, i);
                return NULL;
            } else {
                charSequenceInitNewNode(blocks[i], NULL);
                if (i > 0) {
                    blocks[i - 1]->next = blocks[i];
                }

                size_t toAddSize
                        = MIN(CHAR_SEQUENCE_MAX_LETTERS_IN_BLOCK,
                              strLength - i * CHAR_SEQUENCE_MAX_LETTERS_IN_BLOCK);

                blocks[i]->letters = malloc(sizeof(char)
                                            * (toAddSize + (size_t) 1));

                if (blocks[i]->letters == NULL) {
                    charSequenceFromCStringFreeBlocks(blocks, i);
                    return NULL;
                } else {
                    copyText(str + i * CHAR_SEQUENCE_MAX_LETTERS_IN_BLOCK,
                             blocks[i]->letters, toAddSize);
                    charSequenceInitNewNode(blocks[i], blocks[i]->letters);
                }
            }
        }
    }
    CharSequence result = *blocks;
    free(blocks);
    return result;

}

bool charSequenceNextChar(CharSequenceIterator *it, char *ch) {
    if (charSequenceIteratorEnd(it)) {
        if (ch != NULL) {
            *ch = '\0';
        }
        return false;
    } else {
        if (ch != NULL) {
            *ch = charSequenceIteratorGetChar(it);
        }
        charSequenceIteratorIncrement(it);
        return true;
    }
}

size_t charSequenceLength(CharSequence sequence) {
    size_t result = 0;
    CharSequenceIterator it = charSequenceGetIterator(sequence);
    while (charSequenceNextChar(&it, NULL)) {
        result++;
    }

    return result;
}

size_t charSequenceLengthLimited(CharSequence sequence, size_t limit,
                                 bool *greater) {

    assert(limit != 0);
    size_t result = 0;
    CharSequenceIterator it = charSequenceGetIterator(sequence);


    while (charSequenceNextChar(&it, NULL)) {
        result++;
        if (result == limit) {
            *greater = !charSequenceIteratorEnd(&it);
            return result;
        }
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
        CharSequenceIterator it = charSequenceGetIterator(sequence);
        char out;
        while (charSequenceNextChar(&it, &out)) {
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
    CharSequenceIterator it = charSequenceGetIterator(sequence);
    for (i = 0; str[i] != '\0'; i++) {
        if (!charSequenceNextChar(&it, &let)) {
            return false;
        } else if (let != str[i]) {
            return false;
        }

    }
    return !charSequenceNextChar(&it, &let);


}

CharSequenceIterator charSequenceSequenceEnd(CharSequence sequence) {
    CharSequenceIterator result = charSequenceGetIterator(sequence);
    result.isEnd = true;
    return result;
}

char charSequenceGetChar(CharSequenceIterator *it) {
    return charSequenceIteratorGetChar(it);
}

bool charSequenceCheckDigits(CharSequence sequence, const bool *digits) {
    CharSequence ptr = sequence;
    size_t i;
    char ch;
    while (ptr != NULL) {
        for (i = 0; i < CHARACTER_NUMBER_OF_DIGITS; i++) {
            ch = (char) i + (char) '0';
            if (charSequenceIsDigitAvailable(&ptr->availableDigits, ch)
                && !digits[i]) {
                return false;
            }
        }
        ptr = ptr->next;
    }
    return true;
}

