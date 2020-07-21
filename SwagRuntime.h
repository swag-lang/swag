#pragma once

#ifdef _MSC_VER
#define SWAG_IMPORT extern "C" __declspec(dllimport)
#endif

SWAG_IMPORT void*    swag_runtime_malloc(uint64_t size);
SWAG_IMPORT void*    swag_runtime_realloc(void* addr, uint64_t size);
SWAG_IMPORT void     swag_runtime_free(void* addr);
SWAG_IMPORT void*    swag_runtime_memcpy(void* mem1, const void* mem2, uint64_t size);
SWAG_IMPORT void*    swag_runtime_memset(void* mem, int32_t value, uint64_t size);
SWAG_IMPORT int32_t  swag_runtime_memcmp(const void* mem1, const void* mem2, uint64_t size);
SWAG_IMPORT void     swag_runtime_exit(int32_t exitCode);
SWAG_IMPORT void     swag_runtime_print_n(const char* message, int len);
SWAG_IMPORT int32_t  swag_runtime_strlen(const char* message);
SWAG_IMPORT void     swag_runtime_print(const char* message);
SWAG_IMPORT void     swag_runtime_print_i64(int64_t value);
SWAG_IMPORT void     swag_runtime_print_f64(double value);
SWAG_IMPORT void     swag_runtime_assert(bool expr, const char* file, int line, const char* msg);
SWAG_IMPORT bool     swag_runtime_strcmp(const char* str1, const char* str2, uint32_t num);
SWAG_IMPORT void*    swag_runtime_loadDynamicLibrary(const char* name);
SWAG_IMPORT uint32_t swag_runtime_tlsAlloc();
SWAG_IMPORT void     swag_runtime_tlsSetValue(uint32_t id, void* value);
SWAG_IMPORT void*    swag_runtime_tlsGetValue(uint32_t id);
SWAG_IMPORT void     swag_runtime_convertArgcArgv(void* dest, int argc, char* argv[]);