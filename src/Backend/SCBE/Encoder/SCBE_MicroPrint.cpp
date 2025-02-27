#include "pch.h"

#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/SCBE_Micro.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeManager.h"

namespace
{
    const char* cpuOpName(CPUOp op)
    {
        switch (op)
        {
            case CPUOp::ADD:
                return "add";
            case CPUOp::SUB:
                return "sub";
            case CPUOp::MUL:
                return "mul";
            case CPUOp::DIV:
                return "div";
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
        }

        return "???";
    }

    const char* regName(CPUReg reg, OpBits opBits)
    {
        switch (reg)
        {
            case CPUReg::RAX:
                return opBits == OpBits::B64 ? "rax" : opBits == OpBits::B32   ? "eax"
                                                       : opBits == OpBits::B16 ? "ax"
                                                                               : "al";
            case CPUReg::RBX:
                return "rbx";
            case CPUReg::RCX:
                return "rcx";
            case CPUReg::RDX:
                return "rdx";
            case CPUReg::RDI:
                return "rdi";
            case CPUReg::RSP:
                return "rsp";
            case CPUReg::R8:
                return "r8";
            case CPUReg::R9:
                return "r9";
            case CPUReg::R10:
                return "r10";
        }

        return "???";
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
}

#pragma optimize("", off)
void SCBE_Micro::print() const
{
    const auto num  = concat.totalCount() / sizeof(SCBE_MicroInstruction);
    auto       inst = reinterpret_cast<SCBE_MicroInstruction*>(concat.firstBucket->data);
    for (uint32_t i = 0; i < num; i++, inst++)
    {
        Utf8 instruction, arguments;

        switch (inst->op)
        {
            case SCBE_MicroOp::Debug:
                cpuFct->bc->printInstruction({}, reinterpret_cast<ByteCodeInstruction*>(inst->valueA));
                continue;
            case SCBE_MicroOp::AddLabel:
                continue;

            case SCBE_MicroOp::SymbolRelocationRef:
                // encoder.emitSymbolRelocationRef(inst->name);
                break;
            case SCBE_MicroOp::SymbolRelocationAddr:
                // encoder.emitSymbolRelocationAddr(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                instruction = "mov";
                break;
            case SCBE_MicroOp::SymbolRelocationValue:
                // encoder.emitSymbolRelocationValue(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                instruction = "mov";
                break;
            case SCBE_MicroOp::SymbolGlobalString:
                // encoder.emitSymbolGlobalString(inst->regA, inst->name);
                instruction = "mov";
                break;
            case SCBE_MicroOp::SymbolRelocationPtr:
                // encoder.emitSymbolRelocationPtr(inst->regA, inst->name);
                instruction = "mov";
                break;
            case SCBE_MicroOp::Push:
                // encoder.emitPush(inst->regA);
                instruction = "push";
                arguments   = regName(inst->regA, OpBits::B64);
                break;
            case SCBE_MicroOp::Pop:
                // encoder.emitPop(inst->regA);
                instruction = "pop";
                arguments   = regName(inst->regA, OpBits::B64);
                break;
            case SCBE_MicroOp::Enter:
                // encoder.emitEnter(static_cast<uint32_t>(inst->valueA));
                instruction = "enter";
                break;
            case SCBE_MicroOp::Leave:
                // encoder.emitLeave();
                instruction = "leave";
                break;
            case SCBE_MicroOp::Nop:
                // encoder.emitNop();
                instruction = "nop";
                break;
            case SCBE_MicroOp::Ret:
                // encoder.emitRet();
                instruction = "ret";
                break;
            case SCBE_MicroOp::CallLocal:
                // encoder.emitCallLocal(inst->name);
                instruction = "call near";
                arguments   = inst->name;
                break;
            case SCBE_MicroOp::CallExtern:
                // encoder.emitCallExtern(inst->name);
                instruction = "call far";
                arguments   = inst->name;
                break;
            case SCBE_MicroOp::CallIndirect:
                // encoder.emitCallIndirect(inst->regA);
                instruction = "call";
                arguments   = form("[%s]", regName(inst->regA, OpBits::B64));
                break;
            case SCBE_MicroOp::JumpTable:
                // encoder.emitJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), static_cast<uint32_t>(inst->valueC));
                instruction = "jumptable";
                break;
            case SCBE_MicroOp::Jump0:
                // const auto cmpJump = encoder.emitJump(inst->jumpType, inst->opBitsA);
                instruction = "jump";
                break;
            case SCBE_MicroOp::PatchJump0:
                // const auto jump = reinterpret_cast<const SCBE_MicroInstruction*>(concat.firstBucket->data + inst->valueA);
                break;
            case SCBE_MicroOp::PatchJump1:
                // const auto jump = reinterpret_cast<SCBE_MicroInstruction*>(inst->valueA);
                break;
            case SCBE_MicroOp::Jump1:
                // encoder.emitJump(inst->regA);
                instruction = "jump";
                break;
            case SCBE_MicroOp::Jump2:
                // encoder.emitJump(inst->jumpType, static_cast<int32_t>(inst->valueA), static_cast<int32_t>(inst->valueB));
                instruction = "jump";
                break;
            case SCBE_MicroOp::LoadParam:
                // encoder.emitLoadParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA);
                instruction = "mov";
                break;
            case SCBE_MicroOp::LoadExtendParam:
                // encoder.emitLoadExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB, inst->boolA);
                instruction = "mov";
                break;
            case SCBE_MicroOp::LoadAddressParam:
                // encoder.emitLoadAddressParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->boolA);
                instruction = "lea";
                break;
            case SCBE_MicroOp::StoreParam:
                // encoder.emitStoreParam(static_cast<uint32_t>(inst->valueA), inst->regA, inst->opBitsA, inst->boolA);
                instruction = "mov";
                break;
            case SCBE_MicroOp::Load0:
                // encoder.emitLoad(inst->regA, inst->regB, inst->opBitsA);
                instruction = "mov";
                arguments   = form("%s, %s", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsA));
                break;
            case SCBE_MicroOp::Load1:
                // encoder.emitLoad(inst->regA, inst->opBitsA);
                instruction = "mov";
                break;
            case SCBE_MicroOp::Load2:
                // encoder.emitLoad(inst->regA, inst->regB, inst->valueA, inst->valueB, inst->boolA, inst->cpuOp, inst->opBitsA);
                instruction = "mov";
                break;
            case SCBE_MicroOp::Load3:
                // encoder.emitLoad(inst->regA, inst->valueA);
                instruction = "mov";
                arguments   = form("%s, %lld", regName(inst->regA, OpBits::B64), inst->valueA);
                break;
            case SCBE_MicroOp::Load4:
                // encoder.emitLoad(inst->regA, inst->valueA, inst->opBitsA);
                instruction = "mov";
                break;
            case SCBE_MicroOp::Load5:
                // encoder.emitLoad(inst->regA, inst->regB, inst->valueA, inst->opBitsA);
                instruction = "mov";
                arguments   = form("%s, %s ptr [%s+%d]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsA), regName(inst->regB, inst->opBitsA), inst->valueA);
                break;
            case SCBE_MicroOp::LoadExtend0:
                // encoder.emitLoadExtend(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB, inst->boolA);
                instruction = inst->boolA ? "movs" : "movz";
                arguments   = form("%s, %s ptr [%s+%d]", regName(inst->regA, inst->opBitsA), opBitsName(inst->opBitsB), regName(inst->regB, inst->opBitsA), inst->valueA);
                break;
            case SCBE_MicroOp::LoadExtend1:
                // encoder.emitLoadExtend(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB, inst->boolA);
                instruction = inst->boolA ? "movs" : "movz";
                arguments   = form("%s, %s ptr [%s+%d]", regName(inst->regA, inst->opBitsA), regName(inst->regB, inst->opBitsB));
                break;
            case SCBE_MicroOp::LoadAddress0:
                // encoder.emitLoadAddress(inst->regA, inst->regB, inst->valueA);
                instruction = "lea";
                arguments   = form("%s, [%s+%d]", regName(inst->regA, OpBits::B64), regName(inst->regB, OpBits::B64), inst->valueA);
                break;
            case SCBE_MicroOp::LoadAddress1:
                // encoder.emitLoadAddress(inst->regA, inst->regB, inst->regC, inst->valueA, inst->opBitsA);
                instruction = "lea";
                break;
            case SCBE_MicroOp::Store0:
                // encoder.emitStore(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                instruction = "mov";
                arguments   = form("%s ptr [%s+%d], %s", opBitsName(inst->opBitsA), regName(inst->regA, inst->opBitsA), inst->valueA, regName(inst->regB, inst->opBitsA));
                break;
            case SCBE_MicroOp::Store1:
                // encoder.emitStore(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                instruction = "mov";
                arguments   = form("%s ptr [%s+%d], %s", opBitsName(inst->opBitsA), regName(inst->regA, inst->opBitsA), inst->valueA, inst->valueB);
                break;
            case SCBE_MicroOp::Cmp0:
                // encoder.emitCmp(inst->regA, inst->regB, inst->opBitsA);
                instruction = "cmp";
                break;
            case SCBE_MicroOp::Cmp1:
                // encoder.emitCmp(inst->regA, inst->valueA, inst->opBitsA);
                instruction = "cmp";
                break;
            case SCBE_MicroOp::Cmp2:
                // encoder.emitCmp(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                instruction = "cmp";
                break;
            case SCBE_MicroOp::Cmp3:
                // encoder.emitCmp(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                instruction = "cmp";
                break;
            case SCBE_MicroOp::Set:
                // encoder.emitSet(inst->regA, inst->cpuCond);
                instruction = "set";
                break;
            case SCBE_MicroOp::Clear0:
                // encoder.emitClear(inst->regA, inst->opBitsA);
                instruction = "clear";
                break;
            case SCBE_MicroOp::Clear1:
                // encoder.emitClear(inst->regA, inst->valueA, static_cast<uint32_t>(inst->valueB));
                instruction = "clear";
                break;
            case SCBE_MicroOp::Copy:
                // encoder.emitCopy(inst->regA, inst->regB, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                instruction = "copy";
                break;
            case SCBE_MicroOp::OpUnary0:
                // encoder.emitOpUnary(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA);
                instruction = cpuOpName(inst->cpuOp);
                break;
            case SCBE_MicroOp::OpUnary1:
                // encoder.emitOpUnary(inst->regA, inst->cpuOp, inst->opBitsA);
                instruction = cpuOpName(inst->cpuOp);
                break;
            case SCBE_MicroOp::OpBinary0:
                // encoder.emitOpBinary(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                instruction = cpuOpName(inst->cpuOp);
                break;
            case SCBE_MicroOp::OpBinary1:
                // encoder.emitOpBinary(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                instruction = cpuOpName(inst->cpuOp);
                break;
            case SCBE_MicroOp::OpBinary2:
                // encoder.emitOpBinary(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                instruction = cpuOpName(inst->cpuOp);
                break;
            case SCBE_MicroOp::OpBinary3:
                // encoder.emitOpBinary(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                instruction = cpuOpName(inst->cpuOp);
                break;
            case SCBE_MicroOp::MulAdd:
                // encoder.emitMulAdd(inst->regA, inst->regB, inst->regC, inst->opBitsA);
                break;
        }

        g_Log.setColor(LogColor::Gray);
        while (instruction.length() < 12)
            instruction += " ";
        g_Log.print(form("%s %s\n", instruction.cstr(), arguments.cstr()));
    }
}
