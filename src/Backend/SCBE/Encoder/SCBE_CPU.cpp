#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_CPU.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"

void SCBE_CPU::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
    clearInstructionCache();
    labels.clear();
    labelsToSolve.clear();
}

void SCBE_CPU::clearInstructionCache()
{
    storageConcatCount = UINT32_MAX;
    storageMemOffset   = 0;
    storageNumBits     = OpBits::INVALID;
    storageMemReg      = CPUReg::RAX;
    storageReg         = CPUReg::RAX;
}

CPUSymbol* SCBE_CPU::getSymbol(const Utf8& name)
{
    const auto it = mapSymbols.find(name);
    if (it != mapSymbols.end())
        return &allSymbols[it->second];
    return nullptr;
}

CPUSymbol* SCBE_CPU::getOrAddSymbol(const Utf8& name, CPUSymbolKind kind, uint32_t value, uint16_t sectionIdx)
{
    const auto it = getSymbol(name);
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

uint32_t SCBE_CPU::getParamStackOffset(const CPUFunction* cpuFct, uint32_t paramIdx)
{
    const auto& cc = cpuFct->typeFunc->getCallConv();

    // If this was passed as a register, then get the value from storeS4 (where input registers have been saved)
    // instead of value from the stack
    if (paramIdx < cc.paramByRegisterCount)
        return REG_OFFSET(paramIdx) + cpuFct->offsetLocalStackParams;

    // Value from the caller stack
    return REG_OFFSET(paramIdx) + cpuFct->offsetCallerStackParams;
}

namespace
{
    void emitParameters(SCBE_CPU& pp, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& paramsRegisters, const VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr)
    {
        const auto& cc                = typeFuncBc->getCallConv();
        const bool  returnByStackAddr = CallConv::returnByStackAddress(typeFuncBc);

        const uint32_t callConvRegisters    = cc.paramByRegisterCount;
        const uint32_t maxParamsPerRegister = paramsRegisters.size();

        // Set the first N parameters. Can be return register, or function parameter.
        uint32_t i = 0;
        for (; i < min(callConvRegisters, maxParamsPerRegister); i++)
        {
            auto       type = paramsTypes[i];
            const auto reg  = static_cast<uint32_t>(paramsRegisters[i].reg);

            if (type->isAutoConstPointerRef())
                type = TypeManager::concretePtrRef(type);

            // This is a return register
            if (type == g_TypeMgr->typeInfoUndefined)
            {
                SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                if (retCopyAddr)
                    pp.emitLoad(cc.paramByRegisterInteger[i], reinterpret_cast<uint64_t>(retCopyAddr), OpBits::B64);
                else if (returnByStackAddr)
                    pp.emitLoad(cc.paramByRegisterInteger[i], CPUReg::RDI, reg, OpBits::B64);
                else
                    pp.emitLoadAddress(cc.paramByRegisterInteger[i], CPUReg::RDI, reg);
            }

            // This is a normal parameter, which can be float or integer
            else
            {
                // Pass struct in a register if small enough
                if (CallConv::structParamByValue(typeFuncBc, type))
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                    pp.emitLoad(cc.paramByRegisterInteger[i], CPUReg::RAX, 0, OpBits::B64);
                }
                else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F32))
                {
                    if (paramsRegisters[i].type == CPUPushParamType::Imm)
                    {
                        SWAG_ASSERT(paramsRegisters[i].reg <= UINT32_MAX);
                        pp.emitLoad(CPUReg::RAX, static_cast<uint32_t>(paramsRegisters[i].reg), OpBits::B32);
                        pp.emitLoad(cc.paramByRegisterFloat[i], CPUReg::RAX, OpBits::F32);
                    }
                    else
                    {
                        SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                        pp.emitLoad(cc.paramByRegisterFloat[i], CPUReg::RDI, REG_OFFSET(reg), OpBits::F32);
                    }
                }
                else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F64))
                {
                    if (paramsRegisters[i].type == CPUPushParamType::Imm)
                    {
                        pp.emitLoad(CPUReg::RAX, paramsRegisters[i].reg, OpBits::B64);
                        pp.emitLoad(cc.paramByRegisterFloat[i], CPUReg::RAX, OpBits::F64);
                    }
                    else
                    {
                        SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                        pp.emitLoad(cc.paramByRegisterFloat[i], CPUReg::RDI, REG_OFFSET(reg), OpBits::F64);
                    }
                }
                else
                {
                    switch (paramsRegisters[i].type)
                    {
                        case CPUPushParamType::Imm:
                            if (paramsRegisters[i].reg == 0)
                                pp.emitClear(cc.paramByRegisterInteger[i], OpBits::B64);
                            else
                                pp.emitLoad(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, OpBits::B64);
                            break;
                        case CPUPushParamType::Imm64:
                            pp.emitLoad(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, OpBits::B64);
                            break;
                        case CPUPushParamType::RelocV:
                            pp.emitSymbolRelocationValue(cc.paramByRegisterInteger[i], static_cast<uint32_t>(paramsRegisters[i].reg), 0);
                            break;
                        case CPUPushParamType::RelocAddr:
                            pp.emitSymbolRelocationAddr(cc.paramByRegisterInteger[i], static_cast<uint32_t>(paramsRegisters[i].reg), 0);
                            break;
                        case CPUPushParamType::Addr:
                            pp.emitLoadAddress(cc.paramByRegisterInteger[i], CPUReg::RDI, static_cast<uint32_t>(paramsRegisters[i].reg));
                            break;
                        case CPUPushParamType::RegAdd:
                            pp.emitLoad(cc.paramByRegisterInteger[i], CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                            pp.emitOpBinary(cc.paramByRegisterInteger[i], paramsRegisters[i].val, CPUOp::ADD, OpBits::B64);
                            break;
                        case CPUPushParamType::RegMul:
                            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                            pp.emitOpBinary(CPUReg::RAX, paramsRegisters[i].val, CPUOp::IMUL, OpBits::B64);
                            pp.emitLoad(cc.paramByRegisterInteger[i], CPUReg::RAX, OpBits::B64);
                            break;
                        case CPUPushParamType::RAX:
                            pp.emitLoad(cc.paramByRegisterInteger[i], CPUReg::RAX, OpBits::B64);
                            break;
                        case CPUPushParamType::GlobalString:
                            pp.emitSymbolGlobalString(cc.paramByRegisterInteger[i], reinterpret_cast<const char*>(paramsRegisters[i].reg));
                            break;
                        default:
                            SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                            pp.emitLoad(cc.paramByRegisterInteger[i], CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                            break;
                    }
                }
            }
        }

        // Store all parameters after N on the stack, with an offset of N * sizeof(uint64_t)
        uint32_t memOffset = min(callConvRegisters, maxParamsPerRegister) * sizeof(uint64_t);
        for (; i < paramsRegisters.size(); i++)
        {
            auto type = paramsTypes[i];
            if (type->isAutoConstPointerRef())
                type = TypeManager::concretePtrRef(type);

            const auto reg = static_cast<uint32_t>(paramsRegisters[i].reg);
            SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);

            // This is a C variadic parameter
            if (i >= maxParamsPerRegister)
            {
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
            }

            // This is for a return value
            else if (type == g_TypeMgr->typeInfoUndefined)
            {
                // r is an address to registerRR, for FFI
                if (retCopyAddr)
                    pp.emitLoad(CPUReg::RAX, reinterpret_cast<uint64_t>(retCopyAddr), OpBits::B64);
                else if (returnByStackAddr)
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, reg, OpBits::B64);
                else
                    pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, reg);
                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
            }

            // This is for a normal parameter
            else
            {
                const auto sizeOf = type->sizeOf;

                // Struct by copy. Will be a pointer to the stack
                if (type->isStruct())
                {
                    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);

                    // Store the content of the struct in the stack
                    if (CallConv::structParamByValue(typeFuncBc, type))
                    {
                        switch (sizeOf)
                        {
                            case 1:
                                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, OpBits::B8);
                                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B8);
                                break;
                            case 2:
                                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, OpBits::B16);
                                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B16);
                                break;
                            case 4:
                                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, OpBits::B32);
                                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B32);
                                break;
                            case 8:
                                pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, OpBits::B64);
                                pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                                break;
                            default:
                                break;
                        }
                    }

                    // Store the address of the struct in the stack
                    else
                    {
                        pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    }
                }
                else
                {
                    switch (sizeOf)
                    {
                        case 1:
                            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B8);
                            pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B8);
                            break;
                        case 2:
                            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B16);
                            pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B16);
                            break;
                        case 4:
                            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B32);
                            pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B32);
                            break;
                        case 8:
                            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), OpBits::B64);
                            pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                            break;
                        default:
                            SWAG_ASSERT(false);
                            return;
                    }
                }
            }

            // Push is always aligned
            memOffset += 8;
        }
    }
}

void SCBE_CPU::emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& params, uint32_t offset, void* retCopyAddr)
{
    uint32_t numCallParams = typeFuncBc->parameters.size();
    pushParams.clear();
    pushParamsTypes.clear();

    uint32_t indexParam = params.size() - 1;

    // Variadic are first
    if (typeFuncBc->isFctVariadic())
    {
        auto index = params[indexParam--];
        pushParams.push_back(index);
        pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);

        index = params[indexParam--];
        pushParams.push_back(index);
        pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        numCallParams--;
    }
    else if (typeFuncBc->isFctCVariadic())
    {
        numCallParams--;
    }

    // All parameters
    for (uint32_t i = 0; i < numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteType(typeFuncBc->parameters[i]->typeInfo);
        if (typeParam->isAutoConstPointerRef())
            typeParam = TypeManager::concretePtrRef(typeParam);

        auto index = params[indexParam--];

        if (typeParam->isPointer() ||
            typeParam->isLambdaClosure() ||
            typeParam->isArray())
        {
            pushParams.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else if (typeParam->isStruct())
        {
            pushParams.push_back(index);
            pushParamsTypes.push_back(typeParam);
        }
        else if (typeParam->isSlice() ||
                 typeParam->isString())
        {
            pushParams.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
            index = params[indexParam--];
            pushParams.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else if (typeParam->isAny() ||
                 typeParam->isInterface())
        {
            pushParams.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
            index = params[indexParam--];
            pushParams.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else
        {
            SWAG_ASSERT(typeParam->sizeOf <= sizeof(void*));
            pushParams.push_back(index);
            pushParamsTypes.push_back(typeParam);
        }
    }

    // Return by parameter
    if (CallConv::returnByAddress(typeFuncBc))
    {
        pushParams.push_back({CPUPushParamType::Reg, offset});
        pushParamsTypes.push_back(g_TypeMgr->typeInfoUndefined);
    }

    // Add all C variadic parameters
    if (typeFuncBc->isFctCVariadic())
    {
        for (uint32_t i = typeFuncBc->numParamsRegisters(); i < params.size(); i++)
        {
            auto index = params[indexParam--];
            pushParams.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
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
        uint32_t reg = static_cast<uint32_t>(pushParams[0].reg);
        if (typeFuncBc->isFctVariadic())
            reg = static_cast<uint32_t>(pushParams[2].reg);

        emitCmp(CPUReg::RDI, REG_OFFSET(reg), 0, OpBits::B64);

        // If not zero, jump to closure call
        const auto seekPtrClosure = emitJumpLong(JZ);
        const auto seekJmpClosure = concat.totalCount();

        emitParameters(*this, typeFuncBc, pushParams, pushParamsTypes, retCopyAddr);

        // Jump to after closure call
        const auto seekPtrAfterClosure = emitJumpLong(JUMP);
        const auto seekJmpAfterClosure = concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = static_cast<uint8_t>(concat.totalCount() - seekJmpClosure);

        // First register is closure context, except if variadic, where we have 2 registers for the slice first
        // :VariadicAndClosure
        if (typeFuncBc->isFctVariadic())
        {
            pushParams.erase(2);
            pushParamsTypes.erase(2);
        }
        else
        {
            pushParams.erase(0);
            pushParamsTypes.erase(0);
        }
        emitParameters(*this, typeFuncBc, pushParams, pushParamsTypes, retCopyAddr);

        *seekPtrAfterClosure = static_cast<uint8_t>(concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emitParameters(*this, typeFuncBc, pushParams, pushParamsTypes, retCopyAddr);
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
