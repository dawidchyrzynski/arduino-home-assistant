#ifndef AHA_HAUTILS_H
#define AHA_HAUTILS_H

#include <Arduino.h>

#define AHA_SERIALIZED_TEMP_SIZE 8

class HAUtils
{
public:
    static bool endsWith(
        const char* str,
        const char* suffix
    );
    static void byteArrayToStr(
        char* dst,
        const byte* src,
        const uint16_t length
    );
    static char* byteArrayToStr(
        const byte* src,
        const uint16_t length
    );
    static uint8_t calculateFloatSize(
        float value,
        const uint8_t precision
    );
    static void floatToStr(
        char* dst,
        float value,
        const uint8_t precision
    );
    static uint8_t calculateNumberSize(int32_t value);
    static void numberToStr(char* dst, int32_t value);
};

#endif
