#pragma once
#include "Utf8.h"
#include "Assert.h"

struct Global
{
    void        setup();
    atomic<int> uniqueID = 0;
};

static const uint64_t SWAG_LAMBDA_BC_MARKER      = 0x8000000000000000;
static const uint64_t SWAG_LAMBDA_FOREIGN_MARKER = 0x0400000000000000;

extern Utf8   format(const char* format, ...);
extern string normalizePath(const fs::path& path);
extern void   tokenize(const char* str, char c, vector<Utf8>& tokens);
extern void   tokenizeBlanks(const char* str, vector<Utf8>& tokens);
extern void*  doByteCodeLambda(void* ptr);
extern void*  undoByteCodeLambda(void* ptr);
extern bool   isByteCodeLambda(void* ptr);

extern Utf8 toStringF64(double v);

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
