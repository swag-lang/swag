#include "pch.h"

#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/SCBEMicro.h"
#include "SCBEOptimizer.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"

SCBEMicroOpInfo g_MicroOpInfos[] =
{
#undef SCBE_MICRO_OP
#define SCBE_MICRO_OP(__op, __left, __right) {#__op, __left, __right},
#include "Backend/SCBE/Encoder/Micro/SCBEMicroOpList.h"

};

void SCBEMicro::init(const BuildParameters& buildParameters)
{
    concat.init();
    SCBECPU::init(buildParameters);
}

SCBEMicroInstruction* SCBEMicro::addInstruction(SCBEMicroOp op)
{
    const auto inst = concat.addObj<SCBEMicroInstruction>();
    if (nextIsJumpDest)
        inst->flags.add(MIF_JUMP_DEST);
    inst->op       = op;
    nextIsJumpDest = false;
    return inst;
}

void SCBEMicro::emitEnter(uint32_t sizeStack)
{
    const auto inst = addInstruction(SCBEMicroOp::Enter);
    inst->valueA    = sizeStack;
}

void SCBEMicro::emitLeave()
{
    addInstruction(SCBEMicroOp::Leave);
}

void SCBEMicro::emitDebug(ByteCodeInstruction* ipAddr)
{
    const auto inst = addInstruction(SCBEMicroOp::Debug);
    inst->valueA    = reinterpret_cast<uint64_t>(ipAddr);
}

void SCBEMicro::emitLabel(uint32_t instructionIndex)
{
    const auto inst = addInstruction(SCBEMicroOp::Label);
    inst->valueA    = instructionIndex;
    nextIsJumpDest  = true;
}

void SCBEMicro::emitSymbolRelocationRef(const Utf8& name)
{
    const auto inst = addInstruction(SCBEMicroOp::SymbolRelocationRef);
    inst->name      = name;
}

void SCBEMicro::emitSymbolRelocationAddress(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = addInstruction(SCBEMicroOp::SymbolRelocationAddress);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void SCBEMicro::emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = addInstruction(SCBEMicroOp::SymbolRelocationValue);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void SCBEMicro::emitSymbolGlobalString(CPUReg reg, const Utf8& str)
{
    const auto inst = addInstruction(SCBEMicroOp::SymbolGlobalString);
    inst->regA      = reg;
    inst->name      = str;
}

void SCBEMicro::emitSymbolRelocationPtr(CPUReg reg, const Utf8& name)
{
    const auto inst = addInstruction(SCBEMicroOp::SymbolRelocationPtr);
    inst->regA      = reg;
    inst->name      = name;
}

void SCBEMicro::emitPush(CPUReg reg)
{
    const auto inst = addInstruction(SCBEMicroOp::Push);
    inst->regA      = reg;
}

void SCBEMicro::emitPop(CPUReg reg)
{
    const auto inst = addInstruction(SCBEMicroOp::Pop);
    inst->regA      = reg;
}

void SCBEMicro::emitRet()
{
    addInstruction(SCBEMicroOp::Ret);
}

void SCBEMicro::emitLoadCallerParam(CPUReg reg, uint32_t paramIdx, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadCallerParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitLoadCallerZeroExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadCallerZeroExtendParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBEMicro::emitLoadCallerAddressParam(CPUReg reg, uint32_t paramIdx)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadCallerAddressParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
}

void SCBEMicro::emitStoreCallerParam(uint32_t paramIdx, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::StoreCallerParam);
    inst->valueA    = paramIdx;
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitLoadRR(CPUReg regDst, CPUReg regSrc, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitLoadR(CPUReg regDstSrc, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadR);
    inst->regA      = regDstSrc;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitLoadRI64(CPUReg reg, uint64_t value)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadRI64);
    inst->regA      = reg;
    inst->valueA    = value;
}

void SCBEMicro::emitLoadRI(CPUReg reg, uint64_t value, OpBits opBits)
{
    SWAG_ASSERT(!isFloat(opBits) || reg == CPUReg::XMM0 || reg == CPUReg::XMM1 || reg == CPUReg::XMM2 || reg == CPUReg::XMM3);
    const auto inst = addInstruction(SCBEMicroOp::LoadRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitLoadRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitLoadSignedExtendRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadSignedExtendRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBEMicro::emitLoadSignedExtendRR(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadSignedExtendRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBEMicro::emitLoadZeroExtendRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadZeroExtendRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBEMicro::emitLoadZeroExtendRR(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadZeroExtendRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void SCBEMicro::emitLoadAddressM(CPUReg reg, CPUReg memReg, uint64_t memOffset)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadAddressM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
}

void SCBEMicro::emitLoadAddressAddMul(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::LoadAddressAddMul);
    inst->regA      = regDst;
    inst->regB      = regSrc1;
    inst->regC      = regSrc2;
    inst->valueA    = mulValue;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitStoreMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::StoreMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitStoreMI(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::StoreMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitClearR(CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::ClearR);
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitClearM(CPUReg memReg, uint64_t memOffset, uint32_t count)
{
    const auto inst = addInstruction(SCBEMicroOp::ClearM);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = count;
}

void SCBEMicro::emitSetCC(CPUReg reg, CPUCondFlag setType)
{
    const auto inst = addInstruction(SCBEMicroOp::SetCC);
    inst->regA      = reg;
    inst->cpuCond   = setType;
}

void SCBEMicro::emitCmpRR(CPUReg reg0, CPUReg reg1, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::CmpRR);
    inst->regA      = reg0;
    inst->regB      = reg1;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitCmpRI(CPUReg reg, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::CmpRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitCmpMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::CmpMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitCmpMI(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::CmpMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitOpUnaryM(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::OpUnaryM);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitOpUnaryR(CPUReg reg, CPUOp op, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::OpUnaryR);
    inst->regA      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void SCBEMicro::emitOpBinaryRR(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    SWAG_ASSERT(!isFloat(opBits) || regDst == CPUReg::XMM0 || regDst == CPUReg::XMM1 || regDst == CPUReg::XMM2 || regDst == CPUReg::XMM3);
    
    const auto inst = addInstruction(SCBEMicroOp::OpBinaryRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBEMicro::emitOpBinaryRM(CPUReg regDst, CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(SCBEMicroOp::OpBinaryRM);
    inst->regA      = regDst;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBEMicro::emitOpBinaryMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(SCBEMicroOp::OpBinaryMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBEMicro::emitOpBinaryRI(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(SCBEMicroOp::OpBinaryRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBEMicro::emitOpBinaryMI(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(SCBEMicroOp::OpBinaryMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBEMicro::emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    const auto inst = addInstruction(SCBEMicroOp::JumpTable);
    inst->regA      = table;
    inst->regB      = offset;
    inst->valueA    = currentIp;
    inst->valueB    = offsetTable;
    inst->valueC    = numEntries;
}

void SCBEMicro::emitJumpM(CPUReg reg)
{
    const auto inst = addInstruction(SCBEMicroOp::JumpM);
    inst->regA      = reg;
}

void SCBEMicro::emitJumpCI(CPUCondJump jumpType, uint32_t ipDest)
{
    const auto inst = addInstruction(SCBEMicroOp::JumpCI);
    inst->jumpType  = jumpType;
    inst->valueA    = ipDest;
}

CPUJump SCBEMicro::emitJump(CPUCondJump jumpType, OpBits opBits)
{
    CPUJump cpuJump;
    cpuJump.offset  = concat.totalCount();
    const auto inst = addInstruction(SCBEMicroOp::JumpCC);
    inst->jumpType  = jumpType;
    inst->opBitsA   = opBits;
    return cpuJump;
}

void SCBEMicro::emitPatchJump(const CPUJump& jump)
{
    const auto inst = addInstruction(SCBEMicroOp::PatchJump);
    inst->valueA    = jump.offset;
    nextIsJumpDest  = true;
}

void SCBEMicro::emitPatchJump(const CPUJump& jump, uint64_t offsetDestination)
{
    SWAG_ASSERT(false);
}

void SCBEMicro::emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count)
{
    const auto inst = addInstruction(SCBEMicroOp::Copy);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->valueA    = count;
}

void SCBEMicro::emitNop()
{
    addInstruction(SCBEMicroOp::Nop);
}

void SCBEMicro::emitCallLocal(const Utf8& symbolName)
{
    const auto inst = addInstruction(SCBEMicroOp::CallLocal);
    inst->name      = symbolName;
}

void SCBEMicro::emitCallExtern(const Utf8& symbolName)
{
    const auto inst = addInstruction(SCBEMicroOp::CallExtern);
    inst->name      = symbolName;
}

void SCBEMicro::emitCallIndirect(CPUReg reg)
{
    const auto inst = addInstruction(SCBEMicroOp::CallIndirect);
    inst->regA      = reg;
}

void SCBEMicro::emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits)
{
    const auto inst = addInstruction(SCBEMicroOp::MulAdd);
    inst->regA      = regDst;
    inst->regB      = regMul;
    inst->regC      = regAdd;
    inst->opBitsA   = opBits;
}

void SCBEMicro::encode(SCBECPU& encoder) const
{
    const auto num  = concat.totalCount() / sizeof(SCBEMicroInstruction);
    auto       inst = reinterpret_cast<SCBEMicroInstruction*>(concat.firstBucket->data);
    for (uint32_t i = 0; i < num; i++, inst++)
    {
        switch (inst->op)
        {
            case SCBEMicroOp::End:
                break;
            case SCBEMicroOp::Ignore:
                break;
            case SCBEMicroOp::Enter:
                encoder.emitEnter(static_cast<uint32_t>(inst->valueA));
                break;
            case SCBEMicroOp::Leave:
                encoder.emitLeave();
                break;
            case SCBEMicroOp::Debug:
                encoder.emitDebug(reinterpret_cast<ByteCodeInstruction*>(inst->valueA));
                break;
            case SCBEMicroOp::Label:
                encoder.emitLabel(static_cast<int32_t>(inst->valueA));
                break;
            case SCBEMicroOp::SymbolRelocationRef:
                encoder.emitSymbolRelocationRef(inst->name);
                break;
            case SCBEMicroOp::SymbolRelocationAddress:
                encoder.emitSymbolRelocationAddress(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                break;
            case SCBEMicroOp::SymbolRelocationValue:
                encoder.emitSymbolRelocationValue(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                break;
            case SCBEMicroOp::SymbolGlobalString:
                encoder.emitSymbolGlobalString(inst->regA, inst->name);
                break;
            case SCBEMicroOp::SymbolRelocationPtr:
                encoder.emitSymbolRelocationPtr(inst->regA, inst->name);
                break;
            case SCBEMicroOp::Push:
                encoder.emitPush(inst->regA);
                break;
            case SCBEMicroOp::Pop:
                encoder.emitPop(inst->regA);
                break;
            case SCBEMicroOp::Nop:
                encoder.emitNop();
                break;
            case SCBEMicroOp::Ret:
                encoder.emitRet();
                break;
            case SCBEMicroOp::CallLocal:
                encoder.emitCallLocal(inst->name);
                break;
            case SCBEMicroOp::CallExtern:
                encoder.emitCallExtern(inst->name);
                break;
            case SCBEMicroOp::CallIndirect:
                encoder.emitCallIndirect(inst->regA);
                break;
            case SCBEMicroOp::JumpTable:
                encoder.emitJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->valueC);
                break;
            case SCBEMicroOp::JumpCC:
            {
                const auto cmpJump = encoder.emitJump(inst->jumpType, inst->opBitsA);
                inst->valueA       = reinterpret_cast<uint64_t>(cmpJump.addr);
                inst->valueB       = cmpJump.offset;
                inst->opBitsA      = cmpJump.opBits;
                break;
            }
            case SCBEMicroOp::PatchJump:
            {
                const auto jump = reinterpret_cast<const SCBEMicroInstruction*>(concat.firstBucket->data + inst->valueA);
                CPUJump    cpuJump;
                cpuJump.addr   = reinterpret_cast<void*>(jump->valueA);
                cpuJump.offset = jump->valueB;
                cpuJump.opBits = jump->opBitsA;
                encoder.emitPatchJump(cpuJump);
                break;
            }
            case SCBEMicroOp::JumpM:
                encoder.emitJumpM(inst->regA);
                break;
            case SCBEMicroOp::JumpCI:
                encoder.emitJumpCI(inst->jumpType, static_cast<uint32_t>(inst->valueA));
                break;
            case SCBEMicroOp::LoadCallerParam:
                encoder.emitLoadCallerParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA);
                break;
            case SCBEMicroOp::LoadCallerAddressParam:
                encoder.emitLoadCallerAddressParam(inst->regA, static_cast<uint32_t>(inst->valueA));
                break;
            case SCBEMicroOp::LoadCallerZeroExtendParam:
                encoder.emitLoadCallerZeroExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB);
                break;
            case SCBEMicroOp::StoreCallerParam:
                encoder.emitStoreCallerParam(static_cast<uint32_t>(inst->valueA), inst->regA, inst->opBitsA);
                break;
            case SCBEMicroOp::LoadRR:
                encoder.emitLoadRR(inst->regA, inst->regB, inst->opBitsA);
                break;
            case SCBEMicroOp::LoadR:
                encoder.emitLoadR(inst->regA, inst->opBitsA);
                break;
            case SCBEMicroOp::LoadRI64:
                encoder.emitLoadRI64(inst->regA, inst->valueA);
                break;
            case SCBEMicroOp::LoadRI:
                encoder.emitLoadRI(inst->regA, inst->valueA, inst->opBitsA);
                break;
            case SCBEMicroOp::LoadRM:
                encoder.emitLoadRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA);
                break;
            case SCBEMicroOp::LoadSignedExtendRM:
                encoder.emitLoadSignedExtendRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                break;
            case SCBEMicroOp::LoadSignedExtendRR:
                encoder.emitLoadSignedExtendRR(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                break;
            case SCBEMicroOp::LoadZeroExtendRM:
                encoder.emitLoadZeroExtendRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                break;
            case SCBEMicroOp::LoadZeroExtendRR:
                encoder.emitLoadZeroExtendRR(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                break;
            case SCBEMicroOp::LoadAddressM:
                encoder.emitLoadAddressM(inst->regA, inst->regB, inst->valueA);
                break;
            case SCBEMicroOp::LoadAddressAddMul:
                encoder.emitLoadAddressAddMul(inst->regA, inst->regB, inst->regC, inst->valueA, inst->opBitsA);
                break;
            case SCBEMicroOp::StoreMR:
                encoder.emitStoreMR(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                break;
            case SCBEMicroOp::StoreMI:
                encoder.emitStoreMI(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                break;
            case SCBEMicroOp::CmpRR:
                encoder.emitCmpRR(inst->regA, inst->regB, inst->opBitsA);
                break;
            case SCBEMicroOp::CmpRI:
                encoder.emitCmpRI(inst->regA, inst->valueA, inst->opBitsA);
                break;
            case SCBEMicroOp::CmpMR:
                encoder.emitCmpMR(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                break;
            case SCBEMicroOp::CmpMI:
                encoder.emitCmpMI(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                break;
            case SCBEMicroOp::SetCC:
                encoder.emitSetCC(inst->regA, inst->cpuCond);
                break;
            case SCBEMicroOp::ClearR:
                encoder.emitClearR(inst->regA, inst->opBitsA);
                break;
            case SCBEMicroOp::ClearM:
                encoder.emitClearM(inst->regA, inst->valueA, static_cast<uint32_t>(inst->valueB));
                break;
            case SCBEMicroOp::Copy:
                encoder.emitCopy(inst->regA, inst->regB, static_cast<uint32_t>(inst->valueA));
                break;
            case SCBEMicroOp::OpUnaryM:
                encoder.emitOpUnaryM(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA);
                break;
            case SCBEMicroOp::OpUnaryR:
                encoder.emitOpUnaryR(inst->regA, inst->cpuOp, inst->opBitsA);
                break;
            case SCBEMicroOp::OpBinaryRR:
                encoder.emitOpBinaryRR(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBEMicroOp::OpBinaryMR:
                encoder.emitOpBinaryMR(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBEMicroOp::OpBinaryRI:
                encoder.emitOpBinaryRI(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBEMicroOp::OpBinaryMI:
                encoder.emitOpBinaryMI(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBEMicroOp::OpBinaryRM:
                encoder.emitOpBinaryRM(inst->regA, inst->regB, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBEMicroOp::MulAdd:
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

void SCBEMicro::process(SCBECPU& encoder)
{
#ifdef SWAG_STATS
    g_Stats.numScbeInstructions += concat.totalCount();
#endif

    addInstruction(SCBEMicroOp::End);
    concat.makeLinear();
    
    SCBEOptimizer opt;
    opt.encoder = &encoder;
    opt.optimize(*this);
}
