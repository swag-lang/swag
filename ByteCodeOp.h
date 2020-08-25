#pragma once
static const uint32_t OPFLAG_WRITE_A = 0x00000001;
static const uint32_t OPFLAG_WRITE_B = 0x00000002;
static const uint32_t OPFLAG_WRITE_C = 0x00000004;
static const uint32_t OPFLAG_WRITE_D = 0x00000008;
static const uint32_t OPFLAG_READ_A  = 0x00000010;
static const uint32_t OPFLAG_READ_B  = 0x00000020;
static const uint32_t OPFLAG_READ_C  = 0x00000040;
static const uint32_t OPFLAG_READ_D  = 0x00000080;

static const uint32_t OPFLAG_READ_VAL32_A = 0x00000100;
static const uint32_t OPFLAG_READ_VAL32_B = 0x00000200;
static const uint32_t OPFLAG_READ_VAL32_C = 0x00000400;
static const uint32_t OPFLAG_READ_VAL32_D = 0x00000800;
static const uint32_t OPFLAG_READ_VAL64_A = 0x00001000;
static const uint32_t OPFLAG_READ_VAL64_B = 0x00002000;
static const uint32_t OPFLAG_READ_VAL64_C = 0x00004000;
static const uint32_t OPFLAG_READ_VAL64_D = 0x00008000;

static const uint32_t OPFLAG_WRITE_VAL32_A = 0x00001000;
static const uint32_t OPFLAG_WRITE_VAL32_B = 0x00002000;
static const uint32_t OPFLAG_WRITE_VAL32_C = 0x00004000;
static const uint32_t OPFLAG_WRITE_VAL32_D = 0x00008000;
static const uint32_t OPFLAG_WRITE_VAL64_A = 0x00010000;
static const uint32_t OPFLAG_WRITE_VAL64_B = 0x00020000;
static const uint32_t OPFLAG_WRITE_VAL64_C = 0x00040000;
static const uint32_t OPFLAG_WRITE_VAL64_D = 0x00080000;

static const uint32_t OPFLAG_IMM32_A = 0x00100000;
static const uint32_t OPFLAG_IMM32_B = 0x00200000;
static const uint32_t OPFLAG_IMM32_C = 0x00400000;
static const uint32_t OPFLAG_IMM32_D = 0x00800000;

enum class ByteCodeOp : uint16_t
{
#define BYTECODE_OP(__op, __flags) __op,
#include "ByteCodeOpList.h"
};

extern const char* g_ByteCodeOpNames[];
extern int         g_ByteCodeOpNamesLen[];
extern uint32_t    g_ByteCodeOpFlags[];