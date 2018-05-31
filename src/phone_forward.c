/** @file
 * Implementacja klasy przechowującej przekierowania numerów telefonicznych
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 06.05.2018
 */

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "phone_forward.h"
#include "radix_tree.h"
#include "list.h"
#include "text.h"
#include "character.h"

/**
 * @brief Struktura przechowująca przekierowania numerów telefonów.
 */
struct PhoneForward {
    /**
     * @brief Drzewo reprezentujące przekierowania.
     * Jego węzły przechowują informacje
     * o tym na jaki numer zostały przekierowane (ForwardData->treeNode)
     * oraz o węźle na liście przechowującym informację pozwalającą odwrócić
     * przekierowanie (ForwardData->listNode).
     * Sam węzeł drzewa reprezentuje numer.
     * @see ForwardData
     */
    RadixTree forward;

    /**
     * @brief Drzewo reprezentujące odwrócone przekierowania.
     * Pozwala na odtworzenie numerów przekierowanych na dany numer.
     * Jego wierzchołki przechowują listy wskaźników na wierzchołki
     * drzewa forward przekierowywane na
     * dany wierzchołek.
     * Sam węzeł reprezentuje numer.
     */
    RadixTree backward;
};

/**
 * @brief Struktura przechowująca ciąg numerów telefonów.
 */
struct PhoneNumbers {
    /**
     * @brief Tablica wskaźników na numery.
     */
    char **numbers;

    /**
     * @brief Liczba numerów.
     */
    size_t howMany;
};

/**
 * @brief Tworzy strukturę do przechowywania numerów.
 * @param[in] howMany - ilość przechowywanych numerów.
 * @return Wskaźnik na strukturę do przechowywania @p howMany numerów,
 *         NULL w przypadku problemów z pamięcią.
 */
static struct PhoneNumbers *phfwdCreatePhoneNumbersStructure(size_t howMany) {
    struct PhoneNumbers *result = malloc(sizeof(struct PhoneNumbers));
    if (result == NULL) {
        return NULL;
    } else {
        result->howMany = howMany;
        result->numbers = malloc(result->howMany * sizeof(char *));
        if (result->numbers == NULL) {
            free(result);
            return NULL;
        } else {
            size_t i;
            for (i = 0; i < result->howMany; i++) {
                result->numbers[i] = NULL;
            }
            return result;
        }
    }
}

/**
 * @brief Tworzy strukturę przechowującą zero numerów.
 * @see phfwdCreatePhoneNumbersStructure.
 * @return Wskaźnik na strukturę przechowującą zero numerów,
 *         NULL w przypadku problemów z pamięcią.
 */
static struct PhoneNumbers *phfwdEmptySequenceResult() {
    struct PhoneNumbers *result = phfwdCreatePhoneNumbersStructure(0);
    return result;
}


struct PhoneForward *phfwdNew(void) {
    struct PhoneForward *result = malloc(sizeof(struct PhoneForward));
    if (result == NULL) {
        return NULL;
    } else {
        result->forward = radixTreeCreate();
        if (result->forward == NULL) {
            free(result);
            return NULL;
        } else {
            result->backward = radixTreeCreate();
            if (result->backward == NULL) {
                radixTreeDelete(result->forward, radixTreeEmptyDelFunction,
                                NULL);
                free(result);
                return NULL;
            } else {
                return result;
            }
        }
    }
}

/**
 * @brief Do usuwania drzewa PhoneForward->forward.
 * @see PhoneForward
 * @see radixTreeDelete
 * @param[in] ptrA - wskaźnik na dane do usunięcia.
 * @param ptrB - nieużywany wskaźnik (powinien wskazywać na NULL).
 */
static void phfwdForwardJustDelete(void *ptrA, void *ptrB) {
    assert(ptrA != NULL);
    assert(ptrB == NULL);
    (void) ptrA;
    (void) ptrB;
    free(ptrA);
}

/**
 * @brief Do usuwania drzewa PhoneForward->backward.
 * @see PhoneForward
 * @see radixTreeDelete
 * @param[in] ptrA - wskaźnik na dane (listę) do usunięcia.
 * @param ptrB - nieużywany wskaźnik (powinien wskazywać na NULL).
 */
static void phfwdBackwardJustDelete(void *ptrA, void *ptrB) {
    assert(ptrA != NULL);
    assert(ptrB == NULL);
    (void) ptrA;
    (void) ptrB;
    listDestroy(ptrA);
}

void phfwdDelete(struct PhoneForward *pf) {
    if (pf == NULL) {
        return;
    } else {
        radixTreeDelete(pf->forward, phfwdForwardJustDelete, NULL);
        radixTreeDelete(pf->backward, phfwdBackwardJustDelete, NULL);
        free(pf);
    }
}

/**
 * @brief Przygotowuje drzewa struktury @p pf do dodania danych.
 * Dodaje do drzew struktury @p pf węzły reprezentujące numery
 * @p num1 i @p num2.
 * @param[in] pf - wskaźnik na strukturę przechowującą przekierowania.
 * @param[in] num1 - prefiks do przekierowania.
 * @param[in] num2 - prefiks na który zostanie przekierowany @p num1.
 * @param[out] fwInsert - @p *fwInsert po udanym przygotowaniu będzie wskazywać
 *        na węzeł w drzewie @p pf->forward reprezentujący @p num1.
 * @param[out] bwInsert - @p *bwInsert po udanym przygotowaniu będzie wskazywać
 *        na węzeł w drzewie @p pf->backward reprezentujący @p num2.
 * @return W przypadku udanego przygotowania true,
 *         w przeciwnym przypadku false.
 */
static bool phfwdPrepareTreesForAdd(struct PhoneForward *pf,
                                    const char *num1,
                                    const char *num2,
                                    RadixTreeNode *fwInsert,
                                    RadixTreeNode *bwInsert) {
    RadixTree fw = pf->forward;
    RadixTree bw = pf->backward;

    *fwInsert = radixTreeInsert(fw, num1);

    if (*fwInsert == NULL) {
        return false;
    } else {
        *bwInsert = radixTreeInsert(bw, num2);

        if (*bwInsert == NULL) {
            radixTreeBalance(*fwInsert);
            return false;
        } else {
            return true;
        }
    }
}

/**
 * @brief Uzupełnia dane w węźle bw.
 * Uzupełnia dane w węźle bw pozwalające odwrócić przekierowanie.
 * @param[in] bw - wskaźnik na węzeł.
 * @param[in] redirection - wskaźnik na węzeł reprezentujący
 *        prefiks przekierowywany na @p bw.
 * @return Wskaźnik na uzupełnione dane, w przypadku problemów
 *         z przydzieleniem pamięci NULL.
 */
static ListNode phfwdPrepareBw(RadixTreeNode bw, RadixTreeNode redirection) {
    List list = radixTreeGetNodeData(bw);
    if (list == NULL) {
        list = listCreate();
        if (list == NULL) {
            return NULL;
        }
    }
    ListNode result = listPushBack(list, redirection);
    if (result == NULL) {
        if (listIsEmpty(list)) {
            listDestroy(list);
            assert(radixTreeGetNodeData(bw) == NULL);
        }
        return NULL;
    } else {
        radixTreeSetData(bw, list);
        return result;
    }
}

/**
 * @brief wskaźnik na struct ForwardData.
 * @see struct ForwardData
 */
typedef struct ForwardData *ForwardData;

/**
 * @brief Dane przechowywane w węzłach PhoneForward->forward.
 * @see PhoneForward
 */
struct ForwardData {
    /**
     * @brief Węzeł reprezentujący tekst na który jest przekierowywany prefiks.
     */
    RadixTreeNode treeNode;

    /**
     * Wskaźnik na element listy w węźle treeNode z drzewa
     * PhoneForward->backward reprezentujący wskaźnik na dany węzeł.
     * @see treeNode
     * @see PhoneForward
     */
    ListNode listNode;
};

/**
 * @brief Do balansowania drzewa w przypadku nieudanego wstawienia.
 * Usuwa zbyteczne węzły.
 * @see radixTreeBalance
 * @param[in] fwInsert - wskaźnik na
 * @param[in] bwInsert
 */
static void phfwdPrepareClean(RadixTreeNode fwInsert, RadixTreeNode bwInsert) {
    radixTreeBalance(bwInsert);
    radixTreeBalance(fwInsert);
}

/**
 * @brief Usuwa odwrócone przekierowanie.
 * Usuwa informacje o przekierowaniu z drzewa PhoneForward->backward.
 * @see ForwardData
 * @param[in] fd - informacje o przekierowaniu.
 */
static void phfwdDeleteNodeFromBackwardTree(ForwardData fd) {
    assert(fd != NULL);
    assert(fd->treeNode != NULL);
    assert(fd->listNode != NULL);
    List list = radixTreeGetNodeData(fd->treeNode);
    assert(list != NULL);
    listDeleteNode(fd->listNode);
    if (listIsEmpty(list)) {
        listDestroy(list);
        radixTreeSetData(fd->treeNode, NULL);
        radixTreeBalance(fd->treeNode);
    }
}

/**
 * @brief Wstawia dane o przekierowaniach do węzłów.
 * @param[in] fwInsert - wskaźnik na węzeł do wstawienia danych w drzewie
 *        PhoneForward->forward.
 * @param[in] bwInsert wskaźnik na węzeł do wstawienia danych w drzewie
 *        PhoneForward->backward.
 * @return W przypadku sukcesu zwraca true, w przeciwnym przypadku false.
 */
static bool phfwdAddSetNodes(RadixTreeNode fwInsert, RadixTreeNode bwInsert) {
    ListNode newNode = phfwdPrepareBw(bwInsert, fwInsert);
    if (newNode == NULL) {
        phfwdPrepareClean(fwInsert, bwInsert);
        return false;
    } else {
        ForwardData fd = malloc(sizeof(struct ForwardData));
        if (fd == NULL) {
            listDeleteNode(newNode);
            List list = radixTreeGetNodeData(bwInsert);
            if (listIsEmpty(list)) {
                listDestroy(list);
                radixTreeSetData(bwInsert, NULL);
            }
            phfwdPrepareClean(fwInsert, bwInsert);
            return false;
        } else {
            ForwardData old = radixTreeGetNodeData(fwInsert);
            if (old != NULL) {
                phfwdDeleteNodeFromBackwardTree(old);
                free(old);
                radixTreeSetData(fwInsert, NULL);
            }

            fd->treeNode = bwInsert;
            fd->listNode = newNode;
            radixTreeSetData(fwInsert, fd);

            return true;

        }
    }


}

/**
 * @brief Sprawdza czy @p num1 to poprawny numer.
 * @param[in] num1 - wskaźnik na numer.
 * @return Jeżeli odpowiedź jest pozytywna to true,
 *         w przeciwnym przypadku false.
 */
static bool phfwdIsNumber(const char *num1) {
    if (num1 == NULL || *num1 == '\0') {
        return false;
    } else {
        const char *ptr = num1;
        while (*ptr != '\0') {
            if (!(characterIsDigit(*ptr))) {
                return false;
            }
            ptr++;
        }
        return true;
    }
}

bool phfwdAdd(struct PhoneForward *pf, const char *num1, const char *num2) {
    if (!phfwdIsNumber(num1) || !phfwdIsNumber(num2)
        || strcmp(num1, num2) == 0) {
        return false;
    } else {
        RadixTree fwInsert;
        RadixTree bwInsert;
        return phfwdPrepareTreesForAdd(pf, num1, num2, &fwInsert, &bwInsert)
               && phfwdAddSetNodes(fwInsert, bwInsert);
    }

}

/**
 * @brief Usuwa odpowiedniki danych z PhoneForward->forward w backward.
 * Używany w radixTreeDeleteSubTree.
 * @see radixTreeDeleteSubTree
 * @see phfwdRemove
 * @param[in] data - wskaźnik na dane z węzła drzewa PhoneForward->forward.
 * @param[in] backwardTree - wskaźnik na drzewo PhoneForward->backward.
 */
static void phfwdRemoveCleaner(void *data, void *backwardTree) {
    assert(data != NULL);
    assert(backwardTree != NULL);
    (void) backwardTree;
    ForwardData fd = (ForwardData) data;
    phfwdDeleteNodeFromBackwardTree(fd);
    free(fd);

}

void phfwdRemove(struct PhoneForward *pf, const char *num) {
    if (!phfwdIsNumber(num)) {
        return;
    } else {
        RadixTreeNode subTreeNode;
        int findResult = radixTreeFindLite(pf->forward, num, &subTreeNode);

        if (findResult == RADIX_TREE_FOUND
            || findResult == RADIX_TREE_SUBSTR) {
            radixTreeDeleteSubTree(subTreeNode, phfwdRemoveCleaner,
                                   pf->backward);
        } else {
            return;
        }
    }
}

/**
 * @brief Poprawia wskaźniki dla phfwdGetNumber.
 * @see phfwdGetNumber
 * @param[in] tree - wskaźnik na drzewo numerów.
 * @param[in] num - wskaźnik na tekst reprezentujący numer.
 * @param[in, out] ptr - wskaźnik na wskaźnik na węzeł którego ojciec
 *        reprezentuje
 *        najdłuższy możliwy pasujący prefiks numeru
 *        a krawędź wchodząca do @p ptr pewną jego część
 *        następującą po prefiksie. Po wykonaniu operacji @p *ptr wskazuje
 *        na węzeł reprezentujący najdłuższy pasujący prefiks numeru.
 * @param[in,out] matchedTxt - wskaźnik na wskaźnik na tekst
 *        reprezentujący dopasowanie
 *        numeru w drzewie. Po wykonaniu operacji wskazuje na dopasowanie
 *        wyłączające częściowe dopasowanie krawędzi w @p tree.
 */
static void
phfwdSetPointersForGettingText(RadixTree tree,
                               const char *num, RadixTreeNode *ptr,
                               const char **matchedTxt) {
    size_t nodeMatch = 0;
    int matchMode;
    int findResult = radixTreeFind(tree, num, ptr, matchedTxt,
                                   &nodeMatch, &matchMode);
    if (findResult != RADIX_TREE_FOUND
        && (matchMode != RADIX_TREE_NODE_MATCH_FULL)) {

        *matchedTxt = (*matchedTxt) - nodeMatch;
        *ptr = radixTreeFather(*ptr);
    }
}

/**
 * @brief Pobiera przekierowany numer.
 * @param[in] forward - wskaźnik na węzeł reprezentujący drzewo.
 * @param[in] num - wskaźnik na numer.
 * @return Przekierowany numer.
 */
static const char *phfwdGetNumber(RadixTree forward, const char *num) {
    RadixTreeNode ptr;
    const char *matchedTxt;

    phfwdSetPointersForGettingText(forward, num, &ptr, &matchedTxt);

    while (!radixTreeIsRoot(ptr) && radixTreeGetNodeData(ptr) == NULL) {
        matchedTxt = matchedTxt - radixTreeHowManyChars(ptr);
        ptr = radixTreeFather(ptr);
    }
    char *result = NULL;
    if (radixTreeIsRoot(ptr)) {
        assert(matchedTxt == num);
        result = malloc(strlen(matchedTxt) + (size_t) 1);
        if (result == NULL) {
            return NULL;
        } else {
            strcpy(result, matchedTxt);
            return result;
        }
    } else {
        ForwardData fd = (ForwardData) radixTreeGetNodeData(ptr);
        assert(fd != NULL);
        char *prefix = radixGetFullText(fd->treeNode);
        if (prefix == NULL) {
            return NULL;
        } else {
            result = concatenate(prefix, matchedTxt);
            free(prefix);
            return result;
        }
    }

}

const struct PhoneNumbers *phfwdGet(struct PhoneForward *pf, const char *num) {
    if (!phfwdIsNumber(num)) {
        return phfwdEmptySequenceResult();
    } else {
        struct PhoneNumbers *result = phfwdCreatePhoneNumbersStructure(1);
        if (result == NULL) {
            return NULL;
        } else {
            const char *number = phfwdGetNumber(pf->forward, num);
            if (number == NULL) {
                phnumDelete(result);
                return NULL;
            } else {
                result->numbers[0] = (char *) number;
                return result;
            }
        }

    }
    return NULL;
}

void phnumDelete(const struct PhoneNumbers *pnum) {
    if (pnum != NULL) {
        size_t i;
        for (i = 0; i < pnum->howMany; i++) {
            free(pnum->numbers[i]);
            pnum->numbers[i] = NULL;
        }
        free(pnum->numbers);
        free((void *) pnum);
    }
}

const char *phnumGet(const struct PhoneNumbers *pnum, size_t idx) {
    if (pnum == NULL
        || pnum->howMany <= idx) {
        return NULL;
    } else {
        return pnum->numbers[idx];
    }
}

/**
 * @brief Maksymalna liczba numerów zwróconych w wyniku phfwdGetReverse.
 * Razem z powtórzeniami.
 * @param[in] node - wskaźnik na węzeł
 * @return Maksymalna liczba numerów zwróconych w wyniku phfwdGetReverse od
 *         tekstu reprezentowanego przez węzeł @p node.
 */
static size_t phfwdHowManyRedirections(RadixTreeNode node) {
    size_t result = 1;
    RadixTreeNode pos = node;

    while (!radixTreeIsRoot(pos)) {
        if (radixTreeGetNodeData(pos) != NULL) {
            List list = radixTreeGetNodeData(pos);
            result += listSize(list, SIZE_MAX);
        }
        pos = radixTreeFather(pos);
    }
    return result;
}

/**
 * @brief Dodaje numery które powstają w wyniku phfwdReverse do @p storage.
 * @param[out] storage - wskaźnik na strukturę przechowującą numery, gotową
 *        do przyjęcia numerów (razem z powtórzeniami).
 * @param[in] node - wskaźnik na węzeł reprezentujący najdłuższy
 *        dopasowany prefiks numeru,
 *        z wyłączeniem częściowego dopasowania krawędzi.
 * @param[in] matchedTxt - wskaźnik na dopasowanie numeru (wszystkie znaki
 *        występujące za tym wskaźnikiem nie zostały dopasowane).
 * @return W przypadku udanego dodania true, w przypadku problemów
 *         false.
 */
static bool phfwdAddRedir(struct PhoneNumbers *storage,
                          RadixTreeNode node,
                          const char *matchedTxt) {
    RadixTreeNode pos = node;
    size_t insertPtr = 0;
    while (!radixTreeIsRoot(pos)) {
        if (radixTreeGetNodeData(pos) != NULL) {
            List list = radixTreeGetNodeData(pos);
            ListNode p = listFirstNode(list);
            while (p != NULL) {
                char *prefix = radixGetFullText(listNodeGetValue(p));
                if (prefix == NULL) {
                    return false;
                } else {
                    char *toAdd = concatenate(prefix, matchedTxt);
                    free(prefix);
                    if (toAdd == NULL) {
                        return false;
                    } else {
                        assert(insertPtr < storage->howMany);
                        storage->numbers[insertPtr] = toAdd;
                        insertPtr++;
                    }
                }
                p = listNextNode(p);
            }
        }
        matchedTxt = matchedTxt - radixTreeHowManyChars(pos);
        pos = radixTreeFather(pos);

    }
    char *toAdd = duplicateText(matchedTxt);
    if (toAdd == NULL) {
        return false;
    } else {
        assert(insertPtr < storage->howMany);
        storage->numbers[insertPtr] = toAdd;
        insertPtr++;
    }
    return true;
}

/**
 * @brief Przygotowuje podane struktury do sortowania numerów
 * @param[out] tree - wskaźnik na wskaźnik na drzewo wykorzystane do sortowania.
 *        Po wykonaniu @p *tree wskazuje na nowe puste drzewo.
 * @param[out] ids - wskaźnik na wskaźnik na tablicę indeksów.
 *        Po wykonaniu @p *ids wskazuje na tablicę indeksów od 0 do
 *        @p out->howMany - 1.
 * @param[in] out - wskaźnik na strukturę przechowującą numery.
 * @return Jeżeli operacja się powiedzie to true, false
 *         w przeciwnym przypadku.
 */
static bool phfwdPrepareForSort(RadixTree *tree, size_t **ids,
                                const struct PhoneNumbers *out) {
    *tree = radixTreeCreate();
    if (*tree == NULL) {
        return false;
    } else {
        *ids = malloc(out->howMany * sizeof(size_t));
        if (*ids == NULL) {
            radixTreeDelete(*tree, radixTreeEmptyDelFunction, NULL);
            return false;
        } else {
            size_t i;
            for (i = 0; i < out->howMany; i++) {
                (*ids)[i] = i;
            }
            return true;
        }
    }
}

/**
 * @brief Dodaje numery do drzewa w celu posortowania.
 * Dodaje numery do drzewa w celu posortowania numerów i usunięcia powtórzeń.
 * @param[in] tree - wskaźnik na drzewo wykorzystywane przy sortowaniu.
 * @param[in] ids - tablica indeksów (od 0 do @p out->howMany - 1).
 * @param[in] out - wskaźnik na strukturę z numerami.
 * @return Jeżeli operacja się powiedzie to true, false
 *         w przeciwnym przypadku.
 */
static bool phfwdRadixSortOutAddToTree(RadixTree tree, size_t *ids,
                                       const struct PhoneNumbers *out) {
    size_t i;
    for (i = 0; i < out->howMany; i++) {
        RadixTreeNode ptr = radixTreeInsert(tree, out->numbers[i]);
        if (ptr == NULL) {
            return false;
        } else {
            radixTreeSetData(ptr, &ids[i]);
        }
    }
    return true;
}

/**
 * @brief Dane dla funkcji sortującej.
 * @see phfwdRadixSortOutOrganize.
 */
struct SortFoldData {
    /**
     * @brief Wskaźnik na strukturę z wynikiem.
     */
    struct PhoneNumbers *newOut;

    /**
     * @brief Wskaźnik na strukturę z nieposortowanymi numerami.
     */
    const struct PhoneNumbers *oldOut;

    /**
     * @brief Liczba numerów już dodanych do newOut.
     * @see newOut
     */
    size_t process;

};

/**
 * @brief Pobiera z drzewa numery w kolejności posortowanej.
 * @see SortFoldData
 * @see radixTreeFold
 * @param[in] data - wskaźnik na indeks (przechowywany przez drzewo).
 * @param[in] fData - wskaźnik na SortFoldData.
 * @see SortFoldData
 */
static void phfwdRadixSortOutOrganize(void *data, void *fData) {
    struct SortFoldData *sfd = (struct SortFoldData *) fData;
    size_t *id = (size_t *) data;
    sfd->newOut->numbers[sfd->process] = sfd->oldOut->numbers[*id];
    sfd->process++;
    sfd->oldOut->numbers[*id] = NULL;
}

/**
 * @brief Sortuje numery.
 * Sortuje numery z @p *out i usuwa powtórzenia.
 * @param[in, out] out - wskaźnik na wskaźnik na strukturę z numerami.
 *        po wykonaniu @p *out wskazuje na strukturę z posortowanymi numerami
 *        (bez powtórzeń) stara struktura zostaje usunięta.
 * @return W przypadku sukcesu true, w przypadku problemów false.
 */
static bool phfwdRadixSortOut(struct PhoneNumbers **out) {
    RadixTree tree;
    size_t *ids;
    if (phfwdPrepareForSort(&tree, &ids, *out)) {
        if (phfwdRadixSortOutAddToTree(tree, ids, *out)) {
            size_t howManyUnique = 0;
            radixTreeFold(tree, radixTreeCountDataFunction, &howManyUnique);
            struct PhoneNumbers *newOut =
                    phfwdCreatePhoneNumbersStructure(howManyUnique);

            if (newOut == NULL) {
                free(ids);
                radixTreeDelete(tree, radixTreeEmptyDelFunction, NULL);
                return false;
            } else {
                struct SortFoldData sfd;
                sfd.oldOut = *out;
                sfd.newOut = newOut;
                sfd.process = 0;
                radixTreeFold(tree, phfwdRadixSortOutOrganize, &sfd);
                phnumDelete(*out);
                *out = newOut;
                free(ids);
                radixTreeDelete(tree, radixTreeEmptyDelFunction, NULL);

                return true;

            }
        } else {
            free(ids);
            radixTreeDelete(tree, radixTreeEmptyDelFunction, NULL);
            return false;
        }

    } else {
        return false;
    }
}

/**
 * @brief Pobiera numery dla phfwdReverse.
 * @see phfwdReverse
 * @param[in] backward - wskaźnik na drzewo z informacjami o
 *          odwróconych przekierowaniach.
 * @param[in] num - wskaźnik na numer dla którego wykonujemy operację
 *        odwrócenia przekierowania.
 * @return Struktura z numerami dla phfwdReverse.
 */
static const struct PhoneNumbers *phfwdGetReverse(RadixTree backward,
                                                  const char *num) {
    RadixTreeNode ptr;
    const char *matchedTxt;

    phfwdSetPointersForGettingText(backward, num, &ptr, &matchedTxt);

    size_t numberOfRedirections = phfwdHowManyRedirections(ptr);

    struct PhoneNumbers *result =
            phfwdCreatePhoneNumbersStructure(numberOfRedirections);
    if (result == NULL) {
        return NULL;
    } else {
        if (!phfwdAddRedir(result, ptr, matchedTxt)) {
            phnumDelete(result);
            return NULL;
        } else {
            if (phfwdRadixSortOut(&result)) {
                return result;
            } else {
                phnumDelete(result);
                return NULL;
            }

        }
    }


}

const struct PhoneNumbers *phfwdReverse(struct PhoneForward *pf,
                                        const char *num) {
    if (!phfwdIsNumber(num)) {
        return phfwdEmptySequenceResult();
    } else {
        return phfwdGetReverse(pf->backward, num);
    }
}
/**
 * @brief Wyłuskuje cyfry z ciągu set.
 * @param[in] set - ciąg ze znakami
 * @param[out] result - jeżeli cyfra wystąpiła w ciągu @p set
 *       to w @p result na pozycji kod_ascii_cyfry - '0' znajduje się
 *       wartość true, w przeciwnym razie false.
 * @return Liczba różnych cyfr w ciągu @p set.
 */
static size_t phfwdNonTrivialCountExtractDigitsFromSet(const char *set,
                                                       bool *result) {
    size_t j;
    for (j = 0; j < RADIX_TREE_NUMBER_OF_SONS; ++j) {
        result[j] = false;
    }

    size_t howMany = 0;
    const char *i;
    for (i = set; *i != '\0'; i++) {
        if (characterIsDigit(*i)) {
            result[*i - '0'] = true;
        }
    }

    for (j = 0; j < RADIX_TREE_NUMBER_OF_SONS; ++j) {
        if (result[j]) {
            howMany++;
        }
    }

    return howMany;
}

size_t phfwdNonTrivialCount(struct PhoneForward *pf, const char *set, size_t len) {
    if (pf == NULL || set == NULL || len == 0) {
        return 0;
    } else {
        bool availableDigits[RADIX_TREE_NUMBER_OF_SONS];
        size_t howManyDigitsAvailable =
                phfwdNonTrivialCountExtractDigitsFromSet(set, availableDigits);

        if (howManyDigitsAvailable == 0) {
            return 0;
        } else {
            return radixTreeNonTrivialCount(pf->backward,
                                            len,
                                            availableDigits,
                                            howManyDigitsAvailable);
        }


    }
}
