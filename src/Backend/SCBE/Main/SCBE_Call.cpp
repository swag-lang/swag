#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"
#include "Wmf/ModuleManager.h"
#include "Wmf/Workspace.h"

void SCBE::emitGetParam(SCBE_X64& pp, uint32_t reg, uint32_t paramIdx, OpBits opBits, uint64_t toAdd, OpBits derefBits)
{
    const auto     cpuFct     = pp.cpuFct;
    const auto     typeFunc   = cpuFct->typeFunc;
    const uint32_t paramStack = SCBE_CPU::getParamStackOffset(cpuFct, paramIdx);
    auto           typeParam  = TypeManager::concreteType(typeFunc->parameters[typeFunc->registerIdxToParamIdx(paramIdx)]->typeInfo);
    if (typeParam->isAutoConstPointerRef())
        typeParam = TypeManager::concretePtrRefType(typeParam);

    switch (opBits)
    {
        case OpBits::B8:
        case OpBits::B16:
        case OpBits::B32:
            SWAG_ASSERT(!toAdd);
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, paramStack, OpBits::B64, opBits, false);
            pp.emitStore(CPUReg::RDI, REG_OFFSET(reg), CPUReg::RAX, OpBits::B64);
            return;
        case OpBits::B64:
            SWAG_ASSERT(toAdd <= 0x7FFFFFFFF);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    const bool structByValue = CallConv::structParamByValue(typeFunc, typeParam);

    if (structByValue)
        pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, paramStack);
    else
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, paramStack, OpBits::B64);

    switch (derefBits)
    {
        case OpBits::B8:
        case OpBits::B16:
        case OpBits::B32:
        case OpBits::B64:
            pp.emitLoad(CPUReg::RAX, CPUReg::RAX, static_cast<uint32_t>(toAdd), OpBits::B64, derefBits, false);
            break;
        default:
            pp.emitOpBinary(CPUReg::RAX, toAdd, CPUOp::ADD, OpBits::B64);
            break;
    }

    pp.emitStore(CPUReg::RDI, REG_OFFSET(reg), CPUReg::RAX, OpBits::B64);
}

void SCBE::emitCall(SCBE_X64& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall)
{
    // Push parameters
    pp.emitCallParameters(typeFuncBc, pushParams, offsetRT);

    if (!localCall)
        pp.emitCallFar(funcName);
    else
        pp.emitCall(funcName);

    // Store result
    pp.emitCallResult(typeFuncBc, offsetRT);

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

void SCBE::emitCall(SCBE_X64& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall)
{
    VectorNative<CPUPushParam> p;
    for (const auto r : pushRAParams)
        p.push_back({.type = CPUPushParamType::Reg, .reg = r});
    emitCall(pp, funcName, typeFuncBc, p, offsetRT, localCall);
}

void SCBE::emitInternalCall(SCBE_X64& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT)
{
    const auto typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    // Invert order
    VectorNative<CPUPushParam> p;
    for (uint32_t i = pushRAParams.size() - 1; i != UINT32_MAX; i--)
        p.push_back({.type = CPUPushParamType::Reg, .reg = pushRAParams[i]});

    emitCall(pp, funcName, typeFunc, p, offsetRT, true);
}

void SCBE::emitInternalCallExt(SCBE_X64& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, const TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc)
        typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    // Invert order
    VectorNative<CPUPushParam> p;
    for (uint32_t i = pushParams.size() - 1; i != UINT32_MAX; i--)
        p.push_back({pushParams[i]});

    emitCall(pp, funcName, typeFunc, p, offsetRT, true);
}

void SCBE::emitByteCodeCall(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBc)
{
    int idxReg = 0;
    for (uint32_t idxParam = typeFuncBc->numReturnRegisters() - 1; idxParam != UINT32_MAX; idxParam--, idxReg++)
    {
        switch (idxReg)
        {
            case 0:
                pp.emitSetAddress(CPUReg::RDX, CPUReg::RDI, pp.offsetRT);
                break;
            case 1:
                pp.emitSetAddress(CPUReg::R8, CPUReg::RDI, pp.offsetRT + sizeof(Register));
                break;
            default:
                break;
        }
    }

    uint32_t stackOffset = typeFuncBc->numReturnRegisters() * sizeof(Register);
    for (uint32_t idxParam = pp.pushRAParams.size() - 1; idxParam != UINT32_MAX; idxParam--, idxReg++)
    {
        static constexpr CPUReg IDX_TO_REG[4] = {CPUReg::RDX, CPUReg::R8, CPUReg::R9};

        // Pass by value
        stackOffset += sizeof(Register);
        if (idxReg <= 2)
        {
            pp.emitLoad(IDX_TO_REG[idxReg], CPUReg::RDI, REG_OFFSET(pp.pushRAParams[idxParam]), OpBits::B64);
        }
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(pp.pushRAParams[idxParam]), OpBits::B64);
            pp.emitStore(CPUReg::RSP, stackOffset, CPUReg::RAX, OpBits::B64);
        }
    }
}

void SCBE::emitByteCodeCallParameters(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBc)
{
    // If the closure is assigned to a lambda, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal function)
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBc->isClosure())
    {
        const auto reg = pp.pushRAParams.back();
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(reg), 0, OpBits::B64);

        // If not zero, jump to closure call
        const auto seekPtrClosure = pp.emitJumpLong(JZ);
        const auto seekJmpClosure = pp.concat.totalCount();

        emitByteCodeCall(pp, typeFuncBc);

        // Jump to after closure call
        const auto seekPtrAfterClosure = pp.emitJumpLong(JUMP);
        const auto seekJmpAfterClosure = pp.concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = static_cast<uint8_t>(pp.concat.totalCount() - seekJmpClosure);

        pp.pushRAParams.pop_back();
        emitByteCodeCall(pp, typeFuncBc);

        *seekPtrAfterClosure = static_cast<uint8_t>(pp.concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emitByteCodeCall(pp, typeFuncBc);
    }
}

void SCBE::emitLocalCall(SCBE_X64& pp)
{
    const auto ip      = pp.ip;
    const auto callBc  = reinterpret_cast<ByteCode*>(ip->a.pointer);
    const auto typeFct = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);
    emitCall(pp, callBc->getCallNameFromDecl(), typeFct, pp.pushRAParams, pp.offsetRT, true);
    pp.pushRAParams.clear();
    pp.pushRVParams.clear();

    if (ip->op == ByteCodeOp::LocalCallPopRC)
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, pp.offsetRT + REG_OFFSET(0), OpBits::B64);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->d.u32), CPUReg::RAX, OpBits::B64);
    }
}

void SCBE::emitForeignCall(SCBE_X64& pp)
{
    const auto ip       = pp.ip;
    const auto funcNode = reinterpret_cast<AstFuncDecl*>(ip->a.pointer);
    const auto typeFct  = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    auto moduleName = ModuleManager::getForeignModuleName(funcNode);

    // Dll imported function name will have "__imp_" before (imported mangled name)
    auto callFuncName = funcNode->getFullNameForeignImport();
    callFuncName      = "__imp_" + callFuncName;

    emitCall(pp, callFuncName, typeFct, pp.pushRAParams, pp.offsetRT, false);
    pp.pushRAParams.clear();
    pp.pushRVParams.clear();
}

void SCBE::emitLambdaCall(SCBE_X64& pp)
{
    const auto ip         = pp.ip;
    const auto typeFuncBC = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    // Test if it's a bytecode lambda
    pp.emitLoad(CPUReg::R10, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
    pp.emitOpBinary(CPUReg::R10, SWAG_LAMBDA_BC_MARKER_BIT, CPUOp::BT, OpBits::B64);
    const auto jumpToBCAddr   = pp.emitJumpLong(JB);
    const auto jumpToBCOffset = pp.concat.totalCount();

    // Native lambda
    //////////////////
    pp.emitCallParameters(typeFuncBC, pp.pushRAParams, pp.offsetRT);
    pp.emitCall(CPUReg::R10);
    pp.emitCallResult(typeFuncBC, pp.offsetRT);

    const auto jumpBCToAfterAddr   = pp.emitJumpLong(JUMP);
    const auto jumpBCToAfterOffset = pp.concat.totalCount();

    // ByteCode lambda
    //////////////////
    *jumpToBCAddr = pp.concat.totalCount() - jumpToBCOffset;

    pp.emitLoad(CPUReg::RCX, CPUReg::R10, OpBits::B64);
    emitByteCodeCallParameters(pp, typeFuncBC);
    pp.emitSymbolRelocationAddr(CPUReg::RAX, pp.symPI_byteCodeRun, 0);
    pp.emitLoad(CPUReg::RAX, CPUReg::RAX, 0, OpBits::B64);
    pp.emitCall(CPUReg::RAX);

    // End
    //////////////////
    *jumpBCToAfterAddr = pp.concat.totalCount() - jumpBCToAfterOffset;

    pp.pushRAParams.clear();
    pp.pushRVParams.clear();
}
