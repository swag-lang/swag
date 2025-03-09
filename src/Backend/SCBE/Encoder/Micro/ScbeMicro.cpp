#include "pch.h"

#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "ScbeOptimizer.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"

ScbeMicroOpInfo g_MicroOpInfos[] =
{
#undef SCBE_MICRO_OP
#define SCBE_MICRO_OP(__op, __left, __right) {#__op, __left, __right},
#include "Backend/SCBE/Encoder/Micro/ScbeMicroOpList.h"

};

void ScbeMicro::init(const BuildParameters& buildParameters)
{
    concat.init();
    ScbeCPU::init(buildParameters);
}

ScbeMicroInstruction* ScbeMicro::addInstruction(ScbeMicroOp op)
{
    const auto inst = concat.addObj<ScbeMicroInstruction>();
    if (nextIsJumpDest)
        inst->flags.add(MIF_JUMP_DEST);
    inst->op       = op;
    nextIsJumpDest = false;
    return inst;
}

void ScbeMicro::emitEnter(uint32_t sizeStack)
{
    const auto inst = addInstruction(ScbeMicroOp::Enter);
    inst->valueA    = sizeStack;
}

void ScbeMicro::emitLeave()
{
    addInstruction(ScbeMicroOp::Leave);
}

void ScbeMicro::emitDebug(ByteCodeInstruction* ipAddr)
{
    const auto inst = addInstruction(ScbeMicroOp::Debug);
    inst->valueA    = reinterpret_cast<uint64_t>(ipAddr);
}

void ScbeMicro::emitLabel(uint32_t instructionIndex)
{
    const auto inst = addInstruction(ScbeMicroOp::Label);
    inst->valueA    = instructionIndex;
    nextIsJumpDest  = true;
}

void ScbeMicro::emitSymbolRelocationRef(const Utf8& name)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocationRef);
    inst->name      = name;
}

void ScbeMicro::emitSymbolRelocationAddress(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocationAddress);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void ScbeMicro::emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocationValue);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void ScbeMicro::emitSymbolGlobalString(CPUReg reg, const Utf8& str)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolGlobalString);
    inst->regA      = reg;
    inst->name      = str;
}

void ScbeMicro::emitSymbolRelocationPtr(CPUReg reg, const Utf8& name)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocationPtr);
    inst->regA      = reg;
    inst->name      = name;
}

void ScbeMicro::emitPush(CPUReg reg)
{
    const auto inst = addInstruction(ScbeMicroOp::Push);
    inst->regA      = reg;
}

void ScbeMicro::emitPop(CPUReg reg)
{
    const auto inst = addInstruction(ScbeMicroOp::Pop);
    inst->regA      = reg;
}

void ScbeMicro::emitRet()
{
    addInstruction(ScbeMicroOp::Ret);
}

void ScbeMicro::emitLoadCallerParam(CPUReg reg, uint32_t paramIdx, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallerParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitLoadCallerZeroExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallerZeroExtendParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::emitLoadCallerAddressParam(CPUReg reg, uint32_t paramIdx)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallerAddressParam);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
}

void ScbeMicro::emitStoreCallerParam(uint32_t paramIdx, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::StoreCallerParam);
    inst->valueA    = paramIdx;
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitLoadRR(CPUReg regDst, CPUReg regSrc, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitLoadR(CPUReg regDstSrc, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadR);
    inst->regA      = regDstSrc;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitLoadRI64(CPUReg reg, uint64_t value)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRI64);
    inst->regA      = reg;
    inst->valueA    = value;
}

void ScbeMicro::emitLoadRI(CPUReg reg, uint64_t value, OpBits opBits)
{
    SWAG_ASSERT(!isFloat(opBits) || reg == CPUReg::XMM0 || reg == CPUReg::XMM1 || reg == CPUReg::XMM2 || reg == CPUReg::XMM3);
    const auto inst = addInstruction(ScbeMicroOp::LoadRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitLoadRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitLoadSignedExtendRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadSignedExtendRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::emitLoadSignedExtendRR(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadSignedExtendRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::emitLoadZeroExtendRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadZeroExtendRM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::emitLoadZeroExtendRR(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadZeroExtendRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::emitLoadAddressM(CPUReg reg, CPUReg memReg, uint64_t memOffset)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadAddressM);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
}

void ScbeMicro::emitLoadAddressAddMul(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadAddressAddMul);
    inst->regA      = regDst;
    inst->regB      = regSrc1;
    inst->regC      = regSrc2;
    inst->valueA    = mulValue;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitStoreMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::StoreMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitStoreMI(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::StoreMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitClearR(CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::ClearR);
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitClearM(CPUReg memReg, uint64_t memOffset, uint32_t count)
{
    const auto inst = addInstruction(ScbeMicroOp::ClearM);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = count;
}

void ScbeMicro::emitSetCC(CPUReg reg, CPUCondFlag setType)
{
    const auto inst = addInstruction(ScbeMicroOp::SetCC);
    inst->regA      = reg;
    inst->cpuCond   = setType;
}

void ScbeMicro::emitCmpRR(CPUReg reg0, CPUReg reg1, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpRR);
    inst->regA      = reg0;
    inst->regB      = reg1;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitCmpRI(CPUReg reg, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitCmpMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitCmpMI(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitOpUnaryM(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::OpUnaryM);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitOpUnaryR(CPUReg reg, CPUOp op, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::OpUnaryR);
    inst->regA      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitOpBinaryRR(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    SWAG_ASSERT(!isFloat(opBits) || regDst == CPUReg::XMM0 || regDst == CPUReg::XMM1 || regDst == CPUReg::XMM2 || regDst == CPUReg::XMM3);
    
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRR);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void ScbeMicro::emitOpBinaryRM(CPUReg regDst, CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRM);
    inst->regA      = regDst;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void ScbeMicro::emitOpBinaryMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryMR);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void ScbeMicro::emitOpBinaryRI(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRI);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void ScbeMicro::emitOpBinaryMI(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryMI);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void ScbeMicro::emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpTable);
    inst->regA      = table;
    inst->regB      = offset;
    inst->valueA    = currentIp;
    inst->valueB    = offsetTable;
    inst->valueC    = numEntries;
}

void ScbeMicro::emitJumpM(CPUReg reg)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpM);
    inst->regA      = reg;
}

void ScbeMicro::emitJumpCI(CPUCondJump jumpType, uint32_t ipDest)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpCI);
    inst->jumpType  = jumpType;
    inst->valueA    = ipDest;
}

CPUJump ScbeMicro::emitJump(CPUCondJump jumpType, OpBits opBits)
{
    CPUJump cpuJump;
    cpuJump.offset  = concat.totalCount();
    const auto inst = addInstruction(ScbeMicroOp::JumpCC);
    inst->jumpType  = jumpType;
    inst->opBitsA   = opBits;
    return cpuJump;
}

void ScbeMicro::emitPatchJump(const CPUJump& jump)
{
    const auto inst = addInstruction(ScbeMicroOp::PatchJump);
    inst->valueA    = jump.offset;
    nextIsJumpDest  = true;
}

void ScbeMicro::emitPatchJump(const CPUJump& jump, uint64_t offsetDestination)
{
    SWAG_ASSERT(false);
}

void ScbeMicro::emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count)
{
    const auto inst = addInstruction(ScbeMicroOp::Copy);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->valueA    = count;
}

void ScbeMicro::emitNop()
{
    addInstruction(ScbeMicroOp::Nop);
}

void ScbeMicro::emitCallLocal(const Utf8& symbolName)
{
    const auto inst = addInstruction(ScbeMicroOp::CallLocal);
    inst->name      = symbolName;
}

void ScbeMicro::emitCallExtern(const Utf8& symbolName)
{
    const auto inst = addInstruction(ScbeMicroOp::CallExtern);
    inst->name      = symbolName;
}

void ScbeMicro::emitCallIndirect(CPUReg reg)
{
    const auto inst = addInstruction(ScbeMicroOp::CallIndirect);
    inst->regA      = reg;
}

void ScbeMicro::emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::MulAdd);
    inst->regA      = regDst;
    inst->regB      = regMul;
    inst->regC      = regAdd;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encode(ScbeCPU& encoder) const
{
    const auto num  = concat.totalCount() / sizeof(ScbeMicroInstruction);
    auto       inst = reinterpret_cast<ScbeMicroInstruction*>(concat.firstBucket->data);
    for (uint32_t i = 0; i < num; i++, inst++)
    {
        switch (inst->op)
        {
            case ScbeMicroOp::End:
                break;
            case ScbeMicroOp::Ignore:
                break;
            case ScbeMicroOp::Enter:
                encoder.emitEnter(static_cast<uint32_t>(inst->valueA));
                break;
            case ScbeMicroOp::Leave:
                encoder.emitLeave();
                break;
            case ScbeMicroOp::Debug:
                encoder.emitDebug(reinterpret_cast<ByteCodeInstruction*>(inst->valueA));
                break;
            case ScbeMicroOp::Label:
                encoder.emitLabel(static_cast<int32_t>(inst->valueA));
                break;
            case ScbeMicroOp::SymbolRelocationRef:
                encoder.emitSymbolRelocationRef(inst->name);
                break;
            case ScbeMicroOp::SymbolRelocationAddress:
                encoder.emitSymbolRelocationAddress(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                break;
            case ScbeMicroOp::SymbolRelocationValue:
                encoder.emitSymbolRelocationValue(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                break;
            case ScbeMicroOp::SymbolGlobalString:
                encoder.emitSymbolGlobalString(inst->regA, inst->name);
                break;
            case ScbeMicroOp::SymbolRelocationPtr:
                encoder.emitSymbolRelocationPtr(inst->regA, inst->name);
                break;
            case ScbeMicroOp::Push:
                encoder.emitPush(inst->regA);
                break;
            case ScbeMicroOp::Pop:
                encoder.emitPop(inst->regA);
                break;
            case ScbeMicroOp::Nop:
                encoder.emitNop();
                break;
            case ScbeMicroOp::Ret:
                encoder.emitRet();
                break;
            case ScbeMicroOp::CallLocal:
                encoder.emitCallLocal(inst->name);
                break;
            case ScbeMicroOp::CallExtern:
                encoder.emitCallExtern(inst->name);
                break;
            case ScbeMicroOp::CallIndirect:
                encoder.emitCallIndirect(inst->regA);
                break;
            case ScbeMicroOp::JumpTable:
                encoder.emitJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->valueC);
                break;
            case ScbeMicroOp::JumpCC:
            {
                const auto cmpJump = encoder.emitJump(inst->jumpType, inst->opBitsA);
                inst->valueA       = reinterpret_cast<uint64_t>(cmpJump.addr);
                inst->valueB       = cmpJump.offset;
                inst->opBitsA      = cmpJump.opBits;
                break;
            }
            case ScbeMicroOp::PatchJump:
            {
                const auto jump = reinterpret_cast<const ScbeMicroInstruction*>(concat.firstBucket->data + inst->valueA);
                CPUJump    cpuJump;
                cpuJump.addr   = reinterpret_cast<void*>(jump->valueA);
                cpuJump.offset = jump->valueB;
                cpuJump.opBits = jump->opBitsA;
                encoder.emitPatchJump(cpuJump);
                break;
            }
            case ScbeMicroOp::JumpM:
                encoder.emitJumpM(inst->regA);
                break;
            case ScbeMicroOp::JumpCI:
                encoder.emitJumpCI(inst->jumpType, static_cast<uint32_t>(inst->valueA));
                break;
            case ScbeMicroOp::LoadCallerParam:
                encoder.emitLoadCallerParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA);
                break;
            case ScbeMicroOp::LoadCallerAddressParam:
                encoder.emitLoadCallerAddressParam(inst->regA, static_cast<uint32_t>(inst->valueA));
                break;
            case ScbeMicroOp::LoadCallerZeroExtendParam:
                encoder.emitLoadCallerZeroExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB);
                break;
            case ScbeMicroOp::StoreCallerParam:
                encoder.emitStoreCallerParam(static_cast<uint32_t>(inst->valueA), inst->regA, inst->opBitsA);
                break;
            case ScbeMicroOp::LoadRR:
                encoder.emitLoadRR(inst->regA, inst->regB, inst->opBitsA);
                break;
            case ScbeMicroOp::LoadR:
                encoder.emitLoadR(inst->regA, inst->opBitsA);
                break;
            case ScbeMicroOp::LoadRI64:
                encoder.emitLoadRI64(inst->regA, inst->valueA);
                break;
            case ScbeMicroOp::LoadRI:
                encoder.emitLoadRI(inst->regA, inst->valueA, inst->opBitsA);
                break;
            case ScbeMicroOp::LoadRM:
                encoder.emitLoadRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA);
                break;
            case ScbeMicroOp::LoadSignedExtendRM:
                encoder.emitLoadSignedExtendRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                break;
            case ScbeMicroOp::LoadSignedExtendRR:
                encoder.emitLoadSignedExtendRR(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                break;
            case ScbeMicroOp::LoadZeroExtendRM:
                encoder.emitLoadZeroExtendRM(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB);
                break;
            case ScbeMicroOp::LoadZeroExtendRR:
                encoder.emitLoadZeroExtendRR(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB);
                break;
            case ScbeMicroOp::LoadAddressM:
                encoder.emitLoadAddressM(inst->regA, inst->regB, inst->valueA);
                break;
            case ScbeMicroOp::LoadAddressAddMul:
                encoder.emitLoadAddressAddMul(inst->regA, inst->regB, inst->regC, inst->valueA, inst->opBitsA);
                break;
            case ScbeMicroOp::StoreMR:
                encoder.emitStoreMR(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                break;
            case ScbeMicroOp::StoreMI:
                encoder.emitStoreMI(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                break;
            case ScbeMicroOp::CmpRR:
                encoder.emitCmpRR(inst->regA, inst->regB, inst->opBitsA);
                break;
            case ScbeMicroOp::CmpRI:
                encoder.emitCmpRI(inst->regA, inst->valueA, inst->opBitsA);
                break;
            case ScbeMicroOp::CmpMR:
                encoder.emitCmpMR(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                break;
            case ScbeMicroOp::CmpMI:
                encoder.emitCmpMI(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                break;
            case ScbeMicroOp::SetCC:
                encoder.emitSetCC(inst->regA, inst->cpuCond);
                break;
            case ScbeMicroOp::ClearR:
                encoder.emitClearR(inst->regA, inst->opBitsA);
                break;
            case ScbeMicroOp::ClearM:
                encoder.emitClearM(inst->regA, inst->valueA, static_cast<uint32_t>(inst->valueB));
                break;
            case ScbeMicroOp::Copy:
                encoder.emitCopy(inst->regA, inst->regB, static_cast<uint32_t>(inst->valueA));
                break;
            case ScbeMicroOp::OpUnaryM:
                encoder.emitOpUnaryM(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA);
                break;
            case ScbeMicroOp::OpUnaryR:
                encoder.emitOpUnaryR(inst->regA, inst->cpuOp, inst->opBitsA);
                break;
            case ScbeMicroOp::OpBinaryRR:
                encoder.emitOpBinaryRR(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryMR:
                encoder.emitOpBinaryMR(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryRI:
                encoder.emitOpBinaryRI(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryMI:
                encoder.emitOpBinaryMI(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryRM:
                encoder.emitOpBinaryRM(inst->regA, inst->regB, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::MulAdd:
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

void ScbeMicro::process(ScbeCPU& encoder)
{
#ifdef SWAG_STATS
    g_Stats.numScbeInstructions += concat.totalCount();
#endif

    addInstruction(ScbeMicroOp::End);
    concat.makeLinear();
    
    ScbeOptimizer opt;
    opt.encoder = &encoder;
    opt.optimize(*this);
}
