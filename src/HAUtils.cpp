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
    float value,
    const uint8_t precision
)
{
    bool isSigned = value < 0;
    if (isSigned) {
        value *= -1;
    }

    const int32_t number = static_cast<int32_t>(value);
    uint8_t digitsNb = calculateNumberSize(number);

    if (number == 0 && precision == 0) {
        isSigned = false;
    }

    if (isSigned) {
        digitsNb++; // sign
    }

    if (precision > 0) {
        digitsNb += precision + 1; // dot + decimal
    }

    return digitsNb;
}

void HAUtils::floatToStr(
    char* dst,
    const float& value,
    const uint8_t precision
)
{
    const int32_t number = static_cast<int32_t>(value);
    if (number == 0 && precision == 0) {
        dst[0] = 0x30; // digit 0
        return;
    }

    dtostrf(value, 0, precision, dst);
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