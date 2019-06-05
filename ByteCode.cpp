#include "pch.h"
#include "ByteCode.h"

#define BYTECODE_OP(__op) #__op,

const char* g_ByteCodeOpNames[] = {
#include "ByteCodeOpList.h"
};

Pool<ByteCode> g_Pool_byteCode;