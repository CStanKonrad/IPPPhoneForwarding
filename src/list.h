/** @file
 * Interfejs listy dwukierunkowej
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.04.2018
 */

#ifndef TELEFONY_LIST_H
#define TELEFONY_LIST_H

#include <stddef.h>
#include "radix_tree.h"

/**
 * @brief Typ elementów przechowywanych w liście.
 */
#define LIST_ELEMENT_TYPE RadixTreeNode

/**
 * @brief Wskaźnik na listę.
 * @see struct List
 */
typedef struct List *List;

/**
 * @brief Wskaźnik na węzeł listy.
 * @see struct ListNode
 */
typedef struct ListNode *ListNode;

/**
 * @brief Struktura reprezentująca węzeł listy.
 */
struct ListNode {
    /**
     * @brief Poprzedni węzeł.
     * Wskaźnik na poprzedni w kolejności węzeł.
     */
    ListNode previous;

    /**
     * @brief Następny węzeł.
     * Wskaźnik na następny w kolejności węzeł.
     */
    ListNode next;

    /**
     * @brief Przechowywany element.
     */
    LIST_ELEMENT_TYPE element;
};

/**
 * @brief Struktura reprezentująca listę.
 */
struct List {
    /**
     * @brief Strażnik (atrapa).
     * Atrapa stojąca na początku listy.
     */
    struct ListNode begin;

    /**
     * @brief Strażnik (atrapa).
     * Atrapa stojąca na końcu listy.
     */
    struct ListNode end;
};

/**
 * @brief Tworzy nową pustą listę.
 * #### Złożoność
 * O(1)
 * @return Wskaźnik do nowo stworzonej listy lub NULL,
 *         w przypadku problemów z przydzieleniem pamięci.
 */
List listCreate();

/**
 * @brief Dodaje nowy węzeł za podanym.
 * Dodaje za węzłem @p node nowo stworzony węzeł
 * z przechowywaną wartością @p element.
 * #### Złożoność
 * O(1)
 * @param[in] node  - wskaźnik na węzeł.
 * @param[in] element  - element do wstawienia.
 * @return Wskaźnik do stworzonego węzła,
 *         zaś w przypadku problemów z przydzieleniem pamięci NULL.
 */
ListNode listInsertAfter(ListNode node,
                         LIST_ELEMENT_TYPE element);


/**
 * @brief Dodaje nowy węzeł przed podanym.
 * Dodaje przed węzłem @p node nowo stworzony węzeł
 * z przechowywaną wartością @p element.
 * #### Złożoność
 * O(1)
 * @param[in] node  - wskaźnik na węzeł.
 * @param[in] element  - element do wstawienia.
 * @return Wskaźnik do stworzonego węzła,
 *         zaś w przypadku problemów z przydzieleniem pamięci NULL.
 */
ListNode listInsertBefore(ListNode node,
                          LIST_ELEMENT_TYPE element);

/**
 * @brief Dodaje element na początek listy.
 * Dodaje wezeł z wartością @p element na początek @p list.
 * #### Złożoność
 * O(1)
 * @param[in] list  - wskaźnik na listę.
 * @param[in] element  - element do wstawienia.
 * @return W przypadku niemożności przydzielenia pamięci
 *         NULL, w przeciwnym przypadku
 *         wskaźnik do stworzonego węzła.
 */
ListNode listPushFront(List list,
                       LIST_ELEMENT_TYPE element);


/**
 * @brief Dodaje element na koniec listy.
 * Dodaje wezeł z wartością @p element na koniec @p list.
 * #### Złożoność
 * O(1)
 * @param[in] list  - wskaźnik na listę.
 * @param[in] element  - element do wstawienia.
 * @return W przypadku niemożności przydzielenia pamięci
 *          NULL, w przeciwnym przypadku
 *          wskaźnik do stworzonego węzła.
 */
ListNode listPushBack(List list,
                      LIST_ELEMENT_TYPE element);

/**
 * @brief Usuwa węzeł @p node z listy.
 * #### Złożoność
 * O(1)
 * @param[in] node  - wskaźnik na węzeł.
 */
void listDeleteNode(ListNode node);

/**
 * @brief Usuwa pierwszy węzeł z listy.
 * Usuwa pierwszy węzeł z @p list.
 * Zakłada niepustość @p list.
 * #### Złożoność
 * O(1)
 * @param[in] list  - wskaźnik na listę.
 */
void listPopFront(List list);


/**
 * @brief Usuwa ostatni węzeł z listy.
 * Usuwa ostatni węzeł z @p list.
 * Zakłada niepustość @p list.
 * #### Złożoność
 * O(1)
 * @param[in] list  - wskaźnik na listę.
 */
void listPopBack(List list);


/**
 * @brief Łączy listy.
 * Dołącza listę @p back na koniec listy @p front,
 * struktura reprezentująca @p back zostaje usunięta.
 * #### Złożoność
 * O(1)
 * @param[in] front  - wskaźnik na listę.
 * @param[in] back  - wskaźnik na listę.
 */
void listJoin(List front, List back);

/**
 * @brief Usuwa zawartość listy.
 * Usuwa zawartość listy @p list.
 * #### Złożoność
 * O(ilość elementów w @p list)
 * @param[in] list  - wskaźnik na listę.
 */
void listDeleteContent(List list);

/**
 * @brief Usuwa strukturę.
 * Usuwa całą zawartość @p list
 * oraz strukturę reprezentującą @p list.
 * #### Złożoność
 * O(ilość elementów w @p list)
 * @param[in] list  - wskaźnik na listę.
 */
void listDestroy(List list);

/**
 * @brief Sprawdza czy lista @p list jest pusta.
 * #### Złożoność
 * O(1)
 * @param[in] list  - wskaźnik na listę.
 * @return Niezerowa wartość jeżeli @p list
 *         jest pusta.
 */
int listIsEmpty(List list);

/**
 * @brief Zwraca wskaźnik do pierwszego węzła.
 * #### Złożoność
 * O(1)
 * @param[in] list  - wskaźnik na listę.
 * @return Wskaźnik do pierwszego węzła @p list.
 *         W przypadku braku węzłów NULL.
 */
ListNode listFirstNode(List list);

/**
 * @brief Zwraca wskaźnik do ostatniego węzła.
 * #### Złożoność
 * O(1)
 * @param[in] list  - wskaźnik na listę.
 * @return Wskaźnik do ostatniego węzła listy @p list.
 *         W przypadku braku węzłów NULL.
 */
ListNode listLastNode(List list);

/**
 * @brief Następny w kolejności węzeł.
 * #### Złożoność
 * O(1)
 * @param[in] node  - wskaźnik na węzeł.
 * @return Wskaźnik do następnego w kolejności węzła listy @p list.
 *         W przypadku braku następnego NULL.
 */
ListNode listNextNode(ListNode node);

/**
 * @brief Poprzedni w kolejności węzeł.
 * #### Złożoność
 * O(1)
 * @param[in] node  - wskaźnik na węzeł.
 * @return Wskaźnik do poprzedniego w kolejności węzła listy @p list.
 *         W przypadku braku poprzedniego NULL.
 */
ListNode listPreviousNode(ListNode node);

/**
 * @brief Zwraca wartość w węźle.
 * #### Złożoność
 * O(1)
 * @param[in] node  - wskaźnik na węzeł.
 * @return Zwraca wartość przechowywaną przez węzeł @p node.
 */
LIST_ELEMENT_TYPE listNodeGetValue(ListNode node);

/**
 * @brief Kopiuje ostatnie @p k elementów do tablicy.
 * Do tablicy @p output zapisuje min(@p k, liczba_elementów_listy_list)
 * ostatnich elementów listy @p list.
 * @param[in] list      - wskaźnik na listę.
 * @param[in] k         - liczba elementów do pobrania.
 * @param[out] output    - tablica do umieszczenia wyniku.
 */
void listLastKElementsToArray(List list, size_t k,
                              LIST_ELEMENT_TYPE *output);

/**
 * @brief Kopiuje pierwsze @p k elementów do tablicy.
 * Do tablicy @p output zapisuje min(@p k, liczba_elementów_listy_list)
 * pierwszych elementów listy @p list.
 * @param[in] list      - wskaźnik na listę.
 * @param[in] k         - liczba elementów do pobrania.
 * @param[out] output    - tablica do umieszczenia wyniku.
 */
void listFirstKElementsToArray(List list, size_t k,
                               LIST_ELEMENT_TYPE *output);

/**
 * @brief Sprawdza rozmiar.
 * Do sprawdzania rozmiaru listy @p list.
 * @param[in] list      - wskaźnik na listę.
 * @param[in] maxSize   - ograniczenie.
 * @return W przypadku, gdy liczba elementów przechowywanych w @p list
 *         jest mniejsza niż @p maxSize to liczba tych elementów,
 *         w przeciwnym przypadku @p maxSize.
 */
size_t listSize(List list, size_t maxSize);

#endif //TELEFONY_LIST_H
