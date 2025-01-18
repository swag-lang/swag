#pragma once

using OpFlags                         = Flags<uint64_t>;
constexpr OpFlags OPFLAG_WRITE_A      = 0x0000000000000001;
constexpr OpFlags OPFLAG_WRITE_B      = 0x0000000000000002;
constexpr OpFlags OPFLAG_WRITE_C      = 0x0000000000000004;
constexpr OpFlags OPFLAG_WRITE_D      = 0x0000000000000008;
constexpr OpFlags OPFLAG_WRITE_MASK   = OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_C | OPFLAG_WRITE_D;
constexpr OpFlags OPFLAG_READ_A       = 0x0000000000000010;
constexpr OpFlags OPFLAG_READ_B       = 0x0000000000000020;
constexpr OpFlags OPFLAG_READ_C       = 0x0000000000000040;
constexpr OpFlags OPFLAG_READ_D       = 0x0000000000000080;
constexpr OpFlags OPFLAG_READ_MASK    = OPFLAG_READ_A | OPFLAG_READ_B | OPFLAG_READ_C | OPFLAG_READ_D;
constexpr OpFlags OPFLAG_READ_VAL32_A = 0x0000000000000100;
constexpr OpFlags OPFLAG_READ_VAL32_B = 0x0000000000000200;
constexpr OpFlags OPFLAG_READ_VAL32_C = 0x0000000000000400;
constexpr OpFlags OPFLAG_READ_VAL32_D = 0x0000000000000800;
constexpr OpFlags OPFLAG_READ_VAL64_A = 0x0000000000001000;
constexpr OpFlags OPFLAG_READ_VAL64_B = 0x0000000000002000;
constexpr OpFlags OPFLAG_READ_VAL64_C = 0x0000000000004000;
constexpr OpFlags OPFLAG_READ_VAL64_D = 0x0000000000008000;
constexpr OpFlags OPFLAG_IMM_A        = 0x0000000000010000;
constexpr OpFlags OPFLAG_IMM_B        = 0x0000000000020000;
constexpr OpFlags OPFLAG_IMM_C        = 0x0000000000040000;
constexpr OpFlags OPFLAG_IMM_D        = 0x0000000000080000;
constexpr OpFlags OPFLAG_NOT_PURE     = 0x0000000000100000;
constexpr OpFlags OPFLAG_JUMP         = 0x0000000000200000;
constexpr OpFlags OPFLAG_MEMCPY       = 0x0000000000400000;
constexpr OpFlags OPFLAG_FACTOR       = 0x0000000000800000;
constexpr OpFlags OPFLAG_JUMP_DYN     = 0x0000000001000000;
constexpr OpFlags OPFLAG_REG_ONLY     = 0x0000000002000000;
constexpr OpFlags OPFLAG_PUSH_PARAM   = 0x0000000004000000;
constexpr OpFlags OPFLAG_8            = 0x0000000008000000;
constexpr OpFlags OPFLAG_16           = 0x0000000010000000;
constexpr OpFlags OPFLAG_32           = 0x0000000020000000;
constexpr OpFlags OPFLAG_64           = 0x0000000040000000;
constexpr OpFlags OPFLAG_LOCAL_CALL   = 0x0000000080000000;
constexpr OpFlags OPFLAG_LAMBDA_CALL  = 0x0000000100000000;
constexpr OpFlags OPFLAG_FOREIGN_CALL = 0x0000000200000000;
constexpr OpFlags OPFLAG_CALL         = OPFLAG_LOCAL_CALL | OPFLAG_LAMBDA_CALL | OPFLAG_FOREIGN_CALL;
constexpr OpFlags OPFLAG_REG_READ     = 0x0000000400000000;

enum class ByteCodeOp : uint16_t
{
#define BYTECODE_OP(__op, __flags, __dis) __op,
#include "ByteCodeOpList.h"
};

struct ByteCodeOpDesc
{
    OpFlags     flags;
    uint32_t    nameLen;
    const char* name;
    const char* display;
};

extern ByteCodeOpDesc g_ByteCodeOpDesc[];
