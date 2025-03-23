#include "pch.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"

void Scbe::emitCmpRegReg(ScbeCpu& pp, CpuReg reg0, CpuReg reg1, OpBits opBits)
{
    pp.emitCmpRegReg(reg0, reg1, opBits);
}

void Scbe::emitCmpMemReg(ScbeCpu& pp, CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    pp.emitCmpMemReg(memReg, memOffset, reg, opBits);
}

void Scbe::emitCmpMemImm(ScbeCpu& pp, CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    pp.emitCmpMemImm(memReg, memOffset, value, opBits);
}

void Scbe::emitCmpRegImm(ScbeCpu& pp, CpuReg reg, uint64_t value, OpBits opBits)
{
    pp.emitCmpRegImm(reg, value, opBits);
}

void Scbe::emitLoadMemImm(ScbeCpu& pp, CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    pp.emitLoadMemImm(memReg, memOffset, value, opBits);
}

void Scbe::emitOpUnaryMem(ScbeCpu& pp, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits)
{
    pp.emitOpUnaryMem(memReg, memOffset, op, opBits);
}

void Scbe::emitOpUnaryReg(ScbeCpu& pp, CpuReg reg, CpuOp op, OpBits opBits)
{
    pp.emitOpUnaryReg(reg, op, opBits);
}

void Scbe::emitOpBinaryRegReg(ScbeCpu& pp, CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    pp.emitOpBinaryRegReg(regDst, regSrc, op, opBits, emitFlags);
}

void Scbe::emitOpBinaryRegMem(ScbeCpu& pp, CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    pp.emitOpBinaryRegMem(regDst, memReg, memOffset, op, opBits, emitFlags);
}

void Scbe::emitOpBinaryMemReg(ScbeCpu& pp, CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    pp.emitOpBinaryMemReg(memReg, memOffset, reg, op, opBits, emitFlags);
}

void Scbe::emitOpBinaryRegImm(ScbeCpu& pp, CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    pp.emitOpBinaryRegImm(reg, value, op, opBits, emitFlags);
}

void Scbe::emitOpBinaryMemImm(ScbeCpu& pp, CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    pp.emitOpBinaryMemImm(memReg, memOffset, value, op, opBits, emitFlags);
}
