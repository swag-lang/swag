#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"
#include "stdint.h"

const char* g_RuntimeC = R"(
/****************************** RUNTIME *******************************/
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned char      bool;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned int       char32_t;
typedef unsigned long long uint64_t;

typedef union __register {
    void*    pointer;
    uint64_t u64;
    int64_t  s64;
    double   f64;
    float    f32;
    char32_t ch;
    uint32_t u32;
    int32_t  s32;
    uint16_t u16;
    int16_t  s16;
    uint8_t  u8;
    int8_t   s8;
    bool     b;
} __register;

void __print(const char* message) 
{ 
	printf(message); 
}

void __print_i32(int32_t value)   
{ 
	char buf[100]; 
	snprintf(buf, 100, "%d", value); 
}

void __print_i64(int64_t value)   
{ 
	char buf[100]; 
	snprintf(buf, 100, "%lld", value); 
}

void __print_f32(float value)
{ 
	char buf[100]; 
	snprintf(buf, 100, "%f", value); 
}

void __print_f64(float value)
{ 
	char buf[100]; 
	snprintf(buf, 100, "%lf", value); 
}

void __assert(bool expr, const char* file, int line)
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
    outputC.addString(g_RuntimeC);
    return true;
}
