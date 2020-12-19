#pragma once
#include "Utf8.h"
#include "Assert.h"

const uint32_t SWAG_LIMIT_ARRAY_SIZE    = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_COMPILER_LOAD = 0x7FFFFFFF;
const uint32_t SWAG_LIMIT_SEGMENT       = 0x7FFFFFFF;

struct Global
{
    void        setup();
    atomic<int> uniqueID = 0;
};

extern Utf8   format(const char* format, ...);
extern string normalizePath(const fs::path& path);
extern void   tokenize(const char* str, char c, vector<Utf8>& tokens);
extern void   tokenizeBlanks(const char* str, vector<Utf8>& tokens);
extern void*  doForeignLambda(void* ptr);
extern bool   isForeignLambda(void* ptr);
extern void*  undoForeignLambda(void* ptr);
extern void*  doByteCodeLambda(void* ptr);
extern void*  undoByteCodeLambda(void* ptr);
extern bool   isByteCodeLambda(void* ptr);
extern Utf8   toStringF64(double v);

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
