#include "pch.h"
#include "Backend/SCBE/Encoder/ScbeCPU.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Backend/SCBE/Obj/ScbeCoff.h"
#include "Core/Math.h"
#include "Main/CommandLine.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"

void ScbeCPU::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
    optLevel = buildParameters.buildCfg ? buildParameters.buildCfg->backendOptimize : BuildCfgBackendOptim::O0;
}

namespace
{
    CPUSymbol* getSymbol(ScbeCPU& pp, const Utf8& name)
    {
        const auto it = pp.mapSymbols.find(name);
        if (it != pp.mapSymbols.end())
            return &pp.allSymbols[it->second];
        return nullptr;
    }
}

CPUSymbol* ScbeCPU::getOrAddSymbol(const Utf8& name, CPUSymbolKind kind, uint32_t value, uint16_t sectionIdx)
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

void ScbeCPU::addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type)
{
    CPURelocation reloc;
    reloc.virtualAddress = virtualAddr;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = type;
    relocTableTextSection.table.push_back(reloc);
}

CPUSymbol* ScbeCPU::getOrCreateGlobalString(const Utf8& str)
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

void ScbeCPU::emitLabel(uint32_t instructionIndex)
{
    const auto it = cpuFct->labels.find(instructionIndex);
    if (it == cpuFct->labels.end())
        cpuFct->labels[instructionIndex] = static_cast<int32_t>(concat.totalCount());
}

CPUFunction* ScbeCPU::addFunction(const Utf8& funcName, const CallConv* cc, ByteCode* bc)
{
    concat.align(16);

    CPUFunction* cf  = Allocator::alloc<CPUFunction>();
    cf->cc           = cc;
    cf->bc           = bc;
    cf->symbolIndex  = getOrAddSymbol(funcName, CPUSymbolKind::Function, concat.totalCount() - textSectionOffset)->index;
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
        cf->sizeStackCallParams = Math::align(cf->sizeStackCallParams, cc->stackAlign);
    }

    functions.push_back(cf);
    return cf;
}

void ScbeCPU::endFunction() const
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

bool ScbeCPU::isNoOp(uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags) const
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
    void emitParameters(ScbeCPU& pp, const VectorNative<CPUPushParam>& params, const CallConv* callConv)
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
                    pp.emitLoadRM(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CPUPushParamType::LoadAddress:
                    pp.emitLoadAddressM(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value);
                    break;

                case CPUPushParamType::Constant:
                    if (callConv->useRegisterFloat && type->isNativeFloat())
                        pp.emitLoadRI(callConv->paramByRegisterFloat[idxParam], value, BackendEncoder::getOpBitsByBytes(type->sizeOf, true));
                    else
                        pp.emitLoadRI(callConv->paramByRegisterInteger[idxParam], value, OpBits::B64);
                    break;

                case CPUPushParamType::Constant64:
                    pp.emitLoadRI64(callConv->paramByRegisterInteger[idxParam], value);
                    break;

                case CPUPushParamType::CaptureContext:
                    pp.emitLoadRM(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CPUPushParamType::SwagParamStructValue:
                    pp.emitLoadRM(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoadRM(callConv->paramByRegisterInteger[idxParam], CPUReg::RAX, 0, OpBits::B64);
                    break;

                case CPUPushParamType::SwagRegister:
                    if (callConv->useRegisterFloat && type->isNativeFloat())
                        pp.emitLoadRM(callConv->paramByRegisterFloat[idxParam], params[idxParam].baseReg, value, BackendEncoder::getOpBitsByBytes(type->sizeOf, true));
                    else
                        pp.emitLoadRM(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CPUPushParamType::CPURegister:
                    pp.emitLoadRR(callConv->paramByRegisterInteger[idxParam], static_cast<CPUReg>(value), OpBits::B64);
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
                    pp.emitLoadRM(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    if (params[idxParam].value2)
                        pp.emitOpBinaryRI(callConv->paramByRegisterInteger[idxParam], params[idxParam].value2, CPUOp::ADD, OpBits::B64);
                    break;

                case CPUPushParamType::SwagRegisterMul:
                    pp.emitLoadRM(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    if (params[idxParam].value2 != 1)
                        pp.emitOpBinaryRI(CPUReg::RAX, params[idxParam].value2, CPUOp::IMUL, OpBits::B64);
                    pp.emitLoadRR(callConv->paramByRegisterInteger[idxParam], CPUReg::RAX, OpBits::B64);
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
                    pp.emitLoadRM(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitStoreMR(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::LoadAddress:
                    pp.emitLoadAddressM(CPUReg::RAX, params[idxParam].baseReg, value);
                    pp.emitStoreMR(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::Constant:
                    pp.emitStoreMI(CPUReg::RSP, memOffset, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    break;

                case CPUPushParamType::Constant64:
                    pp.emitLoadRI64(CPUReg::RAX, value);
                    pp.emitStoreMR(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::CaptureContext:
                    pp.emitLoadRM(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitStoreMR(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    break;

                case CPUPushParamType::SwagParamStructValue:
                    pp.emitLoadRM(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoadRM(CPUReg::RAX, CPUReg::RAX, 0, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    pp.emitStoreMR(CPUReg::RSP, memOffset, CPUReg::RAX, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    break;

                case CPUPushParamType::SwagRegister:
                    if (type->isStruct())
                    {
                        pp.emitLoadRM(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                        pp.emitStoreMR(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    }
                    else if (type->sizeOf > sizeof(uint64_t))
                    {
                        pp.emitLoadRM(CPUReg::RAX, params[idxParam].baseReg, value, OpBits::B64);
                        pp.emitStoreMR(CPUReg::RSP, memOffset, CPUReg::RAX, OpBits::B64);
                    }
                    else
                    {
                        pp.emitLoadRM(CPUReg::RAX, params[idxParam].baseReg, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                        pp.emitStoreMR(CPUReg::RSP, memOffset, CPUReg::RAX, BackendEncoder::getOpBitsByBytes(type->sizeOf));
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

void ScbeCPU::emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& cpuParams, const CallConv* callConv)
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
        emitCmpMI(cpuParams[idxParamContext].baseReg, cpuParams[idxParamContext].value, 0, OpBits::B64);

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

void ScbeCPU::emitComputeCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& cpuParams, CPUReg memRegResult, uint32_t memOffsetResult, void* resultAddr)
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

void ScbeCPU::emitStoreCallResult(CPUReg memReg, uint32_t memOffset, const TypeInfoFuncAttr* typeFuncBc)
{
    if (!typeFuncBc->returnByValue())
        return;

    const auto& cc         = typeFuncBc->getCallConv();
    const auto  returnType = typeFuncBc->concreteReturnType();
    if (returnType->isNativeFloat())
        emitStoreMR(memReg, memOffset, cc.returnByRegisterFloat, OpBits::F64);
    else
        emitStoreMR(memReg, memOffset, cc.returnByRegisterInteger, OpBits::B64);
}

void ScbeCPU::emitLoadCallerParam(CPUReg reg, uint32_t paramIdx, OpBits opBits)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitLoadRM(reg, CPUReg::RSP, stackOffset, opBits);
}

void ScbeCPU::emitLoadCallerZeroExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitLoadZeroExtendRM(reg, CPUReg::RSP, stackOffset, numBitsDst, numBitsSrc);
}

void ScbeCPU::emitLoadCallerAddressParam(CPUReg reg, uint32_t paramIdx)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetCallerParam(paramIdx);
    emitLoadAddressM(reg, CPUReg::RSP, stackOffset);
}

void ScbeCPU::emitStoreCallerParam(uint32_t paramIdx, CPUReg reg, OpBits opBits)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetCallerParam(paramIdx);
    emitStoreMR(CPUReg::RSP, stackOffset, reg, opBits);
}

void ScbeCPU::emitSymbolRelocationPtr(CPUReg reg, const Utf8& name)
{
    SWAG_ASSERT(reg == CPUReg::RAX);

    emitLoadRI64(reg, 0);

    CPURelocation relocation;
    relocation.virtualAddress = concat.totalCount() - sizeof(uint64_t) - textSectionOffset;
    const auto callSym        = getOrAddSymbol(name, CPUSymbolKind::Extern);
    relocation.symbolIndex    = callSym->index;
    relocation.type           = IMAGE_REL_AMD64_ADDR64;
    relocTableTextSection.table.push_back(relocation);
}

void ScbeCPU::emitJumpCI(CPUCondJump jumpType, uint32_t ipDest)
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

void ScbeCPU::emitLabels()
{
    for (auto& toSolve : cpuFct->labelsToSolve)
    {
        auto it = cpuFct->labels.find(toSolve.ipDest);
        SWAG_ASSERT(it != cpuFct->labels.end());
        emitPatchJump(toSolve.jump, it->second);
    }
}

void ScbeCPU::emitDebug(ByteCodeInstruction* ipAddr)
{
    ScbeDebug::setLocation(cpuFct, ipAddr, concat.totalCount() - cpuFct->startAddress);
}

void ScbeCPU::emitEnter(uint32_t sizeStack)
{
    // Minimal size stack depends on calling convention
    sizeStack = std::max(sizeStack, static_cast<uint32_t>(cpuFct->cc->paramByRegisterCount * sizeof(void*)));
    sizeStack = Math::align(sizeStack, cpuFct->cc->stackAlign);

    // We need to start at sizeof(void*) because the call has pushed one register on the stack
    cpuFct->offsetCallerStackParams = sizeof(void*) + cpuFct->unwindRegs.size() * sizeof(void*);

    // Push all registers
    for (const auto& reg : cpuFct->unwindRegs)
    {
        emitPush(reg);
        cpuFct->unwindOffsetRegs.push_back(concat.totalCount() - cpuFct->startAddress);
    }

    // Be sure that total alignment is respected
    SWAG_ASSERT(!cpuFct->sizeStackCallParams || Math::isAligned(cpuFct->sizeStackCallParams, cpuFct->cc->stackAlign));
    const auto total = cpuFct->offsetCallerStackParams + cpuFct->sizeStackCallParams + sizeStack;
    if (!Math::isAligned(total, cpuFct->cc->stackAlign))
    {
        const auto totalAligned = Math::align(total, cpuFct->cc->stackAlign);
        sizeStack += totalAligned - total;
    }

    cpuFct->offsetCallerStackParams += sizeStack;
    cpuFct->frameSize = sizeStack + cpuFct->sizeStackCallParams;

    // Stack size check
    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        if (cpuFct->frameSize >= SWAG_LIMIT_PAGE_STACK)
        {
            emitLoadRI(CPUReg::RAX, cpuFct->frameSize, OpBits::B64);
            emitCallLocal(R"(__chkstk)");
        }
    }

    emitOpBinaryRI(CPUReg::RSP, cpuFct->frameSize, CPUOp::SUB, OpBits::B64);
    cpuFct->sizeProlog = concat.totalCount() - cpuFct->startAddress;
}

void ScbeCPU::emitLeave()
{
    emitOpBinaryRI(CPUReg::RSP, cpuFct->frameSize, CPUOp::ADD, OpBits::B64);
    for (auto idxReg = cpuFct->unwindRegs.size() - 1; idxReg != UINT32_MAX; idxReg--)
        emitPop(cpuFct->unwindRegs[idxReg]);
    emitRet();
}
