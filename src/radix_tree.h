/** @file
 * Struktura reprezentująca
 * skompresowane drzewo TRIE
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 04.05.2018
 */

#ifndef TELEFONY_RADIX_TREE_H
#define TELEFONY_RADIX_TREE_H

#define RADIX_TREE_NUMBER_OF_SONS 10
#define RADIX_TREE_ROOT_TXT "r"
#define RADIX_TREE_OPERATION_SUCCESS 1
#define RADIX_TREE_OPERATION_FAIL 0

#define RADIX_TREE_FOUND 1
#define RADIX_TREE_SUBSTR (-1)
#define RADIX_TREE_NOT_FOUND 0

typedef struct RadixTreeNode *RadixTree;
typedef struct RadixTreeNode *RadixTreeNode;

struct RadixTreeNode {
    const char *txt;
    void *data;
    size_t foldI;
    RadixTreeNode sons[RADIX_TREE_NUMBER_OF_SONS];
    RadixTreeNode father;
};


/**
 * @brief Tworzy nowe drzewo.
 * #### Złożoność
 * O(1)
 * @return Wskaźnik do nowo stworzonego drzewa lub NULL,
 *         w przypadku problemów z przydzieleniem pamięci.
 */
RadixTree radixTreeCreate();

int radixTreeFind(RadixTree tree, const char *txt, RadixTreeNode *ptr,
                  const char **txtMatchPtr, const char **nodeMatchPtr);

RadixTreeNode radixTreeInsert(RadixTree tree, const char *txt);

void radixTreeEmptyFunction(void *ptrA, void *ptrB);

void radixTreeDeleteSubTree(RadixTreeNode subTreeNode,
                            void (*f)(void *, void *),
                            void *fData);

void radixTreeDelete(RadixTree tree, void (*f)(void *, void *), void *fData);

void *radixTreeGetNodeData(RadixTreeNode node);

RadixTreeNode radixTreeFather(RadixTreeNode node);

void radixTreeBalance(RadixTreeNode node);


#endif //TELEFONY_RADIX_TREE_H
