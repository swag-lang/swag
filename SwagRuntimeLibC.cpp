#include <stdint.h>
#include "SwagRuntimeLibC.h"

// Only contains function that are not part of ucrt.lib, as for now we use the windows crt and
// only compile for windows

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* memcpy(void* dst, const void* src, size_t size)
{
    char*       _dst = (char*) dst;
    const char* _src = (char*) src;
    for (size_t i = 0; i < size; i++)
        _dst[i] = _src[i];
    return dst;
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C int memcmp(const void* b1, const void* b2, size_t n)
{
    const unsigned char* p1 = (const unsigned char*) b1;
    const unsigned char* p2 = (const unsigned char*) b2;

    if (!n)
        return 0;
    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
            return p1[i] - p2[i];
    }
    return 0;
}