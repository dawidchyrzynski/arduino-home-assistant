#include <Arduino.h>

#include "HAUtils.h"

bool HAUtils::endsWith(const char* str, const char* suffix)
{
    if (str == nullptr || suffix == nullptr) {
        return false;
    }

    const uint16_t& lenstr = strlen(str);
    const uint16_t& lensuffix = strlen(suffix);

    if (lensuffix > lenstr) {
        return false;
    }

    return (strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0);
}

void HAUtils::byteArrayToStr(
    char* dst,
    const byte* src,
    const uint16_t& length
)
{
    const uint16_t& finalLength = (length * 2);
    const char* map = "0123456789abcdef"; // todo: move to progmem

    for (uint8_t i = 0; i < length; i++) {
        dst[i*2] = map[((char)src[i] & 0XF0) >> 4];
        dst[i*2+1] = map[((char)src[i] & 0x0F)];
    }

    dst[finalLength] = '\0';
}

char* HAUtils::byteArrayToStr(
    const byte* src,
    const uint16_t& length
)
{
    char* dst = malloc((length * 2) + 1); // include null terminator
    byteArrayToStr(dst, src, length);

    return dst;
}
