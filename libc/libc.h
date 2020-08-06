#pragma once
#define EXTERN_C extern "C"
#define MAX_COMMAND_ARGUMENTS 512

#ifdef _WIN32
#include "win32/win32.h"
#endif
#include "stdint.h"

EXTERN_C void*               memcpy(void* dst, const void* src, swag_runtime_size_t size);
EXTERN_C int                 memcmp(const void* b1, const void* b2, swag_runtime_size_t n);
EXTERN_C swag_runtime_size_t strlen(const char*);
EXTERN_C char*               strcpy(char*, const char*);
EXTERN_C void                exit(swag_runtime_int32_t code);
EXTERN_C void*               malloc(swag_runtime_size_t);
EXTERN_C void                free(void*);
EXTERN_C void*               realloc(void*, swag_runtime_size_t);

#define isspace(__c) (__c == ' ' || __c == '\t')