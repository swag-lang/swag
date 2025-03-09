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

struct ScbeMicroOpInfo
{
    const char*     name;
    ScbeMicroOpFlags leftFlags;
    ScbeMicroOpFlags rightFlags;
};

extern ScbeMicroOpInfo g_MicroOpInfos[];

using ScbeMicroOpDetails                  = Flags<uint64_t>;
constexpr ScbeMicroOpDetails MOD_ZERO     = 0x0000000000000000;
constexpr ScbeMicroOpDetails MOD_REG_RAX  = 0x0000000000000001;
constexpr ScbeMicroOpDetails MOD_REG_RBX  = 0x0000000000000002;
constexpr ScbeMicroOpDetails MOD_REG_RCX  = 0x0000000000000004;
constexpr ScbeMicroOpDetails MOD_REG_RDX  = 0x0000000000000008;
constexpr ScbeMicroOpDetails MOD_REG_R8   = 0x0000000000000010;
constexpr ScbeMicroOpDetails MOD_REG_R9   = 0x0000000000000020;
constexpr ScbeMicroOpDetails MOD_REG_R10  = 0x0000000000000040;
constexpr ScbeMicroOpDetails MOD_REG_R11  = 0x0000000000000080;
constexpr ScbeMicroOpDetails MOD_REG_R12  = 0x0000000000000100;
constexpr ScbeMicroOpDetails MOD_REG_R13  = 0x0000000000000200;
constexpr ScbeMicroOpDetails MOD_REG_R14  = 0x0000000000000400;
constexpr ScbeMicroOpDetails MOD_REG_R15  = 0x0000000000000800;
constexpr ScbeMicroOpDetails MOD_REG_XMM0 = 0x0000000000001000;
constexpr ScbeMicroOpDetails MOD_REG_XMM1 = 0x0000000000002000;
constexpr ScbeMicroOpDetails MOD_REG_XMM2 = 0x0000000000004000;
constexpr ScbeMicroOpDetails MOD_REG_XMM3 = 0x0000000000008000;
constexpr ScbeMicroOpDetails MOD_REG_RSI  = 0x0000000000010000;
constexpr ScbeMicroOpDetails MOD_REG_RDI  = 0x0000000000020000;
constexpr ScbeMicroOpDetails MOD_REG_ALL  = 0x0000000000FFFFFF;
