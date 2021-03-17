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
    static const char map[] PROGMEM = {"0123456789abcdef"};

    for (uint8_t i = 0; i < length; i++) {
        dst[i*2] = pgm_read_byte(&map[((char)src[i] & 0XF0) >> 4]);
        dst[i*2+1] = pgm_read_byte(&map[((char)src[i] & 0x0F)]);
    }

    dst[finalLength] = '\0';
}

char* HAUtils::byteArrayToStr(
    const byte* src,
    const uint16_t& length
)
{
    char* dst = (char*)malloc((length * 2) + 1); // include null terminator
    byteArrayToStr(dst, src, length);

    return dst;
}

void HAUtils::toStr(
    char* dst,
    uint32_t number
)
{
    uint8_t digitsNb = (number > 1 ? ceil(log10(number)) : 1);
    char tmp[digitsNb + 1]; // including null terminator
    memset(tmp, 0, sizeof(tmp));

    if (number > 0) {
        uint8_t i = (digitsNb - 1);
        while (number > 0) {
            tmp[i] = (number % 10) + '0';
            number /= 10;
            i--;
        }
    } else {
        tmp[0] = '0';
    }

    strcpy(dst, tmp);
}

void HAUtils::toStr(
    char* dst,
    int32_t number
)
{
    if (number < 0) {
        strcpy(dst, "-");
        number *= -1;
    }

    toStr(dst, static_cast<uint32_t>(number));
}



void HAUtils::tempToStr(
    char* dst,
    const double& temp
)
{
    memset(dst, 0, sizeof(AHA_SERIALIZED_TEMP_SIZE));
    //dtostrf(temp, 0, 2, dst); // to do: custom implementation
}

void HAUtils::tempToStr(
    char* dst,
    int16_t temp
)
{
    bool negative = (temp < 0);
    if (negative )

    memset(dst, 0, sizeof(AHA_SERIALIZED_TEMP_SIZE));
    int16_t whole = floor(temp / 100.0);
    int16_t dec = fmod(temp, 100);

    // it shouldn't happen but who knows...
    if (dec < 0) {
        dec *= -1;
    }


    // to do:
    // 1. reverse numbers
    // 2. iterate trough digits using %10 and /10
    // 3. add each digit to str using digit+'0'

}

double HAUtils::strToTemp(
    const char* src
)
{
    return 0;
    //return atof(src); // to do: custo implementation
}

uint8_t HAUtils::getValueTypeLength(const ValueType& type)
{
    switch(type) {
        case ValueTypeUint8:
            return sizeof(uint8_t);

        case ValueTypeUint16:
            return sizeof(uint16_t);

        case ValueTypeUint32:
            return sizeof(uint32_t);

        case ValueTypeInt8:
            return sizeof(int8_t);

        case ValueTypeInt16:
            return sizeof(int16_t);

        case ValueTypeInt32:
            return sizeof(int32_t);

        case ValueTypeDouble:
            return sizeof(double);

        case ValueTypeFloat:
            return sizeof(float);

        default:
            return 0;
    }
}
