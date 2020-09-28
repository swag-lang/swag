#pragma once
#include "libc/stdint.h"

extern "C" SwagU64 swag_runtime_tlsAlloc();
extern "C" void    swag_runtime_tlsSetValue(SwagU64 id, void* value);
extern "C" void*   swag_runtime_tlsGetValue(SwagU64 id);
extern "C" void*   swag_runtime_memcpy(void* destination, const void* source, size_t size);

