#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"
#include "Wmf/ModuleManager.h"
#include "Wmf/Workspace.h"

void Scbe::emitLoadParam(ScbeCpu& pp, CpuReg reg, uint32_t paramIdx, OpBits opBits)
{
    if (paramIdx < pp.cc->paramByRegisterInteger.size())
        pp.emitLoadRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetParam(paramIdx), opBits);
    else
        pp.emitLoadCallerParam(reg, paramIdx, opBits);
}

void Scbe::emitLoadZeroExtendParam(ScbeCpu& pp, CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (paramIdx < pp.cc->paramByRegisterInteger.size())
        pp.emitLoadZeroExtendRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetParam(paramIdx), numBitsDst, numBitsSrc);
    else
        pp.emitLoadCallerZeroExtendParam(reg, paramIdx, numBitsDst, numBitsSrc);
}

void Scbe::emitLoadAddressParam(ScbeCpu& pp, CpuReg reg, uint32_t paramIdx)
{
    if (paramIdx < pp.cc->paramByRegisterInteger.size())
        pp.emitLoadAddressMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetParam(paramIdx));
    else
        pp.emitLoadCallerAddressParam(reg, paramIdx);
}

void Scbe::emitGetParam(ScbeCpu& pp, uint32_t reg, uint32_t paramIdx, OpBits opBits, uint64_t toAdd, OpBits derefBits)
{
    const auto cc        = pp.cc;
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
            emitLoadZeroExtendParam(pp, cc->computeRegI0, paramIdx, OpBits::B64, opBits);
            pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), cc->computeRegI0, OpBits::B64);
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
        emitLoadAddressParam(pp, cc->computeRegI0, paramIdx);
    else
        emitLoadParam(pp, cc->computeRegI0, paramIdx, OpBits::B64);

    switch (derefBits)
    {
        case OpBits::B8:
        case OpBits::B16:
        case OpBits::B32:
        case OpBits::B64:
            pp.emitLoadZeroExtendRegMem(cc->computeRegI0, cc->computeRegI0, static_cast<uint32_t>(toAdd), OpBits::B64, derefBits);
            break;
        default:
            if (toAdd)
                pp.emitOpBinaryRegImm(cc->computeRegI0, toAdd, CpuOp::ADD, OpBits::B64);
            break;
    }

    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), cc->computeRegI0, OpBits::B64);
}

void Scbe::emitCallCPUParams(ScbeCpu&                          pp,
                             const Utf8&                       funcName,
                             const TypeInfoFuncAttr*           typeFuncBc,
                             const VectorNative<CpuPushParam>& pushCPUParams,
                             CpuReg                            memRegResult,
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

void Scbe::emitCallRAParams(ScbeCpu& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, bool localCall)
{
    // Invert order
    VectorNative<CpuPushParam> p;
    for (uint32_t i = pp.pushRAParams.size() - 1; i != UINT32_MAX; i--)
        p.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(pp.pushRAParams[i])});

    emitCallCPUParams(pp, funcName, typeFuncBc, p, CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), localCall);
}

void Scbe::emitInternalCallRAParams(ScbeCpu& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, CpuReg memRegResult, uint32_t memOffsetResult)
{
    const auto typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    VectorNative<CpuPushParam> p;
    for (const auto r : pushRAParams)
        p.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(r)});

    emitCallCPUParams(pp, funcName, typeFunc, p, memRegResult, memOffsetResult, true);
}

void Scbe::emitInternalCallCPUParams(ScbeCpu& pp, const Utf8& funcName, const VectorNative<CpuPushParam>& pushCPUParams, CpuReg memRegResult, uint32_t memOffsetResult)
{
    auto typeFuncBc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    if (!typeFuncBc)
        typeFuncBc = g_TypeMgr->typeInfoModuleCall;
    SWAG_ASSERT(typeFuncBc);

    emitCallCPUParams(pp, funcName, typeFuncBc, pushCPUParams, memRegResult, memOffsetResult, true);
}

void Scbe::emitLocalCall(ScbeCpu& pp)
{
    const auto cc      = pp.cc;
    const auto ip      = pp.ip;
    const auto callBc  = reinterpret_cast<ByteCode*>(ip->a.pointer);
    const auto typeFct = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    emitCallRAParams(pp, callBc->getCallNameFromDecl(), typeFct, true);
    pp.pushRAParams.clear();
    pp.pushRVParams.clear();

    if (ip->op == ByteCodeOp::LocalCallPopRC)
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), OpBits::B64);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32), cc->computeRegI0, OpBits::B64);
    }
}

void Scbe::emitForeignCall(ScbeCpu& pp)
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

void Scbe::emitLambdaCall(ScbeCpu& pp)
{
    const auto ip         = pp.ip;
    const auto typeFuncBc = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    const auto regCall = CallConv::getVolatileRegisterInteger(*pp.cc, typeFuncBc->getCallConv(), VF_EXCLUDE_RETURN | VF_EXCLUDE_PARAMS);

    // Test if it's a bytecode lambda
    pp.emitLoadRegMem(regCall, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
    pp.emitOpBinaryRegImm(regCall, SWAG_LAMBDA_BC_MARKER_BIT, CpuOp::BT, OpBits::B64);
    CpuJump jumpBC;
    pp.emitJump(jumpBC, CpuCondJump::JB, OpBits::B32);

    // Native lambda
    //////////////////

    // Invert order
    VectorNative<CpuPushParam> pushCPUParams;
    for (uint32_t i = pp.pushRAParams.size() - 1; i != UINT32_MAX; i--)
        pushCPUParams.push_back({.type = CpuPushParamType::SwagRegister, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetReg(pp.pushRAParams[i])});

    // Mark the first parameter as the capture context.
    // The "first" parameter has index 2 in case of a variadic function, as the 2 first parameters are the variadic slice
    if (typeFuncBc->isClosure())
        pushCPUParams[typeFuncBc->isFctVariadic() ? 2 : 0].type = CpuPushParamType::CaptureContext;

    pp.emitComputeCallParameters(typeFuncBc, pushCPUParams, CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), nullptr);
    pp.emitCallReg(regCall);
    pp.emitStoreCallResult(CpuReg::Rsp, pp.cpuFct->getStackOffsetRT(0), typeFuncBc);

    CpuJump jumpBCAfter;
    pp.emitJump(jumpBCAfter, CpuCondJump::JUMP, OpBits::B32);

    // ByteCode lambda
    //////////////////

    pp.emitPatchJump(jumpBC);

    pushCPUParams.insert_at_index({.type = CpuPushParamType::CpuRegister, .baseReg = regCall}, 0);
    if (typeFuncBc->numReturnRegisters() >= 1)
        pushCPUParams.insert_at_index({.type = CpuPushParamType::LoadAddress, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetRT(0)}, 1);
    if (typeFuncBc->numReturnRegisters() >= 2)
        pushCPUParams.insert_at_index({.type = CpuPushParamType::LoadAddress, .baseReg = CpuReg::Rsp, .value = pp.cpuFct->getStackOffsetRT(1)}, 2);
    pp.emitCallParameters(typeFuncBc, pushCPUParams, CallConv::get(CallConvKind::Compiler));

    const auto cc = pp.cc;
    pp.emitSymbolRelocationAddress(cc->computeRegI0, pp.symPI_byteCodeRun, 0);
    pp.emitLoadRegMem(cc->computeRegI0, cc->computeRegI0, 0, OpBits::B64);
    pp.emitCallReg(cc->computeRegI0);

    // End
    //////////////////
    pp.emitPatchJump(jumpBCAfter);

    pp.pushRAParams.clear();
    pp.pushRVParams.clear();
}

void Scbe::emitMakeLambda(ScbeCpu& pp)
{
    const auto cc       = pp.cc;
    const auto funcNode = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(pp.ip->b.pointer), AstNodeKind::FuncDecl);
    SWAG_ASSERT(!pp.ip->c.pointer || (funcNode && funcNode->hasExtByteCode() && funcNode->extByteCode()->bc == reinterpret_cast<ByteCode*>(pp.ip->c.pointer)));
    pp.emitSymbolRelocationPtr(cc->computeRegI0, funcNode->getCallName());
    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), cc->computeRegI0, OpBits::B64);
}

void Scbe::emitMakeCallback(ScbeCpu& pp)
{
    const auto cc = pp.cc;

    // Test if it's a bytecode lambda
    pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), OpBits::B64);
    pp.emitLoadRegImm(cc->computeRegI1, SWAG_LAMBDA_BC_MARKER, OpBits::B64);
    pp.emitOpBinaryRegReg(cc->computeRegI1, cc->computeRegI0, CpuOp::AND, OpBits::B64);

    CpuJump jump;
    pp.emitJump(jump, CpuCondJump::JZ, OpBits::B32);

    // ByteCode lambda
    //////////////////

    const auto regCall = CallConv::getVolatileRegisterInteger(*cc, *cc, VF_EXCLUDE_COMPUTE_I0 | VF_EXCLUDE_PARAMS);
    pp.emitSymbolRelocationAddress(regCall, pp.symPI_makeCallback, 0);
    pp.emitLoadRegMem(regCall, regCall, 0, OpBits::B64);

    VectorNative<CpuPushParam> pushCPUParams;
    pushCPUParams.insert_at_index({.type = CpuPushParamType::CpuRegister, .baseReg = cc->computeRegI0}, 0);
    pp.emitCallParameters(nullptr, pushCPUParams, CallConv::get(CallConvKind::Compiler));

    pp.emitCallReg(regCall);

    // End
    //////////////////
    pp.emitPatchJump(jump);
    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), cc->computeRegI0, OpBits::B64);
}
