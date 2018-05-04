
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "phone_forward.h"
#include "radix_tree.h"
#include "list.h"
#include "stdfunc.h"


struct PhoneForward {
    RadixTree forward;
    RadixTree backward;
};

struct PhoneNumbers {
    char **numbers;
    size_t howMany;
};

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
                radixTreeDelete(result->forward, radixTreeEmptyDelFunction, NULL);
                free(result);
                return NULL;
            } else {
                return result;
            }
        }
    }
}

static void phfwdForwardJustDelete(void *ptrA, void *ptrB) {
    assert(ptrA != NULL);
    assert(ptrB == NULL);
    free(ptrA);
}

static void phfwdBackwardJustDelete(void *ptrA, void *ptrB) {
    assert(ptrA != NULL);
    assert(ptrB == NULL);
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


static bool phfwdPrepareTreesForAdd(struct PhoneForward *pf,
                                    char const *num1,
                                    char const *num2,
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

typedef struct ForwardData *ForwardData;
struct ForwardData {
    RadixTreeNode treeNode;
    ListNode listNode;
};

static int phfwdPrepareClean(RadixTreeNode fwInsert, RadixTreeNode bwInsert) {

    radixTreeBalance(bwInsert);
    radixTreeBalance(fwInsert);
}

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

static bool phfwdPrepareNodes(RadixTreeNode fwInsert, RadixTreeNode bwInsert) {
    ListNode newNode = phfwdPrepareBw(bwInsert, fwInsert);
    if (newNode == NULL) {
        phfwdPrepareClean(fwInsert, bwInsert);
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

        }
    }


}

static bool phfwdIsNumber(char const *num1) {
    if (num1 == NULL || *num1 == '\0') {
        return false;
    } else {
        char const *ptr = num1;
        while (*ptr != '\0') {
            if (!(*ptr <= '9' && *ptr >= '0')) {
                return false;
            }
            ptr++;
        }
        return true;
    }
}

bool phfwdAdd(struct PhoneForward *pf, char const *num1, char const *num2) {
    if (!phfwdIsNumber(num1) || !phfwdIsNumber(num2)) {
        return false;
    } else {
        RadixTree fwInsert;
        RadixTree bwInsert;
        return phfwdPrepareTreesForAdd(pf, num1, num2, &fwInsert, &bwInsert) &&
               phfwdPrepareNodes(fwInsert, bwInsert);
    }

}


static void phfwdRemoveCleaner(void *data, void *backwardTree) {
    assert(data != NULL);
    assert(backwardTree != NULL);
    ForwardData fd = (ForwardData) data;
    phfwdDeleteNodeFromBackwardTree(fd);
    free(fd);

}

void phfwdRemove(struct PhoneForward *pf, char const *num) {
    if (!phfwdIsNumber(num)) {
        return;
    } else {
        RadixTreeNode subTreeNode;
        int findResult = radixTreeFindLite(pf->forward, num, &subTreeNode);

        if (findResult == RADIX_TREE_FOUND
            || findResult == RADIX_TREE_SUBSTR) {
            radixTreeDeleteSubTree(subTreeNode, phfwdRemoveCleaner, pf->backward);
        } else {
            return;
        }
    }
}

static void phfwdSetPointersForGettingText(RadixTree tree,
                                           char const *num,
                                           RadixTreeNode *ptr,
                                           char const **matchedTxt,
                                           char const **nodeMatched) {
    int findResult = radixTreeFind(tree, num, ptr, matchedTxt, nodeMatched);
    if (findResult != RADIX_TREE_FOUND
        && (*(*nodeMatched) != '\0')) {
        *matchedTxt = (*matchedTxt) - radixTreeHowManyCharsOffset(*ptr, *nodeMatched);
        *ptr = radixTreeFather(*ptr);
    }
}

static char const *phfwdGetNumber(RadixTree forward, char const *num) {
    RadixTreeNode ptr;
    const char *matchedTxt;
    const char *nodeMatched;
    /*int findResult = radixTreeFind(forward, num, &ptr, &matchedTxt, &nodeMatched);
    if (findResult != RADIX_TREE_FOUND
        && (*nodeMatched != '\0')) {
        matchedTxt = matchedTxt - radixTreeHowManyCharsOffset(ptr, nodeMatched);
        ptr = radixTreeFather(ptr);
    }*/
    phfwdSetPointersForGettingText(forward, num, &ptr, &matchedTxt, &nodeMatched);

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
            /*result = malloc(strlen(prefix) + strlen(matchedTxt) + (size_t) 1);
            if (result == NULL) {
                free(prefix);
                return NULL;
            } else {
                strcpy(result, prefix);
                strcpy(result + strlen(prefix), matchedTxt);
                free(prefix);
                return result;
            }*/

        }
    }

}

struct PhoneNumbers const *phfwdGet(struct PhoneForward *pf, char const *num) {
    if (!phfwdIsNumber(num)) {
        return phfwdEmptySequenceResult();
    } else {
        struct PhoneNumbers *result = phfwdCreatePhoneNumbersStructure(1);
        if (result == NULL) {
            return NULL;
        } else {
            char const *number = phfwdGetNumber(pf->forward, num);
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

void phnumDelete(struct PhoneNumbers const *pnum) {
    if (pnum != NULL) {
        size_t i;
        for (i = 0; i < pnum->howMany; i++) {
            //assert(pnum->numbers[i] != NULL);
            free(pnum->numbers[i]);
            pnum->numbers[i] = NULL;
        }
        free(pnum->numbers);
        free((void *) pnum);
    }
}

char const *phnumGet(struct PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL
        || pnum->howMany <= idx) {
        return NULL;
    } else {
        return pnum->numbers[idx];
    }
}

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

static bool phfwdAddRedir(struct PhoneNumbers *storage,
                          RadixTreeNode node,
                          char const *matchedTxt) {
    RadixTreeNode pos = node;
    size_t insertPtr = 0;
    while (!radixTreeIsRoot(pos)) {
        if (radixTreeGetNodeData(pos) != NULL) {
            List list = radixTreeGetNodeData(pos);
            ListNode p = listFirstNode(list);
            while (p != NULL) {
                char *prefix = radixGetFullText(p->element);
                if (prefix == NULL) {
                    return false;
                } else {
                    char *toAdd = concatenate(prefix, matchedTxt);
                    free(prefix);
                    if (toAdd == NULL) {
                        return false;
                    } else {
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
        storage->numbers[insertPtr] = toAdd;
        insertPtr++;
    }
    return true;
}


static bool phfwdPrepareForSort(RadixTree *tree, size_t **ids,
                                struct PhoneNumbers const *out) {
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

static bool phfwdRadixSortOutAddToTree(RadixTree tree, size_t *ids,
                                       struct PhoneNumbers const *out) {
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

struct SortFoldData {
    struct PhoneNumbers *newOut;
    const struct PhoneNumbers *oldOut;
    size_t process;

};

static void phfwdRadixSortOutOrganize(void *data, void *fData) {
    struct SortFoldData *sfd = (struct SortFoldData *)fData;
    size_t *id = (size_t *)data;
    sfd->newOut->numbers[sfd->process] = sfd->oldOut->numbers[*id];
    sfd->process++;
    sfd->oldOut->numbers[*id] = NULL;
}

static bool phfwdRadixSortOut(struct PhoneNumbers **out) {
    RadixTree tree;
    size_t *ids;
    if (phfwdPrepareForSort(&tree, &ids, *out)) {
        if (phfwdRadixSortOutAddToTree(tree, ids, *out)) {
            size_t howManyUnique = 0;
            radixTreeFold(tree, radixTreeEmptyCountDataFunction, &howManyUnique);
            struct PhoneNumbers *newOut = phfwdCreatePhoneNumbersStructure(howManyUnique);

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

static struct PhoneNumbers const *phfwdGetReverse(RadixTree backward, char const *num) {
    RadixTreeNode ptr;
    const char *matchedTxt;
    const char *nodeMatched;
    phfwdSetPointersForGettingText(backward, num, &ptr, &matchedTxt, &nodeMatched);

    size_t numberOfRedirections = phfwdHowManyRedirections(ptr);
    //printf("eeee:%d\n", numberOfRedirections);

    struct PhoneNumbers *result = phfwdCreatePhoneNumbersStructure(numberOfRedirections);
    if (result == NULL) {
        return NULL;
    } else {
        if(!phfwdAddRedir(result, ptr, matchedTxt)) {
            phnumDelete(result);
            return NULL;
        } else {
            //todo sort and unique
            phfwdRadixSortOut(&result);
            return result;
        }
    }


}




struct PhoneNumbers const *phfwdReverse(struct PhoneForward *pf, char const *num) {
    if (!phfwdIsNumber(num)) {
        return phfwdEmptySequenceResult();
    } else {
        return phfwdGetReverse(pf->backward, num);
    }
}

