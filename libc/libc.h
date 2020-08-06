#pragma once
#define EXTERN_C extern "C"
#define MAX_COMMAND_ARGUMENTS 512

#ifdef _WIN32
#include "win32/win32.h"
#endif

EXTERN_C void*  memcpy(void* dst, const void* src, size_t size);
EXTERN_C int    memcmp(const void* b1, const void* b2, size_t n);
EXTERN_C size_t strlen(const char*);
EXTERN_C char*  strcpy(char*, const char*);
EXTERN_C void   exit(int code);
EXTERN_C void*  malloc(size_t);
EXTERN_C void   free(void*);
EXTERN_C void*  realloc(void*, size_t);

#define isspace(__c) (__c == ' ' || __c == '\t')