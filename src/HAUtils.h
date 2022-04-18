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
        const float& value,
        const uint8_t precision
    );
    static void floatToStr(
        char* dst,
        const float value,
        const uint8_t precision
    );
};

#endif
