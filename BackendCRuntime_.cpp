#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"
#include "stdint.h"

static const char* g_RuntimeH = R"(
#ifndef __SWAG_RUNTIME_DEFINED__
#define __SWAG_RUNTIME_DEFINED__

typedef signed char			swag_int8_t;
typedef short				swag_int16_t;
typedef int					swag_int32_t;
typedef long long			swag_int64_t;
typedef unsigned char		swag_uint8_t;
typedef unsigned char		swag_bool;
typedef unsigned short		swag_uint16_t;
typedef unsigned int		swag_uint32_t;
typedef unsigned int		swag_char32_t;
typedef unsigned long long	swag_uint64_t;
typedef float				swag_float32_t;
typedef double				swag_float64_t;

#ifdef __cplusplus
#define SWAG_EXTERN extern "C"
#else
#define SWAG_EXTERN extern
#endif

#if defined(SWAG_IMPORT)
#define SWAG_IMPEXP __declspec(dllimport)
#elif defined(SWAG_EXPORT)
#define SWAG_IMPEXP __declspec(dllexport)
#else
#define SWAG_IMPEXP
#endif

#endif

)";

static const char* g_RuntimeC = R"(
typedef union __register {
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
    swag_bool		b;
} __register;

)";

static const char* g_Intrinsics= R"(
static void __print(const char* message) 
{ 
	printf(message); 
}

static void __print_i32(swag_int32_t value)   
{ 
	char buf[100]; 
	snprintf(buf, 100, "%d", value); 
	__print(buf);
}

static void __print_i64(swag_int64_t value)   
{ 
	char buf[100]; 
	snprintf(buf, 100, "%lld", value); 
	__print(buf);
}

static void __print_f32(swag_float32_t value)
{ 
	char buf[100]; 
	snprintf(buf, 100, "%f", value); 
	__print(buf);
}

static void __print_f64(swag_float64_t value)
{ 
	char buf[100]; 
	snprintf(buf, 100, "%lf", value); 
	__print(buf);
}

static void __assert(swag_bool expr, const char* file, int line, const char* msg)
{
	if(expr) 
		return;
	__print(file); 
	__print(":"); 
	__print_i32(line);
	if(msg)
		__print(msg);
	else	
		__print(": error: intrinsic assertion failed in native code\n");
	exit(-1);
}

static swag_bool __strcmp(const char* str1, const char* str2)
{
	return !strcmp(str1, str2);
}

static void __memcpy(void* dst, void* src, swag_uint32_t size)
{
	memcpy(dst, src, size);
}

)";

bool BackendC::emitRuntime()
{
    emitSeparator(bufferH, "RUNTIME");
    bufferH.addString(g_RuntimeH);
    emitSeparator(bufferC, "RUNTIME");
    bufferC.addString(g_RuntimeC);
    emitSeparator(bufferC, "INTRINSICS");
    bufferC.addString(g_Intrinsics);
    return true;
}
