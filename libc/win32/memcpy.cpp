#ifdef _WIN32
#include "../libc.h"

EXTERN_C void* memcpy(void* dst, const void* src, size_t size)
{
    char*       _dst = (char*) dst;
    const char* _src = (char*) src;
    for (size_t i = 0; i < size; i++)
        _dst[i] = _src[i];
    return dst;
}

#endif