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
    static const char map[] PROGMEM = {"0123456789abcdef"};
    for (uint8_t i = 0; i < length; i++) {
        dst[i*2] = pgm_read_byte(&map[((char)src[i] & 0XF0) >> 4]);
        dst[i*2+1] = pgm_read_byte(&map[((char)src[i] & 0x0F)]);
    }

    dst[length * 2] = 0;
}

char* HAUtils::byteArrayToStr(
    const byte* src,
    const uint16_t length
)
{
    char* dst = new char[(length * 2) + 1]; // include null terminator
    byteArrayToStr(dst, src, length);

    return dst;
}

uint8_t HAUtils::calculateNumberSize(int32_t value)
{
    const bool isSigned = value < 0;
    if (isSigned) {
        value *= -1;
    }

    uint8_t digitsNb = 1;
    while (value > 9) {
        value /= 10;
        digitsNb++;
    }

    if (isSigned) {
        digitsNb++; // sign
    }

    return digitsNb;
}

void HAUtils::numberToStr(char* dst, int32_t value)
{
    if (value == 0) {
        dst[0] = 0x30; // digit 0
        return;
    }

    const uint8_t digitsNb = calculateNumberSize(value);
    if (value < 0) {
        value *= -1;
        dst[0] = 0x2D; // hyphen
    }

    char* ch = &dst[digitsNb - 1];
    while (value != 0) {
       *ch = (value % 10) + '0';
       value /= 10;
       ch--;
    }
}
