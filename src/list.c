/** @file
 * Implementacja listy dwukierunkowej
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.04.2018
 */

#include <stdlib.h>

#include "list.h"


/**
 * @brief Inicjalizuje węzeł @p node.
 * @param[in] node  - wskaźnik na węzeł.
 */
static void listInitNode(ListNode node) {
    node->previous = NULL;
    node->next = NULL;
}

/**
 * @brief Tworzy i inicjalizuje węzeł listy.
 * @return Wskaźnik na nowo stworzony i zainicjowany węzeł,
 *         w przypadku problemów z przydzieleniem pamięci NULL.
 */
static ListNode listAllocNode() {
    size_t bytesToAlloc = sizeof(struct ListNode);
    ListNode newNode = malloc(bytesToAlloc);

    if (newNode == NULL) {
        return NULL;
    } else {
        listInitNode(newNode);
        return newNode;
    }

}

/**
 * @brief Inicjalizuje strukturę listy @p list.
 * @param[in] list  - wskaźnik na listę.
 */
static void listInitList(List list) {
    listInitNode(&list->begin);
    listInitNode(&list->end);
    list->begin.next = &list->end;
    list->end.previous = &list->begin;
}

List listCreate() {
    List list;
    size_t bytesToAlloc = sizeof(struct List);

    list = malloc(bytesToAlloc);

    if (list == NULL) {
        return NULL;
    } else {
        listInitList(list);

        return list;
    }
}

ListNode listInsertAfter(ListNode node,
                         LIST_ELEMENT_TYPE element) {
    ListNode newNode = listAllocNode();

    if (newNode == NULL) {
        return NULL;
    } else {

        newNode->element = element;

        newNode->next = node->next;
        newNode->previous = node;

        newNode->next->previous = newNode;
        newNode->previous->next = newNode;

        return newNode;
    }
}

ListNode listInsertBefore(ListNode node,
                          LIST_ELEMENT_TYPE element) {
    return listInsertAfter(node->previous, element);
}

ListNode listPushFront(List list,
                       LIST_ELEMENT_TYPE element) {
    return listInsertAfter(&list->begin, element);

}

ListNode listPushBack(List list,
                      LIST_ELEMENT_TYPE element) {
    return listInsertBefore(&list->end, element);
}

void listDeleteNode(ListNode node) {
    node->previous->next = node->next;
    node->next->previous = node->previous;
    free(node);
}

void listPopFront(List list) {
    listDeleteNode(listFirstNode(list));
}

void listPopBack(List list) {
    listDeleteNode(listLastNode(list));
}

void listJoin(List front, List back) {
    if (!listIsEmpty(back)) {
        front->end.previous->next = back->begin.next;
        back->begin.next->previous = front->end.previous;

        front->end = back->end;
    }
    free(back);
}

/** @brief sprawdza czy @p node jest atrapą / strażnikiem.
 * @param[in] node   - wskaźnik na węzeł.
 * @return Niezerowa wartość jeżeli jest  atrapą / strażnikiem,
 *         w przeciwnym razie zero.
 */
static int listIsGuard(ListNode node) {
    return (node->next == NULL || node->previous == NULL);
}

void listDeleteContent(List list) {
    while (!listIsEmpty(list)) {
        listPopFront(list);
    }
}

void listDestroy(List list) {
    listDeleteContent(list);
    free(list);
}

int listIsEmpty(List list) {
    return list->begin.next == &list->end;
}

ListNode listFirstNode(List list) {
    if (listIsEmpty(list)) {
        return NULL;
    } else {
        return list->begin.next;
    }
}

ListNode listLastNode(List list) {
    if (listIsEmpty(list)) {
        return NULL;
    } else {
        return list->end.previous;
    }
}

ListNode listNextNode(ListNode node) {
    if (listIsGuard(node->next)) {
        return NULL;
    } else {
        return node->next;
    }
}

ListNode listPreviousNode(ListNode node) {
    if (listIsGuard(node->previous)) {
        return NULL;
    } else {
        return node->previous;
    }
}

LIST_ELEMENT_TYPE listNodeGetValue(ListNode node) {
    return node->element;
}

/**
 *
 * Wczytuje @p k elementów z listy zaczynając od pozycji @p start
 * i przesuwając się
 * za pomocą funkcji @p next do tablicy @p output
 * W przypadku dojścia do końca listy przerywa wczytywanie.
 * @param[in] next      - funkcja przesuwająca wskaźnik.
 * @param[in] start     - wskaźnik na węzeł startowy.
 * @param[in] k         - liczba elementów do wczytania.
 * @param[out] output    - tablica do umieszczenia wyniku.
 */
static void listReadKElements(
        ListNode (*next)(ListNode),
        ListNode start, size_t k,
        LIST_ELEMENT_TYPE *output) {
    size_t read = 0;
    ListNode pos = start;

    while (read < k && pos != NULL) {
        output[read] = listNodeGetValue(pos);
        read++;
        pos = next(pos);
    }
}

void listLastKElementsToArray(List list, size_t k,
                              LIST_ELEMENT_TYPE *output) {
    listReadKElements(listPreviousNode,
                      listLastNode(list), k, output);
}

void listFirstKElementsToArray(List list, size_t k,
                               LIST_ELEMENT_TYPE *output) {
    listReadKElements(listNextNode,
                      listFirstNode(list), k, output);
}

size_t listSize(List list, size_t maxSize) {
    ListNode ptr = listFirstNode(list);
    size_t countedSize = 0;

    while (ptr != NULL && countedSize < maxSize) {
        countedSize++;
        ptr = listNextNode(ptr);
    }

    return countedSize;
}
