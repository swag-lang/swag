#pragma once

using ScbeMicroOpFlags                   = Flags<uint32_t>;
constexpr ScbeMicroOpFlags MOF_ZERO      = 0x00000000;
constexpr ScbeMicroOpFlags MOF_REG_A     = 0x00000001;
constexpr ScbeMicroOpFlags MOF_REG_B     = 0x00000002;
constexpr ScbeMicroOpFlags MOF_REG_C     = 0x00000004;
constexpr ScbeMicroOpFlags MOF_VALUE_A   = 0x00000008;
constexpr ScbeMicroOpFlags MOF_VALUE_B   = 0x00000010;
constexpr ScbeMicroOpFlags MOF_VALUE_C   = 0x00000020;
constexpr ScbeMicroOpFlags MOF_OPBITS_A  = 0x00000040;
constexpr ScbeMicroOpFlags MOF_OPBITS_B  = 0x00000080;
constexpr ScbeMicroOpFlags MOF_CPU_OP    = 0x00000100;
constexpr ScbeMicroOpFlags MOF_JUMP_TYPE = 0x00000200;
constexpr ScbeMicroOpFlags MOF_NAME      = 0x00000400;
constexpr ScbeMicroOpFlags MOF_CPU_COND  = 0x00000800;
constexpr ScbeMicroOpFlags MOF_READ_REG  = 0x00001000;
constexpr ScbeMicroOpFlags MOF_WRITE_REG = 0x00002000;
constexpr ScbeMicroOpFlags MOF_READ_MEM  = 0x00004000;
constexpr ScbeMicroOpFlags MOF_WRITE_MEM = 0x00008000;

struct ScbeMicroOpInfo
{
    const char*      name;
    ScbeMicroOpFlags leftFlags;
    ScbeMicroOpFlags rightFlags;
};

extern ScbeMicroOpInfo g_MicroOpInfos[];