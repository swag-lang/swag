#pragma once
// clang-format off

void swagAssert(const char* expr, const char* file, int line);

#ifdef SWAG_HAS_ASSERT

#define SWAG_ASSERT(__expr)                          \
    {                                                \
        if (!(__expr))                               \
        {                                            \
            swagAssert(#__expr, __FILE__, __LINE__); \
        }                                            \
    }

#define SWAG_IF_ASSERT(__expr) __expr

#else // !SWAG_HAS_ASSERT

#define SWAG_ASSERT(__expr) {}
#define SWAG_IF_ASSERT(__expr)

#endif // SWAG_HAS_ASSERT

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
