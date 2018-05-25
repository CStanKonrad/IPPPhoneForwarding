/** @file
 * Implementacja systemu zarządzania bazami przekierowań.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */

#include <stdlib.h>
#include <string.h>

#include "phone_bases_system.h"
#include "text.h"


const size_t PHONE_BASES_HASH_BASE = 127;
const size_t PHONE_BASES_HASH_MOD = 1000000009;

static size_t phoneBasesHashId(const char *id) {
    size_t result = 0;
    const char *ptr = id;

    while (*ptr != '\0') {
        result = ((result * PHONE_BASES_HASH_BASE) + (*ptr))
                % PHONE_BASES_HASH_MOD;
    }

    return result;
}

static int phoneBasesInfoEqualId(PhoneBaseInfo pbi, const char *id, size_t hash) {
    return pbi.hash == hash && strcmp(pbi.id, id) == 0;
}


static void phoneBasesFreeNode(PhoneBasesNode pbn) {
    free((void*)pbn->baseInfo.id);
    phfwdDelete(pbn->baseInfo.base);
    free(pbn);
}

void phoneBasesInitPhoneBases(PhoneBases pb) {
    pb->basesList = NULL;
    pb->numberOfBases = 0;
}


PhoneBases phoneBasesCreateNewPhoneBases() {
    PhoneBases pb = malloc(sizeof(struct PhoneBases));
    if (pb == NULL) {
        return NULL;
    }
    phoneBasesInitPhoneBases(pb);

    return pb;
}

size_t phoneBasesHowManyBases(PhoneBases pb) {
    return pb->numberOfBases;
}

struct PhoneForward *phoneBasesGetBase(PhoneBases pb, const char *id) {
    size_t idHash = phoneBasesHashId(id);
    PhoneBasesNode ptr = pb->basesList;
    while (ptr != NULL) {
        if (phoneBasesInfoEqualId(ptr->baseInfo, id, idHash)) {
            return ptr->baseInfo.base;
        }
    }
    return NULL;
}


struct PhoneForward* phoneBasesAddBase(PhoneBases pb, const char *id) {
    struct PhoneForward* result;

    result = phoneBasesGetBase(pb, id);

    if (result != NULL) {
        return result;
    } else {
        PhoneBasesNode newNode = malloc(sizeof(struct PhoneBasesNode));
        if (newNode == NULL) {
            return NULL;
        } else {
            char *copyId = duplicateText(id);

            if (copyId == NULL) {
                free(newNode);
                return NULL;
            } else {
                newNode->baseInfo.base = phfwdNew();
                if (newNode->baseInfo.base == NULL) {
                    free(copyId);
                    free(newNode);
                    return NULL;
                } else {
                    newNode->baseInfo.hash = phoneBasesHashId(copyId);
                    newNode->baseInfo.id = copyId;
                    newNode->next = pb->basesList;
                    pb->basesList = newNode;

                    pb->numberOfBases++;
                    return newNode->baseInfo.base;
                }
            }
        }

    }
}


bool phoneBasesDelBase(PhoneBases pb, const char *id) {
    PhoneBasesNode prev = NULL;
    PhoneBasesNode cur = pb->basesList;
    size_t idHash = phoneBasesHashId(id);

    while (cur != NULL) {
        if (phoneBasesInfoEqualId(cur->baseInfo, id, idHash)) {
            PhoneBasesNode *mnt;
            if (prev != NULL) {
                mnt = &prev->next;
            } else {
                mnt = &pb->basesList;
            }
            (*mnt) = cur->next;
            phoneBasesFreeNode(cur);

            pb->numberOfBases--;
            return true;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }

    return false;
}