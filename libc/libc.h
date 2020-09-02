#pragma once
#define EXTERN_C extern "C"
#define MAX_COMMAND_ARGUMENTS 512

#ifdef _WIN32
#include "win32/win32.h"
#endif
#include "stdint.h"

EXTERN_C void*     memcpy(void* dst, const void* src, SwagSizeT size);
EXTERN_C int       memcmp(const void* b1, const void* b2, SwagSizeT n);
EXTERN_C SwagSizeT strlen(const char*);
EXTERN_C char*     strcpy(char*, const char*);
EXTERN_C void      exit(SwagS32 code);
EXTERN_C void*     malloc(SwagSizeT);
EXTERN_C void      free(void*);
EXTERN_C void*     realloc(void*, SwagSizeT);

#define isspace(__c) (__c == ' ' || __c == '\t')