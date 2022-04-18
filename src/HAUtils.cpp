#ifdef ARDUINO_ARCH_SAMD
#include <avr/dtostrf.h>
#endif

#include <Arduino.h>

#include "HAUtils.h"

bool HAUtils::endsWith(const char* str, const char* suffix)
{
    if (str == nullptr || suffix == nullptr) {
        return false;
    }

    const uint16_t lenstr = strlen(str);
    const uint16_t lensuffix = strlen(suffix);

    if (lensuffix > lenstr || lenstr == 0 || lensuffix == 0) {
        return false;
    }

    return (strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0);
}

void HAUtils::byteArrayToStr(
    char* dst,
    const byte* src,
    const uint16_t length
)
{
    const uint16_t& finalLength = (length * 2);
    static const char map[] PROGMEM = {"0123456789abcdef"};

    for (uint8_t i = 0; i < length; i++) {
        dst[i*2] = pgm_read_byte(&map[((char)src[i] & 0XF0) >> 4]);
        dst[i*2+1] = pgm_read_byte(&map[((char)src[i] & 0x0F)]);
    }

    dst[finalLength] = '\0';
}

char* HAUtils::byteArrayToStr(
    const byte* src,
    const uint16_t length
)
{
    char* dst = (char*)malloc((length * 2) + 1); // include null terminator
    byteArrayToStr(dst, src, length);

    return dst;
}

uint8_t HAUtils::calculateFloatSize(
    const float& value,
    const uint8_t precision
)
{
    const uint8_t digitsNb = value > 0 ? floor(log10(floor(value))) + 1 : 1;

    // sign + digits + dot + decimal
    return (value < 0 ? 1 : 0) + digitsNb + 1 + precision;
}

 void HAUtils::floatToStr(
    char* dst,
    const float value,
    const uint8_t precision
)
{
    dtostrf(value, 0, precision, dst);
}
