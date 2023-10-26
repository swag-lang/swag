#pragma once
const uint32_t OPFLAG_WRITE_A       = 0x00000001;
const uint32_t OPFLAG_WRITE_B       = 0x00000002;
const uint32_t OPFLAG_WRITE_C       = 0x00000004;
const uint32_t OPFLAG_WRITE_D       = 0x00000008;
const uint32_t OPFLAG_WRITE_MASK    = OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_C | OPFLAG_WRITE_D;
const uint32_t OPFLAG_READ_A        = 0x00000010;
const uint32_t OPFLAG_READ_B        = 0x00000020;
const uint32_t OPFLAG_READ_C        = 0x00000040;
const uint32_t OPFLAG_READ_D        = 0x00000080;
const uint32_t OPFLAG_READ_MASK     = OPFLAG_READ_A | OPFLAG_READ_B | OPFLAG_READ_C | OPFLAG_READ_D;
const uint32_t OPFLAG_READ_VAL32_A  = 0x00000100;
const uint32_t OPFLAG_READ_VAL32_B  = 0x00000200;
const uint32_t OPFLAG_READ_VAL32_C  = 0x00000400;
const uint32_t OPFLAG_READ_VAL32_D  = 0x00000800;
const uint32_t OPFLAG_READ_VAL64_A  = 0x00001000;
const uint32_t OPFLAG_READ_VAL64_B  = 0x00002000;
const uint32_t OPFLAG_READ_VAL64_C  = 0x00004000;
const uint32_t OPFLAG_READ_VAL64_D  = 0x00008000;
const uint32_t OPFLAG_IMM_A         = 0x00010000;
const uint32_t OPFLAG_IMM_B         = 0x00020000;
const uint32_t OPFLAG_IMM_C         = 0x00040000;
const uint32_t OPFLAG_IMM_D         = 0x00080000;
const uint32_t OPFLAG_UNPURE        = 0x00100000;
const uint32_t OPFLAG_IS_JUMP       = 0x00200000;
const uint32_t OPFLAG_IS_MEMCPY     = 0x00400000;
const uint32_t OPFLAG_IS_FACTOR     = 0x00800000;
const uint32_t OPFLAG_IS_JUMPDYN    = 0x01000000;
const uint32_t OPFLAG_IS_REGONLY    = 0x02000000;
const uint32_t OPFLAG_IS_PUSH_PARAM = 0x04000000;
const uint32_t OPFLAG_IS_CALL       = 0x08000000;
const uint32_t OPFLAG_IS_8B         = 0x10000000;
const uint32_t OPFLAG_IS_16B        = 0x20000000;
const uint32_t OPFLAG_IS_32B        = 0x40000000;
const uint32_t OPFLAG_IS_64B        = 0x80000000;

enum class ByteCodeOp : uint16_t
{
#define BYTECODE_OP(__op, __flags, __dis) __op,
#include "ByteCodeOpList.h"
};

struct ByteCodeOpDesc
{
    uint32_t    flags;
    uint32_t    nameLen;
    const char* name;
    const char* display;
};

extern ByteCodeOpDesc g_ByteCodeOpDesc[];