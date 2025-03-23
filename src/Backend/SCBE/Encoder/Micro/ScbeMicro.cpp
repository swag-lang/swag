#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/ByteCode/ByteCode.h"
#include "ScbeMicroInstruction.h"
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
    ScbeCpu::init(buildParameters);
}

ScbeMicroInstruction* ScbeMicro::addInstruction(ScbeMicroOp op, CpuEmitFlags emitFlags)
{
    const auto inst = concat.addObj<ScbeMicroInstruction>();
    if (nextIsJumpDest)
        inst->flags.add(MIF_JUMP_DEST);
    inst->op        = op;
    inst->emitFlags = emitFlags;
    nextIsJumpDest  = false;
    return inst;
}

void ScbeMicro::emitEnter(uint32_t sizeStack)
{
    const auto inst = addInstruction(ScbeMicroOp::Enter, EMITF_Zero);
    inst->valueA    = sizeStack;
}

void ScbeMicro::emitLeave()
{
    addInstruction(ScbeMicroOp::Leave, EMITF_Zero);
}

void ScbeMicro::emitDebug(ByteCodeInstruction* ipAddr)
{
    const auto inst = addInstruction(ScbeMicroOp::Debug, EMITF_Zero);
    inst->valueA    = reinterpret_cast<uint64_t>(ipAddr);
}

void ScbeMicro::emitLabel(uint32_t instructionIndex)
{
    const auto inst = addInstruction(ScbeMicroOp::Label, EMITF_Zero);
    inst->valueA    = instructionIndex;
    nextIsJumpDest  = true;
}

void ScbeMicro::emitSymbolRelocationPtr(CpuReg reg, const Utf8& name)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocationPtr, EMITF_Zero);
    inst->regA      = reg;
    inst->name      = name;
}
void ScbeMicro::emitLoadCallerParam(CpuReg reg, uint32_t paramIdx, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallerParam, EMITF_Zero);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitLoadCallerZeroExtendParam(CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallerZeroExtendParam, EMITF_Zero);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::emitLoadCallerAddressParam(CpuReg reg, uint32_t paramIdx)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallerAddressParam, EMITF_Zero);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
}

void ScbeMicro::emitStoreCallerParam(uint32_t paramIdx, CpuReg reg, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::StoreCallerParam, EMITF_Zero);
    inst->valueA    = paramIdx;
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitJumpCondImm(CpuCondJump jumpType, uint32_t ipDest)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpCI, EMITF_Zero);
    inst->jumpType  = jumpType;
    inst->valueA    = ipDest;
}

void ScbeMicro::encodeSymbolRelocationRef(const Utf8& name, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocationRef, emitFlags);
    inst->name      = name;
}

void ScbeMicro::encodeSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocationAddress, emitFlags);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void ScbeMicro::encodeSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocationValue, emitFlags);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void ScbeMicro::encodeSymbolGlobalString(CpuReg reg, const Utf8& str, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolGlobalString, emitFlags);
    inst->regA      = reg;
    inst->name      = str;
}

void ScbeMicro::encodePush(CpuReg reg, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::Push, emitFlags);
    inst->regA      = reg;
}

void ScbeMicro::encodePop(CpuReg reg, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::Pop, emitFlags);
    inst->regA      = reg;
}

void ScbeMicro::encodeRet(CpuEmitFlags emitFlags)
{
    addInstruction(ScbeMicroOp::Ret, emitFlags);
}

void ScbeMicro::encodeLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeLoadRegImm64(CpuReg reg, uint64_t value, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRI64, emitFlags);
    inst->regA      = reg;
    inst->valueA    = value;
}

void ScbeMicro::encodeLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRI, emitFlags);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRM, emitFlags);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadSignedExtendRM, emitFlags);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::encodeLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadSignedExtendRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::encodeLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadZeroExtendRM, emitFlags);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::encodeLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadZeroExtendRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::encodeLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadAddressM, emitFlags);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueB    = memOffset;
}

void ScbeMicro::encodeLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadAddressAddMul, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc1;
    inst->regC      = regSrc2;
    inst->valueA    = mulValue;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadMR, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadMI, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::ClearR, emitFlags);
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::ClearM, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = count;
}

void ScbeMicro::encodeSetCond(CpuReg reg, CpuCondFlag setType, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SetCC, emitFlags);
    inst->regA      = reg;
    inst->cpuCond   = setType;
}

void ScbeMicro::encodeCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpRR, emitFlags);
    inst->regA      = reg0;
    inst->regB      = reg1;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpRI, emitFlags);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpMR, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpMI, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpUnaryM, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpUnaryR, emitFlags);
    inst->regA      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRM, emitFlags);
    inst->regA      = regDst;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryMR, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRI, emitFlags);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryMI, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encodeJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpTable, emitFlags);
    inst->regA      = table;
    inst->regB      = offset;
    inst->valueA    = currentIp;
    inst->valueB    = offsetTable;
    inst->valueC    = numEntries;
}

void ScbeMicro::encodeJumpReg(CpuReg reg, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpM, emitFlags);
    inst->regA      = reg;
}

CpuJump ScbeMicro::encodeJump(CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags)
{
    CpuJump cpuJump;
    cpuJump.offset  = concat.totalCount();
    const auto inst = addInstruction(ScbeMicroOp::JumpCC, emitFlags);
    inst->jumpType  = jumpType;
    inst->opBitsA   = opBits;
    return cpuJump;
}

void ScbeMicro::encodePatchJump(const CpuJump& jump, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::PatchJump, emitFlags);
    inst->valueA    = jump.offset;
    nextIsJumpDest  = true;
}

void ScbeMicro::encodePatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags)
{
    SWAG_ASSERT(false);
}

void ScbeMicro::encodeCopy(CpuReg regDst, CpuReg regSrc, uint32_t count, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::Copy, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->valueA    = count;
}

void ScbeMicro::encodeNop(CpuEmitFlags emitFlags)
{
    addInstruction(ScbeMicroOp::Nop, emitFlags);
}

void ScbeMicro::encodeCallLocal(const Utf8& symbolName, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CallLocal, emitFlags);
    inst->name      = symbolName;
}

void ScbeMicro::encodeCallExtern(const Utf8& symbolName, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CallExtern, emitFlags);
    inst->name      = symbolName;
}

void ScbeMicro::encodeCallReg(CpuReg reg, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CallIndirect, emitFlags);
    inst->regA      = reg;
}

void ScbeMicro::encodeOpMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::MulAdd, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regMul;
    inst->regC      = regAdd;
    inst->opBitsA   = opBits;
}

void ScbeMicro::encode(ScbeCpu& encoder) const
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
            case ScbeMicroOp::SymbolRelocationPtr:
                encoder.emitSymbolRelocationPtr(inst->regA, inst->name);
                break;
            case ScbeMicroOp::JumpCI:
                encoder.emitJumpCondImm(inst->jumpType, static_cast<uint32_t>(inst->valueA));
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

            case ScbeMicroOp::SymbolRelocationRef:
                encoder.encodeSymbolRelocationRef(inst->name, inst->emitFlags);
                break;
            case ScbeMicroOp::SymbolRelocationAddress:
                encoder.encodeSymbolRelocationAddress(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->emitFlags);
                break;
            case ScbeMicroOp::SymbolRelocationValue:
                encoder.encodeSymbolRelocationValue(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->emitFlags);
                break;
            case ScbeMicroOp::SymbolGlobalString:
                encoder.encodeSymbolGlobalString(inst->regA, inst->name, inst->emitFlags);
                break;
            case ScbeMicroOp::Push:
                encoder.encodePush(inst->regA, inst->emitFlags);
                break;
            case ScbeMicroOp::Pop:
                encoder.encodePop(inst->regA, inst->emitFlags);
                break;
            case ScbeMicroOp::Nop:
                encoder.encodeNop(inst->emitFlags);
                break;
            case ScbeMicroOp::Ret:
                encoder.encodeRet(inst->emitFlags);
                break;
            case ScbeMicroOp::CallLocal:
                encoder.encodeCallLocal(inst->name, inst->emitFlags);
                break;
            case ScbeMicroOp::CallExtern:
                encoder.encodeCallExtern(inst->name, inst->emitFlags);
                break;
            case ScbeMicroOp::CallIndirect:
                encoder.encodeCallReg(inst->regA, inst->emitFlags);
                break;
            case ScbeMicroOp::JumpTable:
                encoder.encodeJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->valueC, inst->emitFlags);
                break;
            case ScbeMicroOp::JumpCC:
            {
                const auto cmpJump = encoder.encodeJump(inst->jumpType, inst->opBitsA, inst->emitFlags);
                inst->valueA       = reinterpret_cast<uint64_t>(cmpJump.addr);
                inst->valueB       = cmpJump.offset;
                inst->opBitsA      = cmpJump.opBits;
                break;
            }
            case ScbeMicroOp::PatchJump:
            {
                const auto jump = reinterpret_cast<const ScbeMicroInstruction*>(concat.firstBucket->data + inst->valueA);
                CpuJump    cpuJump;
                cpuJump.addr   = reinterpret_cast<void*>(jump->valueA);
                cpuJump.offset = jump->valueB;
                cpuJump.opBits = jump->opBitsA;
                encoder.encodePatchJump(cpuJump, inst->emitFlags);
                break;
            }
            case ScbeMicroOp::JumpM:
                encoder.encodeJumpReg(inst->regA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadRR:
                encoder.encodeLoadRegReg(inst->regA, inst->regB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadRI64:
                encoder.encodeLoadRegImm64(inst->regA, inst->valueA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadRI:
                encoder.encodeLoadRegImm(inst->regA, inst->valueA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadRM:
                encoder.encodeLoadRegMem(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadSignedExtendRM:
                encoder.encodeLoadSignedExtendRegMem(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadSignedExtendRR:
                encoder.encodeLoadSignedExtendRegReg(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadZeroExtendRM:
                encoder.encodeLoadZeroExtendRegMem(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadZeroExtendRR:
                encoder.encodeLoadZeroExtendRegReg(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadAddressM:
                encoder.encodeLoadAddressMem(inst->regA, inst->regB, inst->valueB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadAddressAddMul:
                encoder.encodeLoadAddressAddMul(inst->regA, inst->regB, inst->regC, inst->valueA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadMR:
                encoder.encodeLoadMemReg(inst->regA, inst->valueA, inst->regB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadMI:
                encoder.encodeLoadMemImm(inst->regA, inst->valueA, inst->valueB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::CmpRR:
                encoder.encodeCmpRegReg(inst->regA, inst->regB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::CmpRI:
                encoder.encodeCmpRegImm(inst->regA, inst->valueA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::CmpMR:
                encoder.encodeCmpMemReg(inst->regA, inst->valueA, inst->regB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::CmpMI:
                encoder.encodeCmpMemImm(inst->regA, inst->valueA, inst->valueB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::SetCC:
                encoder.encodeSetCond(inst->regA, inst->cpuCond, inst->emitFlags);
                break;
            case ScbeMicroOp::ClearR:
                encoder.encodeClearReg(inst->regA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::ClearM:
                encoder.encodeClearMem(inst->regA, inst->valueA, static_cast<uint32_t>(inst->valueB), inst->emitFlags);
                break;
            case ScbeMicroOp::Copy:
                encoder.encodeCopy(inst->regA, inst->regB, static_cast<uint32_t>(inst->valueA), inst->emitFlags);
                break;
            case ScbeMicroOp::OpUnaryM:
                encoder.encodeOpUnaryMem(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpUnaryR:
                encoder.encodeOpUnaryReg(inst->regA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryRR:
                encoder.encodeOpBinaryRegReg(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryMR:
                encoder.encodeOpBinaryMemReg(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryRI:
                encoder.encodeOpBinaryRegImm(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryMI:
                encoder.encodeOpBinaryMemImm(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryRM:
                encoder.encodeOpBinaryRegMem(inst->regA, inst->regB, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::MulAdd:
                encoder.encodeOpMulAdd(inst->regA, inst->regB, inst->regC, inst->opBitsA, inst->emitFlags);
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

void ScbeMicro::pushRegisters() const
{
    Set<CpuReg> regs;

    auto inst = reinterpret_cast<ScbeMicroInstruction*>(concat.firstBucket->data);
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->hasWriteRegA())
            regs.insert(inst->regA);
        if (inst->hasWriteRegB())
            regs.insert(inst->regB);
        if (inst->hasWriteRegC())
            regs.insert(inst->regC);
        inst = inst + 1;
    }

    for (const auto r : regs)
    {
        if (cc->nonVolatileRegisters.contains(r))
            cpuFct->unwindRegs.push_back(r);
    }
}

void ScbeMicro::process(ScbeCpu& encoder)
{
#ifdef SWAG_STATS
    g_Stats.numScbeInstructions += concat.totalCount();
#endif

    addInstruction(ScbeMicroOp::End, EMITF_Zero);
    concat.makeLinear();

    ScbeOptimizer opt;
    opt.encoder = &encoder;
    opt.optimize(*this);

    pushRegisters();
}
