#include "pch.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Core/Math.h"
#include "Main/CommandLine.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"

namespace
{
    void emitParameters(ScbeCpu& pp, const VectorNative<CpuPushParam>& params, const CallConv* callConv)
    {
        const uint32_t numParamsPerRegister = std::min(callConv->paramsRegistersInteger.size(), params.size());
        uint32_t       idxParam             = 0;

        // Set the first N parameters. Can be a return register, or a function parameter.
        while (idxParam < numParamsPerRegister)
        {
            const auto type  = params[idxParam].typeInfo ? params[idxParam].typeInfo : g_TypeMgr->typeInfoU64;
            const auto value = params[idxParam].value;

            switch (params[idxParam].type)
            {
                case CpuPushParamType::Load:
                    pp.emitLoadRegMem(callConv->paramsRegistersInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CpuPushParamType::LoadAddress:
                    pp.emitLoadAddressMem(callConv->paramsRegistersInteger[idxParam], params[idxParam].baseReg, value);
                    break;

                case CpuPushParamType::Constant:
                    if (callConv->useRegisterFloat && type->isNativeFloat())
                        pp.emitLoadRegImm(callConv->paramsRegistersFloat[idxParam], value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    else
                        pp.emitLoadRegImm(callConv->paramsRegistersInteger[idxParam], value, OpBits::B64);
                    break;

                case CpuPushParamType::Constant64:
                    pp.emitLoadRegImm(callConv->paramsRegistersInteger[idxParam], value, OpBits::B64);
                    break;

                case CpuPushParamType::CaptureContext:
                    pp.emitLoadRegMem(callConv->paramsRegistersInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CpuPushParamType::SwagParamStructValue:
                {
                    const auto reg = CallConv::getVolatileRegisterInteger(callConv, callConv, VF_EXCLUDE_PARAMS);
                    pp.emitLoadRegMem(reg, params[idxParam].baseReg, value, OpBits::B64);
                    pp.emitLoadRegMem(callConv->paramsRegistersInteger[idxParam], reg, 0, OpBits::B64);
                    break;
                }

                case CpuPushParamType::SwagRegister:
                    if (callConv->useRegisterFloat && type->isNativeFloat())
                        pp.emitLoadRegMem(callConv->paramsRegistersFloat[idxParam], params[idxParam].baseReg, value, BackendEncoder::getOpBitsByBytes(type->sizeOf));
                    else
                        pp.emitLoadRegMem(callConv->paramsRegistersInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    break;

                case CpuPushParamType::CpuRegister:
                    if (callConv->paramsRegistersInteger[idxParam] != params[idxParam].baseReg)
                        pp.emitLoadRegReg(callConv->paramsRegistersInteger[idxParam], params[idxParam].baseReg, OpBits::B64);
                    break;

                case CpuPushParamType::SymbolRelocValue:
                    SWAG_ASSERT(value < UINT32_MAX);
                    pp.emitLoadSymbolRelocValue(callConv->paramsRegistersInteger[idxParam], static_cast<uint32_t>(value), 0);
                    break;

                case CpuPushParamType::SymbolRelocAddr:
                    SWAG_ASSERT(value < UINT32_MAX);
                    pp.emitLoadSymRelocAddress(callConv->paramsRegistersInteger[idxParam], static_cast<uint32_t>(value), 0);
                    break;

                case CpuPushParamType::SwagRegisterAdd:
                    pp.emitLoadRegMem(callConv->paramsRegistersInteger[idxParam], params[idxParam].baseReg, value, OpBits::B64);
                    if (params[idxParam].value2)
                        pp.emitOpBinaryRegImm(callConv->paramsRegistersInteger[idxParam], params[idxParam].value2, CpuOp::ADD, OpBits::B64);
                    break;

                case CpuPushParamType::SwagRegisterMul:
                    SWAG_ASSERT(pp.cc->computeRegI0 != callConv->paramsRegistersInteger[idxParam]);
                    pp.emitLoadRegMem(pp.cc->computeRegI0, params[idxParam].baseReg, value, OpBits::B64);
                    if (params[idxParam].value2 != 1)
                        pp.emitOpBinaryRegImm(pp.cc->computeRegI0, params[idxParam].value2, CpuOp::IMUL, OpBits::B64);
                    pp.emitLoadRegReg(callConv->paramsRegistersInteger[idxParam], pp.cc->computeRegI0, OpBits::B64);
                    break;

                case CpuPushParamType::GlobalString:
                {
                    const auto sym = pp.getOrCreateGlobalString(reinterpret_cast<const char*>(value));
                    pp.emitLoadSymbolRelocValue(callConv->paramsRegistersInteger[idxParam], sym->index, 0, EMIT_B64);
                    break;
                }

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
                    pp.emitLoadRegImm(pp.cc->computeRegI0, value, OpBits::B64);
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
        CpuJump jumpToNoClosure;
        emitJump(jumpToNoClosure, CpuCondJump::JZ, OpBits::B32);

        // Emit parameters for the closure call (with the pointer to context)
        emitParameters(*this, cpuParams, callConv);
        CpuJump jumpAfterParameters;
        emitJump(jumpAfterParameters, CpuCondJump::JUMP, OpBits::B32);

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

    emitCallParameters(typeFuncBc, pushParams, typeFuncBc->getCallConv());
}

void ScbeCpu::emitStoreCallResult(CpuReg memReg, uint32_t memOffset, const TypeInfoFuncAttr* typeFuncBc)
{
    if (!typeFuncBc->returnByValue())
        return;

    const auto ccFunc     = typeFuncBc->getCallConv();
    const auto returnType = typeFuncBc->concreteReturnType();
    if (returnType->isNativeFloat())
        emitLoadMemReg(memReg, memOffset, ccFunc->returnByRegisterFloat, OpBits::B64);
    else
        emitLoadMemReg(memReg, memOffset, ccFunc->returnByRegisterInteger, OpBits::B64);
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
            CpuJump jump;
            emitJump(jump, jumpType, OpBits::B8);
            emitPatchJump(jump, it->second);
        }
        else
        {
            CpuJump jump;
            emitJump(jump, jumpType, OpBits::B32);
            emitPatchJump(jump, it->second);
        }

        return;
    }

    // Here we do not know the destination label, so we assume 32 bits of offset
    emitJump(label.jump, jumpType, OpBits::B32);
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
    sizeStack = std::max(sizeStack, static_cast<uint32_t>(cc->paramsRegistersInteger.size() * sizeof(void*)));
    sizeStack = Math::align(sizeStack, cc->stackAlign);

    // We need to start at sizeof(void*) because the call has pushed one register on the stack
    cpuFct->offsetCallerStackParams = sizeof(void*) + cpuFct->unwindRegs.size() * sizeof(void*);

    // Push all registers
    for (const auto& reg : cpuFct->unwindRegs)
    {
#ifdef SWAG_STATS
        g_Stats.numScbeInstructions += 1;
#endif
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
            emitCallLocal(R"(__chkstk)", CallConv::get(CallConvKind::Swag));
#ifdef SWAG_STATS
            g_Stats.numScbeInstructions += 2;
#endif
        }
    }

    if (!cpuFct->noStackFrame)
    {
        emitOpBinaryRegImm(CpuReg::Rsp, cpuFct->frameSize, CpuOp::SUB, OpBits::B64);
#ifdef SWAG_STATS
        g_Stats.numScbeInstructions += 1;
#endif
    }

    cpuFct->sizeProlog = concat.totalCount() - cpuFct->startAddress;
}

void ScbeCpu::emitLeave()
{
    if (!cpuFct->noStackFrame)
    {
        emitOpBinaryRegImm(CpuReg::Rsp, cpuFct->frameSize, CpuOp::ADD, OpBits::B64);
#ifdef SWAG_STATS
        g_Stats.numScbeInstructions += 2;
#endif
    }

    for (auto idxReg = cpuFct->unwindRegs.size() - 1; idxReg != UINT32_MAX; idxReg--)
    {
        emitPop(cpuFct->unwindRegs[idxReg]);
#ifdef SWAG_STATS
        g_Stats.numScbeInstructions += 1;
#endif
    }

    emitRet();
}
