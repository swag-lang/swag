#pragma once
#include "Flags.h"

using OpFlags                          = Flags<uint32_t>;
constexpr OpFlags OPFLAG_WRITE_A       = 0x00000001;
constexpr OpFlags OPFLAG_WRITE_B       = 0x00000002;
constexpr OpFlags OPFLAG_WRITE_C       = 0x00000004;
constexpr OpFlags OPFLAG_WRITE_D       = 0x00000008;
constexpr OpFlags OPFLAG_WRITE_MASK    = OPFLAG_WRITE_A | OPFLAG_WRITE_B | OPFLAG_WRITE_C | OPFLAG_WRITE_D;
constexpr OpFlags OPFLAG_READ_A        = 0x00000010;
constexpr OpFlags OPFLAG_READ_B        = 0x00000020;
constexpr OpFlags OPFLAG_READ_C        = 0x00000040;
constexpr OpFlags OPFLAG_READ_D        = 0x00000080;
constexpr OpFlags OPFLAG_READ_MASK     = OPFLAG_READ_A | OPFLAG_READ_B | OPFLAG_READ_C | OPFLAG_READ_D;
constexpr OpFlags OPFLAG_READ_VAL32_A  = 0x00000100;
constexpr OpFlags OPFLAG_READ_VAL32_B  = 0x00000200;
constexpr OpFlags OPFLAG_READ_VAL32_C  = 0x00000400;
constexpr OpFlags OPFLAG_READ_VAL32_D  = 0x00000800;
constexpr OpFlags OPFLAG_READ_VAL64_A  = 0x00001000;
constexpr OpFlags OPFLAG_READ_VAL64_B  = 0x00002000;
constexpr OpFlags OPFLAG_READ_VAL64_C  = 0x00004000;
constexpr OpFlags OPFLAG_READ_VAL64_D  = 0x00008000;
constexpr OpFlags OPFLAG_IMM_A         = 0x00010000;
constexpr OpFlags OPFLAG_IMM_B         = 0x00020000;
constexpr OpFlags OPFLAG_IMM_C         = 0x00040000;
constexpr OpFlags OPFLAG_IMM_D         = 0x00080000;
constexpr OpFlags OPFLAG_NOT_PURE      = 0x00100000;
constexpr OpFlags OPFLAG_IS_JUMP       = 0x00200000;
constexpr OpFlags OPFLAG_IS_MEMCPY     = 0x00400000;
constexpr OpFlags OPFLAG_IS_FACTOR     = 0x00800000;
constexpr OpFlags OPFLAG_IS_JUMP_DYN   = 0x01000000;
constexpr OpFlags OPFLAG_IS_REG_ONLY   = 0x02000000;
constexpr OpFlags OPFLAG_IS_PUSH_PARAM = 0x04000000;
constexpr OpFlags OPFLAG_IS_CALL       = 0x08000000;
constexpr OpFlags OPFLAG_IS_8          = 0x10000000;
constexpr OpFlags OPFLAG_IS_16         = 0x20000000;
constexpr OpFlags OPFLAG_IS_32         = 0x40000000;
constexpr OpFlags OPFLAG_IS_64         = 0x80000000;

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
