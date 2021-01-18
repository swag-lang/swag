#pragma once
extern void swag_assert(const char* expr, const char* file, int line);

#define SWAG_ASSERT(__expr)                           \
    {                                                 \
        if (!(__expr))                                \
        {                                             \
            swag_assert(#__expr, __FILE__, __LINE__); \
        }                                             \
    }
