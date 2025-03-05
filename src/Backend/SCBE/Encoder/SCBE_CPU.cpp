#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_CPU.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Backend/SCBE/Obj/SCBE_Coff.h"
#include "Core/Math.h"
#include "Main/CommandLine.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"

uint32_t CPUFunction::getStackOffsetParam(uint32_t paramIdx) const
{
    // If the parameter has been passed as a CPU register, then we get the value from 'offsetParamsAsRegisters'
    // (where input registers have been saved) instead of the value from the caller stack
    //
    // Otherwise the parameter has been passed by stack, so we get the value for the caller stack offset
    const auto offset = paramIdx < cc->paramByRegisterCount ? offsetParamsAsRegisters : offsetCallerStackParams;
    return sizeStackCallParams + offset + (paramIdx * sizeof(Register));
}

uint32_t CPUFunction::getStackOffsetCallerParam(uint32_t paramIdx) const
{
    return sizeStackCallParams + offsetCallerStackParams + (paramIdx * sizeof(Register));
}

uint32_t CPUFunction::getStackOffsetBCStack() const
{
    return sizeStackCallParams + offsetByteCodeStack;
}

void SCBE_CPU::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
    optLevel = buildParameters.buildCfg ? buildParameters.buildCfg->backendOptimize : BuildCfgBackendOptim::O0;
}

uint32_t SCBE_CPU::getStackOffsetReg(uint32_t reg) const
{
    return cpuFct->sizeStackCallParams + (reg * sizeof(Register));
}

uint32_t SCBE_CPU::getStackOffsetRT(uint32_t reg) const
{
    return cpuFct->sizeStackCallParams + cpuFct->offsetRT + (reg * sizeof(Register));
}

uint32_t SCBE_CPU::getStackOffsetResult() const
{
    return cpuFct->sizeStackCallParams + cpuFct->offsetResult;
}

uint32_t SCBE_CPU::getStackOffsetFLT() const
{
    return cpuFct->sizeStackCallParams + cpuFct->offsetFLT;
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

void SCBE_CPU::emitLabel(uint32_t instructionIndex)
{
    const auto it = cpuFct->labels.find(instructionIndex);
    if (it == cpuFct->labels.end())
        cpuFct->labels[instructionIndex] = static_cast<int32_t>(concat.totalCount());
}

CPUFunction* SCBE_CPU::addFunction(const Utf8& funcName, const CallConv* cc, ByteCode* bc)
{
    concat.align(16);

    CPUFunction* cf  = Allocator::alloc<CPUFunction>();
    cf->cc           = cc;
    cf->bc           = bc;
    cf->symbolIndex  = getOrAddSymbol(funcName, CPUSymbolKind::Function, concat.totalCount() - textSectionOffset)->index;
    cf->startAddress = concat.totalCount();

    if (bc)
    {
        cf->node     = bc->node;
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
        cf->sizeStackCallParams = Math::align(cf->sizeStackCallParams, cc->stackAlign);
    }

    functions.push_back(cf);
    return cf;
}

void SCBE_CPU::endFunction() const
{
    cpuFct->endAddress = concat.totalCount();

    const auto objFileType = SCBE::getObjType(g_CommandLine.target);
    switch (objFileType)
    {
        case BackendObjType::Coff:
        {
            VectorNative<uint32_t> unwindOffsetRegs;
            uint32_t               offset = cpuFct->startAddress;
            for (uint32_t i = 0; i < cpuFct->unwindRegs.size(); i++)
            {
                unwindOffsetRegs.push_back(offset - cpuFct->startAddress);
                offset += 1;
            }
            SCBE_Coff::computeUnwind(cpuFct->unwindRegs, unwindOffsetRegs, cpuFct->frameSize, cpuFct->sizeProlog, cpuFct->unwind);
            break;
        }
        default:
            Report::internalError(module, "SCBE::computeUnwind, unsupported output");
            break;
    }
}

bool SCBE_CPU::isNoOp(uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags) const
{
    if (emitFlags.has(EMITF_Overflow))
        return false;
    if (isFloat(opBits))
        return false;
    if (optLevel <= BuildCfgBackendOptim::O1)
        return false;

    maskValue(value, opBits);
    switch (value)
    {
        case 0:
            return op == CPUOp::ADD || op == CPUOp::SUB || op == CPUOp::SAR || op == CPUOp::SHL || op == CPUOp::SHR || op == CPUOp::ROL || op == CPUOp::ROR || op == CPUOp::BT;
        case 1:
            return op == CPUOp::MUL || op == CPUOp::IMUL || op == CPUOp::DIV || op == CPUOp::IDIV;
        default:
            break;
    }

    return false;
}

namespace
{
    void emitParameters(SCBE_CPU& pp, const VectorNative<CPUPushParam>& params, const CallConv* callConv)
    {
        const uint32_t numParamsPerRegister = std::min(callConv->paramByRegisterCount, params.size());
        uint32_t       idxParam             = 0;

        // Set the first N parameters. Can be a return register, or a function parameter.
        while (idxParam < numParamsPerRegister)
        {
            const auto type  = params[idxParam].typeInfo ? params[idxParam].typeInfo : g_TypeMgr->typeInfoU64;
            const auto value = params[idxParam].value;

            switch (params[idxParam].type)
            {
                case CPUPushParamType::Load:
                    pp.emitLoad(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CPUPushParamType::LoadAddress:
                    pp.emitLoadAddress(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value);
                    break;

                case CPUPushParamType::Constant:
                    if (callConv->useRegisterFloat && type->isNativeFloat())
                        pp.emitLoad(callConv->paramByRegisterFloat[idxParam], value, BackendEncoder::getOpBitsByBytes(type->sizeOf, true));
                    else
                        pp.emitLoad(callConv->paramByRegisterInteger[idxParam], value, OpBits::B64);
                    break;

                case CPUPushParamType::Constant64:
                    pp.emitLoad(callConv->paramByRegisterInteger[idxParam], value);
                    break;

                case CPUPushParamType::CaptureContext:
                    pp.emitLoad(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CPUPushParamType::SwagParamStructValue:
                    pp.emitLoad(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoad(callConv->paramByRegisterInteger[idxParam], CPUReg::RAX, 0, OpBits::B64);
                    break;

                case CPUPushParamType::SwagRegister:
                    if (callConv->useRegisterFloat && type->isNativeFloat())
                        pp.emitLoad(callConv->paramByRegisterFloat[idxParam], params[idxParam].baseReg, value, BackendEncoder::getOpBitsByBytes(type->sizeOf, true));
                    else
                        pp.emitLoad(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CPUPushParamType::CPURegister:
                    pp.emitLoad(callConv->paramByRegisterInteger[idxParam], static_cast<CPUReg>(value), OpBits::B64);
                    break;

                case CPUPushParamType::SymbolRelocationValue:
                    SWAG_ASSERT(value < UINT32_MAX);
                    pp.emitSymbolRelocationValue(callConv->paramByRegisterInteger[idxParam], static_cast<uint32_t>(value), 0);
                    break;

                case CPUPushParamType::SymbolRelocationAddress:
                    SWAG_ASSERT(value < UINT32_MAX);
                    pp.emitSymbolRelocationAddress(callConv->paramByRegisterInteger[idxParam], static_cast<uint32_t>(value), 0);
                    break;

                case CPUPushParamType::SwagRegisterAdd:
                    pp.emitLoad(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    if (params[idxParam].value2)
                        pp.emitOpBinary(callConv->paramByRegisterInteger[idxParam], params[idxParam].value2, CPUOp::ADD, OpBits::B64);
                    break;

                case CPUPushParamType::SwagRegisterMul:
                    pp.emitLoad(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    if (params[idxParam].value2 != 1)
                        pp.emitOpBinary(CPUReg::RAX, params[idxParam].value2, CPUOp::IMUL, OpBits::B64);
                    pp.emitLoad(callConv->paramByRegisterInteger[idxParam], CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::GlobalString:
                    pp.emitSymbolGlobalString(callConv->paramByRegisterInteger[idxParam], reinterpret_cast<const char*>(value));
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
                    pp.emitLoad(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::LoadAddress:
                    pp.emitLoadAddress(CPUReg::RAX, params[idxParam].baseReg, value);
                    pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::Constant:
                    pp.emitStore(CPUReg::RSP, memOffset, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    break;

                case CPUPushParamType::Constant64:
                    pp.emitLoad(CPUReg::RAX, value);
                    pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::CaptureContext:
                    pp.emitLoad(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::SwagParamStructValue:
                    pp.emitLoad(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    break;

                case CPUPushParamType::SwagRegister:
                    if (type->isStruct())
                    {
                        pp.emitLoad(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                        pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    }
                    else if (type->sizeOf > sizeof(uint64_t))
                    {
                        pp.emitLoad(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                        pp.emitStore(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    }
                    else
                    {
                        pp.emitLoad(CPUReg::RAX, params[idxParam].baseReg, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
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

void SCBE_CPU::emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& cpuParams, const CallConv* callConv)
{
    // If a lambda is assigned to a closure, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal lambda function).
    // But as this is dynamic, we need to have two call paths : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBc->isClosure())
    {
        // Get the capture context index, as we will have to remove it when calling a normal lambda
        uint32_t idxParamContext = 0;
        while (idxParamContext < cpuParams.size() && cpuParams[idxParamContext].type != CPUPushParamType::CaptureContext)
            idxParamContext++;
        SWAG_ASSERT(idxParamContext < cpuParams.size());
        SWAG_ASSERT(cpuParams[idxParamContext].type == CPUPushParamType::CaptureContext);

        // First register is closure context, except if variadic, where we have 2 registers for the slice first
        // :VariadicAndClosure
        emitCmp(cpuParams[idxParamContext].baseReg, cpuParams[idxParamContext].value, 0, OpBits::B64);

        // If zero, jump to parameters for a non closure call
        const auto jumpToNoClosure = emitJump(JZ, OpBits::B32);

        // Emit parameters for the closure call (with the pointer to context)
        emitParameters(*this, cpuParams, callConv);
        const auto jumpAfterParameters = emitJump(JUMP, OpBits::B32);

        emitPatchJump(jumpToNoClosure);

        // First register is closure, except if variadic, where we have 2 registers for the slice before.
        // We must remove it as we are in the "not closure" call path.
        // :VariadicAndClosure
        auto params = cpuParams;
        params.erase(idxParamContext);

        emitParameters(*this, params, callConv);

        emitPatchJump(jumpAfterParameters);
    }
    else
    {
        emitParameters(*this, cpuParams, callConv);
    }
}

void SCBE_CPU::emitComputeCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& cpuParams, CPUReg memRegResult, uint32_t memOffsetResult, void* resultAddr)
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
        if (typeFuncBc->structParamByValue(typeParam))
            pushParams[indexParam].type = CPUPushParamType::SwagParamStructValue;
        indexParam += typeParam->numRegisters();
    }

    // Return by parameter
    if (typeFuncBc->returnByAddress())
    {
        if (resultAddr)
            pushParams.insert_at_index({.type = CPUPushParamType::Constant64, .value = reinterpret_cast<uint64_t>(resultAddr)}, indexParam);
        else if (typeFuncBc->returnByStackAddress())
            pushParams.insert_at_index({.type = CPUPushParamType::Load, .baseReg = memRegResult, .value = memOffsetResult}, indexParam);
        else
            pushParams.insert_at_index({.type = CPUPushParamType::LoadAddress, .baseReg = memRegResult, .value = memOffsetResult}, indexParam);
    }

    emitCallParameters(typeFuncBc, pushParams, &typeFuncBc->getCallConv());
}

void SCBE_CPU::emitStoreCallResult(CPUReg memReg, uint32_t memOffset, const TypeInfoFuncAttr* typeFuncBc)
{
    if (!typeFuncBc->returnByValue())
        return;

    SWAG_ASSERT(memReg == CPUReg::RDI || memReg == CPUReg::RSP);

    const auto& cc         = typeFuncBc->getCallConv();
    const auto  returnType = typeFuncBc->concreteReturnType();
    if (returnType->isNativeFloat())
        emitStore(memReg, memOffset, cc.returnByRegisterFloat, OpBits::F64);
    else
        emitStore(memReg, memOffset, cc.returnByRegisterInteger, OpBits::B64);
}

void SCBE_CPU::emitEndProlog()
{
    cpuFct->sizeProlog = concat.totalCount() - cpuFct->startAddress;
}

void SCBE_CPU::emitLoadParam(CPUReg reg, uint32_t paramIdx, OpBits opBits)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitLoad(reg, CPUReg::RSP, stackOffset, opBits);
}

void SCBE_CPU::emitLoadSignedExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitLoadSignedExtend(reg, CPUReg::RSP, stackOffset, numBitsDst, numBitsSrc);
}

void SCBE_CPU::emitLoadZeroExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitLoadZeroExtend(reg, CPUReg::RSP, stackOffset, numBitsDst, numBitsSrc);
}

void SCBE_CPU::emitLoadAddressParam(CPUReg reg, uint32_t paramIdx)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitLoadAddress(reg, CPUReg::RSP, stackOffset);
}

void SCBE_CPU::emitStoreParam(uint32_t paramIdx, CPUReg reg, OpBits opBits)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitStore(CPUReg::RSP, stackOffset, reg, opBits);
}

void SCBE_CPU::emitLoadCallerAddressParam(CPUReg reg, uint32_t paramIdx)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetCallerParam(paramIdx);
    emitLoadAddress(reg, CPUReg::RSP, stackOffset);
}

void SCBE_CPU::emitStoreCallerParam(uint32_t paramIdx, CPUReg reg, OpBits opBits)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetCallerParam(paramIdx);
    emitStore(CPUReg::RSP, stackOffset, reg, opBits);
}

void SCBE_CPU::emitSymbolRelocationPtr(CPUReg reg, const Utf8& name)
{
    SWAG_ASSERT(reg == CPUReg::RAX);

    emitLoad(reg, 0);

    CPURelocation relocation;
    relocation.virtualAddress = concat.totalCount() - sizeof(uint64_t) - textSectionOffset;
    const auto callSym        = getOrAddSymbol(name, CPUSymbolKind::Extern);
    relocation.symbolIndex    = callSym->index;
    relocation.type           = IMAGE_REL_AMD64_ADDR64;
    relocTableTextSection.table.push_back(relocation);
}

void SCBE_CPU::emitJump(CPUCondJump jumpType, uint32_t ipDest)
{
    CPULabelToSolve label;
    label.ipDest = ipDest;

    // Can we solve the label now ?
    const auto it = cpuFct->labels.find(label.ipDest);
    if (it != cpuFct->labels.end())
    {
        const auto currentOffset = static_cast<int32_t>(concat.totalCount()) + 1;
        const int  relOffset     = it->second - (currentOffset + 1);
        if (relOffset >= -127 && relOffset <= 128)
        {
            const auto jump = emitJump(jumpType, OpBits::B8);
            emitPatchJump(jump, it->second);
        }
        else
        {
            const auto jump = emitJump(jumpType, OpBits::B32);
            emitPatchJump(jump, it->second);
        }

        return;
    }

    // Here we do not know the destination label, so we assume 32 bits of offset
    label.jump = emitJump(jumpType, OpBits::B32);
    cpuFct->labelsToSolve.push_back(label);
}

void SCBE_CPU::emitLabels()
{
    for (auto& toSolve : cpuFct->labelsToSolve)
    {
        auto it = cpuFct->labels.find(toSolve.ipDest);
        SWAG_ASSERT(it != cpuFct->labels.end());
        emitPatchJump(toSolve.jump, it->second);
    }
}

void SCBE_CPU::emitDebug(ByteCodeInstruction* ipAddr)
{
    SCBE_Debug::setLocation(cpuFct, ipAddr, concat.totalCount() - cpuFct->startAddress);
}
