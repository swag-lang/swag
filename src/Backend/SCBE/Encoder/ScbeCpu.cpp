#include "pch.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Backend/SCBE/Obj/ScbeCoff.h"
#include "Core/Math.h"
#include "Main/CommandLine.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeInfo.h"
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

void ScbeCpu::addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type)
{
    CpuRelocation reloc;
    reloc.virtualAddress = virtualAddr;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = type;
    relocTableTextSection.table.push_back(reloc);
}

CpuSymbol* ScbeCpu::getOrCreateGlobalString(const Utf8& str)
{
    const auto it = globalStrings.find(str);
    if (it != globalStrings.end())
        return &allSymbols[it->second];

    const Utf8 symName = form("__str%u", static_cast<uint32_t>(globalStrings.size()));
    const auto sym     = getOrAddSymbol(symName, CpuSymbolKind::GlobalString);
    globalStrings[str] = sym->index;
    sym->value         = stringSegment.addStringNoLock(str);

    return sym;
}

void ScbeCpu::emitLabel(uint32_t instructionIndex)
{
    const auto it = cpuFct->labels.find(instructionIndex);
    if (it == cpuFct->labels.end())
        cpuFct->labels[instructionIndex] = static_cast<int32_t>(concat.totalCount());
}

CpuFunction* ScbeCpu::addFunction(const Utf8& funcName, const CallConv* ccFunc, ByteCode* bc)
{
    concat.align(16);

    CpuFunction* cf  = Allocator::alloc<CpuFunction>();
    cc               = ccFunc;
    cf->cc           = ccFunc;
    cf->bc           = bc;
    cf->symbolIndex  = getOrAddSymbol(funcName, CpuSymbolKind::Function, concat.totalCount() - textSectionOffset)->index;
    cf->startAddress = concat.totalCount();

    if (bc)
    {
        if (bc->node)
            cf->node = castAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);
        cf->typeFunc = bc->getCallType();

        // Calling convention, space for at least 'MAX_CALL_CONV_REGISTERS' parameters when calling a function
        // (should ideally be reserved only if we have a call)
        //
        // Because of variadic parameters in fct calls, we need to add some extra room, in case we have to flatten them
        // We want to be sure to have the room to flatten the array of variadic (make all params contiguous). That's
        // why we multiply by 2.
        //
        // Why 2 ?? magic number ??
        cf->sizeStackCallParams = 2 * static_cast<uint32_t>(std::max(CallConv::MAX_CALL_CONV_REGISTERS, (bc->maxCallParams + 1)) * sizeof(void*));
        cf->sizeStackCallParams = Math::align(cf->sizeStackCallParams, ccFunc->stackAlign);
    }

    functions.push_back(cf);
    return cf;
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

VectorNative<CpuReg> ScbeCpu::getReadRegisters(ScbeMicroInstruction* inst)
{
    VectorNative<CpuReg> result;
    if (inst->isCall())
    {
        result.append(cc->paramByRegisterInteger);
        result.append(cc->paramByRegisterFloat);
    }
    else
    {
        if (inst->hasReadRegA())
            result.push_back(inst->regA);
        if (inst->hasReadRegB())
            result.push_back(inst->regB);
        if (inst->hasReadRegC())
            result.push_back(inst->regC);
    }

    return result;
}

VectorNative<CpuReg> ScbeCpu::getWriteRegisters(ScbeMicroInstruction* inst)
{
    VectorNative<CpuReg> result;
    if (inst->isCall())
    {
        result.append(cc->volatileRegistersInteger);
        result.append(cc->volatileRegistersFloat);
        result.push_back(cc->returnByRegisterInteger);
        result.push_back(cc->returnByRegisterFloat);
    }
    else
    {
        if (inst->hasWriteRegA())
            result.push_back(inst->regA);
        if (inst->hasWriteRegB())
            result.push_back(inst->regB);
        if (inst->hasWriteRegC())
            result.push_back(inst->regC);
    }

    return result;
}
