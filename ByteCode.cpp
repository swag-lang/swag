#include "pch.h"

#define BYTECODE_OP(__op) #__op,

const char* g_ByteCodeOpNames[] = {
#include "ByteCodeOpList.h"
};