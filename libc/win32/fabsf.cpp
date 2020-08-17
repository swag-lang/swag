#ifdef _WIN32
#ifndef SWAG_RUNTIME_COMPILER
#include "../libc.h"

EXTERN_C float fabsf(float x)
{
    extern double fabs(double);
    return (float) fabs(x);
}

#endif
#endif