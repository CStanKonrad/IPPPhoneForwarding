/** @file
 * Implementacja tablicy dynamicznie dopasowującej
 * swój rozmiar w miarę potrzeb (Vectora).
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.04.2018
 */

#include <string.h>
#include <stdlib.h>

#include "vector.h"
#include "stdfunc.h"

/**
 * @brief Współczynnik zwiększenia rozmiaru.
 * @see vectorReserve
 */
#define VECTOR_ALLOC_FACTOR 2

/**
 * @brief Współczynnik zmniejszenia rozmiaru.
 * @see vectorAdjustSize
 */
#define VECTOR_FREE_FACTOR 3

/**
 * @brief Inicjuje strukturę Vectora @p vector.
 * @param vector    - wskaźnik na strukturę Vectora.
 */
static void vectorInitEmpty(Vector vector) {
    vector->size = 0;
    vector->allocatedSize = 0;
    vector->array = NULL;
}

/**
 * @brief Ustawia zarezerwowaną pamięć.
 * Tworzy nową tablicę rozmiaru @p maxSize i przepisuje do niej
 * elementy Vectora @p vector (tyle ile się zmieści).
 * @param vector    - wskaźnik na strukturę Vectora
 * @param maxSize   - liczba komórek tablicy do zarezerwowania
 * @return W przypadku problemów z pamięcią VECTOR_MEMORY_ERROR,
 *         w przeciwnym przypadku VECTOR_SUCCES.
 */
static int vectorChangeUsedMemory(Vector vector, size_t maxSize) {
    VECTOR_ELEMENT_TYPE *newArray =
            malloc(sizeof(VECTOR_ELEMENT_TYPE) * maxSize);

    if (newArray == NULL) {
        return VECTOR_MEMORY_ERROR;
    } else {
        size_t minSize = MIN(vectorSize(vector), maxSize);

        memcpy(newArray, vector->array, minSize * sizeof(VECTOR_ELEMENT_TYPE));

        if (vector->array != NULL)
            free(vector->array);

        vector->array = newArray;
        vector->allocatedSize = maxSize;
        vector->size = minSize;

        return VECTOR_SUCCES;
    }
}

Vector vectorCreate() {
    Vector newVector = malloc(sizeof(struct Vector));

    if (newVector == NULL) {
        return NULL;
    } else {
        vectorInitEmpty(newVector);

        return newVector;
    }
}

/**
 * @brief Optymalizuje pamięć zajmowaną przez Vector.
 * @param vector    - wskaźnik na strukturę Vectora
 * @return
 */
static int vectorAdjustSize(Vector vector) {
    if (vectorSize(vector) < vectorReservedSize(vector) / VECTOR_FREE_FACTOR) {
        return vectorChangeUsedMemory(vector, vectorSize(vector));
    } else {
        return VECTOR_SUCCES;
    }
}

size_t vectorSize(Vector vector) {
    return vector->size;
}

size_t vectorReservedSize(Vector vector) {
    return vector->allocatedSize;
}

int vectorReserve(Vector vector, size_t toReserve) {
    if (vectorReservedSize(vector) < toReserve) {
        size_t s;
        s = MAX(toReserve, VECTOR_ALLOC_FACTOR * vectorReservedSize(vector));
        return vectorChangeUsedMemory(vector, s);
    } else {
        return VECTOR_SUCCES;
    }
}

int vectorSoftResize(Vector vector, size_t size) {
    if (vectorReservedSize(vector) >= size) {
        vector->size = size;
        return VECTOR_SUCCES;
    } else {
        if (vectorReserve(vector, size) == VECTOR_MEMORY_ERROR) {
            return VECTOR_MEMORY_ERROR;
        } else {
            return vectorSoftResize(vector, size);
        }

    }
}

void vectorClear(Vector vector) {
    if (vectorReservedSize(vector) > 0) {
        free(vector->array);
        vectorInitEmpty(vector);
    }
}

void vectorSoftClear(Vector vector) {
    vectorSoftResize(vector, 0);
}

void vectorDelete(Vector vector) {
    vectorClear(vector);
    free(vector);
}

int vectorPushBack(Vector vector, VECTOR_ELEMENT_TYPE element) {
    if (vectorReserve(vector, vectorSize(vector) + 1) == VECTOR_MEMORY_ERROR) {
        return VECTOR_MEMORY_ERROR;
    } else {
        vector->array[vectorSize(vector)] = element;
        vector->size++;
        return VECTOR_SUCCES;
    }
}

int vectorPopBack(Vector vector) {
    if (vectorSize(vector) == 0) {
        return VECTOR_OPERATION_ERROR;
    } else {
        vector->size--;
        return vectorAdjustSize(vector);
    }
}

VECTOR_ELEMENT_TYPE *vectorBegin(Vector vector) {
    return vector->array;
}

VECTOR_ELEMENT_TYPE *vectorEnd(Vector vector) {
    return &vector->array[vectorSize(vector)];
}

void vectorSwap(Vector a, Vector b) {
    VECTOR_ELEMENT_TYPE *tmp = a->array;
    a->array = b->array;
    b->array = tmp;

    size_t tmp_t = a->size;
    a->size = b->size;
    b->size = tmp_t;

    tmp_t = a->allocatedSize;
    a->allocatedSize = b->allocatedSize;
    b->allocatedSize = tmp_t;

}
