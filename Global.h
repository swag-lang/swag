#pragma once
#include "Utf8.h"

struct Global
{
    void        setup();
    atomic<int> uniqueID;
};

extern Utf8          format(const char* format, ...);
extern struct Global g_Global;

#define SWAG_CHECK(__expr) \
    {                      \
        if (!(__expr))     \
            return false;  \
    }
#define SWAG_VERIFY(__expr, __err) \
    {                              \
        if (!(__expr))             \
        {                          \
            __err;                 \
            return false;          \
        }                          \
    }
