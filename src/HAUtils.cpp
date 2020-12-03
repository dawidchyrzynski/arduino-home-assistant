#include <Arduino.h>

#include "HAUtils.h"

bool HAUtils::endsWith(const char *str, const char *suffix)
{
    if (str == nullptr || suffix == nullptr) {
        return false;
    }

    uint16_t lenstr = strlen(str);
    uint16_t lensuffix = strlen(suffix);

    if (lensuffix > lenstr) {
        return false;
    }

    return (strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0);
}
