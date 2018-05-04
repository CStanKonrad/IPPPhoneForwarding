
#include <assert.h>
#include <string.h>
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
    struct PhoneNumbers *result = phfwdCreatePhoneNumbersStructure(1);
    if (result == NULL) {
        return NULL;
    } else {
        char *emptySeq = malloc(sizeof(char));
        if (emptySeq == NULL) {
            return NULL;
        } else {
            *emptySeq = '\0';
            result->numbers[0] = emptySeq;
            return result;
        }
        return result;
    }
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
    if (*num1 == '\0') {
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

static char const *phfwdGetNumber(RadixTree forward, char const *num) {
    RadixTreeNode ptr;
    const char *matchedTxt;
    const char *nodeMatched;
    int findResult = radixTreeFind(forward, num, &ptr, &matchedTxt, &nodeMatched);
    if (findResult != RADIX_TREE_FOUND
            && (*nodeMatched != '\0')) {
        matchedTxt = matchedTxt - radixTreeHowManyCharsOffset(ptr, nodeMatched);
        ptr = radixTreeFather(ptr);
    }

    while (!radixTreeIsRoot(ptr) && radixTreeGetNodeData(ptr) == NULL) {
        matchedTxt = matchedTxt - radixTreeHowManyChars(ptr);
        ptr = radixTreeFather(ptr);
    }
    char *result = NULL;
    if (radixTreeIsRoot(ptr)) {
        assert(matchedTxt == num);
        result = malloc(strlen(matchedTxt) + (size_t)1);
        if (result == NULL) {
            return NULL;
        } else {
            strcpy(result, matchedTxt);
        }
    } else {
        ForwardData fd = (ForwardData )radixTreeGetNodeData(ptr);
        assert(fd != NULL);
        char *prefix = radixGetFullText(fd->treeNode);
        if (prefix == NULL) {
            return NULL;
        } else {
            result = malloc(strlen(prefix) + strlen(matchedTxt) + (size_t)1);
            if (result == NULL) {
                free(prefix);
                return NULL;
            } else {
                strcpy(result, prefix);
                strcpy(result + strlen(prefix), matchedTxt);
                free(prefix);
                return result;
            }

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
                result->numbers[0] = (char*)number;
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

