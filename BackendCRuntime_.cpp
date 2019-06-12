#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"

const char* g_RuntimeC = R"(
#include <stdint.h>
#include <codecvt>
#include <iostream>
#include <string>
using namespace std;

union __register {
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
};

void __print(const char* message)
{
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> convert;
    wcout << convert.from_bytes(message);
}

void __assert(bool expr, const char* file, int line)
{
	if(expr) return;
	__print(file); __print(":"); __print(to_string(line).c_str()); __print(": error: "); __print("intrinsic assertion failed\n");
	exit(-1);
}

)";

bool BackendC::emitRuntime()
{
	outputC.addString(g_RuntimeC);
    return true;
}
