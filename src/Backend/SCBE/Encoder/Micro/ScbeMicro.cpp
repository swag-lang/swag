#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizer.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
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

ScbeMicroInstruction* ScbeMicro::getFirstInstruction() const
{
    return reinterpret_cast<ScbeMicroInstruction*>(concat.firstBucket->data);
}

ScbeMicroInstruction* ScbeMicro::addInstruction(ScbeMicroOp op, CpuEmitFlags emitFlags)
{
    const auto inst = concat.addObj<ScbeMicroInstruction>();
    inst->op        = op;
    inst->emitFlags = emitFlags;

#ifdef SWAG_STATS
    if (op != ScbeMicroOp::Enter &&
        op != ScbeMicroOp::Leave &&
        op != ScbeMicroOp::Label &&
        op != ScbeMicroOp::PatchJump &&
        op != ScbeMicroOp::Debug)
    {
        g_Stats.numScbeInstructions += 1;
    }
#endif

    if (op != ScbeMicroOp::Label &&
        op != ScbeMicroOp::PatchJump &&
        op != ScbeMicroOp::Debug)
    {
        if (nextIsJumpDest)
        {
            inst->flags.add(MIF_JUMP_DEST);
            nextIsJumpDest = false;
        }
    }

    return inst;
}

void ScbeMicro::emitEnter(uint32_t sizeStack)
{
    const auto inst = addInstruction(ScbeMicroOp::Enter, EMIT_Zero);
    inst->valueA    = sizeStack;
}

void ScbeMicro::emitLeave()
{
    addInstruction(ScbeMicroOp::Leave, EMIT_Zero);
}

void ScbeMicro::emitDebug(ByteCodeInstruction* ipAddr)
{
    const auto inst = addInstruction(ScbeMicroOp::Debug, EMIT_Zero);
    inst->valueA    = reinterpret_cast<uint64_t>(ipAddr);
}

void ScbeMicro::emitLabel(uint32_t instructionIndex)
{
    const auto inst          = addInstruction(ScbeMicroOp::Label, EMIT_Zero);
    inst->valueA             = instructionIndex;
    nextIsJumpDest           = true;
    labels[instructionIndex] = concat.totalCount() / sizeof(ScbeMicroInstruction);
}

void ScbeMicro::emitSymbolRelocationPtr(CpuReg reg, const Utf8& name)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocPtr, EMIT_Zero);
    inst->regA      = reg;
    inst->name      = name;
}
void ScbeMicro::emitLoadCallerParam(CpuReg reg, uint32_t paramIdx, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallParam, EMIT_Zero);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitLoadCallerZeroExtendParam(CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallZeroExtParam, EMIT_Zero);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
}

void ScbeMicro::emitLoadCallerAddressParam(CpuReg reg, uint32_t paramIdx)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCallAddrParam, EMIT_Zero);
    inst->regA      = reg;
    inst->valueA    = paramIdx;
}

void ScbeMicro::emitStoreCallerParam(uint32_t paramIdx, CpuReg reg, OpBits opBits)
{
    const auto inst = addInstruction(ScbeMicroOp::StoreCallParam, EMIT_Zero);
    inst->valueA    = paramIdx;
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void ScbeMicro::emitJumpCondImm(CpuCondJump jumpType, uint32_t ipDest)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpCI, EMIT_Zero);
    inst->jumpType  = jumpType;
    inst->valueA    = ipDest;
}

CpuEncodeResult ScbeMicro::encodeSymbolRelocationRef(const Utf8& name, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocRef, emitFlags);
    inst->name      = name;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocAddr, emitFlags);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolRelocValue, emitFlags);
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeSymbolGlobalString(CpuReg reg, const Utf8& str, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SymbolGlobalString, emitFlags);
    inst->regA      = reg;
    inst->name      = str;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodePush(CpuReg reg, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::Push, emitFlags);
    inst->regA      = reg;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodePop(CpuReg reg, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::Pop, emitFlags);
    inst->regA      = reg;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeRet(CpuEmitFlags emitFlags)
{
    addInstruction(ScbeMicroOp::Ret, emitFlags);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadRegImm64(CpuReg reg, uint64_t value, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRI64, emitFlags);
    inst->regA      = reg;
    inst->valueA    = value;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRI, emitFlags);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadRM, emitFlags);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadSignedExtRM, emitFlags);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadSignedExtRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadZeroExtRM, emitFlags);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadZeroExtRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadAddr, emitFlags);
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadAddMulCstRegMem(CpuReg regDst, OpBits opBitsDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, uint64_t addValue, CpuOp op, OpBits opBitsSrc, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadAddMulCstRM, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc1;
    inst->regC      = regSrc2;
    inst->valueA    = mulValue;
    inst->valueB    = addValue;
    inst->cpuOp     = op;
    inst->opBitsA   = opBitsDst;
    inst->opBitsB   = opBitsSrc;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadMR, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadMI, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::ClearR, emitFlags);
    inst->regA      = reg;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeSetCondReg(CpuReg reg, CpuCond setType, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::SetCondR, emitFlags);
    inst->regA      = reg;
    inst->cpuCond   = setType;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeLoadCondRegReg(CpuReg regDst, CpuReg regSrc, CpuCond cpuCond, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::LoadCondRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->cpuCond   = cpuCond;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpRR, emitFlags);
    inst->regA      = reg0;
    inst->regB      = reg1;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpRI, emitFlags);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpMR, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CmpMI, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpUnaryM, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpUnaryR, emitFlags);
    inst->regA      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRR, emitFlags);
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRM, emitFlags);
    inst->regA      = regDst;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryMR, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryRI, emitFlags);
    inst->regA      = reg;
    inst->valueA    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpBinaryMI, emitFlags);
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpTable, emitFlags);
    inst->regA      = table;
    inst->regB      = offset;
    inst->valueA    = currentIp;
    inst->valueB    = offsetTable;
    inst->valueC    = numEntries;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeJumpReg(CpuReg reg, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::JumpM, emitFlags);
    inst->regA      = reg;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags)
{
    jump.offsetStart = concat.totalCount();
    const auto inst  = addInstruction(ScbeMicroOp::JumpCond, emitFlags);
    inst->jumpType   = jumpType;
    inst->opBitsA    = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodePatchJump(const CpuJump& jump, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::PatchJump, emitFlags);
    inst->valueA    = jump.offsetStart;
    nextIsJumpDest  = true;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodePatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags)
{
    SWAG_ASSERT(false);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeNop(CpuEmitFlags emitFlags)
{
    addInstruction(ScbeMicroOp::Nop, emitFlags);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeCallLocal(const Utf8& symbolName, const CallConv* callConv, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CallLocal, emitFlags);
    inst->name      = symbolName;
    inst->cc        = callConv;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeCallExtern(const Utf8& symbolName, const CallConv* callConv, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CallExtern, emitFlags);
    inst->name      = symbolName;
    inst->cc        = callConv;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeCallReg(CpuReg reg, const CallConv* callConv, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::CallIndirect, emitFlags);
    inst->regA      = reg;
    inst->cc        = callConv;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeMicro::encodeOpTernaryRegRegReg(CpuReg reg0, CpuReg reg1, CpuReg reg2, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto inst = addInstruction(ScbeMicroOp::OpTernaryRRR, emitFlags);
    inst->regA      = reg0;
    inst->regB      = reg1;
    inst->regC      = reg2;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    return CpuEncodeResult::Zero;
}

void ScbeMicro::encode(ScbeCpu& encoder) const
{
    auto inst = getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
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
            case ScbeMicroOp::SymbolRelocPtr:
                encoder.emitSymbolRelocationPtr(inst->regA, inst->name);
                break;
            case ScbeMicroOp::JumpCI:
                encoder.emitJumpCondImm(inst->jumpType, static_cast<uint32_t>(inst->valueA));
                break;
            case ScbeMicroOp::LoadCallParam:
                encoder.emitLoadCallerParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA);
                break;
            case ScbeMicroOp::LoadCallAddrParam:
                encoder.emitLoadCallerAddressParam(inst->regA, static_cast<uint32_t>(inst->valueA));
                break;
            case ScbeMicroOp::LoadCallZeroExtParam:
                encoder.emitLoadCallerZeroExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB);
                break;
            case ScbeMicroOp::StoreCallParam:
                encoder.emitStoreCallerParam(static_cast<uint32_t>(inst->valueA), inst->regA, inst->opBitsA);
                break;

            case ScbeMicroOp::SymbolRelocRef:
                encoder.emitSymbolRelocationRef(inst->name, inst->emitFlags);
                break;
            case ScbeMicroOp::SymbolRelocAddr:
                encoder.emitSymbolRelocationAddress(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->emitFlags);
                break;
            case ScbeMicroOp::SymbolRelocValue:
                encoder.emitSymbolRelocationValue(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->emitFlags);
                break;
            case ScbeMicroOp::SymbolGlobalString:
                encoder.emitSymbolGlobalString(inst->regA, inst->name, inst->emitFlags);
                break;
            case ScbeMicroOp::Push:
                encoder.emitPush(inst->regA, inst->emitFlags);
                break;
            case ScbeMicroOp::Pop:
                encoder.emitPop(inst->regA, inst->emitFlags);
                break;
            case ScbeMicroOp::Nop:
                encoder.emitNop(inst->emitFlags);
                break;
            case ScbeMicroOp::Ret:
                encoder.emitRet(inst->emitFlags);
                break;
            case ScbeMicroOp::CallLocal:
                encoder.emitCallLocal(inst->name, inst->cc, inst->emitFlags);
                break;
            case ScbeMicroOp::CallExtern:
                encoder.emitCallExtern(inst->name, inst->cc, inst->emitFlags);
                break;
            case ScbeMicroOp::CallIndirect:
                encoder.emitCallReg(inst->regA, inst->cc, inst->emitFlags);
                break;
            case ScbeMicroOp::JumpTable:
                encoder.emitJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), inst->valueC, inst->emitFlags);
                break;
            case ScbeMicroOp::JumpCond:
            {
                CpuJump jump;
                encoder.emitJump(jump, inst->jumpType, inst->opBitsA, inst->emitFlags);
                inst->valueA  = reinterpret_cast<uint64_t>(jump.patchOffsetAddr);
                inst->valueB  = jump.offsetStart;
                inst->opBitsA = jump.opBits;
                break;
            }
            case ScbeMicroOp::PatchJump:
            {
                const auto jump = reinterpret_cast<const ScbeMicroInstruction*>(concat.firstBucket->data + inst->valueA);
                CpuJump    cpuJump;
                cpuJump.patchOffsetAddr = reinterpret_cast<void*>(jump->valueA);
                cpuJump.offsetStart     = jump->valueB;
                cpuJump.opBits          = jump->opBitsA;
                encoder.emitPatchJump(cpuJump, inst->emitFlags);
                break;
            }
            case ScbeMicroOp::JumpM:
                encoder.emitJumpReg(inst->regA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadRR:
                encoder.emitLoadRegReg(inst->regA, inst->regB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadRI64:
                encoder.emitLoadRegImm64(inst->regA, inst->valueA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadRI:
                encoder.emitLoadRegImm(inst->regA, inst->valueA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadRM:
                encoder.emitLoadRegMem(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadSignedExtRM:
                encoder.emitLoadSignedExtendRegMem(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadSignedExtRR:
                encoder.emitLoadSignedExtendRegReg(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadZeroExtRM:
                encoder.emitLoadZeroExtendRegMem(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadZeroExtRR:
                encoder.emitLoadZeroExtendRegReg(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadAddr:
                encoder.emitLoadAddressMem(inst->regA, inst->regB, inst->valueA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadAddMulCstRM:
                encoder.emitLoadAddMulCstRegMem(inst->regA, inst->opBitsA, inst->regB, inst->regC, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsB, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadMR:
                encoder.emitLoadMemReg(inst->regA, inst->valueA, inst->regB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadMI:
                encoder.emitLoadMemImm(inst->regA, inst->valueA, inst->valueB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::CmpRR:
                encoder.emitCmpRegReg(inst->regA, inst->regB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::CmpRI:
                encoder.emitCmpRegImm(inst->regA, inst->valueA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::CmpMR:
                encoder.emitCmpMemReg(inst->regA, inst->valueA, inst->regB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::CmpMI:
                encoder.emitCmpMemImm(inst->regA, inst->valueA, inst->valueB, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::SetCondR:
                encoder.emitSetCondReg(inst->regA, inst->cpuCond, inst->emitFlags);
                break;
            case ScbeMicroOp::LoadCondRR:
                encoder.emitLoadCondRegReg(inst->regA, inst->regB, inst->cpuCond, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::ClearR:
                encoder.emitClearReg(inst->regA, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpUnaryM:
                encoder.emitOpUnaryMem(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpUnaryR:
                encoder.emitOpUnaryReg(inst->regA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryRR:
                encoder.emitOpBinaryRegReg(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryMR:
                encoder.emitOpBinaryMemReg(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryRI:
                encoder.emitOpBinaryRegImm(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryMI:
                encoder.emitOpBinaryMemImm(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpBinaryRM:
                encoder.emitOpBinaryRegMem(inst->regA, inst->regB, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case ScbeMicroOp::OpTernaryRRR:
                encoder.emitOpTernaryRegRegReg(inst->regA, inst->regB, inst->regC, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }

        inst = inst + 1;
    }

    encoder.emitLabels();
}

ScbeMicroInstruction* ScbeMicro::getNextInstruction(ScbeMicroInstruction* inst)
{
    SWAG_ASSERT(inst->op != ScbeMicroOp::End);
    inst++;
    while (inst->op == ScbeMicroOp::Nop ||
           inst->op == ScbeMicroOp::Label ||
           inst->op == ScbeMicroOp::Debug ||
           inst->op == ScbeMicroOp::PatchJump ||
           inst->op == ScbeMicroOp::Ignore)
        inst++;
    return inst;
}

ScbeMicroInstruction* ScbeMicro::getNextFlagSensitive(ScbeMicroInstruction* inst) const
{
    inst++;
    while (true)
    {
        if (inst->op == ScbeMicroOp::End || inst->isJump() || inst->isCall())
            break;
        if (cpu->doesReadFlags(inst) || cpu->doesWriteFlags(inst))
            break;
        inst++;
    }

    return inst;
}

void ScbeMicro::postProcess() const
{
    cpuFct->isEmpty = true;
    auto inst       = getFirstInstruction();
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->hasRegA())
            cpuFct->usedRegs.add(inst->regA);
        if (inst->hasRegB())
            cpuFct->usedRegs.add(inst->regB);
        if (inst->hasRegC())
            cpuFct->usedRegs.add(inst->regC);

        if (inst->op != ScbeMicroOp::Enter && inst->op != ScbeMicroOp::Leave && inst->op != ScbeMicroOp::Ret)
            cpuFct->isEmpty = false;

        inst = getNextInstruction(inst);
    }

    for (const auto r : cpuFct->usedRegs)
    {
        if (cc->nonVolatileRegistersIntegerSet.contains(r))
            cpuFct->unwindRegs.push_back(r);
    }
}

void ScbeMicro::solveLabels()
{
    const auto first = getFirstInstruction();
    auto       inst  = first;
    while (inst->op != ScbeMicroOp::End)
    {
        if (inst->op == ScbeMicroOp::JumpCI)
        {
            const auto it = labels.find(static_cast<uint32_t>(inst->valueA));
            SWAG_ASSERT(it != labels.end());
            inst->valueB = it->second;
        }
        else if (inst->op == ScbeMicroOp::PatchJump)
        {
            const auto jump = reinterpret_cast<ScbeMicroInstruction*>(concat.firstBucket->data + inst->valueA);
            const auto next = getNextInstruction(inst);
            jump->valueB    = static_cast<uint64_t>(next - first);
        }

        inst++;
    }
}

void ScbeMicro::process(ScbeCpu& encoder)
{
    addInstruction(ScbeMicroOp::End, EMIT_Zero);
    concat.makeLinear();

    solveLabels();

    ScbeOptimizer opt;
    opt.encoder = &encoder;
    opt.optimize(*this);

    postProcess();

    if (cpuFct->bc->node && cpuFct->bc->node->hasAttribute(ATTRIBUTE_PRINT_ASM))
        print();
}
