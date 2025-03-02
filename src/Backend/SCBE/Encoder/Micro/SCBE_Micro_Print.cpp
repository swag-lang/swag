#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/SCBE_Micro.h"
#include "Semantic/Type/TypeManager.h"

namespace
{
    Utf8 valueName(uint64_t value, OpBits opBits)
    {
        if (SCBE_CPU::isFloat(opBits))
            return form("%f", std::bit_cast<double>(value));
        return form("%d", value);
    }

    const char* jumpTypeName(CPUCondJump kind)
    {
        switch (kind)
        {
            case JNO:
                return "jno";
            case JNZ:
                return "jnz";
            case JZ:
                return "jz";
            case JL:
                return "jl";
            case JLE:
                return "jle";
            case JB:
                return "jb";
            case JBE:
                return "jbe";
            case JGE:
                return "jge";
            case JAE:
                return "jae";
            case JG:
                return "jg";
            case JA:
                return "ja";
            case JP:
                return "jp";
            case JNP:
                return "jnp";
            case JUMP:
                return "jump";
            default:
                SWAG_ASSERT(false);
                break;
        }

        return "???";
    };

    const char* cpuCondName(CPUCondFlag cond)
    {
        switch (cond)
        {
            case CPUCondFlag::A:
                return "a";
            case CPUCondFlag::O:
                return "o";
            case CPUCondFlag::AE:
                return "ae";
            case CPUCondFlag::G:
                return "g";
            case CPUCondFlag::B:
                return "b";
            case CPUCondFlag::BE:
                return "be";
            case CPUCondFlag::E:
                return "e";
            case CPUCondFlag::EP:
                return "ep";
            case CPUCondFlag::GE:
                return "ge";
            case CPUCondFlag::L:
                return "l";
            case CPUCondFlag::LE:
                return "le";
            case CPUCondFlag::NA:
                return "na";
            case CPUCondFlag::NE:
                return "ne";
            case CPUCondFlag::NEP:
                return "nep";
        }

        return "??";
    }

    Utf8 cpuOpName(CPUOp op, OpBits opBits)
    {
        switch (op)
        {
            case CPUOp::ADD:
                return "add";
            case CPUOp::SUB:
                return "sub";
            case CPUOp::MUL:
                return "mul";
            case CPUOp::IMUL:
                return "imul";
            case CPUOp::DIV:
                return "div";
            case CPUOp::IDIV:
                return "idiv";
            case CPUOp::MOD:
                return "mod";
            case CPUOp::SHL:
                return "shl";
            case CPUOp::SHR:
                return "shr";
            case CPUOp::SAL:
                return "sal";
            case CPUOp::SAR:
                return "sar";
            case CPUOp::AND:
                return "and";
            case CPUOp::OR:
                return "or";
            case CPUOp::XOR:
                return "xor";

            case CPUOp::MOV:
                return opBits == OpBits::F32 ? "movss" : opBits == OpBits::F64 ? "movsd"
                                                                               : "mov";

            case CPUOp::CVTI2F:
                return opBits == OpBits::F32 ? "cvtsi2ss" : opBits == OpBits::F64 ? "cvtsi2sd"
                                                                                  : "cvti2f";
            case CPUOp::FADD:
                return opBits == OpBits::F32 ? "addss" : opBits == OpBits::F64 ? "addsd"
                                                                               : "fadd";
            case CPUOp::FSUB:
                return opBits == OpBits::F32 ? "subss" : opBits == OpBits::F64 ? "subsd"
                                                                               : "fsub";
            case CPUOp::FMIN:
                return "fmin";
            case CPUOp::FMAX:
                return "fmax";
        }

        return form("<%d>", op);
    }

    const char* regName(CPUReg reg, OpBits opBits)
    {
        static constexpr const char* GENERAL_REGS[][4] = {
            {"al", "ax", "eax", "rax"},
            {"cl", "cx", "ecx", "rcx"},
            {"dl", "dx", "edx", "rdx"},
            {"bl", "bx", "ebx", "rbx"},
            {"spl", "sp", "esp", "rsp"},
            {"bpl", "bp", "ebp", "rbp"},
            {"sil", "si", "esi", "rsi"},
            {"dil", "di", "edi", "rdi"},
            {"r8b", "r8w", "r8d", "r8"},
            {"r9b", "r9w", "r9d", "r9"},
            {"r10b", "r10w", "r10d", "r10"},
            {"r11b", "r11w", "r11d", "r11"},
            {"r12b", "r12w", "r12d", "r12"},
            {"r13b", "r13w", "r13d", "r13"},
            {"r14b", "r14w", "r14d", "r14"},
            {"r15b", "r15w", "r15d", "r15"}};

        static constexpr const char* XMM_REGS[] = {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"};

        if (SCBE_CPU::isFloat(opBits))
            return XMM_REGS[static_cast<int>(reg) - static_cast<int>(CPUReg::XMM0)];

        const auto numBytes = static_cast<int>(std::log2(SCBE_CPU::getNumBits(opBits) / 8));
        switch (reg)
        {
            case CPUReg::RAX:
                return GENERAL_REGS[0][numBytes];
            case CPUReg::RCX:
                return GENERAL_REGS[1][numBytes];
            case CPUReg::RDX:
                return GENERAL_REGS[2][numBytes];
            case CPUReg::RBX:
                return GENERAL_REGS[3][numBytes];
            case CPUReg::RSP:
                return GENERAL_REGS[4][numBytes];
            case CPUReg::RBP:
                return GENERAL_REGS[5][numBytes];
            case CPUReg::RSI:
                return GENERAL_REGS[6][numBytes];
            case CPUReg::RDI:
                return GENERAL_REGS[7][numBytes];
            case CPUReg::R8:
                return GENERAL_REGS[8][numBytes];
            case CPUReg::R9:
                return GENERAL_REGS[9][numBytes];
            case CPUReg::R10:
                return GENERAL_REGS[10][numBytes];
            case CPUReg::R11:
                return GENERAL_REGS[11][numBytes];
            case CPUReg::R12:
                return GENERAL_REGS[12][numBytes];
            case CPUReg::R13:
                return GENERAL_REGS[13][numBytes];
            case CPUReg::R14:
                return GENERAL_REGS[14][numBytes];
            case CPUReg::R15:
                return GENERAL_REGS[15][numBytes];
            default:
                return "???";
        }
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
            case OpBits::F32:
                return "dword";
            case OpBits::B64:
            case OpBits::F64:
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
            case OpBits::F32:
                return "F32";
            case OpBits::F64:
                return "F64";
        }

        return "???";
    }

    Utf8 printOpArgs(const SCBE_MicroInstruction* inst, SCBE_MicroOpFlag flags)
    {
        Utf8 res;

        if (flags.has(MOF_NAME))
            res += form("NAME");
        
        if (flags.has(MOF_CPU_COND))
            res += form("CC:%s ", cpuCondName(inst->cpuCond));

        if (flags.has(MOF_CPU_OP))
            res += form("CO:%s ", cpuOpName(inst->cpuOp, OpBits::Zero).cstr());

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

void SCBE_Micro::print() const
{
    const auto  num        = concat.totalCount() / sizeof(SCBE_MicroInstruction);
    auto        inst       = reinterpret_cast<SCBE_MicroInstruction*>(concat.firstBucket->data);
    uint32_t    lastLine   = UINT32_MAX;
    SourceFile* lastFile   = nullptr;
    AstNode*    lastInline = nullptr;

    if (cpuFct->bc)
        cpuFct->bc->printName();

    for (uint32_t i = 0; i < num; i++, inst++)
    {
        ByteCode::PrintInstructionLine         line;
        Vector<ByteCode::PrintInstructionLine> lines;

        if (inst->op == SCBE_MicroOp::Ignore)
            continue;

        if (inst->op == SCBE_MicroOp::Enter)
        {
            for (const auto& reg : cpuFct->unwindRegs)
            {
                line.name = "push";
                line.args = regName(reg, OpBits::B64);
                lines.push_back(line);
            }

            line.name = "sub";
            line.args = form("rsp, %d", cpuFct->frameSize);
            lines.push_back(line);
        }
        else if (inst->op == SCBE_MicroOp::Leave)
        {
            line.name = "add";
            line.args = form("rsp, %d", cpuFct->frameSize);
            lines.push_back(line);

            for (auto idx = cpuFct->unwindRegs.size() - 1; idx != UINT32_MAX; idx--)
            {
                line.name = "pop";
                line.args = regName(cpuFct->unwindRegs[idx], OpBits::B64);
                lines.push_back(line);
            }
        }
        else
        {
            switch (inst->op)
            {
                case SCBE_MicroOp::Debug:
                {
                    const auto curIp = reinterpret_cast<ByteCodeInstruction*>(inst->valueA);
                    ByteCode::printSourceCode({}, cpuFct->bc, curIp, &lastLine, &lastFile, &lastInline);
                    cpuFct->bc->printInstruction({}, curIp);
                    continue;
                }

                case SCBE_MicroOp::Label:
                case SCBE_MicroOp::PatchJump:
                    continue;

                case SCBE_MicroOp::SymbolRelocationRef:
                    // encoder.emitSymbolRelocationRef(inst->name);
                    break;
                case SCBE_MicroOp::SymbolRelocationAddress:
                    // encoder.emitSymbolRelocationAddress(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                    line.name = "lea";
                    line.args = form("%s, [<sym%d>+%d]", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                    break;
                case SCBE_MicroOp::SymbolRelocationValue:
                    // encoder.emitSymbolRelocationValue(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                    line.name = "mov";
                    line.args = form("%s, <sym%d>+%d", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                    break;
                case SCBE_MicroOp::SymbolGlobalString:
                    // encoder.emitSymbolGlobalString(inst->regA, inst->name);
                    line.name = "mov";
                    line.args = form("%s, ptr \"%s\"", regName(inst->regA, OpBits::B64), inst->name.cstr());
                    break;
                case SCBE_MicroOp::SymbolRelocationPtr:
                    // encoder.emitSymbolRelocationPtr(inst->regA, inst->name);
                    line.name = "mov";
                    line.args = form("%s, ptr %s", regName(inst->regA, OpBits::B64), inst->name.cstr());
                    break;
                case SCBE_MicroOp::Push:
                    // encoder.emitPush(inst->regA);
                    line.name = "push";
                    line.args = regName(inst->regA, OpBits::B64);
                    break;
                case SCBE_MicroOp::Pop:
                    // encoder.emitPop(inst->regA);
                    line.name = "pop";
                    line.args = regName(inst->regA, OpBits::B64);
                    break;
                case SCBE_MicroOp::Nop:
                    // encoder.emitNop();
                    line.name = "nop";
                    break;
                case SCBE_MicroOp::Ret:
                    // encoder.emitRet();
                    line.name = "ret";
                    break;
                case SCBE_MicroOp::CallLocal:
                    // encoder.emitCallLocal(inst->name);
                    line.name = "call near";
                    line.args = inst->name;
                    break;
                case SCBE_MicroOp::CallExtern:
                    // encoder.emitCallExtern(inst->name);
                    line.name = "call far";
                    line.args = inst->name;
                    break;
                case SCBE_MicroOp::CallIndirect:
                    // encoder.emitCallIndirect(inst->regA);
                    line.name = "call";
                    line.args = form("[%s]", regName(inst->regA, OpBits::B64));
                    break;
                case SCBE_MicroOp::JumpTable:
                    // encoder.emitJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), static_cast<uint32_t>(inst->valueC));
                    line.name = "jumptable";
                    line.args = form("[%s+%s]", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64));
                    break;
                case SCBE_MicroOp::Jump0:
                    // const auto cmpJump = encoder.emitJump(inst->jumpType, inst->opBitsA);
                    line.name = jumpTypeName(inst->jumpType);
                    break;
                case SCBE_MicroOp::Jump1:
                    // encoder.emitJump(inst->regA);
                    line.name = "jump";
                    line.args = form("[%s]", regName(inst->regA, OpBits::B64));
                    break;
                case SCBE_MicroOp::Jump2:
                    // encoder.emitJump(inst->jumpType, static_cast<int32_t>(inst->valueA), static_cast<int32_t>(inst->valueB));
                    line.name = jumpTypeName(inst->jumpType);
                    line.args = form("%08d", inst->valueA);
                    break;
                case SCBE_MicroOp::LoadParam:
                    // encoder.emitLoadParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA);
                    line.name = "mov";
                    line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), inst->valueA);
                    break;
                case SCBE_MicroOp::LoadSignedExtendParam:
                    // encoder.emitLoadSignedExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB);
                    line.name = "movs";
                    line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), inst->valueA);
                    break;
                case SCBE_MicroOp::LoadZeroExtendParam:
                    // encoder.emitLoadZeroExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB);
                    line.name = "movz";
                    line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), inst->valueA);
                    break;
                case SCBE_MicroOp::LoadAddressParam:
                    // encoder.emitLoadAddressParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->boolA);
                    line.name = "lea";
                    line.args = form("%s, %s ptr [rdi+<param%d>]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), inst->valueA);
                    break;
                case SCBE_MicroOp::StoreParam:
                    // encoder.emitStoreParam(static_cast<uint32_t>(inst->valueA), inst->regA, inst->opBitsA, inst->boolA);
                    line.name = "mov";
                    line.args = form("%s ptr [rdi+<param%d>], %s", opBitsName(inst->opBitsA), inst->valueA, regName(inst->regA, inst->opBitsA));
                    break;
                case SCBE_MicroOp::Load0:
                    // encoder.emitLoad(inst->regA, inst->regB, inst->opBitsA);
                    line.name = "mov";
                    line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                    break;
                case SCBE_MicroOp::Load1:
                    // encoder.emitLoad(inst->regA, inst->opBitsA);
                    line.name = "mov";
                    line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regA, inst->opBitsA));
                    break;
                case SCBE_MicroOp::Load3:
                    // encoder.emitLoad(inst->regA, inst->valueA);
                    line.name = "mov";
                    line.args = form("%s, %lld", regName(inst->regA, OpBits::B64), inst->valueA);
                    break;
                case SCBE_MicroOp::Load4:
                    // encoder.emitLoad(inst->regA, inst->valueA, inst->opBitsA);
                    line.name = cpuOpName(CPUOp::MOV, inst->opBitsA);
                    line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), valueName(inst->valueA, inst->opBitsA).cstr());
                    break;
                case SCBE_MicroOp::Load5:
                    // encoder.emitLoad(inst->regA, inst->regB, inst->valueA, inst->opBitsA);
                    line.name = cpuOpName(CPUOp::MOV, inst->opBitsA);
                    line.args = form("%s, %s ptr [%s+%xh]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, OpBits::B64), inst->valueA);
                    break;
                case SCBE_MicroOp::LoadSignedExtend0:
                    // encoder.emitLoadSignedExtend0(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                    line.name = "movs";
                    line.args = form("%s, %s ptr [%s+%xh]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, OpBits::B64), inst->valueA);
                    break;
                case SCBE_MicroOp::LoadSignedExtend1:
                    // encoder.emitLoadSignedExtend1(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                    line.name = "movs";
                    line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsB));
                    break;
                case SCBE_MicroOp::LoadZeroExtend0:
                    // encoder.emitLoadZeroExtend0(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                    line.name = "movz";
                    line.args = form("%s, %s ptr [%s+%xh]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, OpBits::B64), inst->valueA);
                    break;
                case SCBE_MicroOp::LoadZeroExtend1:
                    // encoder.emitLoadZeroExtend1(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                    line.name = "movz";
                    line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsB));
                    break;
                case SCBE_MicroOp::LoadAddress0:
                    // encoder.emitLoadAddress(inst->regA, inst->regB, inst->valueA);
                    line.name = "lea";
                    line.args = form("%s, [%s+%xh]", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64), inst->valueA);
                    break;
                case SCBE_MicroOp::LoadAddress1:
                    // encoder.emitLoadAddress(inst->regA, inst->regB, inst->regC, inst->valueA, inst->opBitsA);
                    line.name = "lea";
                    line.args = form("%s, [%s+%s*%d]", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA), regName(inst->regC, inst->opBitsA), inst->valueA);
                    break;
                case SCBE_MicroOp::Store0:
                    // encoder.emitStore(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                    line.name = cpuOpName(CPUOp::MOV, inst->opBitsA);
                    line.args = form("%s ptr [%s+%xh], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                    break;
                case SCBE_MicroOp::Store1:
                    // encoder.emitStore(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                    line.name = cpuOpName(CPUOp::MOV, inst->opBitsA);
                    line.args = form("%s ptr [%s+%xh], %d", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                    break;
                case SCBE_MicroOp::Cmp0:
                    // encoder.emitCmp(inst->regA, inst->regB, inst->opBitsA);
                    line.name = "cmp";
                    line.args = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                    break;
                case SCBE_MicroOp::Cmp1:
                    // encoder.emitCmp(inst->regA, inst->valueA, inst->opBitsA);
                    line.name = "cmp";
                    line.args = form("%s, %d", regName(inst->regA, inst->opBitsA), inst->valueA);
                    break;
                case SCBE_MicroOp::Cmp2:
                    // encoder.emitCmp(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                    line.name = "cmp";
                    line.args = form("%s ptr [%s+%xh], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                    break;
                case SCBE_MicroOp::Cmp3:
                    // encoder.emitCmp(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                    line.name = "cmp";
                    line.args = form("%s ptr [%s+%xh], %d", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                    break;
                case SCBE_MicroOp::Set:
                    // encoder.emitSet(inst->regA, inst->cpuCond);
                    line.name = form("set%s", cpuCondName(inst->cpuCond));
                    line.args = form("%s", regName(inst->regA, OpBits::B8));
                    break;
                case SCBE_MicroOp::Clear0:
                    // encoder.emitClear(inst->regA, inst->opBitsA);
                    line.name = "clear";
                    line.args = form("%s", regName(inst->regA, inst->opBitsA));
                    break;
                case SCBE_MicroOp::Clear1:
                    // encoder.emitClear(inst->regA, inst->valueA, static_cast<uint32_t>(inst->valueB));
                    line.name = "clear";
                    line.args = form("ptr %s+%d, %d", regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                    break;
                case SCBE_MicroOp::Copy:
                    // encoder.emitCopy(inst->regA, inst->regB, static_cast<uint32_t>(inst->valueA));
                    line.name = "copy";
                    line.args = form("ptr %s, ptr %s, %d", regName(inst->regA, OpBits::B64), regName(inst->regA, OpBits::B64), inst->valueA);
                    break;
                case SCBE_MicroOp::OpUnary0:
                    // encoder.emitOpUnary(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA);
                    line.name = cpuOpName(inst->cpuOp, inst->opBitsA);
                    line.args += form("%s ptr [%s+%xh]", opBitsName(inst->opBitsA), regName(inst->regA, inst->opBitsA), inst->valueA);
                    break;
                case SCBE_MicroOp::OpUnary1:
                    // encoder.emitOpUnary(inst->regA, inst->cpuOp, inst->opBitsA);
                    line.name = cpuOpName(inst->cpuOp, inst->opBitsA);
                    line.args += form("%s", regName(inst->regA, inst->opBitsA));
                    break;
                case SCBE_MicroOp::OpBinary0:
                    // encoder.emitOpBinary(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                    line.name = cpuOpName(inst->cpuOp, inst->opBitsA);
                    line.args += form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                    break;
                case SCBE_MicroOp::OpBinary1:
                    // encoder.emitOpBinary(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                    line.name = cpuOpName(inst->cpuOp, inst->opBitsA);
                    line.args += form("%s ptr [%s+%xh], %s", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, regName(inst->regB, inst->opBitsA));
                    break;
                case SCBE_MicroOp::OpBinary2:
                    // encoder.emitOpBinary(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                    line.name = cpuOpName(inst->cpuOp, inst->opBitsA);
                    line.args += form("%s, %d", regName(inst->regA, inst->opBitsA), inst->valueA);
                    break;
                case SCBE_MicroOp::OpBinary3:
                    // encoder.emitOpBinary(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                    line.name = cpuOpName(inst->cpuOp, inst->opBitsA);
                    line.args += form("%s ptr [%s+%xh], %d", opBitsName(inst->opBitsA), regName(inst->regA, OpBits::B64), inst->valueA, inst->valueB);
                    break;
                case SCBE_MicroOp::OpBinary4:
                    // encoder.emitOpBinary(inst->regA, inst->regB, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                    line.name = cpuOpName(inst->cpuOp, inst->opBitsA);
                    line.args += form("%s, %s ptr [%s+%xh]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, OpBits::B64), inst->valueA);
                    break;
                case SCBE_MicroOp::MulAdd:
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
        }

        ByteCodePrintOptions po;
        po.flags.add(BCPF_ASM_SCBE);
        ByteCode::alignPrintInstructions(po, lines, true);
        for (const auto& l : lines)
            ByteCode::printInstruction(po, nullptr, l);
    }
}
