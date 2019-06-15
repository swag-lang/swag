#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"
#include "stdint.h"

const char* g_RuntimeH = R"(
/****************************** RUNTIME *******************************/
#ifndef SWAG_RUNTIME_DEFINED
#define SWAG_RUNTIME_DEFINED

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

#endif

)";

const char* g_RuntimeC = R"(
/****************************** RUNTIME *******************************/
typedef union __register {
    void*			pointer;
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

static void __print(const char* message) 
{ 
	printf(message); 
}

static void __print_i32(swag_int32_t value)   
{ 
	char buf[100]; 
	snprintf(buf, 100, "%d", value); 
}

static void __print_i64(swag_int64_t value)   
{ 
	char buf[100]; 
	snprintf(buf, 100, "%lld", value); 
}

static void __print_f32(swag_float32_t value)
{ 
	char buf[100]; 
	snprintf(buf, 100, "%f", value); 
}

static void __print_f64(swag_float64_t value)
{ 
	char buf[100]; 
	snprintf(buf, 100, "%lf", value); 
}

static void __assert(swag_bool expr, const char* file, int line)
{
	if(expr) 
		return;
	__print(file); 
	__print(":"); 
	__print_i32(line); 
	__print(": error: intrinsic assertion failed in native code\n");
	exit(-1);
}

)";

bool BackendC::emitRuntime()
{
	outputH.addString(g_RuntimeH);
    outputC.addString(g_RuntimeC);
    return true;
}
