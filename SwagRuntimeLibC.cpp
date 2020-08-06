#include <stdint.h>
#include "SwagRuntimeLibC.h"

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

#if 0

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C size_t strlen(const char* src)
{
    const char* end = src;
    while (*end != '\0')
        ++end;
    return end - src;
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* memset(void* dst, int val, size_t size)
{
    char* realdst = (char*) dst;
    for (size_t i = 0; i < size; i++)
        realdst[i] = (char) val;
    return dst;
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void exit(int code)
{
#ifdef _WIN32
    extern void ExitProcess(int);
    ExitProcess(code);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* malloc(size_t len)
{
#ifdef _WIN32
    void* HeapAlloc(void*, int, size_t);
    void* GetProcessHeap();
    return HeapAlloc(GetProcessHeap(), 0, len);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void free(void* src)
{
#ifdef _WIN32
    int   HeapFree(void*, int, void*);
    void* GetProcessHeap();
    HeapFree(GetProcessHeap(), 0, src);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* realloc(void* src, size_t newSize)
{
#ifdef _WIN32
    void* HeapReAlloc(void*, int, void*, size_t);
    void* GetProcessHeap();
    return HeapReAlloc(GetProcessHeap(), 0, src, newSize);
#endif
}

#endif