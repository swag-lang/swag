#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Main/CommandLine.h"
#include "Semantic/Type/TypeManager.h"
#pragma optimize("", off)

namespace
{
    const char* jumpTypeName(CpuCondJump kind)
    {
        switch (kind)
        {
            case CpuCondJump::JNO:
                return "jno";
            case CpuCondJump::JNZ:
                return "jnz";
            case CpuCondJump::JZ:
                return "jz";
            case CpuCondJump::JL:
                return "jl";
            case CpuCondJump::JLE:
                return "jle";
            case CpuCondJump::JB:
                return "jb";
            case CpuCondJump::JBE:
                return "jbe";
            case CpuCondJump::JGE:
                return "jge";
            case CpuCondJump::JAE:
                return "jae";
            case CpuCondJump::JG:
                return "jg";
            case CpuCondJump::JA:
                return "ja";
            case CpuCondJump::JP:
                return "jp";
            case CpuCondJump::JNP:
                return "jnp";
            case CpuCondJump::JS:
                return "js";
            case CpuCondJump::JUMP:
                return "jump";
            default:
                SWAG_ASSERT(false);
                break;
        }

        return "???";
    }

    const char* cpuCondName(CpuCond cond)
    {
        switch (cond)
        {
            case CpuCond::A:
                return "a";
            case CpuCond::O:
                return "o";
            case CpuCond::AE:
                return "ae";
            case CpuCond::G:
                return "g";
            case CpuCond::B:
                return "b";
            case CpuCond::BE:
                return "be";
            case CpuCond::E:
                return "e";
            case CpuCond::EP:
                return "ep";
            case CpuCond::GE:
                return "ge";
            case CpuCond::L:
                return "l";
            case CpuCond::LE:
                return "le";
            case CpuCond::NA:
                return "na";
            case CpuCond::NE:
                return "ne";
            case CpuCond::P:
                return "p";
            case CpuCond::NP:
                return "np";
            case CpuCond::NEP:
                return "nep";
        }

        return "??";
    }

    Utf8 cpuOpName(CpuOp op)
    {
        switch (op)
        {
            case CpuOp::ADD:
                return "add";
            case CpuOp::SUB:
                return "sub";
            case CpuOp::BSR:
                return "bsr";
            case CpuOp::BSF:
                return "bsf";
            case CpuOp::MUL:
                return "mul";
            case CpuOp::IMUL:
                return "imul";
            case CpuOp::DIV:
                return "div";
            case CpuOp::IDIV:
                return "idiv";
            case CpuOp::MOD:
                return "mod";
            case CpuOp::SHL:
                return "shl";
            case CpuOp::SHR:
                return "shr";
            case CpuOp::SAL:
                return "sal";
            case CpuOp::SAR:
                return "sar";
            case CpuOp::AND:
                return "and";
            case CpuOp::OR:
                return "or";
            case CpuOp::XOR:
                return "xor";
            case CpuOp::NEG:
                return "neg";
            case CpuOp::POPCNT:
                return "popcnt";
            case CpuOp::MULADD:
                return "muladd";
            case CpuOp::CMPXCHG:
                return "cmpxchg";
            case CpuOp::XCHG:
                return "xchg";
            case CpuOp::MOV:
                return "mov";
            case CpuOp::LEA:
                return "lea";
            case CpuOp::MOVSXD:
                return "movsxd";

            case CpuOp::CVTI2F:
                return "cvtsi2f";
            case CpuOp::CVTU2F64:
                return "cvtu2f";
            case CpuOp::CVTF2I:
                return "cvtf2i";
            case CpuOp::CVTF2F:
                return "cvtf2f";
            case CpuOp::FADD:
                return "fadd";
            case CpuOp::FSUB:
                return "fsub";
            case CpuOp::FMIN:
                return "fmin";
            case CpuOp::FMAX:
                return "fmax";
            case CpuOp::FAND:
                return "fand";
            case CpuOp::FXOR:
                return "fxor";
            case CpuOp::FDIV:
                return "fdiv";
            case CpuOp::FMUL:
                return "fmul";
        }

        return form("<%X>", op);
    }

    const char* regName(CpuReg reg, OpBits opBits)
    {
        if (reg == CpuReg::Max)
            return "";

        static constexpr const char* GENERAL_REGS[][5] = {
            {"al", "ax", "eax", "rax", "..."},
            {"bl", "bx", "ebx", "rbx", "..."},
            {"cl", "cx", "ecx", "rcx", "..."},
            {"dl", "dx", "edx", "rdx", "..."},
            {"r8b", "r8w", "r8d", "r8", "..."},
            {"r9b", "r9w", "r9d", "r9", "..."},
            {"r10b", "r10w", "r10d", "r10", "..."},
            {"r11b", "r11w", "r11d", "r11", "..."},
            {"r12b", "r12w", "r12d", "r12", "..."},
            {"r13b", "r13w", "r13d", "r13", "..."},
            {"r14b", "r14w", "r14d", "r14", "..."},
            {"r15b", "r15w", "r15d", "r15", "..."},
            {"xmm0", "xmm0", "xmm0", "xmm0", "xmm0"},
            {"xmm1", "xmm1", "xmm1", "xmm1", "xmm1"},
            {"xmm2", "xmm2", "xmm2", "xmm2", "xmm2"},
            {"xmm3", "xmm3", "xmm3", "xmm3", "xmm3"},
            {"sil", "si", "esi", "rsi", "..."},
            {"dil", "di", "edi", "rdi", "..."},
            {"spl", "sp", "esp", "rsp", "..."},
            {"bpl", "bp", "ebp", "rbp", "..."},
        };

        const auto numBytes = static_cast<int>(std::log2(ScbeCpu::getNumBits(opBits) / 8));
        return GENERAL_REGS[static_cast<int>(reg)][numBytes];
    }

    const char* opBitsName(OpBits opBits)
    {
        switch (opBits)
        {
            case OpBits::B8:
                return "byte";
            case OpBits::B16:
                return "word";
            case OpBits::B32:
                return "dword";
            case OpBits::B64:
                return "qword";
            case OpBits::B128:
                return "xmmword";
        }

        return "???";
    }

    const char* opBitsNameRaw(OpBits opBits)
    {
        switch (opBits)
        {
            case OpBits::B8:
                return "B8";
            case OpBits::B16:
                return "B16";
            case OpBits::B32:
                return "B32";
            case OpBits::B64:
                return "B64";
            case OpBits::B128:
                return "B128";
        }

        return "???";
    }

    Utf8 printOpArgs(const ScbeMicroInstruction* inst, ScbeMicroOpFlags flags)
    {
        Utf8 res;

        if (flags.has(MOF_NAME))
            res += inst->name;

        if (flags.has(MOF_CPU_COND))
            res += form("CC:%s ", cpuCondName(inst->cpuCond));

        if (flags.has(MOF_CPU_OP))
            res += form("CO:%s ", cpuOpName(inst->cpuOp).cstr());

        if (flags.has(MOF_JUMP_TYPE))
            res += form("JT:%s ", jumpTypeName(inst->jumpType));

        if (flags.has(MOF_REG_A) && flags.has(MOF_OPBITS_A))
            res += form("A:%s ", regName(inst->regA, inst->opBitsA));
        else if (flags.has(MOF_REG_A) && flags.has(MOF_OPBITS_B))
            res += form("A:%s ", regName(inst->regA, inst->opBitsB));
        else if (flags.has(MOF_REG_A))
            res += form("A:%s ", regName(inst->regA, OpBits::B64));

        if (flags.has(MOF_REG_B) && flags.has(MOF_OPBITS_A))
            res += form("B:%s ", regName(inst->regB, inst->opBitsA));
        else if (flags.has(MOF_REG_B) && flags.has(MOF_OPBITS_B))
            res += form("B:%s ", regName(inst->regB, inst->opBitsB));
        else if (flags.has(MOF_REG_B))
            res += form("B:%s ", regName(inst->regB, OpBits::B64));

        if (flags.has(MOF_REG_C) && flags.has(MOF_OPBITS_A))
            res += form("C:%s ", regName(inst->regC, inst->opBitsA));
        else if (flags.has(MOF_REG_C) && flags.has(MOF_OPBITS_B))
            res += form("C:%s ", regName(inst->regC, inst->opBitsB));
        else if (flags.has(MOF_REG_C))
            res += form("C:%s ", regName(inst->regC, OpBits::B64));

        if (flags.has(MOF_VALUE_A))
            res += form("A:0x%llX ", inst->valueA);
        if (flags.has(MOF_VALUE_B))
            res += form("B:0x%llX ", inst->valueB);
        if (flags.has(MOF_VALUE_C))
            res += form("C:0x%llX ", inst->valueC);

        if (flags.has(MOF_OPBITS_A))
            res += form("A:%s ", opBitsNameRaw(inst->opBitsA));
        if (flags.has(MOF_OPBITS_B))
            res += form("B:%s ", opBitsNameRaw(inst->opBitsB));

        return res;
    }
}

void ScbeMicro::printInstructionLine(ScbeMicroInstruction* inst, uint32_t& idx, uint32_t i, ByteCode::PrintInstructionLine& line)
{
    idx++;

    if (inst)
    {
        if (inst->emitFlags.has(EMIT_Lock))
            line.name = "lock " + line.name;

        line.rank = form("%08d", i);

        line.flags.clear();
        if (inst->isJumpDest())
            line.flags += 'J';
        while (line.flags.length() != 10)
            line.flags += '.';

        const auto& def = g_MicroOpInfos[static_cast<int>(inst->op)];
        line.pretty.clear();
        line.pretty += def.name;
        line.pretty += " ";
        while (line.pretty.length() < 20)
            line.pretty += " ";
        line.pretty += printOpArgs(inst, def.leftFlags);
        if (def.rightFlags.flags)
        {
            line.pretty += "| ";
            line.pretty += printOpArgs(inst, def.rightFlags);
        }
    }
    else
    {
        line.rank.clear();
        line.flags.clear();
        line.pretty.clear();
    }

    Vector<ByteCode::PrintInstructionLine> lines;
    lines.push_back(line);

    ByteCodePrintOptions po;
    if (inst && (inst->op == ScbeMicroOp::Label || inst->op == ScbeMicroOp::PatchJump))
    {
        po.flagsColor  = LogColor::Magenta;
        po.prettyColor = LogColor::Magenta;
        po.nameColor   = LogColor::Magenta;
    }
    else
    {
        po.prettyColor = LogColor::Gray;
        po.flagsColor  = LogColor::Undefined;
        po.nameColor   = LogColor::Undefined;
    }

    ByteCode::alignPrintInstructions(po, lines, true);
    for (const auto& l : lines)
        ByteCode::printInstruction(po, nullptr, l);
}

void ScbeMicro::print() const
{
    const uint32_t num        = concat.totalCount() / sizeof(ScbeMicroInstruction);
    auto           inst       = reinterpret_cast<ScbeMicroInstruction*>(concat.firstBucket->data);
    uint32_t       lastLine   = UINT32_MAX;
    SourceFile*    lastFile   = nullptr;
    AstNode*       lastInline = nullptr;

    if (cpuFct->bc)
        cpuFct->bc->printName();

    uint32_t                       idx = 0;
    ByteCode::PrintInstructionLine line;

    for (uint32_t i = 0; i < num; i++, inst++)
    {
        if (inst->op == ScbeMicroOp::Ignore)
            continue;

        line.name.clear();
        line.args.clear();
#ifdef SWAG_DEV_MODE
        line.devMode.clear();
#endif

        switch (inst->op)
        {
            case ScbeMicroOp::Debug:
            {
                ByteCodePrintOptions po;
                po.rankColor   = LogColor::Transparent;
                po.nameColor   = LogColor::Gray;
                po.argsColor   = LogColor::Gray;
                po.flagsColor  = LogColor::Transparent;
                po.prettyColor = LogColor::Transparent;
#ifdef SWAG_DEV_MODE
                po.devModeColor = LogColor::Transparent;
#endif
                const auto curIp = reinterpret_cast<ByteCodeInstruction*>(inst->valueA);
                ByteCode::printSourceCode(po, cpuFct->bc, curIp, &lastLine, &lastFile, &lastInline);
                cpuFct->bc->printInstruction(po, curIp);
                line.name = "debug";
                continue;
            }

            case ScbeMicroOp::Label:
#ifdef SWAG_DEV_MODE
                if (g_CommandLine.dbgPrintBcExt)
                {
                    line.name = "label";
                    break;
                }
#endif
                continue;

            case ScbeMicroOp::PatchJump:
#ifdef SWAG_DEV_MODE
                if (g_CommandLine.dbgPrintBcExt)
                {
                    line.name = "patchjump";
                    break;
                }
#endif
                continue;

            case ScbeMicroOp::SymbolRelocAddr:
                line.name = "lea";
                line.args = form("%s, [<sym%d>+%llX]", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::SymbolRelocValue:
                line.name = "mov";
                if (inst->emitFlags.has(EMIT_B64))
                    line.args = form("%s, <sym%d>+%llX", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                else
                    line.args = form("%s, [<sym%d>+%llX]", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::Push:
                line.name = "push";
                line.args = regName(inst->regA, OpBits::B64);
                break;
            case ScbeMicroOp::Pop:
                line.name = "pop";
                line.args = regName(inst->regA, OpBits::B64);
                break;
            case ScbeMicroOp::Nop:
                line.name = "nop";
                break;
            case ScbeMicroOp::Ret:
                line.name = "ret";
                break;
            case ScbeMicroOp::CallLocal:
                line.name = "call near";
                line.args = Utf8::truncateDisplay(inst->name, 35);
                break;
            case ScbeMicroOp::CallExtern:
                line.name = "call far";
                line.args = Utf8::truncateDisplay(inst->name, 35);
                break;
            case ScbeMicroOp::CallIndirect:
                line.name = "call";
                line.args = form("[%s]", regName(inst->regA, OpBits::B64));
                break;
            case ScbeMicroOp::JumpTable:
                line.name = "jumptable";
                line.args = form("[%s+%s]", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64));
                break;
            case ScbeMicroOp::JumpM:
                line.name = "jump";
                line.args = form("[%s]", regName(inst->regA, OpBits::B64));
                break;
            case ScbeMicroOp::JumpCond:
            case ScbeMicroOp::JumpCondI:
                line.name = jumpTypeName(inst->jumpType);
                line.args = form("%08d", inst->valueB);
                break;
            case ScbeMicroOp::LoadCallParam:
                line.name = "mov";
                line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::LoadCallAddrParam:
                line.name = "lea";
                line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, OpBits::B64), opBitsName(OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadCallZeroExtParam:
                line.name = "movzx";
                line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), inst->valueA);
                break;
            case ScbeMicroOp::LoadRR:
                line.name = "mov";
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::LoadCondRR:
                line.name = form("cmov%s", cpuCondName(inst->cpuCond));
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::LoadRI:
                line.name = "mov";
                line.args = form("%s, 0x%llX", regName(inst->regA, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::LoadRM:
                line.name = "mov";
                if (inst->valueA == 0)
                    line.args = form("%s, %s ptr [%s]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, OpBits::B64));
                else
                    line.args = form("%s, %s ptr [%s+0x%llX]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadSignedExtRM:
                line.name = "movsx";
                if (inst->valueA == 0)
                    line.args = form("%s, %s ptr [%s]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, OpBits::B64));
                else
                    line.args = form("%s, %s ptr [%s+0x%llX]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadSignedExtRR:
                line.name = "movsx";
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsB));
                break;
            case ScbeMicroOp::LoadZeroExtRM:
                line.name = "movzx";
                if (inst->valueA == 0)
                    line.args = form("%s, %s ptr [%s]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, OpBits::B64));
                else
                    line.args = form("%s, %s ptr [%s+0x%llX]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadZeroExtRR:
                line.name = "movzx";
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsB));
                break;
            case ScbeMicroOp::LoadAddr:
                line.name = "lea";
                if (inst->valueA == 0)
                    line.args = form("%s, qword ptr [%s]", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64));
                else
                    line.args = form("%s, qword ptr [%s+0x%llX]", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadAmcMR:
                line.name = "mov";
                if (inst->regA == CpuReg::Max && inst->valueA == 1 && inst->valueB == 0)
                    line.args = form("%s ptr [%s], %s", opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), regName(inst->regC, inst->opBitsB));
                else if (inst->regA == CpuReg::Max && inst->valueB == 0)
                    line.args = form("%s ptr [%s*%d], %s", opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), inst->valueA, regName(inst->regC, inst->opBitsB));
                else if (inst->regA == CpuReg::Max && inst->valueA == 1)
                    line.args = form("%s ptr [%s+0x%llX], %s", opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), inst->valueB, regName(inst->regC, inst->opBitsB));
                else if (inst->regA == CpuReg::Max)
                    line.args = form("%s ptr [%s*%d+0x%llX], %s", opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), inst->valueA, inst->valueB, regName(inst->regC, inst->opBitsB));
                else if (inst->valueA == 1 && inst->valueB == 0)
                    line.args = form("%s ptr [%s+%s], %s", opBitsName(inst->opBitsB), regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA), regName(inst->regC, inst->opBitsB));
                else if (inst->valueB == 0)
                    line.args = form("%s ptr [%s+%s*%d], %s", opBitsName(inst->opBitsB), regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA), inst->valueA, regName(inst->regC, inst->opBitsB));
                else if (inst->valueA == 1)
                    line.args = form("%s ptr [%s+%s+0x%llX], %s", opBitsName(inst->opBitsB), regName(inst->regA, inst->opBitsB), regName(inst->regB, inst->opBitsB), inst->valueB, regName(inst->regC, inst->opBitsB));
                else
                    line.args = form("%s ptr [%s+%s*%d+0x%llX], %s", opBitsName(inst->opBitsB), regName(inst->regA, inst->opBitsB), regName(inst->regB, inst->opBitsB), inst->valueA, inst->valueB, regName(inst->regC, inst->opBitsB));
                break;
            case ScbeMicroOp::LoadAmcMI:
                line.name = "mov";
                if (inst->regA == CpuReg::Max && inst->valueA == 1 && inst->valueB == 0)
                    line.args = form("%s ptr [%s], 0x%llX", opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), inst->valueC);
                else if (inst->regA == CpuReg::Max && inst->valueB == 0)
                    line.args = form("%s ptr [%s*%d], 0x%llX", opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), inst->valueA, inst->valueC);
                else if (inst->regA == CpuReg::Max && inst->valueA == 1)
                    line.args = form("%s ptr [%s+0x%llX], 0x%llX", opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), inst->valueB, inst->valueC);
                else if (inst->regA == CpuReg::Max)
                    line.args = form("%s ptr [%s*%d+0x%llX], 0x%llX", opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), inst->valueA, inst->valueB, inst->valueC);
                else if (inst->valueA == 1 && inst->valueB == 0)
                    line.args = form("%s ptr [%s+%s], 0x%llX", opBitsName(inst->opBitsB), regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA), inst->valueC);
                else if (inst->valueB == 0)
                    line.args = form("%s ptr [%s+%s*%d], 0x%llX", opBitsName(inst->opBitsB), regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA), inst->valueA, inst->valueC);
                else if (inst->valueA == 1)
                    line.args = form("%s ptr [%s+%s+0x%llX], 0x%llX", opBitsName(inst->opBitsB), regName(inst->regA, inst->opBitsB), regName(inst->regB, inst->opBitsB), inst->valueB, inst->valueC);
                else
                    line.args = form("%s ptr [%s+%s*%d+0x%llX], 0x%llX", opBitsName(inst->opBitsB), regName(inst->regA, inst->opBitsB), regName(inst->regB, inst->opBitsB), inst->valueA, inst->valueB, inst->valueC);
                break;
            case ScbeMicroOp::LoadAmcRM:
            case ScbeMicroOp::LoadAddrAmcRM:
                if (inst->op == ScbeMicroOp::LoadAmcRM)
                    line.name = "mov";
                else
                    line.name = "lea";
                if (inst->regB == CpuReg::Max && inst->valueA == 1 && inst->valueB == 0)
                    line.args = form("%s, %s ptr [%s]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regC, inst->opBitsB));
                else if (inst->regB == CpuReg::Max && inst->valueB == 0)
                    line.args = form("%s, %s ptr [%s*%d]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regC, inst->opBitsB), inst->valueA);
                else if (inst->regB == CpuReg::Max && inst->valueA == 1)
                    line.args = form("%s, %s ptr [%s+0x%llX]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regC, inst->opBitsB), inst->valueB);
                else if (inst->regB == CpuReg::Max)
                    line.args = form("%s, %s ptr [%s*%d+0x%llX]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regC, inst->opBitsB), inst->valueA, inst->valueB);
                else if (inst->valueA == 1 && inst->valueB == 0)
                    line.args = form("%s, %s ptr [%s+%s]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, inst->opBitsB), regName(inst->regC, inst->opBitsB));
                else if (inst->valueB == 0)
                    line.args = form("%s, %s ptr [%s+%s*%d]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, inst->opBitsB), regName(inst->regC, inst->opBitsB), inst->valueA);
                else if (inst->valueA == 1)
                    line.args = form("%s, %s ptr [%s+%s+0x%llX]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, inst->opBitsB), regName(inst->regC, inst->opBitsB), inst->valueB);
                else
                    line.args = form("%s, %s ptr [%s+%s*%d+0x%llX]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, inst->opBitsB), regName(inst->regC, inst->opBitsB), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::LoadMR:
                line.name = "mov";
                if (inst->valueA == 0)
                    line.args = form("%s ptr [%s], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), regName(inst->regB, inst->opBitsA));
                else
                    line.args = form("%s ptr [%s+0x%llX], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::LoadMI:
                line.name = "mov";
                if (inst->valueA == 0)
                    line.args = form("%s ptr [%s], 0x%llX", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueB);
                else
                    line.args = form("%s ptr [%s+0x%llX], 0x%llX", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::CmpRR:
                line.name = "cmp";
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::CmpRI:
                line.name = "cmp";
                line.args = form("%s, 0x%llX", regName(inst->regA, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::CmpMR:
                line.name = "cmp";
                if (inst->valueA == 0)
                    line.args = form("%s ptr [%s], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), regName(inst->regB, inst->opBitsA));
                else
                    line.args = form("%s ptr [%s+0x%llX], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::CmpMI:
                line.name = "cmp";
                if (inst->valueA == 0)
                    line.args = form("%s ptr [%s], 0x%llX", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueB);
                else
                    line.args = form("%s ptr [%s+0x%llX], 0x%llX", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::SetCondR:
                line.name = form("set%s", cpuCondName(inst->cpuCond));
                line.args = form("%s", regName(inst->regA, OpBits::B8));
                break;
            case ScbeMicroOp::ClearR:
                line.name = "clear";
                line.args = form("%s", regName(inst->regA, inst->opBitsA));
                break;
            case ScbeMicroOp::OpUnaryM:
                line.name = cpuOpName(inst->cpuOp);
                if (inst->valueA == 0)
                    line.args += form("%s ptr [%s]", opBitsName(inst->opBitsA), regName(inst->regA, inst->opBitsA));
                else
                    line.args += form("%s ptr [%s+0x%llX]", opBitsName(inst->opBitsA), regName(inst->regA, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::OpUnaryR:
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s", regName(inst->regA, inst->opBitsA));
                break;
            case ScbeMicroOp::OpBinaryRR:
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::OpBinaryMR:
                line.name = cpuOpName(inst->cpuOp);
                if (inst->valueA == 0)
                    line.args += form("%s ptr [%s], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), regName(inst->regB, inst->opBitsA));
                else
                    line.args += form("%s ptr [%s+0x%llX], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::OpBinaryRI:
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s, 0x%llX", regName(inst->regA, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::OpBinaryMI:
                line.name = cpuOpName(inst->cpuOp);
                if (inst->valueA == 0)
                    line.args += form("%s ptr [%s], 0x%llX", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueB);
                else
                    line.args += form("%s ptr [%s+0x%llX], 0x%llX", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::OpBinaryRM:
                line.name = cpuOpName(inst->cpuOp);
                if (inst->valueA == 0)
                    line.args += form("%s, %s ptr [%s]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, OpBits::B64));
                else
                    line.args += form("%s, %s ptr [%s+0x%llX]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::OpTernaryRRR:
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s, %s %s %s", regName(inst->regA, inst->opBitsA), regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA), regName(inst->regC, inst->opBitsA));
                break;

            case ScbeMicroOp::Enter:
                for (auto r : cpuFct->unwindRegs)
                {
                    line.name = "push";
                    line.args = form("%s", regName(r, OpBits::B64));
                    printInstructionLine(nullptr, idx, i, line);
                }
                if (!cpuFct->noStackFrame)
                {
                    line.name = "sub";
                    line.args = "rsp, <framesize>";
                    printInstructionLine(inst, idx, i, line);
                }
                continue;

            case ScbeMicroOp::Leave:
                if (!cpuFct->noStackFrame)
                {
                    line.name = "add";
                    line.args = "rsp, <framesize>";
                    printInstructionLine(inst, idx, i, line);
                }
                for (uint32_t j = cpuFct->unwindRegs.size() - 1; j != UINT32_MAX; j--)
                {
                    const auto r = cpuFct->unwindRegs[j];
                    line.name    = "pop";
                    line.args    = form("%s", regName(r, OpBits::B64));
                    printInstructionLine(nullptr, idx, i, line);
                }
                continue;

            case ScbeMicroOp::End:
                line.name = "end";
                line.args = form("(%d instructions)", idx);
                break;
        }

        printInstructionLine(inst, idx, i, line);
    }

    g_Log.writeEol();
}
