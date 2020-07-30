#pragma once

extern "C" void*    swag_runtime_malloc(uint64_t size);
extern "C" void*    swag_runtime_realloc(void* addr, uint64_t size);
extern "C" void     swag_runtime_free(void* addr);
extern "C" void*    swag_runtime_memcpy(void* mem1, const void* mem2, uint64_t size);
extern "C" void*    swag_runtime_memset(void* mem, int32_t value, uint64_t size);
extern "C" int32_t  swag_runtime_memcmp(const void* mem1, const void* mem2, uint64_t size);
extern "C" void     swag_runtime_exit(int32_t exitCode);
extern "C" void     swag_runtime_print_n(const char* message, int len);
extern "C" int32_t  swag_runtime_strlen(const char* message);
extern "C" void     swag_runtime_print(const char* message);
extern "C" void     swag_runtime_print_i64(int64_t value);
extern "C" void     swag_runtime_print_f64(double value);
extern "C" bool     swag_runtime_strcmp(const char* str1, const char* str2, uint32_t num);
extern "C" void*    swag_runtime_loadDynamicLibrary(const char* name);
extern "C" uint32_t swag_runtime_tlsAlloc();
extern "C" void     swag_runtime_tlsSetValue(uint32_t id, void* value);
extern "C" void*    swag_runtime_tlsGetValue(uint32_t id);
extern "C" void     swag_runtime_convertArgcArgv(void* dest, int argc, char* argv[]);

static uint32_t SWAG_ASSERT_DEVMODE = 0x00000001;
static uint32_t SWAG_ASSERT_RETURN  = 0x00000002;

extern "C" void swag_runtime_assert(bool expr, const char* file, int line, const char* msg, uint32_t flags);
