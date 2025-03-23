#include "pch.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Backend/SCBE/Obj/ScbeCoff.h"
#include "Core/Math.h"
#include "Main/CommandLine.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"

void ScbeCpu::init(const BuildParameters& buildParameters)
{
    BackendEncoder::init(buildParameters);
    optLevel = buildParameters.buildCfg ? buildParameters.buildCfg->backendOptimize : BuildCfgBackendOptim::O0;
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

bool ScbeCpu::manipulateRegister(ScbeMicroInstruction* inst, CpuReg reg) const
{
    const auto details = getInstructionDetails(inst);
    return details.has(1ULL << static_cast<uint32_t>(reg));
}

void ScbeCpu::maskValue(uint64_t& value, OpBits opBits)
{
    switch (opBits)
    {
        case OpBits::B8:
            value &= 0xFF;
            break;
        case OpBits::B16:
            value &= 0xFFFF;
            break;
        case OpBits::B32:
            value &= 0xFFFFFFFF;
            break;
    }
}

bool ScbeCpu::isNoOp(uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) const
{
    if (emitFlags.has(EMITF_Overflow))
        return false;
    if (optLevel <= BuildCfgBackendOptim::O1)
        return false;

    maskValue(value, opBits);
    switch (value)
    {
        case 0:
            return op == CpuOp::ADD || op == CpuOp::SUB || op == CpuOp::SAR || op == CpuOp::SHL || op == CpuOp::SHR || op == CpuOp::ROL || op == CpuOp::ROR || op == CpuOp::BT;
        case 1:
            return op == CpuOp::MUL || op == CpuOp::IMUL || op == CpuOp::DIV || op == CpuOp::IDIV;
        default:
            break;
    }

    return false;
}

namespace
{
    void emitParameters(ScbeCpu& pp, const VectorNative<CpuPushParam>& params, const CallConv* callConv)
    {
        const uint32_t numParamsPerRegister = std::min(callConv->paramByRegisterInteger.size(), params.size());
        uint32_t       idxParam             = 0;

        // Set the first N parameters. Can be a return register, or a function parameter.
        while (idxParam < numParamsPerRegister)
        {
            const auto type  = params[idxParam].typeInfo ? params[idxParam].typeInfo : g_TypeMgr->typeInfoU64;
            const auto value = params[idxParam].value;

            switch (params[idxParam].type)
            {
                case CpuPushParamType::Load:
                    pp.emitLoadRegMem(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CpuPushParamType::LoadAddress:
                    pp.emitLoadAddressMem(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value);
                    break;

                case CpuPushParamType::Constant:
                    if (callConv->useRegisterFloat && type->isNativeFloat())
                        pp.emitLoadRegImm(callConv->paramByRegisterFloat[idxParam], value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    else
                        pp.emitLoadRegImm(callConv->paramByRegisterInteger[idxParam], value, OpBits::B64);
                    break;

                case CpuPushParamType::Constant64:
                    pp.emitLoadRegImm64(callConv->paramByRegisterInteger[idxParam], value);
                    break;

                case CpuPushParamType::CaptureContext:
                    pp.emitLoadRegMem(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CpuPushParamType::SwagParamStructValue:
                {
                    const auto reg = CallConv::getVolatileRegisterInteger(*callConv, *callConv, VF_EXCLUDE_PARAMS);
                    pp.emitLoadRegMem(reg, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoadRegMem(callConv->paramByRegisterInteger[idxParam], reg, 0, OpBits::B64);
                    break;
                }

                case CpuPushParamType::SwagRegister:
                    if (callConv->useRegisterFloat && type->isNativeFloat())
                        pp.emitLoadRegMem(callConv->paramByRegisterFloat[idxParam], params[idxParam].baseReg, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    else
                        pp.emitLoadRegMem(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CpuPushParamType::CpuRegister:
                    if (callConv->paramByRegisterInteger[idxParam] != params[idxParam].baseReg)
                        pp.emitLoadRegReg(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, OpBits::B64);
                    break;

                case CpuPushParamType::SymbolRelocationValue:
                    SWAG_ASSERT(value < UINT32_MAX);
                    pp.emitSymbolRelocationValue(callConv->paramByRegisterInteger[idxParam], static_cast<uint32_t>(value), 0);
                    break;

                case CpuPushParamType::SymbolRelocationAddress:
                    SWAG_ASSERT(value < UINT32_MAX);
                    pp.emitSymbolRelocationAddress(callConv->paramByRegisterInteger[idxParam], static_cast<uint32_t>(value), 0);
                    break;

                case CpuPushParamType::SwagRegisterAdd:
                    pp.emitLoadRegMem(callConv->paramByRegisterInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    if (params[idxParam].value2)
                        pp.emitOpBinaryRegImm(callConv->paramByRegisterInteger[idxParam], params[idxParam].value2, CpuOp::ADD, OpBits::B64);
                    break;

                case CpuPushParamType::SwagRegisterMul:
                    SWAG_ASSERT(pp.cc->computeRegI0 != callConv->paramByRegisterInteger[idxParam]);
                    pp.emitLoadRegMem(pp.cc->computeRegI0, params[idxParam].baseReg, value, OpBits::B64);
                    if (params[idxParam].value2 != 1)
                        pp.emitOpBinaryRegImm(pp.cc->computeRegI0, params[idxParam].value2, CpuOp::IMUL, OpBits::B64);
                    pp.emitLoadRegReg(callConv->paramByRegisterInteger[idxParam], pp.cc->computeRegI0, OpBits::B64);
                    break;

                case CpuPushParamType::GlobalString:
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
                case CpuPushParamType::Load:
                    pp.emitLoadRegMem(pp.cc->computeRegI0, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoadMemReg(CpuReg::Rsp, memOffset, pp.cc->computeRegI0, OpBits::B64);
                    break;

                case CpuPushParamType::LoadAddress:
                    pp.emitLoadAddressMem(pp.cc->computeRegI0, params[idxParam].baseReg, value);
                    pp.emitLoadMemReg(CpuReg::Rsp, memOffset, pp.cc->computeRegI0, OpBits::B64);
                    break;

                case CpuPushParamType::Constant:
                    pp.emitLoadMemImm(CpuReg::Rsp, memOffset, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    break;

                case CpuPushParamType::Constant64:
                    pp.emitLoadRegImm64(pp.cc->computeRegI0, value);
                    pp.emitLoadMemReg(CpuReg::Rsp, memOffset, pp.cc->computeRegI0, OpBits::B64);
                    break;

                case CpuPushParamType::CaptureContext:
                    pp.emitLoadRegMem(pp.cc->computeRegI0, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoadMemReg(CpuReg::Rsp, memOffset, pp.cc->computeRegI0, OpBits::B64);
                    break;

                case CpuPushParamType::SwagParamStructValue:
                    pp.emitLoadRegMem(pp.cc->computeRegI0, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoadRegMem(pp.cc->computeRegI0, pp.cc->computeRegI0, 0, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    pp.emitLoadMemReg(CpuReg::Rsp, memOffset, pp.cc->computeRegI0, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    break;

                case CpuPushParamType::SwagRegister:
                    if (type->isStruct())
                    {
                        pp.emitLoadRegMem(pp.cc->computeRegI0, params[idxParam].baseReg, value, OpBits::B64);
                        pp.emitLoadMemReg(CpuReg::Rsp, memOffset, pp.cc->computeRegI0, OpBits::B64);
                    }
                    else if (type->sizeOf > sizeof(uint64_t))
                    {
                        pp.emitLoadRegMem(pp.cc->computeRegI0, params[idxParam].baseReg, value, OpBits::B64);
                        pp.emitLoadMemReg(CpuReg::Rsp, memOffset, pp.cc->computeRegI0, OpBits::B64);
                    }
                    else
                    {
                        pp.emitLoadRegMem(pp.cc->computeRegI0, params[idxParam].baseReg, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                        pp.emitLoadMemReg(CpuReg::Rsp, memOffset, pp.cc->computeRegI0, BackendEncoder::getOpBitsByBytes(type->sizeOf));
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

void ScbeCpu::emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CpuPushParam>& cpuParams, const CallConv* callConv)
{
    // If a lambda is assigned to a closure, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal lambda function).
    // But as this is dynamic, we need to have two call paths : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBc && typeFuncBc->isClosure())
    {
        // Get the capture context index, as we will have to remove it when calling a normal lambda
        uint32_t idxParamContext = 0;
        while (idxParamContext < cpuParams.size() && cpuParams[idxParamContext].type != CpuPushParamType::CaptureContext)
            idxParamContext++;
        SWAG_ASSERT(idxParamContext < cpuParams.size());
        SWAG_ASSERT(cpuParams[idxParamContext].type == CpuPushParamType::CaptureContext);

        // First register is closure context, except if variadic, where we have 2 registers for the slice first
        // :VariadicAndClosure
        emitCmpMemImm(cpuParams[idxParamContext].baseReg, cpuParams[idxParamContext].value, 0, OpBits::B64);

        // If zero, jump to parameters for a non closure call
        const auto jumpToNoClosure = emitJump(CpuCondJump::JZ, OpBits::B32);

        // Emit parameters for the closure call (with the pointer to context)
        emitParameters(*this, cpuParams, callConv);
        const auto jumpAfterParameters = emitJump(CpuCondJump::JUMP, OpBits::B32);

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

void ScbeCpu::emitComputeCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CpuPushParam>& cpuParams, CpuReg memRegResult, uint32_t memOffsetResult, void* resultAddr)
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
            pushParams[indexParam].type = CpuPushParamType::SwagParamStructValue;
        indexParam += typeParam->numRegisters();
    }

    // Return by parameter
    if (typeFuncBc->returnByAddress())
    {
        if (resultAddr)
            pushParams.insert_at_index({.type = CpuPushParamType::Constant64, .value = reinterpret_cast<uint64_t>(resultAddr)}, indexParam);
        else if (typeFuncBc->returnByStackAddress())
            pushParams.insert_at_index({.type = CpuPushParamType::Load, .baseReg = memRegResult, .value = memOffsetResult}, indexParam);
        else
            pushParams.insert_at_index({.type = CpuPushParamType::LoadAddress, .baseReg = memRegResult, .value = memOffsetResult}, indexParam);
    }

    emitCallParameters(typeFuncBc, pushParams, &typeFuncBc->getCallConv());
}

void ScbeCpu::emitStoreCallResult(CpuReg memReg, uint32_t memOffset, const TypeInfoFuncAttr* typeFuncBc)
{
    if (!typeFuncBc->returnByValue())
        return;

    const auto& ccFunc     = typeFuncBc->getCallConv();
    const auto  returnType = typeFuncBc->concreteReturnType();
    if (returnType->isNativeFloat())
        emitLoadMemReg(memReg, memOffset, ccFunc.returnByRegisterFloat, OpBits::B64);
    else
        emitLoadMemReg(memReg, memOffset, ccFunc.returnByRegisterInteger, OpBits::B64);
}

void ScbeCpu::emitLoadCallerParam(CpuReg reg, uint32_t paramIdx, OpBits opBits)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitLoadRegMem(reg, CpuReg::Rsp, stackOffset, opBits);
}

void ScbeCpu::emitLoadCallerZeroExtendParam(CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetParam(paramIdx);
    emitLoadZeroExtendRegMem(reg, CpuReg::Rsp, stackOffset, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadCallerAddressParam(CpuReg reg, uint32_t paramIdx)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetCallerParam(paramIdx);
    emitLoadAddressMem(reg, CpuReg::Rsp, stackOffset);
}

void ScbeCpu::emitStoreCallerParam(uint32_t paramIdx, CpuReg reg, OpBits opBits)
{
    const uint32_t stackOffset = cpuFct->getStackOffsetCallerParam(paramIdx);
    emitLoadMemReg(CpuReg::Rsp, stackOffset, reg, opBits);
}

void ScbeCpu::emitSymbolRelocationPtr(CpuReg reg, const Utf8& name)
{
    emitLoadRegImm64(reg, 0);

    CpuRelocation relocation;
    relocation.virtualAddress = concat.totalCount() - sizeof(uint64_t) - textSectionOffset;
    const auto callSym        = getOrAddSymbol(name, CpuSymbolKind::Extern);
    relocation.symbolIndex    = callSym->index;
    relocation.type           = IMAGE_REL_AMD64_ADDR64;
    relocTableTextSection.table.push_back(relocation);
}

void ScbeCpu::emitJumpCondImm(CpuCondJump jumpType, uint32_t ipDest)
{
    CpuLabelToSolve label;
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

void ScbeCpu::emitLabels()
{
    for (auto& toSolve : cpuFct->labelsToSolve)
    {
        auto it = cpuFct->labels.find(toSolve.ipDest);
        SWAG_ASSERT(it != cpuFct->labels.end());
        emitPatchJump(toSolve.jump, it->second);
    }
}

void ScbeCpu::emitDebug(ByteCodeInstruction* ipAddr)
{
    ScbeDebug::setLocation(cpuFct, ipAddr, concat.totalCount() - cpuFct->startAddress);
}

void ScbeCpu::emitEnter(uint32_t sizeStack)
{
    // Minimal size stack depends on calling convention
    sizeStack = std::max(sizeStack, static_cast<uint32_t>(cc->paramByRegisterInteger.size() * sizeof(void*)));
    sizeStack = Math::align(sizeStack, cc->stackAlign);

    // We need to start at sizeof(void*) because the call has pushed one register on the stack
    cpuFct->offsetCallerStackParams = sizeof(void*) + cpuFct->unwindRegs.size() * sizeof(void*);

    // Push all registers
    for (const auto& reg : cpuFct->unwindRegs)
    {
        emitPush(reg);
        cpuFct->unwindOffsetRegs.push_back(concat.totalCount() - cpuFct->startAddress);
    }

    // Be sure that total alignment is respected
    SWAG_ASSERT(!cpuFct->sizeStackCallParams || Math::isAligned(cpuFct->sizeStackCallParams, cc->stackAlign));
    const auto total = cpuFct->offsetCallerStackParams + cpuFct->sizeStackCallParams + sizeStack;
    if (!Math::isAligned(total, cc->stackAlign))
    {
        const auto totalAligned = Math::align(total, cc->stackAlign);
        sizeStack += totalAligned - total;
    }

    cpuFct->offsetCallerStackParams += sizeStack;
    cpuFct->frameSize = sizeStack + cpuFct->sizeStackCallParams;

    // Stack size check
    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        if (cpuFct->frameSize >= SWAG_LIMIT_PAGE_STACK)
        {
            emitLoadRegImm(CpuReg::Rax, cpuFct->frameSize, OpBits::B64);
            emitCallLocal(R"(__chkstk)");
        }
    }

    emitOpBinaryRegImm(CpuReg::Rsp, cpuFct->frameSize, CpuOp::SUB, OpBits::B64);
    cpuFct->sizeProlog = concat.totalCount() - cpuFct->startAddress;
}

void ScbeCpu::emitLeave()
{
    emitOpBinaryRegImm(CpuReg::Rsp, cpuFct->frameSize, CpuOp::ADD, OpBits::B64);
    for (auto idxReg = cpuFct->unwindRegs.size() - 1; idxReg != UINT32_MAX; idxReg--)
        emitPop(cpuFct->unwindRegs[idxReg]);
    emitRet();
}

void ScbeCpu::emitSymbolRelocationRef(const Utf8& name)
{
    encodeSymbolRelocationRef(name);
}

void ScbeCpu::emitSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    encodeSymbolRelocationAddress(reg, symbolIndex, offset);
}

void ScbeCpu::emitSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    encodeSymbolRelocationValue(reg, symbolIndex, offset);
}

void ScbeCpu::emitSymbolGlobalString(CpuReg reg, const Utf8& str)
{
    encodeSymbolGlobalString(reg, str);
}

void ScbeCpu::emitPush(CpuReg reg)
{
    encodePush(reg);
}

void ScbeCpu::emitPop(CpuReg reg)
{
    encodePop(reg);
}

void ScbeCpu::emitNop()
{
    encodeNop();
}

void ScbeCpu::emitRet()
{
    encodeRet();
}

void ScbeCpu::emitCallLocal(const Utf8& symbolName)
{
    encodeCallLocal(symbolName);
}

void ScbeCpu::emitCallExtern(const Utf8& symbolName)
{
    encodeCallExtern(symbolName);
}

void ScbeCpu::emitCallReg(CpuReg reg)
{
    encodeCallReg(reg);
}

void ScbeCpu::emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    encodeJumpTable(table, offset, currentIp, offsetTable, numEntries);
}

CpuJump ScbeCpu::emitJump(CpuCondJump jumpType, OpBits opBits)
{
    return encodeJump(jumpType, opBits);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump)
{
    encodePatchJump(jump);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump, uint64_t offsetDestination)
{
    encodePatchJump(jump, offsetDestination);
}

void ScbeCpu::emitJumpReg(CpuReg reg)
{
    encodeJumpReg(reg);
}

void ScbeCpu::emitLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits)
{
    encodeLoadRegMem(reg, memReg, memOffset, opBits);
}

void ScbeCpu::emitLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits)
{
    encodeLoadRegImm(reg, value, opBits);
}

void ScbeCpu::emitLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits)
{
    encodeLoadRegReg(regDst, regSrc, opBits);
}

void ScbeCpu::emitLoadRegImm64(CpuReg reg, uint64_t value)
{
    encodeLoadRegImm64(reg, value);
}

void ScbeCpu::emitLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    encodeLoadSignedExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    encodeLoadSignedExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    encodeLoadZeroExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    encodeLoadZeroExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    encodeLoadAddressAddMul(regDst, regSrc1, regSrc2, mulValue, opBits);
}

void ScbeCpu::emitLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset)
{
    encodeLoadAddressMem(reg, memReg, memOffset);
}

void ScbeCpu::emitLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    encodeLoadMemReg(memReg, memOffset, reg, opBits);
}

void ScbeCpu::emitLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    encodeLoadMemImm(memReg, memOffset, value, opBits);
}

void ScbeCpu::emitCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits)
{
    encodeCmpRegReg(reg0, reg1, opBits);
}

void ScbeCpu::emitCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    encodeCmpMemReg(memReg, memOffset, reg, opBits);
}

void ScbeCpu::emitCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    encodeCmpMemImm(memReg, memOffset, value, opBits);
}

void ScbeCpu::emitCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits)
{
    encodeCmpRegImm(reg, value, opBits);
}

void ScbeCpu::emitSetCond(CpuReg reg, CpuCondFlag setType)
{
    encodeSetCond(reg, setType);
}

void ScbeCpu::emitClearReg(CpuReg reg, OpBits opBits)
{
    encodeClearReg(reg, opBits);
}

void ScbeCpu::emitClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count)
{
    encodeClearMem(memReg, memOffset, count);
}

void ScbeCpu::emitCopy(CpuReg regDst, CpuReg regSrc, uint32_t count)
{
    encodeCopy(regDst, regSrc, count);
}

void ScbeCpu::emitOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits)
{
    encodeOpUnaryMem(memReg, memOffset, op, opBits);
}

void ScbeCpu::emitOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits)
{
    encodeOpUnaryReg(reg, op, opBits);
}

void ScbeCpu::emitOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpBinaryRegReg(regDst, regSrc, op, opBits, emitFlags);
}

void ScbeCpu::emitOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpBinaryRegMem(regDst, memReg, memOffset, op, opBits, emitFlags);
}

void ScbeCpu::emitOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpBinaryMemReg(memReg, memOffset, reg, op, opBits, emitFlags);
}

void ScbeCpu::emitOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpBinaryRegImm(reg, value, op, opBits, emitFlags);
}

void ScbeCpu::emitOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpBinaryMemImm(memReg, memOffset, value, op, opBits, emitFlags);
}

void ScbeCpu::emitOpMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits)
{
    encodeOpMulAdd(regDst, regMul, regAdd, opBits);
}
