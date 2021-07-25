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

extern Utf8     format(const char* format, ...);
extern string   normalizePath(const fs::path& path);
extern void     tokenize(const char* str, char c, vector<Utf8>& tokens);
extern void     tokenizeBlanks(const char* str, vector<Utf8>& tokens);
extern void*    doForeignLambda(void* ptr);
extern bool     isForeignLambda(void* ptr);
extern void*    undoForeignLambda(void* ptr);
extern void*    doByteCodeLambda(void* ptr);
extern void*    undoByteCodeLambda(void* ptr);
extern bool     isByteCodeLambda(void* ptr);
extern Utf8     toStringF64(double v);
extern Utf8     toNiceSize(size_t size);
extern uint32_t fuzzyCompare(const Utf8& str1, const Utf8& str2);

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
