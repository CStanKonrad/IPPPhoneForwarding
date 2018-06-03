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
#include <stdbool.h>
#include "character.h"
#include "char_sequence.h"

/**
 * @see RadixTreeNode
 */
#define RADIX_TREE_NUMBER_OF_SONS CHARACTER_NUMBER_OF_DIGITS

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
struct RadixTreeNode;

/**
 * @brief Tworzy drzewo i inicjuje je.
 * #### Złożoność
 * O(1)
 * @return Wskaźnik na stworzone drzewo, w przypadku
 *         problemów z pamięcią NULL.
 */
RadixTree radixTreeCreate();

/**
 * @brief Sprawdza, czy @p node jest korzeniem drzewa.
 * Sprzawdza, czy @p node jest węzłem reprezentującym drzewo.
 * @param[in] node - wskaźnik na węzeł drzewa.
 * @return Niezerowa wartość w przypadku gdy @p node jest korzeniem,
 *         zero w przeciwnym wypadku.
 */
int radixTreeIsRoot(RadixTreeNode node);

/**
 * @param[in] node - wskaźnik na węzeł.
 * @return Liczba znaków na krawędzi wchodzącej do @p node.
 */
size_t radixTreeHowManyChars(RadixTreeNode node);

/**
 * @brief Wyszukuje węzeł reprezentujący @p txt.
 * @param[in] tree - wskaźnik na drzewo.
 * @param[in] txt - wskaźnik na numer.
 * @param[out] ptr - miejsce gdzie powinno powstać rozgałęzienie
 *       (miejsce w którym należy zmodyfikować drzewo w przypadku
 *       dodania @p txt).
 * @param[out] txtMatchPtr - ustawia na taką pozycję że cały tekst do
 *       *txtMatchPtr wyłącznie da się uzyskać przechodząc w dół (od ojca
 *       do syna) po krawędziach w drzewie @p tree,
 *       a próba dalszego dopasowania jest
 *       niemożliwa.
 * @param[out] nodeMatch - maksymalne dopsaowanie w ramach krawędzi. Zwraca
 *       długość numeru @p txt który został dopasowany do numeru na krawędzi
 *       wchodzącej do @p *ptr.
 * @param[out] nodeMatchMode - jeżeli w pełni dopasowano krawędź wchodzącą do
 *       @p *ptr to ustawiany jest na RADIX_TREE_NODE_MATCH_FULL,
 *       w przeciwnym przypadku na RADIX_TREE_NODE_MATCH_PARTIAL.
 * @return W przypadku gdy węzeł reprezentujący @p txt istnieje w drzewie
 *         RADIX_TREE_FOUND, w przypadku gdy @p txt jest podciągiem
 *         numeru reprezentowanego przez któryś z węzłów RADIX_TREE_SUBSTR,
 *         w przeciwnym wypadku RADIX_TREE_NOT_FOUND.
 */
int radixTreeFind(RadixTree tree, const char *txt, RadixTreeNode *ptr,
                  const char **txtMatchPtr, size_t *nodeMatch,
                  int *nodeMatchMode);

/**
 * @brief Okrojona wersja radixTreeFind.
 * @see radixTreeFind
 * @param[in] tree - wskaźnik na drzewo.
 * @param[in] txt - miejsce gdzie powinno powstać rozgałęzienie
 *       (miejsce w którym należy zmodyfikować drzewo w przypadku
 *       dodania @p txt).
 * @param[out] ptr - miejsce gdzie powinno powstać rozgałęzienie.
 * @return W przypadku gdy węzeł reprezentujący @p txt istnieje w drzewie
 *         RADIX_TREE_FOUND, w przypadku gdy @p txt jest podciągiem
 *         tekstu reprezentowanego przez któryż z węzłów RADIX_TREE_SUBSTR,
 *         w przeciwnym wypadku RADIX_TREE_NOT_FOUND.
 */
int radixTreeFindLite(RadixTree tree, const char *txt, RadixTreeNode *ptr);

/**
 * @brief Sprawia że w drzewie powstaje ścieżka reprezentująca numer @p txt.
 * @see radixGetFullText
 * @param[in, out] tree - wskaźnik na drzewo.
 * @param[in] txt - wskaźnik na tekst reprezentujący numer.
 * @return Wskaźnik do węzła dla którego wywołanie
 *         radixGetFullText zwróci @p txt,
 *         w przypadku problemów z przydzieleniem pamięci NULL.
 */
RadixTreeNode radixTreeInsert(RadixTree tree, const char *txt);

/**
 * @brief Nie robi nic.
 * Do usuwania drzewa bez usuwania danych przechowywanych przez węzły.
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
 * @param[in, out] subTreeNode - wskaźnik na węzeł drzewa.
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
 * @param[in, out] tree - wskaźnik na drzewo.
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
 * @param[in, out] node - wskaźnik na węzeł.
 * @param[in] ptr - wskaźnik na dane.
 */
void radixTreeSetData(RadixTreeNode node, void *ptr);

/**
 * @brief Poprzednik na ścieżce do korzenia.
 * @param[in] node - wskaźnik na węzeł.
 * @return Wskaźnik do ojca węzła @p node (poprzednika na ścieżce do korzenia,
 *         NULL w przypadku braku takiego węzła.
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
 * @remarks Wynik musi zostać zwolniony przy pomocy free.
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
 * @param[in, out] tree - wskaźnik na drzewo.
 * @param[in] f - wskaźnik na funkcję przetwarzającą.
 * @param[in,out] fData - wskaźnik na dane do funkcji @p f.
 */
void radixTreeFold(RadixTree tree, void (*f)(void *, void *), void *fData);

/**
 * @brief Funkcja licząca wynik dla  @ref phfwdNonTrivialCount
 * z wyjątkiem uwzględnionych przez @p phfwdNonTrivialCount
 * przypadków szczególnych.
 * @see phfwdNonTrivialCount
 * @param[in] tree - drzewo z informacjami pozwalającymi odwrócić przekierowanie.
 * @param[in] goalLen - szukana długość numeru.
 * @param[in] availableDigits - tablica z wartościami true na pozycjach
 *       odpowiadających dostępnym cyfrom (pozycja = kod_ascii_cyfry - '0').
 * @param[in] howManyDigitsAvailable - liczba różnych cyfr.
 * @return Liczba nietrywialnych numerów modulo 2^(liczba_bitów_size_t).
 */
size_t radixTreeNonTrivialCount(RadixTree tree, size_t goalLen,
                         const bool *availableDigits,
                         size_t howManyDigitsAvailable);

#endif //TELEFONY_RADIX_TREE_H
