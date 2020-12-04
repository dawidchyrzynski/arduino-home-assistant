#ifndef HAUTILS_H
#define HAUTILS_H

class HAUtils
{
public:
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
};

#endif
