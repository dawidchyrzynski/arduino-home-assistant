#ifdef ARDUINO_ARCH_SAMD
#include <avr/dtostrf.h>
#endif

#include <Arduino.h>

#include "HAUtils.h"

static const char HAZero[] PROGMEM = {"0"};
static const char HASign[] PROGMEM = {"-"};

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
    const uint8_t digitsNb = calculateNumberSize(number);

    if (number == 0 && precision == 0) {
        isSigned = false;
    }

    // sign + digits + dot + decimal
    return (isSigned ? 1 : 0) + digitsNb + (precision > 0 ? 1 + precision : 0);
}

void HAUtils::floatToStr(
    char* dst,
    const float& value,
    const uint8_t precision
)
{
    const int32_t number = static_cast<int32_t>(value);
    if (number == 0 && precision == 0) {
        strcat_P(dst, HAZero);
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

    const uint8_t digitsNb = value > 0 ? floor(log10(value)) + 1 : 1;

    // sign + digits
    return (isSigned ? 1 : 0) + digitsNb;
}

void HAUtils::numberToStr(char* dst, int32_t value)
{
    const bool isSigned = value < 0;

    if (value == 0) {
        strcat_P(dst, HAZero);
        return;
    } else if (isSigned) {
        value *= -1;
        strcat_P(dst, HASign);
    }

    const uint8_t digitsNb = calculateNumberSize(value);
    char tmp[digitsNb + 1];
    memset(tmp, 0, sizeof(tmp));

    uint8_t i = digitsNb - 1;
    while (value != 0) {
       tmp[i] = (value % 10) + '0';
       value /= 10;
       i--;
    }

    strcat(dst, tmp);
}