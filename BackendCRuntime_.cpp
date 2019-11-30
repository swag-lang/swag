#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
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
typedef unsigned char		swag_bool_t;
typedef unsigned short		swag_uint16_t;
typedef unsigned int		swag_uint32_t;
typedef unsigned int		swag_char32_t;
typedef unsigned long long	swag_uint64_t;
typedef float				swag_float32_t;
typedef double				swag_float64_t;

/* Visual studio */
#ifdef _MSC_VER
#define SWAG_IMPORT __declspec(dllimport)
#define SWAG_EXPORT __declspec(dllexport)
#else
#define SWAG_IMPORT 
#define SWAG_EXPORT
#endif

#ifdef _WIN32
SWAG_IMPORT void *				LoadLibraryA(const char*);
SWAG_IMPORT swag_uint32_t		TlsAlloc();
SWAG_IMPORT swag_int32_t		TlsSetValue(swag_uint32_t, void*);
SWAG_IMPORT void *				TlsGetValue(swag_uint32_t);
SWAG_IMPORT void				OutputDebugString(const char*);

#define __loadDynamicLibrary	LoadLibraryA
#define __tlsAlloc				TlsAlloc
#define __tlsSetValue			TlsSetValue
#define __tlsGetValue			TlsGetValue
typedef swag_uint32_t			swag_tls_id_t;
#endif

typedef union swag_register_t {
    swag_uint8_t*	pointer;
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
} swag_process_infos_t;

swag_process_infos_t __process_infos = {0};
)";

static constexpr const char* g_Intrinsics = R"(
#include "malloc.h"

static void __print_n(const char* message, int len) 
{ 
	if(!message) message = "<null>";
	printf("%.*s", len, message);
}

static void __print(const char* message) 
{ 
	if(!message) message = "<null>";
	printf(message);
}

static void __print_i64(swag_int64_t value)   
{ 
	char buf[100]; 
	snprintf(buf, 100, "%lld", value); 
	__print(buf);
}

static void __print_f64(swag_float64_t value)
{ 
	char buf[100]; 
	snprintf(buf, 100, "%lf", value); 
	__print(buf);
}

static void __assert(swag_bool_t expr, const char* file, int line, const char* msg)
{
	if(expr) 
		return;
	__print(file); 
	__print(":"); 
	__print_i64(line);
	if(msg)
		__print(msg);
	else	
		__print(": error: assertion failed in native code\n");
	exit(-1);
}

static swag_bool_t __strcmp(const char* str1, const char* str2, swag_uint32_t num)
{
	if(!str1 || !str2)
		return str1 == str2;
	return !strncmp(str1, str2, num);
}

)";

bool BackendC::emitRuntime()
{
    emitSeparator(bufferC, "RUNTIME");
    CONCAT_FIXED_STR(bufferC, g_RuntimeC);
    emitSeparator(bufferC, "INTRINSICS");
    CONCAT_FIXED_STR(bufferC, g_Intrinsics);
    return true;
}
