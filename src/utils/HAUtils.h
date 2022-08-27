#ifndef AHA_HAUTILS_H
#define AHA_HAUTILS_H

#include <Arduino.h>

/**
 * This class provides some useful methods to make life easier.
 */
class HAUtils
{
public:
    /**
     * Checks whether the given `str` ends with the given `suffix`.
     * 
     * @param str Input string to check.
     * @param suffix Suffix to find
     * @returns True if the given suffix is present at the end of the given string.
     */
    static bool endsWith(
        const char* str,
        const char* suffix
    );

    /**
     * Converts the given byte array into hex string.
     * Each byte will be represented by two bytes, so the output size will be `length * 2`
     * 
     * @param dst Destination where the string will be saved.
     * @param src Bytes array to convert.
     * @param length Length of the bytes array.
     */
    static void byteArrayToStr(
        char* dst,
        const byte* src,
        const uint16_t length
    );

    /**
     * Converts the given byte array into hex string.
     * This method allocates a new memory.
     * 
     * @param src Bytes array to convert.
     * @param length Length of the bytes array.
     * @returns Newly allocated string containing the hex representation.
     */
    static char* byteArrayToStr(
        const byte* src,
        const uint16_t length
    );

    /**
     * Calculates the number of digits in the given number.
     * 
     * @param value Input number. It can be signed value.
     * @returns Number of digits (including the sign if the value is signed).
     */
    static uint8_t calculateNumberSize(int32_t value);

    /**
     * Converts the given number to the string.
     * 
     * @param dst Destination where the number will be saved.
     * @param value Number to convert.
     * @note The `dst` size should be calculated using HAUtils::calculateNumberSize method plus 1 extra byte for the null terminator.
     */
    static void numberToStr(char* dst, int32_t value);
};

#endif
