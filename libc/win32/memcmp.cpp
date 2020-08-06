#ifdef _WIN32
#include "../libc.h"

EXTERN_C int memcmp(const void* b1, const void* b2, size_t n)
{
    const unsigned char* p1 = (const unsigned char*)b1;
    const unsigned char* p2 = (const unsigned char*)b2;

    if (!n)
        return 0;
    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
            return p1[i] - p2[i];
    }
    return 0;
}

#endif