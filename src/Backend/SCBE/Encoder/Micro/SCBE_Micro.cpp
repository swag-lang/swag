#include "pch.h"

#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/SCBE_Micro.h"
#include "SCBE_Optimizer.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"

SCBE_MicroOpInfo g_MicroOpInfos[] =
{
#undef SCBE_MICRO_OP
#define SCBE_MICRO_OP(__op, __left, __right) {#__op, __left, __right},
#include "Backend/SCBE/Encoder/Micro/SCBE_MicroOpList.h"

};

void SCBE_Micro::init(const BuildParameters& buildParameters)
{
    concat.init();
    SCBE_CPU::init(buildParameters);
}

SCBE_MicroInstruction* SCBE_Micro::addInstruction(SCBE_MicroOp op)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    if (nextIsJumpDest)
        inst->flags.add(MIF_JUMP_DEST);
    inst->op       = op;
    nextIsJumpDest = false;
    return inst;
}

void SCBE_Micro::emitEnter(uint32_t sizeStack)
{
    const auto inst = addInstruction(SCBE_MicroOp::Enter);
    inst->valueA    = sizeStack;
}

void SCBE_Micro::emitLeave()
{
    addInstruction(SCBE_MicroOp::Leave);
}

void SCBE_Micro::emitDebug(ByteCodeInstruction* ipAddr)
{
    const auto inst = addInstruction(SCBE_MicroOp::Debug);
    inst->valueA    = reinterpret_cast<uint64_t>(ipAddr);
}

void SCBE_Micro::emitLabel(uint32_t instructionIndex)
{
    const auto inst = addInstruction(SCBE_MicroOp::Label);
    inst->valueA    = instructionIndex;
    nextIsJumpDest  = true;
}

void SCBE_Micro::emitSymbolRelocationRef(const Utf8& name)
{
    const auto inst = addInstruction(SCBE_MicroOp::SymbolRelocationRef);
    inst->name      = name;
}

void SCBE_Micro::emitSymbolRelocationAddress(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = addInstruction(SCBE_MicroOp::SymbolRelocationAddress);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void SCBE_Micro::emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = addInstruction(SCBE_MicroOp::SymbolRelocationValue);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void SCBE_Micro::emitSymbolGlobalString(CPUReg reg, const Utf8& str)
{
    const auto inst = addInstruction(SCBE_MicroOp::SymbolGlobalString);
    inst->regA      = reg;
    inst->name      = str;
}

void SCBE_Micro::emitSymbolRelocationPtr(CPUReg reg, const Utf8& name)
{
    const auto inst = addInstruction(SCBE_MicroOp::SymbolRelocationPtr);
    inst->regA      = reg;
    inst->name      = name;
}

void SCBE_Micro::emitPush(CPUReg reg)
{
    const auto inst = addInstruction(SCBE_MicroOp::Push);
    inst->regA      = reg;
}

void SCBE_Micro::emitPop(CPUReg reg)
{
    const auto inst = addInstruction(SCBE_MicroOp::Pop);
    inst->regA      = reg;
}

void SCBE_Micro::emitRet()
{
    addInstruction(SCBE_MicroOp::Ret);
}

void SCBE_Micro::emitLoadCallerParam(CPUReg reg, uint32_t paramIdx, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadCallerParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadCallerZeroExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadCallerZeroExtendParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBE_Micro::emitLoadCallerAddressParam(CPUReg reg, uint32_t paramIdx)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadCallerAddressParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
}

void SCBE_Micro::emitStoreCallerParam(uint32_t paramIdx, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::StoreCallerParam);
    inst->valueA    = paramIdx;
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadRR(CPUReg regDst, CPUReg regSrc, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadR(CPUReg regDstSrc, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadR);
    inst->regA      = regDstSrc;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadRI64(CPUReg reg, uint64_t value)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadRI64);
    inst->regA      = reg;
    inst->valueA    = value;
}

void SCBE_Micro::emitLoadRI(CPUReg reg, uint64_t value, OpBits opBits)
{
    SWAG_ASSERT(!isFloat(opBits) || reg == CPUReg::XMM0 || reg == CPUReg::XMM1 || reg == CPUReg::XMM2 || reg == CPUReg::XMM3);
    const auto inst = addInstruction(SCBE_MicroOp::LoadRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadSignedExtendRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadSignedExtendRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBE_Micro::emitLoadSignedExtendRR(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadSignedExtendRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBE_Micro::emitLoadZeroExtendRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadZeroExtendRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBE_Micro::emitLoadZeroExtendRR(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadZeroExtendRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBE_Micro::emitLoadAddressM(CPUReg reg, CPUReg memReg, uint64_t memOffset)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadAddressM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
}

void SCBE_Micro::emitLoadAddressAddMul(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::LoadAddressAddMul);
    inst->regA      = regDst;
    inst->regB      = regSrc1;
    inst->regC      = regSrc2;
    inst->valueA    = mulValue;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitStoreMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::StoreMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitStoreMI(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::StoreMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitClearR(CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::ClearR);
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitClearM(CPUReg memReg, uint64_t memOffset, uint32_t count)
{
    const auto inst = addInstruction(SCBE_MicroOp::ClearM);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = count;
}

void SCBE_Micro::emitSetCC(CPUReg reg, CPUCondFlag setType)
{
    const auto inst = addInstruction(SCBE_MicroOp::SetCC);
    inst->regA      = reg;
    inst->cpuCond   = setType;
}

void SCBE_Micro::emitCmpRR(CPUReg reg0, CPUReg reg1, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::CmpRR);
    inst->regA      = reg0;
    inst->regB      = reg1;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitCmpRI(CPUReg reg, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::CmpRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitCmpMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::CmpMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitCmpMI(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::CmpMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitOpUnaryM(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::OpUnaryM);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitOpUnaryR(CPUReg reg, CPUOp op, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::OpUnaryR);
    inst->regA      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitOpBinaryRR(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    SWAG_ASSERT(!isFloat(opBits) || regDst == CPUReg::XMM0 || regDst == CPUReg::XMM1 || regDst == CPUReg::XMM2 || regDst == CPUReg::XMM3);
    
    const auto inst = addInstruction(SCBE_MicroOp::OpBinaryRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitOpBinaryRM(CPUReg regDst, CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(SCBE_MicroOp::OpBinaryRM);
    inst->regA      = regDst;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitOpBinaryMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(SCBE_MicroOp::OpBinaryMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitOpBinaryRI(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(SCBE_MicroOp::OpBinaryRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitOpBinaryMI(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(SCBE_MicroOp::OpBinaryMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    const auto inst = addInstruction(SCBE_MicroOp::JumpTable);
    inst->regA      = table;
    inst->regB      = offset;
    inst->valueA    = currentIp;
    inst->valueB    = offsetTable;
    inst->valueC    = numEntries;
}

void SCBE_Micro::emitJumpM(CPUReg reg)
{
    const auto inst = addInstruction(SCBE_MicroOp::JumpM);
    inst->regA      = reg;
}

void SCBE_Micro::emitJumpCI(CPUCondJump jumpType, uint32_t ipDest)
{
    const auto inst = addInstruction(SCBE_MicroOp::JumpCI);
    inst->jumpType  = jumpType;
    inst->valueA    = ipDest;
}

CPUJump SCBE_Micro::emitJump(CPUCondJump jumpType, OpBits opBits)
{
    CPUJump cpuJump;
    cpuJump.offset  = concat.totalCount();
    const auto inst = addInstruction(SCBE_MicroOp::JumpCC);
    inst->jumpType  = jumpType;
    inst->opBitsA   = opBits;
    return cpuJump;
}

void SCBE_Micro::emitPatchJump(const CPUJump& jump)
{
    const auto inst = addInstruction(SCBE_MicroOp::PatchJump);
    inst->valueA    = jump.offset;
    nextIsJumpDest  = true;
}

void SCBE_Micro::emitPatchJump(const CPUJump& jump, uint64_t offsetDestination)
{
    SWAG_ASSERT(false);
}

void SCBE_Micro::emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count)
{
    const auto inst = addInstruction(SCBE_MicroOp::Copy);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->valueA    = count;
}

void SCBE_Micro::emitNop()
{
    addInstruction(SCBE_MicroOp::Nop);
}

void SCBE_Micro::emitCallLocal(const Utf8& symbolName)
{
    const auto inst = addInstruction(SCBE_MicroOp::CallLocal);
    inst->name      = symbolName;
}

void SCBE_Micro::emitCallExtern(const Utf8& symbolName)
{
    const auto inst = addInstruction(SCBE_MicroOp::CallExtern);
    inst->name      = symbolName;
}

void SCBE_Micro::emitCallIndirect(CPUReg reg)
{
    const auto inst = addInstruction(SCBE_MicroOp::CallIndirect);
    inst->regA      = reg;
}

void SCBE_Micro::emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits)
{
    const auto inst = addInstruction(SCBE_MicroOp::MulAdd);
    inst->regA      = regDst;
    inst->regB      = regMul;
    inst->regC      = regAdd;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::encode(SCBE_CPU& encoder) const
{
    const auto num  = concat.totalCount() / sizeof(SCBE_MicroInstruction);
    auto       inst = reinterpret_cast<SCBE_MicroInstruction*>(concat.firstBucket->data);
    for (uint32_t i = 0; i < num; i++, inst++)
    {
        switch (inst->op)
        {
            case SCBE_MicroOp::End:
                break;
            case SCBE_MicroOp::Ignore:
                break;
            case SCBE_MicroOp::Enter:
                encoder.emitEnter(static_cast<uint32_t>(inst->valueA));
                break;
            case SCBE_MicroOp::Leave:
                encoder.emitLeave();
                break;
            case SCBE_MicroOp::Debug:
                encoder.emitDebug(reinterpret_cast<ByteCodeInstruction*>(inst->valueA));
                break;
            case SCBE_MicroOp::Label:
                encoder.emitLabel(static_cast<int32_t>(inst->valueA));
                break;
            case SCBE_MicroOp::SymbolRelocationRef:
                encoder.emitSymbolRelocationRef(inst->name);
                break;
            case SCBE_MicroOp::SymbolRelocationAddress:
                encoder.emitSymbolRelocationAddress(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                break;
            case SCBE_MicroOp::SymbolRelocationValue:
                encoder.emitSymbolRelocationValue(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                break;
            case SCBE_MicroOp::SymbolGlobalString:
                encoder.emitSymbolGlobalString(inst->regA, inst->name);
                break;
            case SCBE_MicroOp::SymbolRelocationPtr:
                encoder.emitSymbolRelocationPtr(inst->regA, inst->name);
                break;
            case SCBE_MicroOp::Push:
                encoder.emitPush(inst->regA);
                break;
            case SCBE_MicroOp::Pop:
                encoder.emitPop(inst->regA);
                break;
            case SCBE_MicroOp::Nop:
                encoder.emitNop();
                break;
            case SCBE_MicroOp::Ret:
                encoder.emitRet();
                break;
            case SCBE_MicroOp::CallLocal:
                encoder.emitCallLocal(inst->name);
                break;
            case SCBE_MicroOp::CallExtern:
                encoder.emitCallExtern(inst->name);
                break;
            case SCBE_MicroOp::CallIndirect:
                encoder.emitCallIndirect(inst->regA);
                break;
            case SCBE_MicroOp::JumpTable:
                encoder.emitJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->valueC);
                break;
            case SCBE_MicroOp::JumpCC:
            {
                const auto cmpJump = encoder.emitJump(inst->jumpType, inst->opBitsA);
                inst->valueA       = reinterpret_cast<uint64_t>(cmpJump.addr);
                inst->valueB       = cmpJump.offset;
                inst->opBitsA      = cmpJump.opBits;
                break;
            }
            case SCBE_MicroOp::PatchJump:
            {
                const auto jump = reinterpret_cast<const SCBE_MicroInstruction*>(concat.firstBucket->data + inst->valueA);
                CPUJump    cpuJump;
                cpuJump.addr   = reinterpret_cast<void*>(jump->valueA);
                cpuJump.offset = jump->valueB;
                cpuJump.opBits = jump->opBitsA;
                encoder.emitPatchJump(cpuJump);
                break;
            }
            case SCBE_MicroOp::JumpM:
                encoder.emitJumpM(inst->regA);
                break;
            case SCBE_MicroOp::JumpCI:
                encoder.emitJumpCI(inst->jumpType, static_cast<uint32_t>(inst->valueA));
                break;
            case SCBE_MicroOp::LoadCallerParam:
                encoder.emitLoadCallerParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA);
                break;
            case SCBE_MicroOp::LoadCallerAddressParam:
                encoder.emitLoadCallerAddressParam(inst->regA, static_cast<uint32_t>(inst->valueA));
                break;
            case SCBE_MicroOp::LoadCallerZeroExtendParam:
                encoder.emitLoadCallerZeroExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB);
                break;
            case SCBE_MicroOp::StoreCallerParam:
                encoder.emitStoreCallerParam(static_cast<uint32_t>(inst->valueA), inst->regA, inst->opBitsA);
                break;
            case SCBE_MicroOp::LoadRR:
                encoder.emitLoadRR(inst->regA, inst->regB, inst->opBitsA);
                break;
            case SCBE_MicroOp::LoadR:
                encoder.emitLoadR(inst->regA, inst->opBitsA);
                break;
            case SCBE_MicroOp::LoadRI64:
                encoder.emitLoadRI64(inst->regA, inst->valueA);
                break;
            case SCBE_MicroOp::LoadRI:
                encoder.emitLoadRI(inst->regA, inst->valueA, inst->opBitsA);
                break;
            case SCBE_MicroOp::LoadRM:
                encoder.emitLoadRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA);
                break;
            case SCBE_MicroOp::LoadSignedExtendRM:
                encoder.emitLoadSignedExtendRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                break;
            case SCBE_MicroOp::LoadSignedExtendRR:
                encoder.emitLoadSignedExtendRR(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                break;
            case SCBE_MicroOp::LoadZeroExtendRM:
                encoder.emitLoadZeroExtendRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                break;
            case SCBE_MicroOp::LoadZeroExtendRR:
                encoder.emitLoadZeroExtendRR(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                break;
            case SCBE_MicroOp::LoadAddressM:
                encoder.emitLoadAddressM(inst->regA, inst->regB, inst->valueA);
                break;
            case SCBE_MicroOp::LoadAddressAddMul:
                encoder.emitLoadAddressAddMul(inst->regA, inst->regB, inst->regC, inst->valueA, inst->opBitsA);
                break;
            case SCBE_MicroOp::StoreMR:
                encoder.emitStoreMR(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                break;
            case SCBE_MicroOp::StoreMI:
                encoder.emitStoreMI(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                break;
            case SCBE_MicroOp::CmpRR:
                encoder.emitCmpRR(inst->regA, inst->regB, inst->opBitsA);
                break;
            case SCBE_MicroOp::CmpRI:
                encoder.emitCmpRI(inst->regA, inst->valueA, inst->opBitsA);
                break;
            case SCBE_MicroOp::CmpMR:
                encoder.emitCmpMR(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                break;
            case SCBE_MicroOp::CmpMI:
                encoder.emitCmpMI(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                break;
            case SCBE_MicroOp::SetCC:
                encoder.emitSetCC(inst->regA, inst->cpuCond);
                break;
            case SCBE_MicroOp::ClearR:
                encoder.emitClearR(inst->regA, inst->opBitsA);
                break;
            case SCBE_MicroOp::ClearM:
                encoder.emitClearM(inst->regA, inst->valueA, static_cast<uint32_t>(inst->valueB));
                break;
            case SCBE_MicroOp::Copy:
                encoder.emitCopy(inst->regA, inst->regB, static_cast<uint32_t>(inst->valueA));
                break;
            case SCBE_MicroOp::OpUnaryM:
                encoder.emitOpUnaryM(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA);
                break;
            case SCBE_MicroOp::OpUnaryR:
                encoder.emitOpUnaryR(inst->regA, inst->cpuOp, inst->opBitsA);
                break;
            case SCBE_MicroOp::OpBinaryRR:
                encoder.emitOpBinaryRR(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::OpBinaryMR:
                encoder.emitOpBinaryMR(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::OpBinaryRI:
                encoder.emitOpBinaryRI(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::OpBinaryMI:
                encoder.emitOpBinaryMI(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::OpBinaryRM:
                encoder.emitOpBinaryRM(inst->regA, inst->regB, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::MulAdd:
                encoder.emitMulAdd(inst->regA, inst->regB, inst->regC, inst->opBitsA);
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }
    }

    encoder.emitLabels();

    if (cpuFct->bc->node && cpuFct->bc->node->hasAttribute(ATTRIBUTE_PRINT_ASM))
        print();
}

void SCBE_Micro::process(SCBE_CPU& encoder)
{
#ifdef SWAG_STATS
    g_Stats.numScbeInstructions += concat.totalCount();
#endif

    addInstruction(SCBE_MicroOp::End);
    concat.makeLinear();
    
    SCBE_Optimizer opt;
    opt.encoder = &encoder;
    opt.optimize(*this);
}
