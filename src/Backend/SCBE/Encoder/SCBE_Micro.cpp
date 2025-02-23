#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_Micro.h"
#include "Semantic/Type/TypeManager.h"

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
    inst->value64A  = symbolIndex;
    inst->value64B  = offset;
}

void SCBE_Micro::emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::SymbolRelocationValue;
    inst->regA      = reg;
    inst->value64A  = symbolIndex;
    inst->value64B  = offset;
}

void SCBE_Micro::emitSymbolGlobalString(CPUReg reg, const Utf8& str)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::SymbolGlobalString;
    inst->regA      = reg;
    inst->name      = str;
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

void SCBE_Micro::emitRet()
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Ret;
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
    inst->value64A  = memOffset;
    inst->value64B  = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->boolA     = isImmediate;
}

void SCBE_Micro::emitLoad64(CPUReg reg, uint64_t value)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load3;
    inst->regA      = reg;
    inst->value64B  = value;
}

void SCBE_Micro::emitLoad(CPUReg reg, uint64_t value, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load4;
    inst->regA      = reg;
    inst->value64A  = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Load4;
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->value64A  = memOffset;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitLoadExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadExtend0;
    inst->regA      = reg;
    inst->regB      = memReg;
    inst->value64A  = memOffset;
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
    inst->value64A  = memOffset;
}

void SCBE_Micro::emitLoadAddress(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::LoadAddress1;
    inst->regA      = regDst;
    inst->regB      = regSrc1;
    inst->regC      = regSrc2;
    inst->value64A  = mulValue;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitStore(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Store0;
    inst->regA      = memReg;
    inst->regB      = reg;
    inst->value64A  = memOffset;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitStore(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Store1;
    inst->regA      = memReg;
    inst->value64A  = memOffset;
    inst->value64B  = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitClear(CPUReg reg, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Clear0;
    inst->regA      = reg;
}

void SCBE_Micro::emitClear(CPUReg memReg, uint64_t memOffset, uint32_t count)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Clear1;
    inst->regA      = memReg;
    inst->value64A  = memOffset;
    inst->value64B  = count;
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
    inst->value64A  = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitCmp(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Cmp2;
    inst->regA      = memReg;
    inst->value64A  = memOffset;
    inst->regB      = reg;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitCmp(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Cmp3;
    inst->regA      = memReg;
    inst->value64A  = memOffset;
    inst->value64B  = value;
    inst->opBitsA   = opBits;
}

void SCBE_Micro::emitOpUnary(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::OpUnary0;
    inst->regA      = memReg;
    inst->value64A  = memOffset;
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
    inst->value64A  = memOffset;
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
    inst->value64A  = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitOpBinary(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::OpBinary3;
    inst->regA      = memReg;
    inst->value64A  = memOffset;
    inst->value64B  = value;
    inst->cpuOp     = op;
    inst->opBitsA   = opBits;
    inst->emitFlags = emitFlags;
}

void SCBE_Micro::emitJumpTable(CPUReg table, CPUReg offset)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::JumpTable;
    inst->regA      = table;
    inst->regB      = offset;
}

CPUJump SCBE_Micro::emitJump(CPUCondJump jumpType, OpBits opBits)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Jump0;
    inst->jumpType  = jumpType;
    inst->opBitsA   = opBits;
    return CPUJump{.addr = inst};
}

void SCBE_Micro::emitJump(CPUReg reg)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Jump1;
    inst->regA      = reg;
}

void SCBE_Micro::patchJump(const CPUJump& jump, uint64_t offsetDestination)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::PatchJump;
    inst->value64A  = offsetDestination;
    inst->value64B  = reinterpret_cast<uint64_t>(jump.addr);
}

void SCBE_Micro::emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count, uint32_t offset)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Copy;
    inst->regA      = regDst;
    inst->regB      = regSrc;
    inst->value64A  = count;
    inst->value64B  = offset;
}

void SCBE_Micro::emitCallExtern(const Utf8& symbolName)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::CallExtern;
    inst->name      = symbolName;
}

void SCBE_Micro::emitCallLocal(const Utf8& symbolName)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::CallLocal;
    inst->name      = symbolName;
}

void SCBE_Micro::emitCallIndirect(CPUReg reg)
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Nop;
    inst->regA      = reg;
}

void SCBE_Micro::emitNop()
{
    const auto inst = concat.addObj<SCBE_MicroInstruction>();
    inst->op        = SCBE_MicroOp::Nop;
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
