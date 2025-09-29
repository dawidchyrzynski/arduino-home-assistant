#ifndef AHA_HAUTILS_H
#define AHA_HAUTILS_H

#include <stdint.h>

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
     * Converts given byte array into the string of hex values.
     * @param data Bytes to convert.
     * @param length Number of bytes to convert.
     * @return Dynamically allocated string with hex values. You're responsible for deleting it.
     */
    static char* byteArrayToStr(
		const byte* data, 
		const uint16_t length
	);

    /**
     * Converts given byte array into the string of hex values with optional uppercase formatting.
     * @param data Bytes to convert.
     * @param length Number of bytes to convert.
     * @param uppercase If true, the output will be uppercase; otherwise, it will be lowercase.
     * @return Dynamically allocated string with hex values. You're responsible for deleting it.
     */
    static char* byteArrayToStr(
		const byte* data, 
		const uint16_t length, 
		bool uppercase
	);
};

#endif
