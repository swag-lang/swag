#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Core/Math.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"
#include "Wmf/ModuleManager.h"
#include "Wmf/Workspace.h"

void SCBE::emitGetParam(SCBE_CPU& pp, uint32_t reg, uint32_t paramIdx, OpBits opBits, uint64_t toAdd, OpBits derefBits)
{
    const auto typeFunc  = pp.cpuFct->typeFunc;
    auto       typeParam = TypeManager::concreteType(typeFunc->parameters[typeFunc->registerIdxToParamIdx(paramIdx)]->typeInfo);
    if (typeParam->isAutoConstPointerRef())
        typeParam = TypeManager::concretePtrRefType(typeParam);

    switch (opBits)
    {
        case OpBits::B8:
        case OpBits::B16:
        case OpBits::B32:
        {
            SWAG_ASSERT(!toAdd);
            pp.emitLoadZeroExtendParam(CPUReg::RAX, paramIdx, OpBits::B64, opBits);
            pp.emitStore(CPUReg::RSP, pp.getStackOffsetReg(reg), CPUReg::RAX, OpBits::B64);
            return;
        }
        case OpBits::B64:
            SWAG_ASSERT(toAdd <= 0x7FFFFFFFF);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    if (typeFunc->structParamByValue(typeParam))
        pp.emitLoadAddressParam(CPUReg::RAX, paramIdx);
    else
        pp.emitLoadParam(CPUReg::RAX, paramIdx, OpBits::B64);

    switch (derefBits)
    {
        case OpBits::B8:
        case OpBits::B16:
        case OpBits::B32:
        case OpBits::B64:
            pp.emitLoadZeroExtend(CPUReg::RAX, CPUReg::RAX, static_cast<uint32_t>(toAdd), OpBits::B64, derefBits);
            break;
        default:
            if (toAdd)
                pp.emitOpBinary(CPUReg::RAX, toAdd, CPUOp::ADD, OpBits::B64);
            break;
    }

    pp.emitStore(CPUReg::RSP, pp.getStackOffsetReg(reg), CPUReg::RAX, OpBits::B64);
}

void SCBE::emitCallCPUParams(SCBE_CPU&                         pp,
                             const Utf8&                       funcName,
                             const TypeInfoFuncAttr*           typeFuncBc,
                             const VectorNative<CPUPushParam>& pushCPUParams,
                             CPUReg                            memRegResult,
                             uint32_t                          memOffsetResult,
                             bool                              localCall)
{
    // Push parameters
    pp.emitComputeCallParameters(typeFuncBc, pushCPUParams, memRegResult, memOffsetResult, nullptr);

    if (!localCall)
        pp.emitCallExtern(funcName);
    else
        pp.emitCallLocal(funcName);

    // Store result
    pp.emitStoreCallResult(memRegResult, memOffsetResult, typeFuncBc);

    // In case of stack trace, we force a "nop" just after the function call, in order
    // to be sure that there's at least one instruction before the potential next line.
    // Because the stack trace is based on the instruction just after the call, not the
    // call itself. So we want to be sure that the next instruction is at the same line as
    // the call. That's why we add a nop.
    if (!typeFuncBc->returnType || typeFuncBc->returnType->isVoid())
    {
        if (typeFuncBc->declNode && typeFuncBc->declNode->token.sourceFile && typeFuncBc->declNode->token.sourceFile->module)
        {
            if (typeFuncBc->declNode->token.sourceFile->module->buildCfg.errorStackTrace)
            {
                pp.emitNop();
            }
        }
    }
}

void SCBE::emitCallRAParams(SCBE_CPU& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, bool localCall)
{
    // Invert order
    VectorNative<CPUPushParam> p;
    for (uint32_t i = pp.pushRAParams.size() - 1; i != UINT32_MAX; i--)
        p.push_back({.type = CPUPushParamType::SwagRegister, .baseReg = CPUReg::RSP, .value = pp.getStackOffsetReg(pp.pushRAParams[i])});

    emitCallCPUParams(pp, funcName, typeFuncBc, p, CPUReg::RSP, pp.getStackOffsetRT(0), localCall);
}

void SCBE::emitInternalCallRAParams(SCBE_CPU& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, CPUReg memRegResult, uint32_t memOffsetResult)
{
    const auto typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    VectorNative<CPUPushParam> p;
    for (const auto r : pushRAParams)
        p.push_back({.type = CPUPushParamType::SwagRegister, .baseReg = CPUReg::RSP, .value = pp.getStackOffsetReg(r)});

    emitCallCPUParams(pp, funcName, typeFunc, p, memRegResult, memOffsetResult, true);
}

void SCBE::emitInternalCallCPUParams(SCBE_CPU& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushCPUParams, CPUReg memRegResult, uint32_t memOffsetResult)
{
    auto typeFuncBc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    if (!typeFuncBc)
        typeFuncBc = g_TypeMgr->typeInfoModuleCall;
    SWAG_ASSERT(typeFuncBc);

    emitCallCPUParams(pp, funcName, typeFuncBc, pushCPUParams, memRegResult, memOffsetResult, true);
}

void SCBE::emitLocalCall(SCBE_CPU& pp)
{
    const auto ip      = pp.ip;
    const auto callBc  = reinterpret_cast<ByteCode*>(ip->a.pointer);
    const auto typeFct = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    emitCallRAParams(pp, callBc->getCallNameFromDecl(), typeFct, true);
    pp.pushRAParams.clear();
    pp.pushRVParams.clear();

    if (ip->op == ByteCodeOp::LocalCallPopRC)
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RSP, pp.getStackOffsetRT(0), OpBits::B64);
        pp.emitStore(CPUReg::RSP, pp.getStackOffsetReg(ip->d.u32), CPUReg::RAX, OpBits::B64);
    }
}

void SCBE::emitForeignCall(SCBE_CPU& pp)
{
    const auto ip       = pp.ip;
    const auto funcNode = reinterpret_cast<AstFuncDecl*>(ip->a.pointer);
    const auto typeFct  = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    auto moduleName = ModuleManager::getForeignModuleName(funcNode);

    // Dll imported function name will have "__imp_" before (imported mangled name)
    auto callFuncName = funcNode->getFullNameForeignImport();
    callFuncName      = "__imp_" + callFuncName;

    emitCallRAParams(pp, callFuncName, typeFct, false);
    pp.pushRAParams.clear();
    pp.pushRVParams.clear();
}

void SCBE::emitLambdaCall(SCBE_CPU& pp)
{
    const auto ip         = pp.ip;
    const auto typeFuncBc = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    // Test if it's a bytecode lambda
    pp.emitLoad(CPUReg::R10, CPUReg::RSP, pp.getStackOffsetReg(ip->a.u32), OpBits::B64);
    pp.emitOpBinary(CPUReg::R10, SWAG_LAMBDA_BC_MARKER_BIT, CPUOp::BT, OpBits::B64);
    const auto jumpBC = pp.emitJump(JB, OpBits::B32);

    // Native lambda
    //////////////////

    // Invert order
    VectorNative<CPUPushParam> pushCPUParams;
    for (uint32_t i = pp.pushRAParams.size() - 1; i != UINT32_MAX; i--)
        pushCPUParams.push_back({.type = CPUPushParamType::SwagRegister, .baseReg = CPUReg::RSP, .value = pp.getStackOffsetReg(pp.pushRAParams[i])});

    // Mark the first parameter as the capture context.
    // The "first" parameter has index 2 in case of a variadic function, as the 2 first parameters are the variadic slice
    if (typeFuncBc->isClosure())
        pushCPUParams[typeFuncBc->isFctVariadic() ? 2 : 0].type = CPUPushParamType::CaptureContext;

    pp.emitComputeCallParameters(typeFuncBc, pushCPUParams, CPUReg::RSP, pp.getStackOffsetRT(0), nullptr);
    pp.emitCallIndirect(CPUReg::R10);
    pp.emitStoreCallResult(CPUReg::RSP, pp.getStackOffsetRT(0), typeFuncBc);

    const auto jumpBCAfter = pp.emitJump(JUMP, OpBits::B32);

    // ByteCode lambda
    //////////////////

    pp.emitPatchJump(jumpBC);

    pushCPUParams.insert_at_index({.type = CPUPushParamType::CPURegister, .value = static_cast<uint64_t>(CPUReg::R10)}, 0);
    if (typeFuncBc->numReturnRegisters() >= 1)
        pushCPUParams.insert_at_index({.type = CPUPushParamType::LoadAddress, .baseReg = CPUReg::RSP, .value = pp.getStackOffsetRT(0)}, 1);
    if (typeFuncBc->numReturnRegisters() >= 2)
        pushCPUParams.insert_at_index({.type = CPUPushParamType::LoadAddress, .baseReg = CPUReg::RSP, .value = pp.getStackOffsetRT(1)}, 2);
    pp.emitCallParameters(typeFuncBc, pushCPUParams, CallConv::get(CallConvKind::ByteCode));

    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symPI_byteCodeRun, 0);
    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, OpBits::B64);
    pp.emitCallIndirect(CPUReg::RAX);

    // End
    //////////////////
    pp.emitPatchJump(jumpBCAfter);

    pp.pushRAParams.clear();
    pp.pushRVParams.clear();
}

void SCBE::emitEnter(SCBE_CPU& pp, uint32_t sizeStack)
{
    // Minimal size stack depends on calling convention
    sizeStack = std::max(sizeStack, static_cast<uint32_t>(pp.cpuFct->cc->paramByRegisterCount * sizeof(void*)));
    sizeStack = Math::align(sizeStack, pp.cpuFct->cc->stackAlign);

    // We need to start at sizeof(void*) because the call has pushed one register on the stack
    pp.cpuFct->offsetCallerStackParams = sizeof(void*) + pp.cpuFct->unwindRegs.size() * sizeof(void*);

    // Push all registers
    for (const auto& reg : pp.cpuFct->unwindRegs)
        pp.emitPush(reg);

    // Be sure that total alignment is respected
    SWAG_ASSERT(!pp.cpuFct->sizeStackCallParams || Math::isAligned(pp.cpuFct->sizeStackCallParams, pp.cpuFct->cc->stackAlign));
    const auto total = pp.cpuFct->offsetCallerStackParams + pp.cpuFct->sizeStackCallParams + sizeStack;
    if (!Math::isAligned(total, pp.cpuFct->cc->stackAlign))
    {
        const auto totalAligned = Math::align(total, pp.cpuFct->cc->stackAlign);
        sizeStack += totalAligned - total;
    }

    pp.cpuFct->offsetCallerStackParams += sizeStack;
    pp.cpuFct->frameSize = sizeStack + pp.cpuFct->sizeStackCallParams;

    // Stack size check
    if (g_CommandLine.target.os == SwagTargetOs::Windows)
    {
        if (pp.cpuFct->frameSize >= SWAG_LIMIT_PAGE_STACK)
        {
            pp.emitLoad(CPUReg::RAX, pp.cpuFct->frameSize, OpBits::B64);
            pp.emitCallLocal(R"(__chkstk)");
        }
    }

    pp.emitOpBinary(CPUReg::RSP, pp.cpuFct->frameSize, CPUOp::SUB, OpBits::B64);
    pp.emitEndProlog();
}

void SCBE::emitLeave(SCBE_CPU& pp)
{
    pp.emitOpBinary(CPUReg::RSP, pp.cpuFct->frameSize, CPUOp::ADD, OpBits::B64);
    for (auto idxReg = pp.cpuFct->unwindRegs.size() - 1; idxReg != UINT32_MAX; idxReg--)
        pp.emitPop(pp.cpuFct->unwindRegs[idxReg]);
    pp.emitRet();
}
