/** @file
 * Implementacja struktury reprezentującej
 * skompresowane drzewo TRIE
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 04.05.2018
 */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "radix_tree.h"
#include "text.h"

/**
 * @brief Kod operacji zakończonej sukcesem.
 */
#define RADIX_TREE_OPERATION_SUCCESS 1

/**
 * @brief Kod operacji zakończonej błędem.
 */
#define RADIX_TREE_OPERATION_FAIL 0

int radixTreeIsRoot(RadixTreeNode node) {
    return node->txt != NULL
           && charSequenceEqualToString(node->txt, RADIX_TREE_ROOT_TXT);
}

/**
 * @brief Inicjuje węzeł drzewa.
 * #### Złożoność
 * O(1)
 * @param[in] node  - wskaźnik na węzeł.
 */
static void radixTreeInitNode(RadixTreeNode node) {
    node->data = NULL;
    node->txt = NULL;

    node->father = NULL;

    size_t i;
    for (i = 0; i < RADIX_TREE_NUMBER_OF_SONS; i++) {
        node->sons[i] = NULL;
    }

}

/**
 * @brief Zwalnia węzeł.
 * #### Złożoność
 * O(1)
 * @remarks Zakłada, że do węzła nie są przypisane dane.
 * @param[in] node - wskaźnik na węzeł drzewa.
 */
static void radixTreeFreeNode(RadixTreeNode node) {
    assert(node->data == NULL);
    if (node->txt != NULL) {
        charSequenceDelete(node->txt);
        node->txt = NULL;
    }
    free(node);
}


/**
 * @brief Inicjuje nowo powstałe drzewo.
 * #### Złożoność
 * O(1)
 * @param[in] tree  - wskaźnik na drzewo.
 * @return RADIX_TREE_OPERATION_FAIL w przypadku problemów,
 *         w przeciwnym przypadku RADIX_TREE_OPERATION_SUCCESS.
 */
static int radixTreeInitTree(RadixTree tree) {
    radixTreeInitNode(tree);
    tree->txt = charSequenceFromCString(RADIX_TREE_ROOT_TXT);

    if (tree->txt == NULL) {
        return RADIX_TREE_OPERATION_FAIL;
    } else {
        return RADIX_TREE_OPERATION_SUCCESS;
    }
}


/**
 * @brief Tworzy węzeł drzewa i inicjuje go.
 * #### Złożoność
 * O(1)
 * @return Wskaźnik na stworzony węzeł, w przypadku
 *         problemów z pamięcią NULL.
 */
static RadixTreeNode radixTreeCreateNode() {
    RadixTreeNode result = malloc(sizeof(struct RadixTreeNode));
    if (result == NULL) {
        return NULL;
    } else {
        radixTreeInitNode(result);
        return result;
    }
}


RadixTree radixTreeCreate() {
    RadixTree result = malloc(sizeof(struct RadixTreeNode));
    if (result == NULL) {
        return NULL;
    } else {
        if (radixTreeInitTree(result) != RADIX_TREE_OPERATION_SUCCESS) {
            free(result);
            return NULL;
        } else {
            return result;
        }
    }
}

/**
 * @brief Konwertuje znak na numer syna.
 * @param[in] sonCh - pierwsza litera na krawędzi do syna.
 * @return Numer syna.
 */
static size_t radixTreeConvertCharToNumber(char sonCh) {
    assert(sonCh <= '9' && sonCh >= '0');
    return (size_t) sonCh - (size_t) '0';
}

/**
 * @brief Sprawdza czy węzeł @p node ma syna o numerze @p son.
 * @param[in] node - wskaźnik na węzeł.
 * @param[in] son - pierwsza litera na krawędzi do syna.
 * @return Niezerowa wartość jeżeli ma, zerowa w przeciwnym wypadku.
 */
static int radixTreeHasSon(RadixTreeNode node, char son) {
    return node->sons[radixTreeConvertCharToNumber(son)] != NULL;
}

/**
 * @brief Liczba synów węzła @p node.
 * @param[in] node - wskaźnik na węzeł.
 * @return Liczba synów węzła @p node.
 */
static size_t radixTreeHowManySons(RadixTreeNode node) {
    size_t result = 0;
    size_t i;
    for (i = 0; i < RADIX_TREE_NUMBER_OF_SONS; i++) {
        if (node->sons[i] != NULL) {
            result++;
        }
    }
    return result;
}

/**
 * @brief Czy węzeł @p node ma synów.
 * @param[in] node - wskaźnik na węzeł.
 * @return Niezerowa wartość jeżeli ma, zerowa w przeciwnym wypadku.
 */
static int radixTreeHasSons(RadixTreeNode node) {
    return radixTreeHowManySons(node) != 0;
}

/**
 * @brief Czy @p node może zostać usunięty.
 * @param[in] node - wskaźnik na węzeł.
 * @return Niezerowa wartość jeżeli może, zerowa w przeciwnym wypadku.
 */
static int radixTreeIsNodeRedundant(RadixTreeNode node) {
    return (!radixTreeIsRoot(node))
           && (!radixTreeHasSons(node))
           && node->data == NULL;
}

/**
 * @brief Czy @p node może zostać scalony z synem.
 * @param[in] node - wskaźnik na węzeł.
 * @return Niezerowa wartość jeżeli może, zerowa w przeciwnym wypadku.
 */
static int radixTreeCanBeMergedWithSon(RadixTreeNode node) {
    return (!radixTreeIsRoot(node))
           && radixTreeHowManySons(node) == 1
           && node->data == NULL;
}

/**
 * @brief Przesuwa do @p son.
 * Sprawia że wskaźnik @p *ptr wskazuje na syna węzła @p *ptr do którego
 * prowadzi krawędź z pierwszą literą @p son.
 * @param[in,out] ptr - wskaźnik na wskaźnik na węzeł.
 * @param[in] son - znak odpowiadający synowi.
 */
static void radixTreeMoveToSon(RadixTreeNode *ptr, char son) {
    assert(radixTreeHasSon(*ptr, son));
    *ptr = (*ptr)->sons[radixTreeConvertCharToNumber(son)];
}

/**
 * @brief Ustawia syna.
 * Ustawia wierzchołkowi @p node syna (pod krawędzią zaczynającą się literą
 * @p son na @p ch.
 * @param[in] node - wskaźnik na węzeł.
 * @param[in] son - litera odpowiadająca synowi.
 * @param[in] ch - wskaźnik na przyszłego syna węzła @p node.
 */
static void radixTreeChangeSon(RadixTreeNode node, char son,
                               RadixTreeNode ch) {
    if (node != NULL) {
        node->sons[radixTreeConvertCharToNumber(son)] = ch;
    }
}

/**
 * @brief Przesuwa dopasowanie w ramach węzła.
 * Po wykonaniu się procedury wartość wkaźnika @p *txt oznacza, że
 * wszystkie znaki występujące przed @p **txt występują w drzewie
 * jako podciąg pewnej drogi od korzenia do liścia.
 * Wartość wskaźnika @p nodeTxtPtr oznacza, że wszystkie znaki występujące
 * na krawędzi wchodzącej do @p node wyłącznie są sufiksem dopasowania
 * @p *txt.
 * @param[in] node - wskaźnik na węzeł drzewa.
 * @param[in,out] txt - wskaźnik na wskaźnik na dopasowanie numeru.
 *        @p *txt
 *        po próbie dopasowania wskazuje
 *        na element za ostatnim pasującym, w przypadku pełnego
 *        dopasowania na '\0'.
 * @param[out] nodeTxtPtr - wskaźnik na iterator dopasowania
 *        w ramach węzła @p node (krawędzi do niego wchodzącej).
 *        @p *nodeTxtPtr
 *        po próbie dopasowania wskazuje
 *        na element za ostatnim pasującym, w przypadku pełnego
 *        dopasowania na węzeł odpowiadający znakowi '\0'.
 * @return RADIX_TREE_OPERATION_SUCCESS w przypadku pełnego dopasowania,
 *         RADIX_TREE_OPERATION_FAIL w przeciwnym przypadku.
 */
static int radixTreeMoveTxt(RadixTreeNode node, const char **txt,
                            CharSequenceIterator *nodeTxtPtr) {
    CharSequenceIterator i = charSequenceGetIterator(node->txt);

    if (radixTreeIsRoot(node)) {
        i = charSequenceSequenceEnd(node->txt);
    } else {
        while (charSequenceGetChar(&i) != '\0'
               && *(*txt) != '\0'
               && charSequenceGetChar(&i) == *(*txt)) {
            charSequenceNextChar(&i, NULL);
            (*txt)++;
        }
    }

    *nodeTxtPtr = i;
    if (charSequenceGetChar(&i) == '\0') {
        return RADIX_TREE_OPERATION_SUCCESS;
    } else {
        return RADIX_TREE_OPERATION_FAIL;
    }
}

/**
 * @brief Próbuje dopasować kolejny węzeł (krawędź do niego wchodzącą).
 * @see radixTreeMoveTxt
 * @param[in,out] node - wskaźnik na wskaźnik na wierzchołek.
 *        Po udanym przesunięciu @p *node wskazuje na następny wierzchołek, ew
 *        na węzeł dla którego krawędź udało się częściowo dopasować.
 * @param[in,out] txt - wskaźnik na dopasowywany tekst.
 *        Po próbie @p *txt dopasowania wskazuje
 *        na element za ostatnim pasującym, w przypadku pełnego
 *        dopasowania na '\0'.
 * @param[out] nodeTxtPtr - podobnie do @p txt tylko dotyczy dopasowania
 *        w ramach węzła.
 * @return RADIX_TREE_OPERATION_FAIL w przypadku niemożności dalszego
 *         dopasowania, RADIX_TREE_OPERATION_SUCCESS w przeciwnym przypadku.
 */
static int radixTreeMove(RadixTreeNode *node, const char **txt,
                         CharSequenceIterator *nodeTxtPtr) {
    assert(*(*txt) != '\0');
    if (!radixTreeHasSon(*node, *(*txt))) {
        return RADIX_TREE_OPERATION_FAIL;
    } else {
        radixTreeMoveToSon(node, *(*txt));
        return radixTreeMoveTxt(*node, txt, nodeTxtPtr);
    }

}

/**
 * @brief Wyszukuje węzeł reprezentujący @p txt.
 * @param[in] tree - wskaźnik na drzewo.
 * @param[in] txt - wskaźnik na tekst.
 * @param[out] ptr - miejsce gdzie powinno powstać rozgałęzienie.
 * @param[out] txtMatchPtr - ustawia na taką pozycję że cały tekst do
 *       @p *txtMatchPtr wyłącznie jest dopasowany w drzewie
 *       @p tree (istnieje droga od korzenia do liścia idąca w dół,
 *       taka, że @p txt jest jej podciągiem).
 * @param[out] nodeMatchPtr - wskaźnik iterator dopasowania krawędzi wchodzącej
 *       do
 *       @p *ptr.
 * @return W przypadku gdy węzeł reprezentujący @p txt istnieje w drzewie
 *         RADIX_TREE_FOUND, w przypadku gdy @p txt jest podciągiem
 *         tekstu reprezentowanego przez któryś z węzłów RADIX_TREE_SUBSTR,
 *         w przeciwnym wypadku RADIX_TREE_NOT_FOUND.
 */
static int radixTreeFindEx(RadixTree tree,
                           const char *txt,
                           RadixTreeNode *ptr,
                           const char **txtMatchPtr,
                           CharSequenceIterator *nodeMatchPtr) {
    *ptr = tree;
    *txtMatchPtr = txt;
    *nodeMatchPtr = charSequenceSequenceEnd((*ptr)->txt);

    while (*(*txtMatchPtr) != '\0'
           && radixTreeMove(ptr, txtMatchPtr, nodeMatchPtr)
              == RADIX_TREE_OPERATION_SUCCESS);

    if (charSequenceGetChar((nodeMatchPtr)) == '\0'
        && *(*txtMatchPtr) == '\0') {
        return RADIX_TREE_FOUND;
    } else if (*(*txtMatchPtr) == '\0') {
        return RADIX_TREE_SUBSTR;
    } else {
        return RADIX_TREE_NOT_FOUND;
    }
}

/**
 * @see radixTreeFind
 * @param[in] node - wskaźnik na węzeł drzewa.
 * @param[in] txt - wskaźnik na tekst *nodeMatchPtr z @ref radixTreeFindEx.
 * @return Długość dopasowania krawędzi wchodzącej do węzła @p node.
 */
static size_t radixTreeHowManyCharsOffset(RadixTreeNode node,
                                          CharSequenceIterator *txt) {
    CharSequenceIterator ptr = charSequenceGetIterator(node->txt);
    size_t result = 0;
    while (!charSequenceIteratorsEqual(&ptr, txt)) {
        charSequenceNextChar(&ptr, NULL);
        result++;
    }
    return result;
}

int radixTreeFind(RadixTree tree, const char *txt, RadixTreeNode *ptr,
                  const char **txtMatchPtr, size_t *nodeMatch,
                  int *nodeMatchMode) {
    CharSequenceIterator nodeMatchPtr;
    int result = radixTreeFindEx(tree, txt, ptr, txtMatchPtr, &nodeMatchPtr);

    if (charSequenceGetChar(&nodeMatchPtr) == '\0') {
        *nodeMatch = charSequenceLength((*ptr)->txt);
        *nodeMatchMode = RADIX_TREE_NODE_MATCH_FULL;
    } else {
        *nodeMatch = radixTreeHowManyCharsOffset(*ptr, &nodeMatchPtr);
        *nodeMatchMode = RADIX_TREE_NODE_MATCH_PARTIAL;
    }
    return result;
}


size_t radixTreeHowManyChars(RadixTreeNode node) {
    return charSequenceLength(node->txt);
}

/**
 * @brief Rozdziela węzeł na dwa.
 * Rozdziela węzeł @p node na dwa tnąc krawędź do niego wchodzącą w punkcie
 * @p splitPtr.
 * @param[in] node - wskaźnik na węzeł.
 * @param[in] splitPtr - wskaźnik na iterator wskazujący na
 *        miejsce rozcinające w tekście krawędzi
 *        (przechowywane w węźle).
 * @return RADIX_TREE_OPERATION_SUCCESS w przypadku udanego rozcięcia,
 *         RADIX_TREE_OPERATION_FAIL w przeciwnym przypadku.
 */
static int radixTreeSplitNode(RadixTreeNode node, CharSequenceIterator *splitPtr) {
    RadixTreeNode newNode = radixTreeCreateNode();

    if (newNode == NULL) {
        return RADIX_TREE_OPERATION_FAIL;
    } else {
//todo
        newNode->txt = node->txt;

        CharSequence ptr = charSequenceSplitByIterator(node->txt, splitPtr);

        node->txt = ptr;

        newNode->father = node->father;
        CharSequenceIterator it = charSequenceGetIterator(newNode->txt);
        radixTreeChangeSon(node->father, charSequenceGetChar(&it),
                           newNode);

        node->father = newNode;
        it = charSequenceGetIterator(node->txt);
        radixTreeChangeSon(newNode, charSequenceGetChar(&it), node);
        return RADIX_TREE_OPERATION_SUCCESS;

    }
}

/**
 * @brief Dodaje węzłowi @p node pustego syna.
 * @param[in] node - wskaźnik na węzeł.
 * @param[in] txt - wskaźnik na tekst krawędzi do syna.
 * @return Wskaźnik na dodany węzeł, w przypadku problemów z
 *         przydzieleniem pamięci NULL.
 */
static RadixTreeNode radixTreeInsertLeaf(RadixTreeNode node, const char *txt) {
    CharSequence textToInsert = charSequenceFromCString(txt);
    if (textToInsert == NULL) {
        return NULL;
    } else {
        RadixTreeNode newNode = radixTreeCreateNode();
        if (newNode == NULL) {
            charSequenceDelete(textToInsert);
            return NULL;
        } else {
            newNode->txt = textToInsert;

            newNode->father = node;
            CharSequenceIterator it = charSequenceGetIterator(newNode->txt);
            assert(!radixTreeHasSon(node, charSequenceGetChar(&it)));
            radixTreeChangeSon(node, charSequenceGetChar(&it),
                               newNode);

            return newNode;
        }
    }
}

RadixTreeNode radixTreeInsert(RadixTree tree, const char *txt) {
    RadixTreeNode insertPtr;
    const char *matchPtr;
    CharSequenceIterator nodeMatchPtr;
    int findResult = radixTreeFindEx(tree, txt, &insertPtr,
                                     &matchPtr, &nodeMatchPtr);

    if (findResult == RADIX_TREE_FOUND) {
        return insertPtr;
    } else if (findResult == RADIX_TREE_SUBSTR) {
        int splitResult = radixTreeSplitNode(insertPtr, &nodeMatchPtr);
        if (splitResult == RADIX_TREE_OPERATION_SUCCESS) {
            return insertPtr->father;
        } else {
            return NULL;
        }
    } else if (findResult == RADIX_TREE_NOT_FOUND) {
        if (charSequenceGetChar(&nodeMatchPtr) != '\0') {
            int splitResult = radixTreeSplitNode(insertPtr, &nodeMatchPtr);
            if (splitResult == RADIX_TREE_OPERATION_SUCCESS) {
                return radixTreeInsert(tree, txt);
            } else {
                return NULL;
            }
        } else {
            return radixTreeInsertLeaf(insertPtr, matchPtr);
        }
    } else {
        return NULL;
    }
}

void radixTreeDeleteSubTree(RadixTreeNode subTreeNode,
                            void (*f)(void *, void *),
                            void *fData) {
    RadixTreeNode pos = subTreeNode, tmp;
    pos->foldI = 0;

    while (!(pos == subTreeNode
             && pos->foldI == RADIX_TREE_NUMBER_OF_SONS)) {
        size_t *i = &pos->foldI;
        if (*i == RADIX_TREE_NUMBER_OF_SONS) {
            if (pos->data != NULL) {
                f(pos->data, fData);
                pos->data = NULL;
            }
            tmp = pos;
            pos = pos->father;
            CharSequenceIterator it = charSequenceGetIterator(tmp->txt);
            radixTreeChangeSon(pos, charSequenceGetChar(&it), NULL);
            radixTreeFreeNode(tmp);

        } else {
            if (pos->sons[*i] != NULL) {
                pos = pos->sons[*i];
                pos->foldI = 0;
            }
            (*i)++;
        }
    }

    if (subTreeNode->data != NULL) {
        f(subTreeNode->data, fData);
        subTreeNode->data = NULL;
    }
    if (!radixTreeIsRoot(subTreeNode)) {
        CharSequenceIterator it = charSequenceGetIterator(subTreeNode->txt);
        radixTreeChangeSon(subTreeNode->father,
                           charSequenceGetChar(&it), NULL);
    }
    radixTreeFreeNode(subTreeNode);
}

void radixTreeDelete(RadixTree tree, void (*f)(void *, void *), void *fData) {
    radixTreeDeleteSubTree(tree, f, fData);
}

void radixTreeEmptyDelFunction(void *ptrA, void *ptrB) {
    (void) ptrA;
    (void) ptrB;
}

void *radixTreeGetNodeData(RadixTreeNode node) {
    return node->data;
}

RadixTreeNode radixTreeFather(RadixTreeNode node) {
    return node->father;
}

/**
 * @brief Zwraca wskaźnik na pierwszego syna węzła @p node.
 * Zwraca wskaźnik na syna węzła @p node o najmniejszym numerze.
 * @param[in] node - wskaźnik na węzeł.
 * @return Wskaźnik na pierwszego syna węzła @p node.
 */
static RadixTreeNode radixTreeFirstSon(RadixTreeNode node) {
    size_t i;
    for (i = 0; i < RADIX_TREE_NUMBER_OF_SONS; i++) {
        if (node->sons[i] != NULL) {
            return node->sons[i];
        }
    }
    return NULL;
}

/**
 * @brief Próbuje scalić węzeł @p a z węzłem @p b.
 * @remarks Zakłada że węzeł @p a spełnia
 *          predykat radixTreeCanBeMergedWithSon
 * @see radixTreeCanBeMergedWithSon
 * @remarks Adres węzła ba nie ulega zmianie.
 * @param[in] a - wskaźnik na węzeł (zostanie usunięty po scaleniu).
 * @param[in] b - wskaźnik na węzeł (zostanie tym scalonym).
 * @return  W przypadku problemów RADIX_TREE_OPERATION_FAIL,
 *          w przeciwnym przypadku RADIX_TREE_OPERATION_SUCCESS.
 */
static int radixTreeMerge(RadixTreeNode a, RadixTreeNode b) {
    assert(charSequenceLength(b->txt) != 0);
    charSequenceMerge(a->txt, b->txt);
    b->txt = a->txt;
    a->txt = NULL;

    assert(charSequenceLength(b->txt) != 0);

    b->father = a->father;
    CharSequenceIterator it = charSequenceGetIterator(b->txt);
    radixTreeChangeSon(a->father, charSequenceGetChar(&it), b);
    radixTreeFreeNode(a);

    return RADIX_TREE_OPERATION_SUCCESS;

}

void radixTreeBalance(RadixTreeNode node) {
    RadixTreeNode pos = node, tmp;
    size_t skipped = 0;
    const size_t canSkip = 5;

    while (!radixTreeIsRoot(pos)
           && skipped <= canSkip) {
        if (radixTreeIsNodeRedundant(pos)) {
            tmp = pos;
            pos = pos->father;
            CharSequenceIterator it = charSequenceGetIterator(tmp->txt);
            radixTreeChangeSon(pos, charSequenceGetChar(&it), NULL);
            radixTreeFreeNode(tmp);
        } else if (radixTreeCanBeMergedWithSon(pos)) {
            tmp = pos;
            pos = pos->father;
            int mergeResult = radixTreeMerge(tmp, radixTreeFirstSon(tmp));
            if (mergeResult != RADIX_TREE_OPERATION_SUCCESS) {
                skipped++;
            }
        } else {
            pos = pos->father;
            skipped++;
        }
    }

}

void radixTreeSetData(RadixTreeNode node, void *ptr) {
    node->data = ptr;
}

int radixTreeFindLite(RadixTree tree, const char *txt, RadixTreeNode *ptr) {
    const char *unused1;
    size_t unused2;
    int unused3;
    return radixTreeFind(tree, txt, ptr, &unused1, &unused2, &unused3);
}

char *radixGetFullText(RadixTreeNode node) {
    RadixTreeNode pos = node;
    size_t length = 0;

    while (!radixTreeIsRoot(pos)) {
        length += radixTreeHowManyChars(pos);
        pos = radixTreeFather(pos);
    }

    char *result = malloc(length + (size_t) 1);
    if (result == NULL) {
        return NULL;
    } else {
        result[length] = '\0';
        pos = node;
        while (!radixTreeIsRoot(pos)) {
            size_t len = charSequenceLength(pos->txt);
            size_t i;
            length -= len;
            CharSequenceIterator ptr = charSequenceGetIterator(pos->txt);
            char out;
            for (i = 0; i < len; i++) {
                charSequenceNextChar(&ptr, &out);
                result[length + i] = out;
            }
            pos = radixTreeFather(pos);
        }
        return result;
    }

}

void radixTreeFold(RadixTree tree, void (*f)(void *, void *), void *fData) {

    RadixTreeNode pos = tree;
    pos->foldI = 0;

    while (!(pos == tree
             && pos->foldI == RADIX_TREE_NUMBER_OF_SONS)) {
        size_t *i = &pos->foldI;
        if (*i == 0) {
            if (pos->data != NULL) {
                f(pos->data, fData);
            }
        }

        if (*i == RADIX_TREE_NUMBER_OF_SONS) {
            pos = pos->father;

        } else {
            if (pos->sons[*i] != NULL) {
                pos = pos->sons[*i];
                pos->foldI = 0;
            }
            (*i)++;
        }
    }
}

void radixTreeCountDataFunction(void *ptrA, void *ptrB) {
    size_t *counter = (size_t *) ptrB;
    if (ptrA != NULL) {
        (*counter)++;
    }
}
