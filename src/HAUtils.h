#ifndef HAUTILS_H
#define HAUTILS_H

#include <Arduino.h>

class HAUtils
{
public:
    enum ValueType {
        ValueTypeUnknown = -1,
        ValueTypeUint8,
        ValueTypeUint16,
        ValueTypeUint32,
        ValueTypeInt8,
        ValueTypeInt16,
        ValueTypeInt32,
        ValueTypeDouble,
        ValueTypeFloat
    };

    static bool endsWith(
        const char* str,
        const char* suffi
    );
    static void byteArrayToStr(
        char* dst,
        const byte* src,
        const uint16_t& length
    );
    static char* byteArrayToStr(
        const byte* src,
        const uint16_t& length
    );

    template <typename A, typename B >
    static bool compareType(A a, B b) { return false; }

    template <typename A, typename B >
    static bool compareType(A a, A b) { return true; }

    template <typename T>
    static ValueType determineValueType() {
        if (compareType<T, uint8_t>(0, (uint8_t)0)) return ValueTypeUint8;
        else if (compareType<T, uint16_t>(0, (uint16_t)0)) return ValueTypeUint16;
        else if (compareType<T, uint32_t>(0, (uint32_t)0)) return ValueTypeUint32;
        else if (compareType<T, int8_t>(0, (int8_t)0)) return ValueTypeInt8;
        else if (compareType<T, int16_t>(0, (int16_t)0)) return ValueTypeInt16;
        else if (compareType<T, int32_t>(0, (int32_t)0)) return ValueTypeInt32;
        else if (compareType<T, double>(0, (double)0)) return ValueTypeDouble;
        else if (compareType<T, float>(0, (float)0)) return ValueTypeFloat;

        return ValueTypeUnknown;
    }

    static uint8_t getValueTypeLength(const ValueType& type);
};

#endif
