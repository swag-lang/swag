#include "pch.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Backend/SCBE/Obj/ScbeCoff.h"
#include "Core/Math.h"
#include "Main/CommandLine.h"
#include "Micro/ScbeMicro.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"

void ScbeCpu::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
    optLevel = buildParameters.buildCfg ? buildParameters.buildCfg->backendOptimize : BuildCfgBackendOptim::O0;
    cpu      = this;
}

namespace
{
    CpuSymbol* getSymbol(ScbeCpu& pp, const Utf8& name)
    {
        const auto it = pp.mapSymbols.find(name);
        if (it != pp.mapSymbols.end())
            return &pp.allSymbols[it->second];
        return nullptr;
    }
}

CpuSymbol* ScbeCpu::getOrAddSymbol(const Utf8& name, CpuSymbolKind kind, uint32_t value, uint16_t sectionIdx)
{
    const auto it = getSymbol(*this, name);
    if (it)
    {
        if (it->kind == kind)
            return it;
        if (kind == CpuSymbolKind::Extern)
            return it;
        if (kind == CpuSymbolKind::Function && it->kind == CpuSymbolKind::Extern)
        {
            it->kind  = kind;
            it->value = value;
            return it;
        }

        SWAG_ASSERT(false);
    }

    CpuSymbol sym;
    sym.name       = name;
    sym.kind       = kind;
    sym.value      = value;
    sym.sectionIdx = sectionIdx;
    SWAG_ASSERT(allSymbols.size() < UINT32_MAX);
    sym.index = allSymbols.size();
    allSymbols.emplace_back(std::move(sym));
    mapSymbols[name] = allSymbols.size() - 1;
    return &allSymbols.back();
}

CpuSymbol* ScbeCpu::getOrAddString(const Utf8& str)
{
    const auto it = globalStrings.find(str);
    if (it != globalStrings.end())
        return &allSymbols[it->second];

    const Utf8 symName = form("__str%u", static_cast<uint32_t>(globalStrings.size()));
    const auto sym     = getOrAddSymbol(symName, CpuSymbolKind::Constant);
    globalStrings[str] = sym->index;
    sym->value         = constantSegment.addStringNoLock(str);

    return sym;
}

CpuSymbol* ScbeCpu::getOrAddConstant(uint64_t value, OpBits opBits)
{
    const auto it = globalConstants.find(value);
    if (it != globalConstants.end())
        return &allSymbols[it->second];

    const Utf8 symName     = form("__cst%u", static_cast<uint32_t>(globalConstants.size()));
    const auto sym         = getOrAddSymbol(symName, CpuSymbolKind::Constant);
    globalConstants[value] = sym->index;

    ComputedValue cv;
    cv.reg.u64 = value;

    switch (opBits)
    {
        case OpBits::B32:
            sym->value = constantSegment.addComputedValueNoLock(g_TypeMgr->typeInfoU32, cv, nullptr);
            break;
        case OpBits::B64:
            sym->value = constantSegment.addComputedValueNoLock(g_TypeMgr->typeInfoU64, cv, nullptr);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    return sym;
}

void ScbeCpu::addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type)
{
    CpuRelocation reloc;
    reloc.virtualAddress = virtualAddr;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = type;
    relocTableTextSection.table.push_back(reloc);
}

void ScbeCpu::emitLabel(uint32_t instructionIndex)
{
    const auto it = cpuFct->labels.find(instructionIndex);
    if (it == cpuFct->labels.end())
        cpuFct->labels[instructionIndex] = static_cast<int32_t>(concat.totalCount());
}

void ScbeCpu::addFunction(const Utf8& funcName, const CallConv* ccFunc, ByteCode* bc)
{
    cpuFct           = Allocator::alloc<CpuFunction>();
    cpuFct->funcName = funcName;
    cpuFct->pp       = Allocator::alloc<ScbeMicro>();
    cpuFct->cc       = ccFunc;
    cpuFct->bc       = bc;

    if (bc)
    {
        if (bc->node)
            cpuFct->node = castAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);
        cpuFct->typeFunc = bc->getCallType();

        // Calling convention, space for at least 'MAX_CALL_CONV_REGISTERS' parameters when calling a function.
        // (should ideally be reserved only if we have a call)
        //
        // Because of variadic parameters in fct calls, we need to add some extra room, in case we have to flatten them.
        // We want to be sure to have the room to flatten the array of variadic (make all params contiguous). That's
        // why we multiply by 2.
        //
        // Why 2 ?? Magic number ??
        cpuFct->sizeStackCallParams = 2 * static_cast<uint32_t>(std::max(CallConv::MAX_CALL_CONV_REGISTERS, (bc->maxCallParams + 1)) * sizeof(void*));
        cpuFct->sizeStackCallParams = Math::align(cpuFct->sizeStackCallParams, ccFunc->stackAlign);
    }

    functions.push_back(cpuFct);
}

void ScbeCpu::startFunction()
{
    concat.align(16);
    cpuFct->symbolIndex  = getOrAddSymbol(cpuFct->funcName, CpuSymbolKind::Function, concat.totalCount() - textSectionOffset)->index;
    cpuFct->startAddress = concat.totalCount();
}

void ScbeCpu::endFunction() const
{
    cpuFct->endAddress = concat.totalCount();

    const auto objFileType = Scbe::getObjType(g_CommandLine.target);
    switch (objFileType)
    {
        case BackendObjType::Coff:
            ScbeCoff::computeUnwind(cpuFct->unwindRegs, cpuFct->unwindOffsetRegs, cpuFct->frameSize, cpuFct->sizeProlog, cpuFct->unwind);
            break;
        default:
            Report::internalError(module, "SCBE::computeUnwind, unsupported output");
            break;
    }
}

RegisterSet ScbeCpu::getReadRegisters(ScbeMicroInstruction* inst)
{
    RegisterSet result;
    if (inst->isCall())
    {
        result.append(cpuFct->cc->paramsRegistersIntegerSet);
        result.append(cpuFct->cc->paramsRegistersFloatSet);
    }

    if (inst->hasReadRegA())
        result.add(inst->regA);
    if (inst->hasReadRegB())
        result.add(inst->regB);
    if (inst->hasReadRegC())
        result.add(inst->regC);

    return result;
}

RegisterSet ScbeCpu::getWriteRegisters(ScbeMicroInstruction* inst)
{
    RegisterSet result;
    if (inst->isCall())
    {
        result.append(cpuFct->cc->volatileRegistersIntegerSet);
        result.append(cpuFct->cc->volatileRegistersFloatSet);
        result.add(cpuFct->cc->returnByRegisterInteger);
        result.add(cpuFct->cc->returnByRegisterFloat);
    }
    else
    {
        if (inst->hasWriteRegA())
            result.add(inst->regA);
        if (inst->hasWriteRegB())
            result.add(inst->regB);
        if (inst->hasWriteRegC())
            result.add(inst->regC);
    }

    return result;
}

RegisterSet ScbeCpu::getReadWriteRegisters(ScbeMicroInstruction* inst)
{
    auto result = getReadRegisters(inst);
    result.append(getWriteRegisters(inst));
    return result;
}

bool ScbeCpu::acceptsRegA(const ScbeMicroInstruction* inst, CpuReg reg)
{
    auto result = CpuEncodeResult::Zero;
    switch (inst->op)
    {
        case ScbeMicroOp::LoadRI:
            result = encodeLoadRegImm(reg, inst->valueA, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadMI:
            result = encodeLoadMemImm(reg, inst->valueA, inst->valueB, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadMR:
            result = encodeLoadMemReg(reg, inst->valueA, inst->regB, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadRM:
            result = encodeLoadRegMem(reg, inst->regB, inst->valueA, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadRR:
            result = encodeLoadRegReg(reg, inst->regB, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::CmpRI:
            result = encodeCmpRegImm(reg, inst->valueA, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::CmpMI:
            result = encodeCmpMemImm(reg, inst->valueA, inst->valueB, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::CmpMR:
            result = encodeCmpMemReg(reg, inst->valueA, inst->regB, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::CmpRR:
            result = encodeCmpRegReg(reg, inst->regB, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpUnaryR:
            result = encodeOpUnaryReg(reg, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpBinaryRR:
            result = encodeOpBinaryRegReg(reg, inst->regB, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpBinaryRI:
            result = encodeOpBinaryRegImm(reg, inst->valueB, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpBinaryMI:
            result = encodeOpBinaryMemImm(reg, inst->valueA, inst->valueB, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpBinaryMR:
            result = encodeOpBinaryMemReg(reg, inst->valueA, inst->regB, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpBinaryRM:
            result = encodeOpBinaryRegMem(reg, inst->regB, inst->valueA, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpTernaryRRR:
            result = encodeOpTernaryRegRegReg(reg, inst->regB, inst->regC, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadZeroExtRR:
            result = encodeLoadZeroExtendRegReg(reg, inst->regB, inst->opBitsA, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadSignedExtRR:
            result = encodeLoadSignedExtendRegReg(reg, inst->regB, inst->opBitsA, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadAmcMR:
            result = encodeLoadAmcMemReg(reg, inst->regB, inst->valueA, inst->valueB, inst->opBitsA, inst->regC, inst->opBitsB, EMIT_CanEncode);
            break;
    }

    return result == CpuEncodeResult::Zero;
}

bool ScbeCpu::acceptsRegB(const ScbeMicroInstruction* inst, CpuReg reg)
{
    auto result = CpuEncodeResult::Zero;
    switch (inst->op)
    {
        case ScbeMicroOp::CmpRR:
            result = encodeCmpRegReg(inst->regA, reg, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpBinaryRR:
            result = encodeOpBinaryRegReg(inst->regA, reg, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpBinaryMR:
            result = encodeOpBinaryMemReg(inst->regA, inst->valueA, reg, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::OpTernaryRRR:
            result = encodeOpTernaryRegRegReg(inst->regA, reg, inst->regC, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadZeroExtRR:
            result = encodeLoadZeroExtendRegReg(inst->regA, reg, inst->opBitsA, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadSignedExtRR:
            result = encodeLoadSignedExtendRegReg(inst->regA, reg, inst->opBitsA, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadZeroExtRM:
            result = encodeLoadZeroExtendRegMem(inst->regA, reg, inst->valueA, inst->opBitsA, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadSignedExtRM:
            result = encodeLoadSignedExtendRegMem(inst->regA, reg, inst->valueA, inst->opBitsA, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadRR:
            result = encodeLoadRegReg(inst->regA, reg, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadRM:
            result = encodeLoadRegMem(inst->regA, reg, inst->valueA, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadAddrRM:
            result = encodeLoadAddressRegMem(inst->regA, reg, inst->valueA, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadAddrAmcRM:
            result = encodeLoadAddressAmcRegMem(inst->regA, inst->opBitsA, reg, inst->regC, inst->valueA, inst->valueB, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadAmcRM:
            result = encodeLoadAmcRegMem(inst->regA, inst->opBitsA, reg, inst->regC, inst->valueA, inst->valueB, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadAmcMR:
            result = encodeLoadAmcMemReg(inst->regA, reg, inst->valueA, inst->valueB, inst->opBitsA, inst->regC, inst->opBitsB, EMIT_CanEncode);
            break;
    }

    return result == CpuEncodeResult::Zero;
}

bool ScbeCpu::acceptsRegC(const ScbeMicroInstruction* inst, CpuReg reg)
{
    auto result = CpuEncodeResult::Zero;
    switch (inst->op)
    {
        case ScbeMicroOp::OpTernaryRRR:
            result = encodeOpTernaryRegRegReg(inst->regA, inst->regB, reg, inst->cpuOp, inst->opBitsA, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadAddrAmcRM:
            result = encodeLoadAddressAmcRegMem(inst->regA, inst->opBitsA, inst->regB, reg, inst->valueA, inst->valueB, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadAmcRM:
            result = encodeLoadAmcRegMem(inst->regA, inst->opBitsA, inst->regB, reg, inst->valueA, inst->valueB, inst->opBitsB, EMIT_CanEncode);
            break;
        case ScbeMicroOp::LoadAmcMR:
            result = encodeLoadAmcMemReg(inst->regA, inst->regB, inst->valueA, inst->valueB, inst->opBitsA, reg, inst->opBitsB, EMIT_CanEncode);
            break;
    }

    return result == CpuEncodeResult::Zero;
}

bool ScbeCpu::doesReadFlags(ScbeMicroInstruction* inst) const
{
    switch (inst->op)
    {
        case ScbeMicroOp::LoadCondRR:
        case ScbeMicroOp::SetCondR:
            return true;

        case ScbeMicroOp::JumpCond:
        case ScbeMicroOp::JumpCondI:
            return inst->jumpType != CpuCondJump::JUMP;
    }

    return false;
}

bool ScbeCpu::doesWriteFlags(ScbeMicroInstruction* inst) const
{
    switch (inst->op)
    {
        case ScbeMicroOp::OpBinaryRI:
        case ScbeMicroOp::OpBinaryRR:
        case ScbeMicroOp::OpBinaryMI:
        case ScbeMicroOp::OpBinaryMR:
        case ScbeMicroOp::OpBinaryRM:
        case ScbeMicroOp::OpUnaryR:
        case ScbeMicroOp::OpUnaryM:
            return true;
    }

    return false;
}
