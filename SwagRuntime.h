#pragma once

void*    swag_runtime_malloc(uint64_t size);
void*    swag_runtime_realloc(void* addr, uint64_t size);
void     swag_runtime_free(void* addr);
void*    swag_runtime_memcpy(void* mem1, const void* mem2, uint64_t size);
void*    swag_runtime_memset(void* mem, int32_t value, uint64_t size);
int32_t  swag_runtime_memcmp(const void* mem1, const void* mem2, uint64_t size);
void     swag_runtime_exit(int32_t exitCode);
void     swag_runtime_print_n(const char* message, int len);
int32_t  swag_runtime_strlen(const char* message);
void     swag_runtime_print(const char* message);
void     swag_runtime_print_i64(int64_t value);
void     swag_runtime_print_f64(double value);
bool     swag_runtime_strcmp(const char* str1, const char* str2, uint32_t num);
void*    swag_runtime_loadDynamicLibrary(const char* name);
uint32_t swag_runtime_tlsAlloc();
void     swag_runtime_tlsSetValue(uint32_t id, void* value);
void*    swag_runtime_tlsGetValue(uint32_t id);
void     swag_runtime_convertArgcArgv(void* dest, int argc, char* argv[]);

static uint32_t SWAG_ASSERT_DEVMODE = 0x00000001;
static uint32_t SWAG_ASSERT_RETURN  = 0x00000002;

void swag_runtime_assert(bool expr, const char* file, int line, const char* msg, uint32_t flags);
