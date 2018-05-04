
#include <assert.h>
#include "phone_forward.h"
#include "radix_tree.h"
#include "list.h"


struct PhoneForward {
    RadixTree forward;
    RadixTree backward;
};

struct PhoneNumbers {
    //todo
    int todo;
};

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
    bool wasFwEmpty = false;
    if (radixTreeGetNodeData(fwInsert) == NULL) {
        wasFwEmpty = true;
        radixTreeSetData(fwInsert, !NULL);
    }
    radixTreeBalance(bwInsert);
    if (wasFwEmpty) {
        radixTreeSetData(fwInsert, NULL);
        radixTreeBalance(fwInsert);
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
                listDeleteNode(old->listNode);
                List list = radixTreeGetNodeData(old->treeNode);
                if (listIsEmpty(list)) {
                    listDestroy(list);
                    radixTreeSetData(old->treeNode, NULL);
                    radixTreeBalance(old->treeNode);
                }
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
    if (!phfwdIsNumber(num1), !phfwdIsNumber(num2)) {
        return false;
    } else {
        RadixTree fwInsert;
        RadixTree bwInsert;
        return phfwdPrepareTreesForAdd(pf, num1, num2, &fwInsert, &bwInsert) &&
               phfwdPrepareNodes(fwInsert, bwInsert);
    }

}

