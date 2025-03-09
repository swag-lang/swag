#pragma once

using ScbeMicroOpFlag                   = Flags<uint32_t>;
constexpr ScbeMicroOpFlag MOF_ZERO      = 0x00000000;
constexpr ScbeMicroOpFlag MOF_REG_A     = 0x00000001;
constexpr ScbeMicroOpFlag MOF_REG_B     = 0x00000002;
constexpr ScbeMicroOpFlag MOF_REG_C     = 0x00000004;
constexpr ScbeMicroOpFlag MOF_VALUE_A   = 0x00000008;
constexpr ScbeMicroOpFlag MOF_VALUE_B   = 0x00000010;
constexpr ScbeMicroOpFlag MOF_VALUE_C   = 0x00000020;
constexpr ScbeMicroOpFlag MOF_OPBITS_A  = 0x00000040;
constexpr ScbeMicroOpFlag MOF_OPBITS_B  = 0x00000080;
constexpr ScbeMicroOpFlag MOF_CPU_OP    = 0x00000100;
constexpr ScbeMicroOpFlag MOF_JUMP_TYPE = 0x00000200;
constexpr ScbeMicroOpFlag MOF_NAME      = 0x00000400;
constexpr ScbeMicroOpFlag MOF_CPU_COND  = 0x00000800;
