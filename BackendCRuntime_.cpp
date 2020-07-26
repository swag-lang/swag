#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerClClangWin32.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"
#include "stdint.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Should match runtime in Context.h
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static constexpr const char* g_RuntimeC = R"(
typedef signed char __i8_t;
typedef short __i16_t;
typedef int __i32_t;
typedef long long __i64_t;
typedef unsigned char __ui8_t;
typedef _Bool __b_t;
typedef unsigned short __ui16_t;
typedef unsigned int __ui32_t;
typedef unsigned int __ch_t;
typedef unsigned long long __ui64_t;
typedef float __f32_t;
typedef double __f64_t;

#ifdef _WIN32
typedef __ui32_t swag_tls_id_t;
#endif

typedef union __r_t {
__ui8_t* p;
__ui64_t u64;
__i64_t	s64;
__f64_t	f64;
__f32_t	f32;
__ch_t ch;
__ui32_t u32;
__i32_t	s32;
__ui16_t u16;
__i16_t	s16;
__ui8_t	u8;
__i8_t s8;
__b_t b;
} __r_t;

typedef struct swag_interface_t
{
void*data;
void*itable;
} swag_interface_t;

typedef void(*swag_allocator_t)(__r_t*,__r_t*);
typedef void(*swag_bytecoderun_t)(void*,...);

typedef struct swag_context_t {
swag_interface_t allocator;
} swag_context_t;

typedef struct swag_slice_t {
void* addr;
__ui64_t count;
} swag_slice_t;

typedef struct swag_process_infos_t {
swag_slice_t arguments;
swag_tls_id_t contextTlsId;
swag_context_t* defaultContext;
swag_bytecoderun_t byteCodeRun;
} swag_process_infos_t;
)";

static constexpr const char* g_SwagRuntime = R"(
/* Visual studio */
#ifdef _MSC_VER
#define SWAG_IMPORT __declspec(dllimport)
#define SWAG_EXPORT __declspec(dllexport)
#else
#define SWAG_IMPORT 
#define SWAG_EXPORT
#endif

SWAG_IMPORT void* swag_runtime_loadDynamicLibrary(const char*);
SWAG_IMPORT __ui32_t swag_runtime_tlsAlloc();
SWAG_IMPORT void swag_runtime_tlsSetValue(__ui32_t,void*);
SWAG_IMPORT void*swag_runtime_tlsGetValue(__ui32_t);
SWAG_IMPORT void*swag_runtime_malloc(__ui64_t);
SWAG_IMPORT void*swag_runtime_realloc(void*,__ui64_t);
SWAG_IMPORT void swag_runtime_free(void*);
SWAG_IMPORT void*swag_runtime_memcpy(void*,const void*,__ui64_t);
SWAG_IMPORT void*swag_runtime_memset(void*,__i32_t,__ui64_t);
SWAG_IMPORT __i32_t swag_runtime_memcmp(const void*,const void*,__ui64_t);
SWAG_IMPORT void swag_runtime_exit(__i32_t);
SWAG_IMPORT void swag_runtime_print_n(const char*,int);
SWAG_IMPORT void swag_runtime_print(const char*);
SWAG_IMPORT void swag_runtime_print_i64(__i64_t);
SWAG_IMPORT void swag_runtime_print_f64(__f64_t);
SWAG_IMPORT void swag_runtime_assert(__b_t,const char*,int,const char*,__b_t);
SWAG_IMPORT __b_t swag_runtime_strcmp(const char*,const char*,__ui32_t);
SWAG_IMPORT __i32_t swag_runtime_strlen(const char*);
SWAG_IMPORT void swag_runtime_convertArgcArgv(void*,int,char*[]);
)";

bool BackendC::emitRuntime(OutputFile& bufferC, int precompileIndex)
{
    CONCAT_FIXED_STR(bufferC, g_RuntimeC);
    CONCAT_FIXED_STR(bufferC, g_SwagRuntime);

    if (precompileIndex == 0)
    {
        CONCAT_FIXED_STR(bufferC, "swag_process_infos_t __process_infos = {0};\n");
    }
    else
    {
        CONCAT_FIXED_STR(bufferC, "extern swag_process_infos_t __process_infos;\n");
    }

    return true;
}
