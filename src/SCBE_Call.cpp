#include "pch.h"
#include "Module.h"
#include "SCBE.h"
#include "TypeManager.h"
#include "Workspace.h"

void SCBE::emitGetParam(SCBE_X64& pp, const CPUFunction* cpuFct, int reg, uint32_t paramIdx, int sizeOf, uint64_t toAdd, int deRefSize)
{
    const auto     typeFunc   = cpuFct->typeFunc;
    const auto&    cc         = typeFunc->getCallConv();
    const uint32_t paramStack = SCBE_X64::getParamStackOffset(cpuFct, paramIdx);
    auto           typeParam  = TypeManager::concreteType(typeFunc->parameters[typeFunc->registerIdxToParamIdx(paramIdx)]->typeInfo);
    if (typeParam->isAutoConstPointerRef())
        typeParam = TypeManager::concretePtrRefType(typeParam);

    switch (sizeOf)
    {
    case 1:
        SWAG_ASSERT(!toAdd);
        if (paramIdx < cpuFct->numScratchRegs)
            pp.emit_Extend_U8U64(RAX, (CPURegister) (cc.firstScratchRegister + paramIdx));
        else
            pp.emit_LoadU8U64_Indirect(paramStack, RAX, RDI);
        pp.emit_Store64_Indirect(REG_OFFSET(reg), RAX);
        return;
    case 2:
        SWAG_ASSERT(!toAdd);
        if (paramIdx < cpuFct->numScratchRegs)
            pp.emit_Extend_U16U64(RAX, (CPURegister) (cc.firstScratchRegister + paramIdx));
        else
            pp.emit_LoadU16U64_Indirect(paramStack, RAX, RDI);
        pp.emit_Store64_Indirect(REG_OFFSET(reg), RAX);
        return;
    case 4:
        SWAG_ASSERT(!toAdd);
        if (paramIdx < cpuFct->numScratchRegs)
            pp.emit_CopyN(RAX, (CPURegister) (cc.firstScratchRegister + paramIdx), CPUBits::B32);
        else
            pp.emit_Load32_Indirect(paramStack, RAX, RDI);
        pp.emit_Store64_Indirect(REG_OFFSET(reg), RAX);
        return;
    default:
        break;
    }

    SWAG_ASSERT(toAdd <= 0x7FFFFFFFF);

    const bool structByValue = CallConv::structParamByValue(typeFunc, typeParam);

    // Use scratch registers
    if (paramIdx < cpuFct->numScratchRegs && !structByValue)
    {
        const auto scratch = (CPURegister) (cc.firstScratchRegister + paramIdx);
        if (toAdd == 0 && deRefSize == 0)
        {
            pp.emit_Store64_Indirect(REG_OFFSET(reg), scratch);
        }
        else
        {
            switch (deRefSize)
            {
            case 1:
                pp.emit_LoadU8U64_Indirect((uint32_t) toAdd, RAX, scratch);
                break;
            case 2:
                pp.emit_LoadU16U64_Indirect((uint32_t) toAdd, RAX, scratch);
                break;
            case 4:
                pp.emit_Load32_Indirect((uint32_t) toAdd, RAX, scratch);
                break;
            case 8:
                pp.emit_Load64_Indirect((uint32_t) toAdd, RAX, scratch);
                break;
            default:
                pp.emit_LoadAddress_Indirect((uint32_t) toAdd, RAX, scratch);
                break;
            }

            pp.emit_Store64_Indirect(REG_OFFSET(reg), RAX);
        }
    }

    // Normal access, use stack
    else
    {
        if (structByValue)
            pp.emit_LoadAddress_Indirect(paramStack, RAX, RDI);
        else
            pp.emit_Load64_Indirect(paramStack, RAX, RDI);

        switch (deRefSize)
        {
        case 1:
            pp.emit_LoadU8U64_Indirect((uint32_t) toAdd, RAX, RAX);
            break;
        case 2:
            pp.emit_LoadU16U64_Indirect((uint32_t) toAdd, RAX, RAX);
            break;
        case 4:
            pp.emit_Load32_Indirect((uint32_t) toAdd, RAX, RAX);
            break;
        case 8:
            pp.emit_Load64_Indirect((uint32_t) toAdd, RAX, RAX);
            break;
        default:
            pp.emit_OpN_Immediate(RAX, toAdd, CPUOp::ADD, CPUBits::B64);
            break;
        }

        pp.emit_Store64_Indirect(REG_OFFSET(reg), RAX);
    }
}

void SCBE::emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall)
{
    // Push parameters
    pp.emit_Call_Parameters(typeFunc, pushParams, offsetRT);

    if (!localCall)
        pp.emit_Call_Far(funcName);
    else
        pp.emit_Call(funcName);

    // Store result
    pp.emit_Call_Result(typeFunc, offsetRT);

    // In case of stack trace, we force a "nop" just after the function call, in order
    // to be sure that there's at least one instruction before the potential next line.
    // Because the stack trace is based on the instruction just after the call, not the
    // call itself. So we want to be sure that the next instruction is at the same line as
    // the call. That's why we add a nop.
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
    {
        if (typeFunc->declNode && typeFunc->declNode->sourceFile && typeFunc->declNode->sourceFile->module)
        {
            if (typeFunc->declNode->sourceFile->module->buildCfg.errorStackTrace)
            {
                pp.emit_Nop();
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

void SCBE::emitInternalCall(SCBE_X64& pp, Module*, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT)
{
    const auto typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    // Invert order
    VectorNative<CPUPushParam> p;
    for (int i = (int) pushRAParams.size() - 1; i >= 0; i--)
        p.push_back({CPUPushParamType::Reg, pushRAParams[i]});

    emitCall(pp, typeFunc, funcName, p, offsetRT, true);
}

void SCBE::emitInternalCallExt(SCBE_X64& pp, Module*, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc)
        typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    // Invert order
    VectorNative<CPUPushParam> p;
    for (int i = (int) pushParams.size() - 1; i >= 0; i--)
        p.push_back({pushParams[i]});

    emitCall(pp, typeFunc, funcName, p, offsetRT, true);
}

void SCBE::emitByteCodeCall(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    int idxReg = 0;
    for (int idxParam = typeFuncBc->numReturnRegisters() - 1; idxParam >= 0; idxParam--, idxReg++)
    {
        switch (idxReg)
        {
        case 0:
            pp.emit_LoadAddress_Indirect(offsetRT, RDX, RDI);
            break;
        case 1:
            pp.emit_LoadAddress_Indirect(offsetRT + sizeof(Register), R8, RDI);
            break;
        default:
            break;
        }
    }

    uint32_t stackOffset = typeFuncBc->numReturnRegisters() * sizeof(Register);
    for (int idxParam = (int) pushRAParams.size() - 1; idxParam >= 0; idxParam--, idxReg++)
    {
        static constexpr CPURegister IDX_TO_REG[4] = {RDX, R8, R9};

        // Pass by value
        stackOffset += sizeof(Register);
        if (idxReg <= 2)
        {
            pp.emit_Load64_Indirect(REG_OFFSET(pushRAParams[idxParam]), IDX_TO_REG[idxReg]);
        }
        else
        {
            pp.emit_Load64_Indirect(REG_OFFSET(pushRAParams[idxParam]), RAX);
            pp.emit_Store64_Indirect(stackOffset, RAX, RSP);
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
        pp.emit_Load64_Indirect(REG_OFFSET(reg), RAX);
        pp.emit_TestN(RAX, RAX, CPUBits::B64);

        // If not zero, jump to closure call
        pp.emit_LongJumpOp(JZ);
        pp.concat.addU32(0);
        const auto seekPtrClosure = pp.concat.getSeekPtr() - 4;
        const auto seekJmpClosure = pp.concat.totalCount();

        emitByteCodeCall(pp, typeFuncBC, offsetRT, pushRAParams);

        // Jump to after closure call
        pp.emit_LongJumpOp(JUMP);
        pp.concat.addU32(0);
        const auto seekPtrAfterClosure = pp.concat.getSeekPtr() - 4;
        const auto seekJmpAfterClosure = pp.concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = (uint8_t) (pp.concat.totalCount() - seekJmpClosure);

        pushRAParams.pop_back();
        emitByteCodeCall(pp, typeFuncBC, offsetRT, pushRAParams);

        *seekPtrAfterClosure = (uint8_t) (pp.concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emitByteCodeCall(pp, typeFuncBC, offsetRT, pushRAParams);
    }
}
