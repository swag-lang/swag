#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "ScbeMicroInstruction.h"
#include "Semantic/Type/TypeManager.h"

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
            case CpuCondJump::JUMP:
                return "jump";
            default:
                SWAG_ASSERT(false);
                break;
        }

        return "???";
    }

    const char* cpuCondName(CpuCondFlag cond)
    {
        switch (cond)
        {
            case CpuCondFlag::A:
                return "a";
            case CpuCondFlag::O:
                return "o";
            case CpuCondFlag::AE:
                return "ae";
            case CpuCondFlag::G:
                return "g";
            case CpuCondFlag::B:
                return "b";
            case CpuCondFlag::BE:
                return "be";
            case CpuCondFlag::E:
                return "e";
            case CpuCondFlag::EP:
                return "ep";
            case CpuCondFlag::GE:
                return "ge";
            case CpuCondFlag::L:
                return "l";
            case CpuCondFlag::LE:
                return "le";
            case CpuCondFlag::NA:
                return "na";
            case CpuCondFlag::NE:
                return "ne";
            case CpuCondFlag::NEP:
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
            case CpuOp::CMOVE:
                return "cmove";              
            case CpuOp::CMOVG:
                return "cmovg";            
            case CpuOp::CMOVGE:
                return "cmovge";
            case CpuOp::CMOVB:
                return "cmovb";            
            case CpuOp::CMOVBE:
                return "cmovbe";
            case CpuOp::CMOVL:
                return "cmovl";
            case CpuOp::POPCNT:
                return "popcnt";

            case CpuOp::CMP:
                return "cmp";
            case CpuOp::MOV:
                return "mov";

            case CpuOp::CVTI2F:
                return "cvtsi2f";
            case CpuOp::CVTF2I:
                return "cvtf2i";
            case CpuOp::UCOMIF:
                return "ucomif";
            case CpuOp::FADD:
                return "fadd";
            case CpuOp::FSUB:
                return "fsub";
            case CpuOp::FMIN:
                return "fmin";
            case CpuOp::FMAX:
                return "fmax";
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
        static constexpr const char* GENERAL_REGS[][4] = {
            {"al", "ax", "eax", "rax"},
            {"bl", "bx", "ebx", "rbx"},
            {"cl", "cx", "ecx", "rcx"},
            {"dl", "dx", "edx", "rdx"},
            {"r8b", "r8w", "r8d", "r8"},
            {"r9b", "r9w", "r9d", "r9"},
            {"r10b", "r10w", "r10d", "r10"},
            {"r11b", "r11w", "r11d", "r11"},
            {"r12b", "r12w", "r12d", "r12"},
            {"r13b", "r13w", "r13d", "r13"},
            {"r14b", "r14w", "r14d", "r14"},
            {"r15b", "r15w", "r15d", "r15"},
            {"xmm0", "xmm0", "xmm0", "xmm0"},
            {"xmm1", "xmm1", "xmm1", "xmm1"},
            {"xmm2", "xmm2", "xmm2", "xmm2"},
            {"xmm3", "xmm3", "xmm3", "xmm3"},
            {"sil", "si", "esi", "rsi"},
            {"dil", "di", "edi", "rdi"},
            {"spl", "sp", "esp", "rsp"},
            {"bpl", "bp", "ebp", "rbp"},
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
        }

        return "???";
    }

    Utf8 printOpArgs(const ScbeMicroInstruction* inst, ScbeMicroOpFlags flags)
    {
        Utf8 res;

        if (flags.has(MOF_NAME))
            res += form("NAME");

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
            res += form("A:%llxh ", inst->valueA);
        if (flags.has(MOF_VALUE_B))
            res += form("B:%llxh ", inst->valueB);
        if (flags.has(MOF_VALUE_C))
            res += form("B:%xh ", inst->valueC);

        if (flags.has(MOF_OPBITS_A))
            res += form("A:%s ", opBitsNameRaw(inst->opBitsA));
        if (flags.has(MOF_OPBITS_B))
            res += form("B:%s ", opBitsNameRaw(inst->opBitsB));

        return res;
    }
}

void ScbeMicro::print() const
{
    const auto  num        = concat.totalCount() / sizeof(ScbeMicroInstruction);
    auto        inst       = reinterpret_cast<ScbeMicroInstruction*>(concat.firstBucket->data);
    uint32_t    lastLine   = UINT32_MAX;
    SourceFile* lastFile   = nullptr;
    AstNode*    lastInline = nullptr;

    if (cpuFct->bc)
        cpuFct->bc->printName();

    for (uint32_t i = 0; i < num; i++, inst++)
    {
        ByteCode::PrintInstructionLine         line;
        Vector<ByteCode::PrintInstructionLine> lines;

        if (inst->op == ScbeMicroOp::Ignore)
            continue;

        switch (inst->op)
        {
            case ScbeMicroOp::Debug:
            {
                const auto curIp = reinterpret_cast<ByteCodeInstruction*>(inst->valueA);
                ByteCode::printSourceCode({}, cpuFct->bc, curIp, &lastLine, &lastFile, &lastInline);
                cpuFct->bc->printInstruction({}, curIp);
                continue;
            }

            case ScbeMicroOp::Label:
            case ScbeMicroOp::PatchJump:
                continue;

            case ScbeMicroOp::SymbolRelocationRef:
                // encoder.emitSymbolRelocationRef(inst->name);
                break;
            case ScbeMicroOp::SymbolRelocationAddress:
                // encoder.emitSymbolRelocationAddress(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                line.name = "lea";
                line.args = form("%s, [<sym%d>+%d]", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::SymbolRelocationValue:
                // encoder.emitSymbolRelocationValue(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                line.name = "mov";
                line.args = form("%s, <sym%d>+%d", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::SymbolGlobalString:
                // encoder.emitSymbolGlobalString(inst->regA, inst->name);
                line.name = "mov";
                line.args = form("%s, ptr \"%s\"", regName(inst->regA, OpBits::B64), inst->name.cstr());
                break;
            case ScbeMicroOp::SymbolRelocationPtr:
                // encoder.emitSymbolRelocationPtr(inst->regA, inst->name);
                line.name = "mov";
                line.args = form("%s, ptr %s", regName(inst->regA, OpBits::B64), inst->name.cstr());
                break;
            case ScbeMicroOp::Push:
                // encoder.emitPush(inst->regA);
                line.name = "push";
                line.args = regName(inst->regA, OpBits::B64);
                break;
            case ScbeMicroOp::Pop:
                // encoder.emitPop(inst->regA);
                line.name = "pop";
                line.args = regName(inst->regA, OpBits::B64);
                break;
            case ScbeMicroOp::Nop:
                // encoder.emitNop();
                line.name = "nop";
                break;
            case ScbeMicroOp::Ret:
                // encoder.emitRet();
                line.name = "ret";
                break;
            case ScbeMicroOp::CallLocal:
                // encoder.emitCallLocal(inst->name);
                line.name = "call near";
                line.args = inst->name;
                break;
            case ScbeMicroOp::CallExtern:
                // encoder.emitCallExtern(inst->name);
                line.name = "call far";
                line.args = inst->name;
                break;
            case ScbeMicroOp::CallIndirect:
                // encoder.emitCallIndirect(inst->regA);
                line.name = "call";
                line.args = form("[%s]", regName(inst->regA, OpBits::B64));
                break;
            case ScbeMicroOp::JumpTable:
                // encoder.emitJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), static_cast<uint32_t>(inst->valueC));
                line.name = "jumptable";
                line.args = form("[%s+%s]", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64));
                break;
            case ScbeMicroOp::JumpCC:
                // const auto cmpJump = encoder.emitJump(inst->jumpType, inst->opBitsA);
                line.name = jumpTypeName(inst->jumpType);
                break;
            case ScbeMicroOp::JumpM:
                // encoder.emitJump(inst->regA);
                line.name = "jump";
                line.args = form("[%s]", regName(inst->regA, OpBits::B64));
                break;
            case ScbeMicroOp::JumpCI:
                // encoder.emitJump(inst->jumpType, static_cast<int32_t>(inst->valueA), static_cast<int32_t>(inst->valueB));
                line.name = jumpTypeName(inst->jumpType);
                line.args = form("%08d", inst->valueA);
                break;
            case ScbeMicroOp::LoadCallerParam:
                // encoder.emitLoadCallerParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA);
                line.name = "mov";
                line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::LoadCallerAddressParam:
                // encoder.emitLoadCallerAddressParam(inst->regA, static_cast<uint32_t>(inst->valueA));
                line.name = "lea";
                line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, OpBits::B64), opBitsName(OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadCallerZeroExtendParam:
                // encoder.emitLoadCallerZeroExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB);
                line.name = "movzx";
                line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), inst->valueA);
                break;
            case ScbeMicroOp::LoadRR:
                // encoder.emitLoad(inst->regA, inst->regB, inst->opBitsA);
                line.name = "mov";
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::LoadRI64:
                // encoder.emitLoad(inst->regA, inst->valueA);
                line.name = "mov";
                line.args = form("%s, %lld", regName(inst->regA, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadRI:
                // encoder.emitLoad(inst->regA, inst->valueA, inst->opBitsA);
                line.name = "mov";
                line.args = form("%s, %x", regName(inst->regA, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::LoadRM:
                // encoder.emitLoad(inst->regA, inst->regB, inst->valueA, inst->opBitsA);
                line.name = "mov";
                line.args = form("%s, %s ptr [%s+%xh]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadSignedExtendRM:
                // encoder.emitLoadSignedExtend0(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                line.name = "movsx";
                line.args = form("%s, %s ptr [%s+%xh]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadSignedExtendRR:
                // encoder.emitLoadSignedExtend1(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                line.name = "movsx";
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsB));
                break;
            case ScbeMicroOp::LoadZeroExtendRM:
                // encoder.emitLoadZeroExtend0(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                line.name = "movzx";
                line.args = form("%s, %s ptr [%s+%xh]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::LoadZeroExtendRR:
                // encoder.emitLoadZeroExtend1(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                line.name = "movzx";
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsB));
                break;
            case ScbeMicroOp::LoadAddressM:
                // encoder.emitLoadAddress(inst->regA, inst->regB, inst->valueA);
                line.name = "lea";
                line.args = form("%s, [%s+%xh]", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64), inst->valueB);
                break;
            case ScbeMicroOp::LoadAddressAddMul:
                // encoder.emitLoadAddress(inst->regA, inst->regB, inst->regC, inst->valueA, inst->opBitsA);
                line.name = "lea";
                line.args = form("%s, [%s+%s*%d]", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA), regName(inst->regC, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::LoadMR:
                // encoder.emitStore(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                line.name = "mov";
                line.args = form("%s ptr [%s+%xh], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::LoadMI:
                // encoder.emitStore(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                line.name = "mov";
                line.args = form("%s ptr [%s+%xh], %d", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::CmpRR:
                // encoder.emitCmp(inst->regA, inst->regB, inst->opBitsA);
                line.name = "cmp";
                line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::CmpRI:
                // encoder.emitCmp(inst->regA, inst->valueA, inst->opBitsA);
                line.name = "cmp";
                line.args = form("%s, %d", regName(inst->regA, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::CmpMR:
                // encoder.emitCmp(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                line.name = "cmp";
                line.args = form("%s ptr [%s+%xh], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::CmpMI:
                // encoder.emitCmp(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                line.name = "cmp";
                line.args = form("%s ptr [%s+%xh], %d", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::SetCC:
                // encoder.emitSet(inst->regA, inst->cpuCond);
                line.name = form("set%s", cpuCondName(inst->cpuCond));
                line.args = form("%s", regName(inst->regA, OpBits::B8));
                break;
            case ScbeMicroOp::ClearR:
                // encoder.emitClear(inst->regA, inst->opBitsA);
                line.name = "clear";
                line.args = form("%s", regName(inst->regA, inst->opBitsA));
                break;
            case ScbeMicroOp::ClearM:
                // encoder.emitClear(inst->regA, inst->valueA, static_cast<uint32_t>(inst->valueB));
                line.name = "clear";
                line.args = form("byte ptr [%s+%d], %d", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::Copy:
                // encoder.emitCopy(inst->regA, inst->regB, static_cast<uint32_t>(inst->valueA));
                line.name = "copy";
                line.args = form("byte ptr [%s], byte ptr [%s], %d", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::OpUnaryM:
                // encoder.emitOpUnary(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA);
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s ptr [%s+%xh]", opBitsName(inst->opBitsA), regName(inst->regA, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::OpUnaryR:
                // encoder.emitOpUnary(inst->regA, inst->cpuOp, inst->opBitsA);
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s", regName(inst->regA, inst->opBitsA));
                break;
            case ScbeMicroOp::OpBinaryRR:
                // encoder.emitOpBinary(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::OpBinaryMR:
                // encoder.emitOpBinary(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s ptr [%s+%xh], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                break;
            case ScbeMicroOp::OpBinaryRI:
                // encoder.emitOpBinary(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s, %d", regName(inst->regA, inst->opBitsA), inst->valueA);
                break;
            case ScbeMicroOp::OpBinaryMI:
                // encoder.emitOpBinary(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s ptr [%s+%xh], %d", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                break;
            case ScbeMicroOp::OpBinaryRM:
                // encoder.emitOpBinary(inst->regA, inst->regB, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                line.name = cpuOpName(inst->cpuOp);
                line.args += form("%s, %s ptr [%s+%xh]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case ScbeMicroOp::MulAdd:
                // encoder.emitMulAdd(inst->regA, inst->regB, inst->regC, inst->opBitsA);
                line.name = "muladd";
                line.args += form("%s, %s*%s+%s", regName(inst->regA, inst->opBitsA), regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA), regName(inst->regC, inst->opBitsA));
                break;
        }

        line.rank = form("%08d", i);

        while (line.name.length() != 12)
            line.name += ' ';
        line.pretty = line.name + line.args;

        const auto& def = g_MicroOpInfos[static_cast<int>(inst->op)];
        line.name       = def.name;
        line.args       = printOpArgs(inst, def.leftFlags);
        if (def.rightFlags.flags)
        {
            line.args += "| ";
            line.args += printOpArgs(inst, def.rightFlags);
        }
        line.args.makeUpper();

        if (inst->flags.has(MIF_JUMP_DEST))
            line.flags += 'J';
        while (line.flags.length() != 10)
            line.flags += '.';

        lines.push_back(line);

        ByteCodePrintOptions po;
        po.flags.add(BCPF_ASM_SCBE);
        ByteCode::alignPrintInstructions(po, lines, true);
        for (const auto& l : lines)
            ByteCode::printInstruction(po, nullptr, l);
    }
}
