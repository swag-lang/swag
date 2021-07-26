#pragma once
#include "Utf8.h"
#include "Assert.h"

struct Global
{
    void        setup();
    atomic<int> uniqueID        = 0;
    atomic<int> compilerAllocTh = 0;
    bool        exiting         = false;
};

extern void* doForeignLambda(void* ptr);
extern bool  isForeignLambda(void* ptr);
extern void* undoForeignLambda(void* ptr);
extern void* doByteCodeLambda(void* ptr);
extern void* undoByteCodeLambda(void* ptr);
extern bool  isByteCodeLambda(void* ptr);

inline bool isPowerOfTwo(size_t v)
{
    if (!v)
        return false;
    return (v & (v - 1)) == 0;
}

extern struct Global g_Global;

#define SWAG_CHECK(__expr) \
    do                     \
    {                      \
        if (!(__expr))     \
            return false;  \
    } while (0)
#define SWAG_VERIFY(__expr, __err) \
    do                             \
    {                              \
        if (!(__expr))             \
        {                          \
            __err;                 \
            return false;          \
        }                          \
    } while (0)
