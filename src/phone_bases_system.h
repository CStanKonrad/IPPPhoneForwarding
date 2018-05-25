/** @file
 * Interfejs systemu zarządzania bazami przekierowań.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */

#ifndef TELEFONY_PHONE_BASES_SYSTEM_H
#define TELEFONY_PHONE_BASES_SYSTEM_H

#include <stdbool.h>

#include "phone_forward.h"

typedef struct PhoneBaseInfo PhoneBaseInfo;

typedef struct PhoneBases* PhoneBases;

typedef struct PhoneBasesNode* PhoneBasesNode;


struct PhoneBaseInfo {
    size_t hash;
    const char *id;
    struct PhoneForward *base;
};

struct PhoneBasesNode {
    PhoneBaseInfo baseInfo;
    PhoneBasesNode next;
};

struct PhoneBases {
    PhoneBasesNode basesList;
    size_t numberOfBases;
};

PhoneBases phoneBasesCreateNewPhoneBases();

size_t phoneBasesHowManyBases(PhoneBases pb);

struct PhoneForward* phoneBasesGetBase(PhoneBases pb, const char *id);

struct PhoneForward* phoneBasesAddBase(PhoneBases pb, const char *id);

bool phoneBasesDelBase(PhoneBases pb, const char *id);


#endif //TELEFONY_PHONE_BASES_SYSTEM_H
