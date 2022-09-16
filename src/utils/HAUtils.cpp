#ifdef ARDUINO_ARCH_SAMD
#include <avr/dtostrf.h>
#endif

#include <Arduino.h>
#include "HAUtils.h"
#include "HADictionary.h"

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
    for (uint8_t i = 0; i < length; i++) {
        dst[i*2] = pgm_read_byte(&HAHexMap[((char)src[i] & 0XF0) >> 4]);
        dst[i*2+1] = pgm_read_byte(&HAHexMap[((char)src[i] & 0x0F)]);
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

int32_t HAUtils::getPrecisionBase(const uint8_t precision)
{
    switch (precision) {
    case 1:
        return 10;

    case 2:
        return 100;

    case 3:
        return 1000;

    default:
        return 1;
    }
}

uint8_t HAUtils::getFloatZeroPaddingSize(
    const int32_t value,
    const uint8_t precision
)
{
    if (precision == 0) {
        return 0;
    }

    const uint8_t valueDigitsNb = calculateNumberSize(value);
    const uint8_t precisionBaseDigitsDb = calculateNumberSize(
        getPrecisionBase(precision)
    );

    return valueDigitsNb > precisionBaseDigitsDb
        ? 0
        : precisionBaseDigitsDb - valueDigitsNb;
}

int32_t HAUtils::processFloatValue(float value, const uint8_t precision)
{
    return static_cast<int32_t>(value * getPrecisionBase(precision));
}

float HAUtils::getFloatValue(int32_t value, const uint8_t precision)
{
    return (float)value / (float)getPrecisionBase(precision);
}

uint8_t HAUtils::calculateNumberSize(int32_t value, const uint8_t precision)
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

    if (precision > 0) {
        if (value == 0) {
            return 1;
        }

        // one digit + dot + decimal digits (+ sign)
        const uint8_t minValue = isSigned ? precision + 3 : precision + 2;
        return digitsNb >= minValue ? digitsNb + 1 : minValue;
    }

    return digitsNb;
}

void HAUtils::numberToStr(char* dst, int32_t value, const uint8_t precision)
{
    char* prefixCh = &dst[0];
    if (value == 0) {
        *prefixCh = '0';
        return;
    }

    const uint8_t digitsNb = calculateNumberSize(value, precision);
    if (value < 0) {
        value *= -1;
        *prefixCh = '-';
        prefixCh++;
    }

    uint8_t zeroPadding = getFloatZeroPaddingSize(digitsNb, precision);
    if (zeroPadding > 0) {
        memset(prefixCh, '0', zeroPadding + 1);
        prefixCh++;
        *prefixCh = '.';
    }

    char* ch = &dst[digitsNb - 1];
    char* dotPos = precision > 0 ? &dst[digitsNb - 1 - precision] : nullptr;

    while (value != 0) {
        if (ch == dotPos) {
            *dotPos = '.';
            ch--;
            continue;
        }

        *ch = (value % 10) + '0';
        value /= 10;
        ch--;
    }
}
