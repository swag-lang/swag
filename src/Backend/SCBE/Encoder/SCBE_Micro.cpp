#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_Micro.h"
#include "Semantic/Type/TypeManager.h"

void SCBE_Micro::init(const BuildParameters& buildParameters)
{
    concat.init();
    SCBE_CPU::init(buildParameters);
}

void SCBE_Micro::emitDebug(ByteCodeInstruction* ipAddr)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Debug;
    inst->valueA    = reinterpret_cast<uint64_t>(ipAddr);
}

void SCBE_Micro::emitLabel(uint32_t instructionIndex)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::AddLabel;
    inst->valueA    = instructionIndex;
}

void SCBE_Micro::emitLabels()
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::ComputeLabels;
}

void SCBE_Micro::emitSymbolRelocationRef(const Utf8& name)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::SymbolRelocationRef;
    inst->name      = name;
}

void SCBE_Micro::emitSymbolRelocationAddr(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::SymbolRelocationAddr;
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void SCBE_Micro::emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::SymbolRelocationValue;
    inst->regA      = reg;
    inst->valueA    = symbolIndex;
    inst->valueB    = offset;
}

void SCBE_Micro::emitSymbolGlobalString(CPUReg reg, const Utf8& str)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::SymbolGlobalString;
    inst->regA      = reg;
    inst->name      = str;
}

void SCBE_Micro::emitSymbolRelocationPtr(CPUReg reg, const Utf8& name)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::SymbolRelocationPtr;
    inst->regA      = reg;
    inst->name      = name;
}

void SCBE_Micro::emitPush(CPUReg reg)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Push;
    inst->regA      = reg;
}

void SCBE_Micro::emitPop(CPUReg reg)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Pop;
    inst->regA      = reg;
}

void SCBE_Micro::emitEnter(uint32_t sizeStack)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Enter;
    inst->valueA    = sizeStack;
}

void SCBE_Micro::emitLeave()
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Leave;
}

void SCBE_Micro::emitRet()
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Ret;
}

void SCBE_Micro::emitLoadParam(CPUReg reg, uint32_t paramIdx, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadParam;
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadExtendParam;
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
    inst->boolA     = isSigned;
}

void SCBE_Micro::emitLoadAddressParam(CPUReg reg, uint32_t paramIdx, bool forceStack)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadAddressParam;
    inst->regA      = reg;
    inst->valueA    = paramIdx;
    inst->boolA     = forceStack;
}

void SCBE_Micro::emitStoreParam(uint32_t paramIdx, CPUReg reg, OpBits opBits, bool forceStack)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::StoreParam;
    inst->valueA    = paramIdx;
    inst->regA      = reg;
    inst->opBitsA   = opBits;
    inst->boolA     = forceStack;
}

void SCBE_Micro::emitLoad(CPUReg regDst, CPUReg regSrc, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load0;
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoad(CPUReg regDstSrc, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load1;
    inst->regA      = regDstSrc;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, uint64_t value, bool isImmediate, CPUOp op, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load2;
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->boolA     = isImmediate;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoad(CPUReg reg, uint64_t value)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load3;
    inst->regA      = reg;
    inst->valueA    = value;
}

void SCBE_Micro::emitLoad(CPUReg reg, uint64_t value, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load4;
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load5;
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadExtend0;
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
    inst->boolA     = isSigned;
}

void SCBE_Micro::emitLoadExtend(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadExtend1;
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->opBitsA   = numBitsDst;
    inst->opBitsB   = numBitsSrc;
    inst->boolA     = isSigned;
}

void SCBE_Micro::emitLoadAddress(CPUReg reg, CPUReg memReg, uint64_t memOffset)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadAddress0;
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->valueA    = memOffset;
}

void SCBE_Micro::emitLoadAddress(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadAddress1;
    inst->regA      = regDst;
    inst->regB      = regSrc1;
    inst->regC      = regSrc2;
    inst->valueA    = mulValue;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitStore(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Store0;
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitStore(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Store1;
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitClear(CPUReg reg, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Clear0;
    inst->regA      = reg;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitClear(CPUReg memReg, uint64_t memOffset, uint32_t count)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Clear1;
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = count;
}

void SCBE_Micro::emitSet(CPUReg reg, CPUCondFlag setType)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Set;
    inst->regA      = reg;
    inst->cpuCond   = setType;
}

void SCBE_Micro::emitCmp(CPUReg reg0, CPUReg reg1, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Cmp0;
    inst->regA      = reg0;
    inst->regB      = reg1;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitCmp(CPUReg reg, uint64_t value, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Cmp1;
    inst->regA      = reg;
    inst->valueA    = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitCmp(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Cmp2;
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitCmp(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Cmp3;
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitOpUnary(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::OpUnary0;
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitOpUnary(CPUReg reg, CPUOp op, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::OpUnary1;
    inst->regA      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitOpBinary(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::OpBinary0;
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitOpBinary(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::OpBinary1;
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->regB      = reg;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitOpBinary(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::OpBinary2;
    inst->regA      = reg;
    inst->valueA    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitOpBinary(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::OpBinary3;
    inst->regA      = memReg;
    inst->valueA    = memOffset;
    inst->valueB    = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::JumpTable;
    inst->regA      = table;
    inst->regB      = offset;
    inst->valueA    = currentIp;
    inst->valueB    = offsetTable;
    inst->valueC    = numEntries;
}

void SCBE_Micro::emitJump(CPUReg reg)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Jump1;
    inst->regA      = reg;
}

void SCBE_Micro::emitJump(CPUCondJump jumpType, int32_t currentIp, int32_t jumpOffset)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Jump2;
    inst->jumpType  = jumpType;
    inst->valueA    = currentIp;
    inst->valueB    = jumpOffset;
}

CPUJump SCBE_Micro::emitJump(CPUCondJump jumpType, OpBits opBits)
{
    CPUJump cpuJump;
    cpuJump.offset = concat.totalCount();

    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Jump0;
    inst->jumpType  = jumpType;
    inst->opBitsA   = opBits;
    return cpuJump;
}

void SCBE_Micro::emitPatchJump(const CPUJump& jump)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::PatchJump0;
    inst->valueA    = jump.offset;
}

void SCBE_Micro::emitPatchJump(const CPUJump& jump, uint64_t offsetDestination)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::PatchJump1;
    inst->valueA    = jump.offset;
    inst->valueB    = offsetDestination;
}

void SCBE_Micro::emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count, uint32_t offset)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Copy;
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->valueA    = count;
    inst->valueB    = offset;
}

void SCBE_Micro::emitNop()
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Nop;
}

void SCBE_Micro::emitCallLocal(const Utf8& symbolName)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::CallLocal;
    inst->name      = symbolName;
}

void SCBE_Micro::emitCallExtern(const Utf8& symbolName)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::CallExtern;
    inst->name      = symbolName;
}

void SCBE_Micro::emitCallIndirect(CPUReg reg)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::CallIndirect;
    inst->regA      = reg;
}

void SCBE_Micro::emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::MulAdd;
    inst->regA      = regDst;
    inst->regB      = regMul;
    inst->regC      = regAdd;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::encode(SCBE_CPU& encoder)
{
    concat.makeLinear();

    const auto num  = concat.totalCount() / sizeof(SCBE_MicroInstruction);
    auto       inst = reinterpret_cast<SCBE_MicroInstruction*>(concat.firstBucket->data);
    for (uint32_t i = 0; i < num; i++, inst++)
    {
        switch (inst->op)
        {
            case SCBE_MicroOp::Debug:
                encoder.emitDebug(reinterpret_cast<ByteCodeInstruction*>(inst->valueA));
                break;
            case SCBE_MicroOp::AddLabel:
                encoder.emitLabel(static_cast<int32_t>(inst->valueA));
                break;
            case SCBE_MicroOp::ComputeLabels:
                encoder.emitLabels();
                break;
            case SCBE_MicroOp::SymbolRelocationRef:
                encoder.emitSymbolRelocationRef(inst->name);
                break;
            case SCBE_MicroOp::SymbolRelocationAddr:
                encoder.emitSymbolRelocationAddr(inst->regA, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
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
            case SCBE_MicroOp::Enter:
                encoder.emitEnter(static_cast<uint32_t>(inst->valueA));
                break;
            case SCBE_MicroOp::Leave:
                encoder.emitLeave();
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
                encoder.emitJumpTable(inst->regA, inst->regB, static_cast<int32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB), static_cast<uint32_t>(inst->valueC));
                break;
            case SCBE_MicroOp::Jump0:
            {
                const auto cmpJump = encoder.emitJump(inst->jumpType, inst->opBitsA);
                inst->valueA       = reinterpret_cast<uint64_t>(cmpJump.addr);
                inst->valueB       = cmpJump.offset;
                inst->opBitsA      = cmpJump.opBits;
                break;
            }
            case SCBE_MicroOp::PatchJump0:
            {
                const auto jump = reinterpret_cast<const SCBE_MicroInstruction*>(concat.firstBucket->data + inst->valueA);
                CPUJump    cpuJump;
                cpuJump.addr   = reinterpret_cast<void*>(jump->valueA);
                cpuJump.offset = jump->valueB;
                cpuJump.opBits = jump->opBitsA;
                encoder.emitPatchJump(cpuJump);
                break;
            }
            case SCBE_MicroOp::PatchJump1:
            {
                const auto jump = reinterpret_cast<SCBE_MicroInstruction*>(inst->valueA);
                CPUJump    cpuJump;
                cpuJump.addr   = reinterpret_cast<void*>(jump->valueA);
                cpuJump.offset = jump->valueB;
                cpuJump.opBits = jump->opBitsA;
                encoder.emitPatchJump(cpuJump, inst->valueB);
                break;
            }
            case SCBE_MicroOp::Jump1:
                encoder.emitJump(inst->regA);
                break;
            case SCBE_MicroOp::Jump2:
                encoder.emitJump(inst->jumpType, static_cast<int32_t>(inst->valueA), static_cast<int32_t>(inst->valueB));
                break;
            case SCBE_MicroOp::LoadParam:
                encoder.emitLoadParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA);
                break;
            case SCBE_MicroOp::LoadExtendParam:
                encoder.emitLoadExtendParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->opBitsA, inst->opBitsB, inst->boolA);
                break;
            case SCBE_MicroOp::LoadAddressParam:
                encoder.emitLoadAddressParam(inst->regA, static_cast<uint32_t>(inst->valueA), inst->boolA);
                break;
            case SCBE_MicroOp::StoreParam:
                encoder.emitStoreParam(static_cast<uint32_t>(inst->valueA), inst->regA, inst->opBitsA, inst->boolA);
                break;
            case SCBE_MicroOp::Load0:
                encoder.emitLoad(inst->regA, inst->regB, inst->opBitsA);
                break;
            case SCBE_MicroOp::Load1:
                encoder.emitLoad(inst->regA, inst->opBitsA);
                break;
            case SCBE_MicroOp::Load2:
                encoder.emitLoad(inst->regA, inst->regB, inst->valueA, inst->valueB, inst->boolA, inst->cpuOp, inst->opBitsA);
                break;
            case SCBE_MicroOp::Load3:
                encoder.emitLoad(inst->regA, inst->valueA);
                break;
            case SCBE_MicroOp::Load4:
                encoder.emitLoad(inst->regA, inst->valueA, inst->opBitsA);
                break;
            case SCBE_MicroOp::Load5:
                encoder.emitLoad(inst->regA, inst->regB, inst->valueA, inst->opBitsA);
                break;
            case SCBE_MicroOp::LoadExtend0:
                encoder.emitLoadExtend(inst->regA, inst->regB, inst->valueA, inst->opBitsA, inst->opBitsB, inst->boolA);
                break;
            case SCBE_MicroOp::LoadExtend1:
                encoder.emitLoadExtend(inst->regA, inst->regB, inst->opBitsA, inst->opBitsB, inst->boolA);
                break;
            case SCBE_MicroOp::LoadAddress0:
                encoder.emitLoadAddress(inst->regA, inst->regB, inst->valueA);
                break;
            case SCBE_MicroOp::LoadAddress1:
                encoder.emitLoadAddress(inst->regA, inst->regB, inst->regC, inst->valueA, inst->opBitsA);
                break;
            case SCBE_MicroOp::Store0:
                encoder.emitStore(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                break;
            case SCBE_MicroOp::Store1:
                encoder.emitStore(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                break;
            case SCBE_MicroOp::Cmp0:
                encoder.emitCmp(inst->regA, inst->regB, inst->opBitsA);
                break;
            case SCBE_MicroOp::Cmp1:
                encoder.emitCmp(inst->regA, inst->valueA, inst->opBitsA);
                break;
            case SCBE_MicroOp::Cmp2:
                encoder.emitCmp(inst->regA, inst->valueA, inst->regB, inst->opBitsA);
                break;
            case SCBE_MicroOp::Cmp3:
                encoder.emitCmp(inst->regA, inst->valueA, inst->valueB, inst->opBitsA);
                break;
            case SCBE_MicroOp::Set:
                encoder.emitSet(inst->regA, inst->cpuCond);
                break;
            case SCBE_MicroOp::Clear0:
                encoder.emitClear(inst->regA, inst->opBitsA);
                break;
            case SCBE_MicroOp::Clear1:
                encoder.emitClear(inst->regA, inst->valueA, static_cast<uint32_t>(inst->valueB));
                break;
            case SCBE_MicroOp::Copy:
                encoder.emitCopy(inst->regA, inst->regB, static_cast<uint32_t>(inst->valueA), static_cast<uint32_t>(inst->valueB));
                break;
            case SCBE_MicroOp::OpUnary0:
                encoder.emitOpUnary(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA);
                break;
            case SCBE_MicroOp::OpUnary1:
                encoder.emitOpUnary(inst->regA, inst->cpuOp, inst->opBitsA);
                break;
            case SCBE_MicroOp::OpBinary0:
                encoder.emitOpBinary(inst->regA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::OpBinary1:
                encoder.emitOpBinary(inst->regA, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::OpBinary2:
                encoder.emitOpBinary(inst->regA, inst->valueA, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::OpBinary3:
                encoder.emitOpBinary(inst->regA, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, inst->emitFlags);
                break;
            case SCBE_MicroOp::MulAdd:
                encoder.emitMulAdd(inst->regA, inst->regB, inst->regC, inst->opBitsA);
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }
    }
}
