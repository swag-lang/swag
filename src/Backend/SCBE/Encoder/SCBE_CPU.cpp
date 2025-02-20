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
        patchJump(toSolve.jump, it->second);
    }

    labels.clear();
    labelsToSolve.clear();
}

namespace
{
    void emitParameters(SCBE_CPU& pp, const CallConv& cc, const VectorNative<CPUPushParam>& params)
    {
        const uint32_t numParamsPerRegister = min(cc.paramByRegisterCount, params.size());
        uint32_t       idxParam             = 0;

        // Set the first N parameters. Can be a return register, or a function parameter.
        while (idxParam < numParamsPerRegister)
        {
            const auto type  = params[idxParam].typeInfo ? params[idxParam].typeInfo : g_TypeMgr->typeInfoU64;
            const auto value = params[idxParam].value;

            switch (params[idxParam].type)
            {
                case CPUPushParamType::Load:
                    pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, value, OpBits::B64);
                    break;

                case CPUPushParamType::LoadAddress:
                    pp.emitLoadAddress(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, value);
                    break;

                case CPUPushParamType::Constant:
                    if (cc.useRegisterFloat && type->isNativeFloat())
                        pp.emitLoad(cc.paramByRegisterFloat[idxParam], value, BackendEncoder::getOpBitsByBytes(type->sizeOf, true));
                    else
                        pp.emitLoad(cc.paramByRegisterInteger[idxParam], value, OpBits::B64);
                    break;

                case CPUPushParamType::Constant64:
                    pp.emitLoad64(cc.paramByRegisterInteger[idxParam], value);
                    break;

                case CPUPushParamType::SwagRegister:
                    if (cc.structParamByValue(type))
                    {
                        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(value), OpBits::B64);
                        pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RAX, 0, OpBits::B64);
                    }
                    else if (cc.useRegisterFloat && type->isNativeFloat())
                    {
                        pp.emitLoad(cc.paramByRegisterFloat[idxParam], CPUReg::RDI, REG_OFFSET(value), BackendEncoder::getOpBitsByBytes(type->sizeOf, true));
                    }
                    else
                    {
                        pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, REG_OFFSET(value), OpBits::B64);
                    }
                    break;

                case CPUPushParamType::CPURegister:
                    pp.emitLoad(cc.paramByRegisterInteger[idxParam], static_cast<CPUReg>(value), OpBits::B64);
                    break;

                case CPUPushParamType::SymRelationValue:
                    SWAG_ASSERT(value < UINT32_MAX);
                    pp.emitSymbolRelocationValue(cc.paramByRegisterInteger[idxParam], static_cast<uint32_t>(value), 0);
                    break;

                case CPUPushParamType::SymRelocationAddress:
                    SWAG_ASSERT(value < UINT32_MAX);
                    pp.emitSymbolRelocationAddr(cc.paramByRegisterInteger[idxParam], static_cast<uint32_t>(value), 0);
                    break;

                case CPUPushParamType::SwagRegisterAdd:
                    pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RDI, REG_OFFSET(value), OpBits::B64);
                    pp.emitOpBinary(cc.paramByRegisterInteger[idxParam], params[idxParam].value2, CPUOp::ADD, OpBits::B64);
                    break;

                case CPUPushParamType::SwagRegisterMul:
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(value), OpBits::B64);
                    pp.emitOpBinary(CPUReg::RAX, params[idxParam].value2, CPUOp::IMUL, OpBits::B64);
                    pp.emitLoad(cc.paramByRegisterInteger[idxParam], CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::GlobalString:
                    pp.emitSymbolGlobalString(cc.paramByRegisterInteger[idxParam], reinterpret_cast<const char*>(value));
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
            const auto type  = params[idxParam].typeInfo ? params[idxParam].typeInfo : g_TypeMgr->typeInfoU64;
            const auto value = params[idxParam].value;

            switch (params[idxParam].type)
            {
                case CPUPushParamType::Load:
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, value, OpBits::B64);
                    pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::LoadAddress:
                    pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, value);
                    pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::Constant:
                    pp.emitStore(CPUReg::RSP, memOffset, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    break;

                case CPUPushParamType::Constant64:
                    pp.emitLoad64(CPUReg::RAX, value);
                    pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::SwagRegister:
                    if (cc.structParamByValue(type))
                    {
                        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(value), OpBits::B64);
                        pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                        pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    }
                    else if (type->isStruct())
                    {
                        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(value), OpBits::B64);
                        pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    }
                    else if (type->sizeOf > sizeof(uint64_t))
                    {
                        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(value), OpBits::B64);
                        pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    }
                    else
                    {
                        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(value), BackendEncoder::getOpBitsByBytes(type->sizeOf));
                        pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    }
                    break;

                default:
                    SWAG_ASSERT(false);
                    break;
            }

            memOffset += sizeof(uint64_t); // Push is always aligned
            idxParam++;
        }
    }
}

void SCBE_CPU::emitCallParameters(const CallConv& callConv, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& cpuParams)
{
    // If a lambda is assigned to a closure, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal lambda function).
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBc->isClosure())
    {
        uint32_t idxParamContext = 0;
        if (cpuParams[0].type == CPUPushParamType::CPURegister) // Bytecode
        {
            idxParamContext++;
            idxParamContext += typeFuncBc->numReturnRegisters();
        }
        else if (typeFuncBc->isFctVariadic())
        {
            idxParamContext += 2;
        }

        SWAG_ASSERT(cpuParams[idxParamContext].type == CPUPushParamType::SwagRegister);

        // First register is closure context, except if variadic, where we have 2 registers for the slice first
        // :VariadicAndClosure
        const auto reg = cpuParams[idxParamContext].value;
        emitCmp(CPUReg::RDI, REG_OFFSET(reg), 0, OpBits::B64);

        // If zero, jump to parameters for a non closure call
        const auto jumpToNoClosure = emitJump(JZ, OpBits::B32);

        // Emit parameters for the closure call (with the pointer to context)
        emitParameters(*this, callConv, cpuParams);
        const auto jumpAfterParameters = emitJump(JUMP, OpBits::B32);

        patchJump(jumpToNoClosure, concat.totalCount());

        // First register is closure, except if variadic, where we have 2 registers for the slice before.
        // We must remove it as we are in the "not closure" call path.
        // :VariadicAndClosure
        auto params = cpuParams;
        params.erase(idxParamContext);

        emitParameters(*this, callConv, params);

        patchJump(jumpAfterParameters, concat.totalCount());
    }
    else
    {
        emitParameters(*this, callConv, cpuParams);
    }
}

void SCBE_CPU::emitComputeCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& cpuParams, uint32_t resultOffsetRT, void* resultAddr)
{
    
    uint32_t numCallParams = typeFuncBc->parameters.size();
    uint32_t indexParam    = 0;

    // For variadic, we have a slice at the start
    if (typeFuncBc->isFctVariadic())
        indexParam += 2;
    // We should not count the variadic parameter as a real parameter
    if (typeFuncBc->isFctVariadic() || typeFuncBc->isFctCVariadic())
        numCallParams--;

    // Set type of all parameters
    pushParams = cpuParams;
    for (uint32_t i = 0; i < numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteType(typeFuncBc->parameters[i]->typeInfo);
        if (typeParam->isAutoConstPointerRef())
            typeParam = TypeManager::concretePtrRef(typeParam);
        pushParams[indexParam].typeInfo = typeParam;
        indexParam += typeParam->numRegisters();
    }

    // Return by parameter
    if (CallConv::returnByAddress(typeFuncBc))
    {
        if (resultAddr)
            pushParams.insert_at_index({.type = CPUPushParamType::Constant64, .value = reinterpret_cast<uint64_t>(resultAddr)}, indexParam);
        else if (CallConv::returnByStackAddress(typeFuncBc))
            pushParams.insert_at_index({.type = CPUPushParamType::Load, .value = resultOffsetRT}, indexParam);
        else
            pushParams.insert_at_index({.type = CPUPushParamType::LoadAddress, .value = resultOffsetRT}, indexParam);
    }
    
    emitCallParameters(typeFuncBc->getCallConv(), typeFuncBc, pushParams);
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
