#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"
#include "Wmf/ModuleManager.h"
#include "Wmf/Workspace.h"

void SCBE::emitGetParam(SCBE_X64& pp, const CPUFunction* cpuFct, uint32_t reg, uint32_t paramIdx, int sizeOf, uint64_t toAdd, int deRefSize)
{
    const auto     typeFunc   = cpuFct->typeFunc;
    const auto&    cc         = typeFunc->getCallConv();
    const uint32_t paramStack = SCBE_CPU::getParamStackOffset(cpuFct, paramIdx);
    auto           typeParam  = TypeManager::concreteType(typeFunc->parameters[typeFunc->registerIdxToParamIdx(paramIdx)]->typeInfo);
    if (typeParam->isAutoConstPointerRef())
        typeParam = TypeManager::concretePtrRefType(typeParam);

    switch (sizeOf)
    {
        case 1:
            SWAG_ASSERT(!toAdd);
            if (paramIdx < cpuFct->numScratchRegs)
                pp.emitCastU8U64(RAX, static_cast<CPURegister>(cc.firstScratchRegister + paramIdx));
            else
                pp.emitLoadU8U64Indirect(paramStack, RAX, RDI);
            pp.emitStoreIndirect(REG_OFFSET(reg), RAX, RDI, CPUBits::B64);
            return;
        case 2:
            SWAG_ASSERT(!toAdd);
            if (paramIdx < cpuFct->numScratchRegs)
                pp.emitCastU16U64(RAX, static_cast<CPURegister>(cc.firstScratchRegister + paramIdx));
            else
                pp.emitLoadU16U64Indirect(paramStack, RAX, RDI);
            pp.emitStoreIndirect(REG_OFFSET(reg), RAX, RDI, CPUBits::B64);
            return;
        case 4:
            SWAG_ASSERT(!toAdd);
            if (paramIdx < cpuFct->numScratchRegs)
                pp.emitCopy(RAX, static_cast<CPURegister>(cc.firstScratchRegister + paramIdx), CPUBits::B32);
            else
                pp.emitLoadIndirect(paramStack, RAX, RDI, CPUBits::B32);
            pp.emitStoreIndirect(REG_OFFSET(reg), RAX, RDI, CPUBits::B64);
            return;
        default:
            SWAG_ASSERT(toAdd <= 0x7FFFFFFFF);
            break;
    }

    const bool structByValue = CallConv::structParamByValue(typeFunc, typeParam);

    // Use scratch registers
    if (paramIdx < cpuFct->numScratchRegs && !structByValue)
    {
        const auto scratch = static_cast<CPURegister>(cc.firstScratchRegister + paramIdx);
        if (toAdd == 0 && deRefSize == 0)
        {
            pp.emitStoreIndirect(REG_OFFSET(reg), scratch, RDI, CPUBits::B64);
        }
        else
        {
            switch (deRefSize)
            {
                case 1:
                    pp.emitLoadU8U64Indirect(static_cast<uint32_t>(toAdd), RAX, scratch);
                    break;
                case 2:
                    pp.emitLoadU16U64Indirect(static_cast<uint32_t>(toAdd), RAX, scratch);
                    break;
                case 4:
                    pp.emitLoadIndirect(static_cast<uint32_t>(toAdd), RAX, scratch, CPUBits::B32);
                    break;
                case 8:
                    pp.emitLoadIndirect(static_cast<uint32_t>(toAdd), RAX, scratch, CPUBits::B64);
                    break;
                default:
                    pp.emitLoadAddressIndirect(static_cast<uint32_t>(toAdd), RAX, scratch);
                    break;
            }

            pp.emitStoreIndirect(REG_OFFSET(reg), RAX, RDI, CPUBits::B64);
        }
    }

    // Normal access, use stack
    else
    {
        if (structByValue)
            pp.emitLoadAddressIndirect(paramStack, RAX, RDI);
        else
            pp.emitLoadIndirect(paramStack, RAX, RDI, CPUBits::B64);

        switch (deRefSize)
        {
            case 1:
                pp.emitLoadU8U64Indirect(static_cast<uint32_t>(toAdd), RAX, RAX);
                break;
            case 2:
                pp.emitLoadU16U64Indirect(static_cast<uint32_t>(toAdd), RAX, RAX);
                break;
            case 4:
                pp.emitLoadIndirect(static_cast<uint32_t>(toAdd), RAX, RAX, CPUBits::B32);
                break;
            case 8:
                pp.emitLoadIndirect(static_cast<uint32_t>(toAdd), RAX, RAX, CPUBits::B64);
                break;
            default:
                pp.emitOpImmediate(RAX, toAdd, CPUOp::ADD, CPUBits::B64);
                break;
        }

        pp.emitStoreIndirect(REG_OFFSET(reg), RAX, RDI, CPUBits::B64);
    }
}

void SCBE::emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall)
{
    // Push parameters
    pp.emitCallParameters(typeFunc, pushParams, offsetRT);

    if (!localCall)
        pp.emitCallFar(funcName);
    else
        pp.emitCall(funcName);

    // Store result
    pp.emitCallResult(typeFunc, offsetRT);

    // In case of stack trace, we force a "nop" just after the function call, in order
    // to be sure that there's at least one instruction before the potential next line.
    // Because the stack trace is based on the instruction just after the call, not the
    // call itself. So we want to be sure that the next instruction is at the same line as
    // the call. That's why we add a nop.
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
    {
        if (typeFunc->declNode && typeFunc->declNode->token.sourceFile && typeFunc->declNode->token.sourceFile->module)
        {
            if (typeFunc->declNode->token.sourceFile->module->buildCfg.errorStackTrace)
            {
                pp.emitNop();
            }
        }
    }
}

void SCBE::emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall)
{
    VectorNative<CPUPushParam> p;
    for (const auto r : pushRAParams)
        p.push_back({CPUPushParamType::Reg, r});
    emitCall(pp, typeFunc, funcName, p, offsetRT, localCall);
}

void SCBE::emitInternalCall(SCBE_X64& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT)
{
    const auto typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    // Invert order
    VectorNative<CPUPushParam> p;
    for (uint32_t i = pushRAParams.size() - 1; i != UINT32_MAX; i--)
        p.push_back({CPUPushParamType::Reg, pushRAParams[i]});

    emitCall(pp, typeFunc, funcName, p, offsetRT, true);
}

void SCBE::emitInternalCallExt(SCBE_X64& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc)
        typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    // Invert order
    VectorNative<CPUPushParam> p;
    for (uint32_t i = pushParams.size() - 1; i != UINT32_MAX; i--)
        p.push_back({pushParams[i]});

    emitCall(pp, typeFunc, funcName, p, offsetRT, true);
}

void SCBE::emitByteCodeCall(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    int idxReg = 0;
    for (uint32_t idxParam = typeFuncBc->numReturnRegisters() - 1; idxParam != UINT32_MAX; idxParam--, idxReg++)
    {
        switch (idxReg)
        {
            case 0:
                pp.emitLoadAddressIndirect(offsetRT, RDX, RDI);
                break;
            case 1:
                pp.emitLoadAddressIndirect(offsetRT + sizeof(Register), R8, RDI);
                break;
            default:
                break;
        }
    }

    uint32_t stackOffset = typeFuncBc->numReturnRegisters() * sizeof(Register);
    for (uint32_t idxParam = pushRAParams.size() - 1; idxParam != UINT32_MAX; idxParam--, idxReg++)
    {
        static constexpr CPURegister IDX_TO_REG[4] = {RDX, R8, R9};

        // Pass by value
        stackOffset += sizeof(Register);
        if (idxReg <= 2)
        {
            pp.emitLoadIndirect(REG_OFFSET(pushRAParams[idxParam]), IDX_TO_REG[idxReg], RDI, CPUBits::B64);
        }
        else
        {
            pp.emitLoadIndirect(REG_OFFSET(pushRAParams[idxParam]), RAX, RDI, CPUBits::B64);
            pp.emitStoreIndirect(stackOffset, RAX, RSP, CPUBits::B64);
        }
    }
}

void SCBE::emitByteCodeCallParameters(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    // If the closure is assigned to a lambda, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal function)
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBC->isClosure())
    {
        const auto reg = pushRAParams.back();
        pp.emitLoadIndirect(REG_OFFSET(reg), RAX, RDI, CPUBits::B64);
        pp.emitTest(RAX, RAX, CPUBits::B64);

        // If not zero, jump to closure call
        const auto seekPtrClosure = pp.emitLongJumpOp(JZ);
        const auto seekJmpClosure = pp.concat.totalCount();

        emitByteCodeCall(pp, typeFuncBC, offsetRT, pushRAParams);

        // Jump to after closure call
        const auto seekPtrAfterClosure = pp.emitLongJumpOp(JUMP);
        const auto seekJmpAfterClosure = pp.concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = static_cast<uint8_t>(pp.concat.totalCount() - seekJmpClosure);

        pushRAParams.pop_back();
        emitByteCodeCall(pp, typeFuncBC, offsetRT, pushRAParams);

        *seekPtrAfterClosure = static_cast<uint8_t>(pp.concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emitByteCodeCall(pp, typeFuncBC, offsetRT, pushRAParams);
    }
}

void SCBE::emitLocalCall(SCBE_X64& pp, uint32_t offsetRT, const ByteCodeInstruction* ip, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams)
{
    const auto callBc  = reinterpret_cast<ByteCode*>(ip->a.pointer);
    const auto typeFct = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);
    emitCall(pp, typeFct, callBc->getCallNameFromDecl(), pushRAParams, offsetRT, true);
    pushRAParams.clear();
    pushRVParams.clear();

    if (ip->op == ByteCodeOp::LocalCallPopRC)
    {
        pp.emitLoadIndirect(offsetRT + REG_OFFSET(0), RAX, RDI, CPUBits::B64);
        pp.emitStoreIndirect(REG_OFFSET(ip->d.u32), RAX, RDI, CPUBits::B64);
    }
}

void SCBE::emitForeignCall(SCBE_X64& pp, uint32_t offsetRT, const ByteCodeInstruction* ip, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams)
{
    const auto funcNode = reinterpret_cast<AstFuncDecl*>(ip->a.pointer);
    const auto typeFct  = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    auto moduleName = ModuleManager::getForeignModuleName(funcNode);

    // Dll imported function name will have "__imp_" before (imported mangled name)
    auto callFuncName = funcNode->getFullNameForeignImport();
    callFuncName      = "__imp_" + callFuncName;

    emitCall(pp, typeFct, callFuncName, pushRAParams, offsetRT, false);
    pushRAParams.clear();
    pushRVParams.clear();
}

void SCBE::emitLambdaCall(SCBE_X64& pp, const Concat& concat, uint32_t offsetRT, const ByteCodeInstruction* ip, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams)
{
    const auto typeFuncBC = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    // Test if it's a bytecode lambda
    pp.emitLoadIndirect(REG_OFFSET(ip->a.u32), R10, RDI, CPUBits::B64);
    pp.emitOpImmediate(R10, SWAG_LAMBDA_BC_MARKER_BIT, CPUOp::BT, CPUBits::B64);
    const auto jumpToBCAddr   = pp.emitLongJumpOp(JB);
    const auto jumpToBCOffset = concat.totalCount();

    // Native lambda
    //////////////////
    pp.emitCallParameters(typeFuncBC, pushRAParams, offsetRT);
    pp.emitCallIndirect(R10);
    pp.emitCallResult(typeFuncBC, offsetRT);

    const auto jumpBCToAfterAddr   = pp.emitLongJumpOp(JUMP);
    const auto jumpBCToAfterOffset = concat.totalCount();

    // ByteCode lambda
    //////////////////
    *jumpToBCAddr = concat.totalCount() - jumpToBCOffset;

    pp.emitCopy(RCX, R10, CPUBits::B64);
    emitByteCodeCallParameters(pp, typeFuncBC, offsetRT, pushRAParams);
    pp.emitSymbolRelocationAddr(RAX, pp.symPI_byteCodeRun, 0);
    pp.emitLoadIndirect(0, RAX, RAX, CPUBits::B64);
    pp.emitCallIndirect(RAX);

    // End
    //////////////////
    *jumpBCToAfterAddr = concat.totalCount() - jumpBCToAfterOffset;

    pushRAParams.clear();
    pushRVParams.clear();
}
