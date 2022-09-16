#ifndef AHA_HAUTILS_H
#define AHA_HAUTILS_H

#include <Arduino.h>

/**
 * This class provides some useful methods to make life easier.
 */
class HAUtils
{
public:
    typedef int64_t Number;
    static const Number NumberMax = INT64_MAX;

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
     * Returns divider for the given precision.
     * The maximum precision is 3.
     *
     * @param precision
     */
    static Number getPrecisionBase(const uint8_t precision);

    /**
     * Returns amount of zeroes that need to be added at the beginning of the given number.
     *
     * @param value The value to check.
     * @param precision The precision to use (amount of digits in the decimal part). Maximum value is 3.
     */
    static uint8_t getFloatZeroPaddingSize(
        const Number value,
        const uint8_t precision
    );

    /**
     * Converts the given float to the Number representation.
     * The input float is multiped by pow(value, precision).
     *
     * @param value The input number.
     * @param precision The precision to use (amount of digits in the decimal part). Maximum value is 3.
     */
    static Number processFloatValue(float value, const uint8_t precision);

    /**
     * Converts the given number into float representation.
     *
     * @param value The input number.
     * @param precision The precision to use (amount of digits in the decimal part). Maximum value is 3.
     */
    static float getFloatValue(Number value, const uint8_t precision);

    /**
     * Calculates the number of digits in the given number.
     * 
     * @param value Input number. It can be signed value.
     * @param precision Number of decimal digits in the number.
     * @returns Number of digits (including the sign if the value is signed).
     */
    static uint8_t calculateNumberSize(Number value, const uint8_t precision = 0);

    /**
     * Converts the given number to the string.
     * It can be integer or floating point number.
     * In a case of float the input value needs to be multiped by `pow(10, precision)`.
     * 
     * @param dst Destination where the number will be saved.
     * @param value Number to convert.
     * @param precision Number of decimal digits in the number.
     * @note The `dst` size should be calculated using HAUtils::calculateNumberSize method plus 1 extra byte for the null terminator.
     */
    static void numberToStr(char* dst, Number value, const uint8_t precision = 0);

    /**
     * Converts the given string into number.
     * If the given string doesn't contain a valid number the `HAUtils::NumberMax` is returned.
     *
     * @param src A string containing the number.
     */
    static HAUtils::Number strToNumber(const char* src);
};

#endif
