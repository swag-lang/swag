#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"
#include "stdint.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Should match runtime in Context.h
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static constexpr const char* g_RuntimeC = R"(
typedef signed char __s8_t;
typedef short __s16_t;
typedef int __s32_t;
typedef long long __s64_t;
typedef unsigned char __u8_t;
typedef _Bool __b_t;
typedef unsigned short __u16_t;
typedef unsigned int __u32_t;
typedef unsigned int __ch_t;
typedef unsigned long long __u64_t;
typedef float __f32_t;
typedef double __f64_t;

#ifdef _WIN32
typedef __u32_t swag_tls_id_t;
#endif

typedef union __r_t {
__u8_t* p;
__u64_t u64;
__s64_t	s64;
__f64_t	f64;
__f32_t	f32;
__ch_t ch;
__u32_t u32;
__s32_t	s32;
__u16_t u16;
__s16_t	s16;
__u8_t	u8;
__s8_t s8;
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
__u64_t count;
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

void* swag_runtime_loadDynamicLibrary(const void*);
__u32_t swag_runtime_tlsAlloc();
void swag_runtime_tlsSetValue(__u32_t,void*);
void*swag_runtime_tlsGetValue(__u32_t);
void*swag_runtime_malloc(__u64_t);
void*swag_runtime_realloc(void*,__u64_t);
void swag_runtime_free(void*);
void*swag_runtime_memcpy(void*,const void*,__u64_t);
void*swag_runtime_memset(void*,__s32_t,__u64_t);
__s32_t swag_runtime_memcmp(const void*,const void*,__u64_t);
void swag_runtime_exit(__s32_t);
void swag_runtime_print_n(const void*,int);
void swag_runtime_print(const void*);
void swag_runtime_print_i64(__s64_t);
void swag_runtime_print_f64(__f64_t);
void swag_runtime_assert(__b_t,const void*,int,const void*,__u32_t);
__b_t swag_runtime_strcmp(const void*,const void*,__u32_t);
__s32_t swag_runtime_strlen(const void*);
void swag_runtime_convertArgcArgv(void*,int,void*[]);
__b_t swag_runtime_comparetype(const void*,const void*);
void* swag_runtime_interfaceof(const void*,const void*);

__f32_t sqrtf(__f32_t);
__f64_t sqrt(__f64_t);
)";

bool BackendC::emitRuntime(OutputFile& bufferC, int precompileIndex)
{
    if (g_CommandLine.devMode)
    {
        CONCAT_FIXED_STR(bufferC, "/* File generated with --devmode:true */\n");
        CONCAT_FIXED_STR(bufferC, "#define SWAG_DEVMODE\n");
    }

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
