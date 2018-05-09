/** @file
 * Interfejs modułu reprezentującego
 * skompresowane drzewo TRIE.
 * https://en.wikipedia.org/wiki/Radix_tree
 * @remarks Drzewo samo się nie balansuje.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 04.05.2018
 */

#ifndef TELEFONY_RADIX_TREE_H
#define TELEFONY_RADIX_TREE_H

#include <stddef.h>
#include "char_sequence.h"

/**
 * @see RadixTreeNode
 */
#define RADIX_TREE_NUMBER_OF_SONS 10

/**
 * @see radixTreeFind
 */
#define RADIX_TREE_NODE_MATCH_FULL 1

/**
 * @see radixTreeFind
 */
#define RADIX_TREE_NODE_MATCH_PARTIAL 0

/**
 * @see RadixTreeNode
 */
#define RADIX_TREE_ROOT_TXT "r"

/**
 * @see radixTreeFind
 */
#define RADIX_TREE_FOUND 1

/**
 * @see radixTreeFind
 */
#define RADIX_TREE_SUBSTR (-1)

/**
 * @see radixTreeFind
 */
#define RADIX_TREE_NOT_FOUND 0

/**
 * @brief Wskaźnik na drzewo - węzeł reprezentujący korzeń.
 * @see RadixTreeNode
 */
typedef struct RadixTreeNode *RadixTree;

/**
 * @brief Wskaźnik na węzeł drzewa.
 * @see RadixTreeNode
 */
typedef struct RadixTreeNode *RadixTreeNode;

/**
 * @brief Struktura reprezentująca węzeł drzewa.
 */
struct RadixTreeNode {
    /**
     * @brief Tekst przechowywane przez węzeł.
     * Tekst odpowiadający krawędzi wchodzącej do węzła.
     * Korzeń przechowuje wartość RADIX_TREE_ROOT_TXT.
     * @see RADIX_TREE_ROOT_TXT
     */
    CharSequence txt;

    /**
     * @brief Dane przechowywane przez węzeł.
     */
    void *data;

    /**
     * @brief Zmienna pomocnicza do przechodzenia drzewa bez użycia rekurencji.
     */
    size_t foldI;

    /**
     * @brief Synowie węzła w drzewie.
     * @see RADIX_TREE_NUMBER_OF_SONS
     */
    RadixTreeNode sons[RADIX_TREE_NUMBER_OF_SONS];

    /**
     * @brief Ojciec węzła w drzewie.
     */
    RadixTreeNode father;
};


/**
 * @brief Tworzy drzewo i inicjuje je.
 * #### Złożoność
 * O(1)
 * @return Wskaźnik na stworzone drzewo, w przypadku
 *         problemów z pamięcią NULL.
 */
RadixTree radixTreeCreate();


/**
 * @brief Sprawdza czy @p node jest korzeniem drzewa.
 * Sprzawdza czy @p node jest węzłem reprezentującym drzewo.
 * @param[in] node - wskaźnik na węzeł drzewa.
 * @return Niezerowa wartość w przypadku gdy @p node jest korzeniem,
 *         zero w przeciwnym wypadku.
 */
int radixTreeIsRoot(RadixTreeNode node);

/**
 * @param[in] node - wskaźnik na węzeł.
 * @return Ilość znaków na krawędzi wchodzącej do @p node.
 */
size_t radixTreeHowManyChars(RadixTreeNode node);

/**
 * @brief Wyszukuje węzeł reprezentujący @p txt.
 * @param[in] tree - wskaźnik na drzewo.
 * @param[in] txt - wskaźnik na tekst.
 * @param[out] ptr - miejsce gdzie powinno powstać rozgałęzienie.
 * @param[out] txtMatchPtr - ustawia na taką pozycję że cały tekst do
 *       *txtMatchPtr wyłącznie jest dopasowany w drzewie
 *       @p tree.
 * @param[out] nodeMatch - ilość znaków dopasowanych na krawędzi wchodzącej do
 *       @p *ptr.
 * @param[out] nodeMatchMode - jeżeli w pełni dopasowano krawędź wchodzącą do
 *       @p *ptr to ustawiany jest na RADIX_TREE_NODE_MATCH_FULL,
 *       w przeciwnym przypadku na RADIX_TREE_NODE_MATCH_PARTIAL.
 * @return W przypadku gdy węzeł reprezentujący @p txt istnieje w drzewie
 *         RADIX_TREE_FOUND, w przypadku gdy @p txt jest podciągiem
 *         tekstu reprezentowanego przez któryś z węzłów RADIX_TREE_SUBSTR,
 *         w przeciwnym wypadku RADIX_TREE_NOT_FOUND.
 */
int radixTreeFind(RadixTree tree, const char *txt, RadixTreeNode *ptr,
                  const char **txtMatchPtr, size_t *nodeMatch,
                  int *nodeMatchMode);

/**
 * @brief Okrojona wersja radixTreeFind.
 * @see radixTreeFind
 * @param[in] tree - wskaźnik na drzewo.
 * @param[in] txt - wskaźnik na tekst.
 * @param[out] ptr - miejsce gdzie powinno powstać rozgałęzienie.
 * @return W przypadku gdy węzeł reprezentujący @p txt istnieje w drzewie
 *         RADIX_TREE_FOUND, w przypadku gdy @p txt jest podciągiem
 *         tekstu reprezentowanego przez któryż z węzłów RADIX_TREE_SUBSTR,
 *         w przeciwnym wypadku RADIX_TREE_NOT_FOUND.
 */
int radixTreeFindLite(RadixTree tree, const char *txt, RadixTreeNode *ptr);

/**
 * @brief Sprawia że w drzewie powstaje ścieżka reprezentująca tekst @p txt.
 * @remarks Nie balansuje drzewa
 * @see radixTreeBalance
 * @see radixGetFullText
 * @param[in] tree - wskaźnik na drzewo.
 * @param[in] txt - wskaźnik na tekst.
 * @return Wskaźnik do węzła dla którego wywołanie
 *         radixGetFullText zwróci @p txt,
 *         w przypadku problemów z przydzieleniem pamięci NULL.
 */
RadixTreeNode radixTreeInsert(RadixTree tree, const char *txt);

/**
 * @brief Nie robi nic.
 * Do usuwanie drzewa bez usuwania danych przechowywanych przez węzły.
 * @see radixTreeDeleteSubTree
 * @see radixTreeDelete
 * @param ptrA - nieużywany wskaźnik.
 * @param ptrB - nieużywany wskaźnik.
 */
void radixTreeEmptyDelFunction(void *ptrA, void *ptrB);

/**
 * @brief Zlicza liczbę węzłów z przypisanymi danymi
 * @see radixTreeFold
 * @param[in] ptrA - wskaźnik na dane
 * @param[out] ptrB - wskaźnik na licznik (typu size_t)
 */
void radixTreeCountDataFunction(void *ptrA, void *ptrB);

/**
 * @brief Usuwa poddrzewo.
 * Usuwa poddrzewo reprezentowane przez @p subTreeNode
 * wywołując dla węzłów z przypisanymi danymi
 * f(wskaźnik_na_dane_przechowywane_przez_węzeł, fData).
 * @param[in] subTreeNode - wskaźnik na węzeł drzewa.
 * @param[in] f - wskaźnik na funkcję czyszczącą.
 * @param fData - dane pomocnicze do funkcji czyszczącej.
 */
void radixTreeDeleteSubTree(RadixTreeNode subTreeNode,
                            void (*f)(void *, void *),
                            void *fData);

/**
 * @brief Usuwa drzewo.
 * Usuwa @p tree wywołując dla węzłów z przypisanymi danymi
 * f(wskaźnik_na_dane_przechowywane_przez_węzeł, fData).
 * @param[in] tree - wskaźnik na drzewo.
 * @param[in] f - wskaźnik na funkcję czyszczącą.
 * @param fData - dane pomocnicze do funkcji czyszczącej.
 */
void radixTreeDelete(RadixTree tree, void (*f)(void *, void *), void *fData);

/**
 * @brief Pobiera dane z węzła.
 * @see radixTreeSetData
 * @param[in] node - wskaźnik na węzeł drzewa.
 * @return Wskaźnik na dane przypisane do węzła @p node.
 */
void *radixTreeGetNodeData(RadixTreeNode node);

/**
 * @brief Przypisuje dane do węzła
 * Sprawia że węzeł @p node posiada wskaźnik na dane wskazywane przez
 * @p ptr.
 * @param[in] node - wskaźnik na węzeł.
 * @param[in] ptr - wskaźnik na dane.
 */
void radixTreeSetData(RadixTreeNode node, void *ptr);

/**
 * @brief Poprzednik na ścieżce do korzenia.
 * @param[in] node - wskaźnik na węzeł.
 * @return Wskaźnik do ojca węzła @p node (poprzednika na ścieżce do korzenia,
 *         NULL w przypadku braku.
 */
RadixTreeNode radixTreeFather(RadixTreeNode node);

/**
 * @brief Optymalizuje pamięć zajmowaną przez drzewo.
 * Część węzłów na ścieżce od @p node do korzenia nie przechowująca danych
 * zostaje w miarę możliwości usunięta lub scalona.
 * @param[in] node - wskaźnik na węzeł.
 */
void radixTreeBalance(RadixTreeNode node);

/**
 * @brief Tekst reprezentujący węzeł.
 * @remarks Musi zostać zwolniony przy pomocy free.
 * @param[in] node - wskaźnik na węzeł drzewa.
 * @return Wskaźnik na tekst reprezentujący ścieżkę od korzenia do węzła,
 *         w przypadku problemów z przydzieleniem pamięci zwraca NULL.
 */
char *radixGetFullText(RadixTreeNode node);


/**
 * @brief Przetwarza drzewo.
 * Przechodzi po węzłach drzewa @p tree w porządku leksykograficznym
 * względem przechowywanego tekstu i na każdym węźle przechowującym jakieś dane
 * wywołuje f(dane_węzła, fData).
 * @param[in] tree - wskaźnik na drzewo.
 * @param[in] f - wskaźnik na funkcję przetwarzającą.
 * @param[in,out] fData - wskaźnik na dane do funkcji @p f.
 */
void radixTreeFold(RadixTree tree, void (*f)(void *, void *), void *fData);

#endif //TELEFONY_RADIX_TREE_H
