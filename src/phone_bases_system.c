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

/**
 * @see struct PhoneBaseInfo
 */
typedef struct PhoneBaseInfo PhoneBaseInfo;

/**
 * @see struct PhoneBasesNode
 */
typedef struct PhoneBasesNode *PhoneBasesNode;

/**
 * @brief Informacje o bazie przekierowań.
 */
struct PhoneBaseInfo {
    /**
     * @brief Wartość funkcji skrótu dla @p id.
     */
    size_t hash;

    /**
     * @brief Identyfikator bazy.
     */
    const char *id;

    /**
     * @brief Wskaźnik na bazę przekierowań.
     */
    struct PhoneForward *base;
};


/**
 *  @brief Węzeł listy baz przekierowań.
 *  @see PhoneBases.
 */
struct PhoneBasesNode {
    /**
     * @brief Informacje o przechowywanej bazie.
     */
    PhoneBaseInfo baseInfo;

    /**
     * @brief Następny węzeł.
     * Jeżeli nie ma następnego węzła to @p next
     * ma wartość NULL.
     */
    PhoneBasesNode next;
};

/**
 * @brief Struktura przechowująca bazy przekierowań numerów telefonów.
 */
struct PhoneBases {
    /**
     * @brief Pierwszy węzeł listy.
     * NULL w przypadku przechowywania zera baz.
     */
    PhoneBasesNode basesList;

    /**
     * @brief Liczba przechowywanych baz.
     */
    size_t numberOfBases;
};

/**
 * @see phoneBasesHashId
 */
const size_t PHONE_BASES_HASH_BASE = 127;

/**
 * @see phoneBasesHashId
 */
const size_t PHONE_BASES_HASH_MOD = 1000000009;

/**
 * @brief Przyporządkowuje identyfikatorowi @p id numer.
 * @param[in] id - ciąg znaków w stylu c.
 * @return Wartość funkcji mieszającej dla @p id.
 */
static size_t phoneBasesHashId(const char *id) {
    size_t result = 0;
    const char *ptr = id;

    while (*ptr != '\0') {
        result = ((result * PHONE_BASES_HASH_BASE) + (*ptr))
                 % PHONE_BASES_HASH_MOD;
        ptr++;
    }

    return result;
}

/**
 * @brief Sprawdza czy @p pbi odpowiada bazie o identyfikatorze @p id.
 * @param[in] pbi - wskaźnik na strukturę przechowującą informacje
 *       o bazie przekierowań numerów.
 * @param[in] id - identyfikator (ciąg numerów w stylu c).
 * @param[in] hash - wartość @p phoneBasesHashId dla @p id.
 * @return Niezerowa wartość jeżeli @p pbi opisuje bazę
 *         o identyfikatorze @p id.
 */
static int phoneBasesInfoEqualId(PhoneBaseInfo pbi, const char *id, size_t hash) {
    return pbi.hash == hash && strcmp(pbi.id, id) == 0;
}

/**
 * @brief Usuwa węzeł @p pbn
 * @param[in] pbn - wskaźnik na usuwany węzeł.
 */
static void phoneBasesFreeNode(PhoneBasesNode pbn) {
    free((void *) pbn->baseInfo.id);
    phfwdDelete(pbn->baseInfo.base);
    free(pbn);
}

/**
 * @brief Inicjuje strukturę przechowującą bazy przekierowań.
 * @param[in, out] pb - wskaźnik na strukturę przechowującą bazy przekierowań.
 */
static void phoneBasesInitPhoneBases(PhoneBases pb) {
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

/**
 * @brief Usuwa wszystkie węzły występujące za @p node włącznie.
 * @param[in] node - wskaźnik na strukturę reprezentującą węzeł.
 */
static void phoneBasesDeleteNodesList(PhoneBasesNode node) {
    if (node != NULL) {
        phoneBasesDeleteNodesList(node->next);
        node->next = NULL;
        phoneBasesFreeNode(node);
    }
}

void phoneBasesDestroyPhoneBases(PhoneBases pb) {
    phoneBasesDeleteNodesList(pb->basesList);
    pb->basesList = NULL;
    free(pb);
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
        ptr = ptr->next;
    }
    return NULL;
}

struct PhoneForward *phoneBasesAddBase(PhoneBases pb, const char *id) {
    struct PhoneForward *result;

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