#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_CPU.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"

void SCBE_CPU::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
    labels.clear();
    labelsToSolve.clear();
}

namespace
{
    CPUSymbol* getSymbol(SCBE_CPU& pp, const Utf8& name)
    {
        const auto it = pp.mapSymbols.find(name);
        if (it != pp.mapSymbols.end())
            return &pp.allSymbols[it->second];
        return nullptr;
    }
}

CPUSymbol* SCBE_CPU::getOrAddSymbol(const Utf8& name, CPUSymbolKind kind, uint32_t value, uint16_t sectionIdx)
{
    const auto it = getSymbol(*this, name);
    if (it)
    {
        if (it->kind == kind)
            return it;
        if (kind == CPUSymbolKind::Extern)
            return it;
        if (kind == CPUSymbolKind::Function && it->kind == CPUSymbolKind::Extern)
        {
            it->kind  = kind;
            it->value = value;
            return it;
        }

        SWAG_ASSERT(false);
    }

    CPUSymbol sym;
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

void SCBE_CPU::addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type)
{
    CPURelocation reloc;
    reloc.virtualAddress = virtualAddr;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = type;
    relocTableTextSection.table.push_back(reloc);
}

CPUSymbol* SCBE_CPU::getOrCreateGlobalString(const Utf8& str)
{
    const auto it = globalStrings.find(str);
    if (it != globalStrings.end())
        return &allSymbols[it->second];

    const Utf8 symName = form("__str%u", static_cast<uint32_t>(globalStrings.size()));
    const auto sym     = getOrAddSymbol(symName, CPUSymbolKind::GlobalString);
    globalStrings[str] = sym->index;
    sym->value         = stringSegment.addStringNoLock(str);

    return sym;
}

uint32_t SCBE_CPU::getOrCreateLabel(uint32_t instructionIndex)
{
    const auto it = labels.find(instructionIndex);
    if (it == labels.end())
    {
        const auto count         = concat.totalCount();
        labels[instructionIndex] = static_cast<int32_t>(count);
        return count;
    }

    return it->second;
}

CPUFunction* SCBE_CPU::registerFunction(AstNode* node, uint32_t symbolIndex)
{
    CPUFunction cf;
    cf.node        = node;
    cf.symbolIndex = symbolIndex;
    functions.push_back(cf);
    return &functions.back();
}

void SCBE_CPU::solveLabels()
{
    for (auto& toSolve : labelsToSolve)
    {
        auto it = labels.find(toSolve.ipDest);
        SWAG_ASSERT(it != labels.end());
        emitJump(toSolve.jump, it->second);
    }

    labels.clear();
    labelsToSolve.clear();
}

uint32_t SCBE_CPU::getParamStackOffset(const CPUFunction* cpuFunction, uint32_t paramIdx)
{
    const auto& cc = cpuFunction->typeFunc->getCallConv();

    // If this was passed as a register, then get the value from storeS4 (where input registers have been saved)
    // instead of value from the stack
    if (paramIdx < cc.paramByRegisterCount)
        return REG_OFFSET(paramIdx) + cpuFunction->offsetLocalStackParams;

    // Value from the caller stack
    return REG_OFFSET(paramIdx) + cpuFunction->offsetCallerStackParams;
}

namespace
{
    void emitParameters(SCBE_CPU& pp, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& params, void* retCopyAddr)
    {
        const auto&    cc                   = typeFuncBc->getCallConv();
        const bool     returnByStackAddr    = CallConv::returnByStackAddress(typeFuncBc);
        const uint32_t numParamsPerRegister = min(cc.paramByRegisterCount, params.size());
        uint32_t       idxParam             = 0;

        // Set the first N parameters. Can be a return register, or a function parameter.
        while (idxParam < numParamsPerRegister)
        {
            const auto type = params[idxParam].typeInfo ? params[idxParam].typeInfo : g_TypeMgr->typeInfoU64;
            const auto reg  = params[idxParam].reg;

            switch (params[idxParam].type)
            {
                case CPUPushParamType::Return:
                    if (retCopyAddr)
                        pp.emitLoad(cc.paramByRegisterInteger[idxParam], reinterpret_cast<uint64_t>(retCopyAddr), OpBits::B64);
                    else if (returnByStackAddr)
                        pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, reg, OpBits::B64);
                    else
                        pp.emitLoadAddress(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, reg);
                    break;

                case CPUPushParamType::Reg:
                    if (CallConv::structParamByValue(typeFuncBc, type))
                    {
                        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                        pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RAX, 0, OpBits::B64);
                    }
                    else if (cc.useRegisterFloat && type->isNativeFloat())
                        pp.emitLoad(cc.paramByRegisterFloat[idxParam], CPUReg::RDI, REG_OFFSET(reg), BackendEncoder::getOpBitsByBytes(type->sizeOf, true));
                    else
                        pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                    break;

                case CPUPushParamType::Imm:
                    if (cc.useRegisterFloat && type->isNativeFloat())
                        pp.emitLoad(cc.paramByRegisterFloat[idxParam], params[idxParam].reg, BackendEncoder::getOpBitsByBytes(type->sizeOf, true));
                    else
                        pp.emitLoad(cc.paramByRegisterInteger[idxParam], params[idxParam].reg, OpBits::B64);
                    break;

                case CPUPushParamType::Imm64:
                    pp.emitLoad(cc.paramByRegisterInteger[idxParam], params[idxParam].reg, OpBits::B64);
                    break;

                case CPUPushParamType::RelocV:
                    pp.emitSymbolRelocationValue(cc.paramByRegisterInteger[idxParam], static_cast<uint32_t>(params[idxParam].reg), 0);
                    break;

                case CPUPushParamType::RelocAddr:
                    pp.emitSymbolRelocationAddr(cc.paramByRegisterInteger[idxParam], static_cast<uint32_t>(params[idxParam].reg), 0);
                    break;

                case CPUPushParamType::Addr:
                    pp.emitLoadAddress(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, static_cast<uint32_t>(params[idxParam].reg));
                    break;

                case CPUPushParamType::RegAdd:
                    pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                    pp.emitOpBinary(cc.paramByRegisterInteger[idxParam], params[idxParam].val, CPUOp::ADD, OpBits::B64);
                    break;

                case CPUPushParamType::RegMul:
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                    pp.emitOpBinary(CPUReg::RAX, params[idxParam].val, CPUOp::IMUL, OpBits::B64);
                    pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::RAX:
                    pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::GlobalString:
                    pp.emitSymbolGlobalString(cc.paramByRegisterInteger[idxParam], reinterpret_cast<const char*>(params[idxParam].reg));
                    break;

                default:
                    SWAG_ASSERT(false);
                    break;
            }

            idxParam++;
        }

        // Store all parameters after N on the stack, with an offset of N * sizeof(uint64_t)
        uint32_t memOffset = numParamsPerRegister * sizeof(uint64_t);
        while (idxParam < params.size())
        {
            const auto type = params[idxParam].typeInfo ? params[idxParam].typeInfo : g_TypeMgr->typeInfoU64;
            const auto reg  = params[idxParam].reg;

            if (params[idxParam].type == CPUPushParamType::Return)
            {
                if (retCopyAddr)
                    pp.emitLoad(CPUReg::RAX, reinterpret_cast<uint64_t>(retCopyAddr), OpBits::B64);
                else if (returnByStackAddr)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, reg, OpBits::B64);
                else
                    pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, reg);
                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
            }
            else if (CallConv::structParamByValue(typeFuncBc, type))
            {
                SWAG_ASSERT(params[idxParam].type == CPUPushParamType::Reg);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, BackendEncoder::getOpBitsByBytes(type->sizeOf));
            }
            else if (type->isStruct())
            {
                SWAG_ASSERT(params[idxParam].type == CPUPushParamType::Reg);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
            }
            else
            {
                SWAG_ASSERT(params[idxParam].type == CPUPushParamType::Reg);
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), BackendEncoder::getOpBitsByBytes(type->sizeOf));
                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, BackendEncoder::getOpBitsByBytes(type->sizeOf));
            }

            memOffset += sizeof(uint64_t); // Push is always aligned
            idxParam++;
        }
    }
}

void SCBE_CPU::emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& params, uint32_t offset, void* retCopyAddr)
{
    uint32_t numCallParams = typeFuncBc->parameters.size();
    uint32_t indexParam    = 0;

    pushParams.clear();

    // Variadic are first
    if (typeFuncBc->isFctVariadic())
    {
        pushParams.push_back(params[indexParam++]);
        pushParams.push_back(params[indexParam++]);
    }

    if (typeFuncBc->isFctVariadic() || typeFuncBc->isFctCVariadic())
    {
        numCallParams--;
    }

    // All parameters
    for (uint32_t i = 0; i < numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteType(typeFuncBc->parameters[i]->typeInfo);
        if (typeParam->isAutoConstPointerRef())
            typeParam = TypeManager::concretePtrRef(typeParam);

        if (typeParam->isPointer() ||
            typeParam->isLambdaClosure() ||
            typeParam->isArray())
        {
            pushParams.push_back(params[indexParam++]);
        }
        else if (typeParam->isSlice() ||
                 typeParam->isString() ||
                 typeParam->isAny() ||
                 typeParam->isInterface())
        {
            pushParams.push_back(params[indexParam++]);
            pushParams.push_back(params[indexParam++]);
        }
        else
        {
            auto param     = params[indexParam++];
            param.typeInfo = typeParam;
            pushParams.push_back(param);
        }
    }

    // Return by parameter
    if (CallConv::returnByAddress(typeFuncBc))
    {
        pushParams.push_back({.type = CPUPushParamType::Return, .reg = offset});
    }

    // Add all C variadic parameters
    if (typeFuncBc->isFctCVariadic())
    {
        for (uint32_t i = typeFuncBc->numParamsRegisters(); i < params.size(); i++)
        {
            pushParams.push_back(params[indexParam++]);
        }
    }

    // If a lambda is assigned to a closure, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal lambda function).
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBc->isClosure())
    {
        SWAG_ASSERT(pushParams[0].type == CPUPushParamType::Reg);

        // First register is closure context, except if variadic, where we have 2 registers for the slice first
        // :VariadicAndClosure
        const auto reg = typeFuncBc->isFctVariadic() ? pushParams[2].reg : pushParams[0].reg;
        emitCmp(CPUReg::RDI, REG_OFFSET(reg), 0, OpBits::B64);

        // If not zero, jump to closure call
        const auto jumpClosure = emitJump(JZ, OpBits::B8);

        emitParameters(*this, typeFuncBc, pushParams, retCopyAddr);

        // Jump to after closure call
        const auto jumpAfterClosure = emitJump(JUMP, OpBits::B8);

        // Update jump to closure call
        emitJump(jumpClosure, concat.totalCount());

        // First register is closure context, except if variadic, where we have 2 registers for the slice first
        // :VariadicAndClosure
        if (typeFuncBc->isFctVariadic())
            pushParams.erase(2);
        else
            pushParams.erase(0);
        emitParameters(*this, typeFuncBc, pushParams, retCopyAddr);

        emitJump(jumpAfterClosure, concat.totalCount());
    }
    else
    {
        emitParameters(*this, typeFuncBc, pushParams, retCopyAddr);
    }
}

void SCBE_CPU::emitStoreCallResult(CPUReg memReg, uint32_t memOffset, const TypeInfoFuncAttr* typeFuncBc)
{
    if (!CallConv::returnByValue(typeFuncBc))
        return;

    SWAG_ASSERT(memReg == CPUReg::RDI);
    const auto& cc         = typeFuncBc->getCallConv();
    const auto  returnType = typeFuncBc->concreteReturnType();
    if (returnType->isNativeFloat())
        emitStore(memReg, memOffset, cc.returnByRegisterFloat, OpBits::F64);
    else
        emitStore(memReg, memOffset, cc.returnByRegisterInteger, OpBits::B64);
}
