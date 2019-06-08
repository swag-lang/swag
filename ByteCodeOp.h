#pragma once
enum class ByteCodeOp : uint16_t
{
#define BYTECODE_OP(__op) __op,
#include "ByteCodeOpList.h"
};

extern const wchar_t* g_ByteCodeOpNames[];