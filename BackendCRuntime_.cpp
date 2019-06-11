#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"

const char* g_RuntimeC = R"(
#include <stdint.h>
#include <locale>
#include <codecvt>
#include <iostream>
using namespace std;

union __Register {
    uint64_t u64;
    int64_t  s64;
    uint32_t u32;
    int32_t  s32;
    uint16_t u16;
    int16_t  s16;
    uint8_t  u8;
    int8_t   s8;
    float    f32;
    double   f64;
    char32_t ch;
    bool     b;
    void*    pointer;
};

void __print(const char* message)
{
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> convert;
    wcout << convert.from_bytes(message);
}

)";

bool BackendC::emitRuntime()
{
	outputC.addString(g_RuntimeC);
    return true;
}
