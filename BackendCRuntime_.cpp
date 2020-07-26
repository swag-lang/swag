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
typedef signed char			swag_int8_t;
typedef short				swag_int16_t;
typedef int					swag_int32_t;
typedef long long			swag_int64_t;
typedef unsigned char		swag_uint8_t;
typedef _Bool				swag_bool_t;
typedef unsigned short		swag_uint16_t;
typedef unsigned int		swag_uint32_t;
typedef unsigned int		swag_char32_t;
typedef unsigned long long	swag_uint64_t;
typedef float				swag_float32_t;
typedef double				swag_float64_t;

#ifdef _WIN32
typedef swag_uint32_t swag_tls_id_t;
#endif

typedef union swag_register_t {
    swag_uint8_t*	p;
    swag_uint64_t	u64;
    swag_int64_t	s64;
    swag_float64_t	f64;
    swag_float32_t	f32;
    swag_char32_t	ch;
    swag_uint32_t	u32;
    swag_int32_t	s32;
    swag_uint16_t	u16;
    swag_int16_t	s16;
    swag_uint8_t	u8;
    swag_int8_t		s8;
    swag_bool_t		b;
} swag_register_t;

typedef struct swag_interface_t
{
    void* data;
    void* itable;
} swag_interface_t;

typedef void(*swag_allocator_t)(swag_register_t*,swag_register_t*);
typedef void(*swag_bytecoderun_t)(void*, ...);

typedef struct swag_context_t {
	swag_interface_t allocator;
} swag_context_t;

typedef struct swag_slice_t {
	void*			addr;
	swag_uint64_t	count;
} swag_slice_t;

typedef struct swag_process_infos_t {
	swag_slice_t		arguments;
	swag_tls_id_t		contextTlsId;
	swag_context_t*		defaultContext;
	swag_bytecoderun_t	byteCodeRun;
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

SWAG_IMPORT void*           swag_runtime_loadDynamicLibrary(const char*);
SWAG_IMPORT swag_uint32_t   swag_runtime_tlsAlloc();
SWAG_IMPORT void            swag_runtime_tlsSetValue(swag_uint32_t, void*);
SWAG_IMPORT void*           swag_runtime_tlsGetValue(swag_uint32_t);
SWAG_IMPORT void*           swag_runtime_malloc(swag_uint64_t);
SWAG_IMPORT void*           swag_runtime_realloc(void*, swag_uint64_t);
SWAG_IMPORT void            swag_runtime_free(void*);
SWAG_IMPORT void*           swag_runtime_memcpy(void*, const void*, swag_uint64_t);
SWAG_IMPORT void*           swag_runtime_memset(void*, swag_int32_t, swag_uint64_t);
SWAG_IMPORT swag_int32_t    swag_runtime_memcmp(const void*, const void*, swag_uint64_t);
SWAG_IMPORT void            swag_runtime_exit(swag_int32_t);
SWAG_IMPORT void            swag_runtime_print_n(const char* message, int len);
SWAG_IMPORT void            swag_runtime_print(const char* message);
SWAG_IMPORT void            swag_runtime_print_i64(swag_int64_t value);
SWAG_IMPORT void            swag_runtime_print_f64(swag_float64_t value);
SWAG_IMPORT void            swag_runtime_assert(swag_bool_t expr, const char* file, int line, const char* msg, swag_bool_t devMode);
SWAG_IMPORT swag_bool_t     swag_runtime_strcmp(const char* str1, const char* str2, swag_uint32_t num);
SWAG_IMPORT swag_int32_t    swag_runtime_strlen(const char* message);
SWAG_IMPORT void            swag_runtime_convertArgcArgv(void* dest, int argc, char* argv[]);
)";

bool BackendC::emitRuntime(OutputFile& bufferC, int precompileIndex)
{
    if (g_CommandLine.devMode)
    {
        CONCAT_FIXED_STR(bufferC, "/* FILE GENERATED WITH --DEVMODE:true */\n");
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
