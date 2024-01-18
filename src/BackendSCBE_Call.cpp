#include "pch.h"
#include "BackendSCBE.h"
#include "TypeManager.h"
#include "Module.h"
#include "Workspace.h"

uint32_t BackendSCBE::getParamStackOffset(CoffFunction* coffFct, int paramIdx)
{
    const auto& cc = coffFct->typeFunc->getCallConv();

    // If this was passed as a register, then get the value from storeS4 (where input registers have been saved)
    // instead of value from the stack
    if (paramIdx < (int) cc.paramByRegisterCount)
        return REG_OFFSET(paramIdx) + coffFct->offsetLocalStackParams;

    // Value from the caller stack
    else
        return REG_OFFSET(paramIdx) + coffFct->offsetCallerStackParams;
}

void BackendSCBE::emitGetParam(EncoderX64& pp, CoffFunction* coffFct, int reg, uint32_t paramIdx, int sizeOf, uint64_t toAdd, int deRefSize)
{
    auto        typeFunc   = coffFct->typeFunc;
    const auto& cc         = typeFunc->getCallConv();
    int         paramStack = getParamStackOffset(coffFct, paramIdx);
    auto        typeParam  = TypeManager::concreteType(typeFunc->parameters[typeFunc->registerIdxToParamIdx(paramIdx)]->typeInfo);
    if (typeParam->isAutoConstPointerRef())
        typeParam = TypeManager::concretePtrRefType(typeParam);

    switch (sizeOf)
    {
    case 1:
        SWAG_ASSERT(!toAdd);
        if (paramIdx < coffFct->numScratchRegs)
            pp.emit_Extend_U8U64((CPURegister) (cc.firstScratchRegister + paramIdx), RAX);
        else
            pp.emit_LoadU8U64_Indirect(paramStack, RAX, RDI);
        pp.emit_Store64_Indirect(REG_OFFSET(reg), RAX);
        return;
    case 2:
        SWAG_ASSERT(!toAdd);
        if (paramIdx < coffFct->numScratchRegs)
            pp.emit_Extend_U16U64((CPURegister) (cc.firstScratchRegister + paramIdx), RAX);
        else
            pp.emit_LoadU16U64_Indirect(paramStack, RAX, RDI);
        pp.emit_Store64_Indirect(REG_OFFSET(reg), RAX);
        return;
    case 4:
        SWAG_ASSERT(!toAdd);
        if (paramIdx < coffFct->numScratchRegs)
            pp.emit_CopyN(RAX, (CPURegister) (cc.firstScratchRegister + paramIdx), CPUBits::B32);
        else
            pp.emit_Load32_Indirect(paramStack, RAX, RDI);
        pp.emit_Store64_Indirect(REG_OFFSET(reg), RAX);
        return;
    }

    SWAG_ASSERT(toAdd <= 0x7FFFFFFFF);

    bool structByValue = CallConv::structParamByValue(typeFunc, typeParam);

    // Use scratch registers
    if (paramIdx < coffFct->numScratchRegs && !structByValue)
    {
        auto scratch = (CPURegister) (cc.firstScratchRegister + paramIdx);
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

void BackendSCBE::emitCall(EncoderX64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall)
{
    // Push parameters
    pp.emit_Call_Parameters(typeFunc, pushParams, offsetRT);

    // Dll imported function name will have "__imp_" before (imported mangled name)
    if (!localCall)
        pp.emit_Call_Far("__imp_" + funcName);
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

void BackendSCBE::emitCall(EncoderX64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall)
{
    VectorNative<CPUPushParam> p;
    for (auto r : pushRAParams)
        p.push_back({CPUPushParamType::Reg, r});
    emitCall(pp, typeFunc, funcName, p, offsetRT, localCall);
}

void BackendSCBE::emitInternalCall(EncoderX64& pp, Module* moduleToGen, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT)
{
    auto typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    SWAG_ASSERT(typeFunc);

    // Invert order
    VectorNative<CPUPushParam> p;
    for (int i = (int) pushRAParams.size() - 1; i >= 0; i--)
        p.push_back({CPUPushParamType::Reg, pushRAParams[i]});

    emitCall(pp, typeFunc, funcName, p, offsetRT, true);
}

void BackendSCBE::emitInternalCallExt(EncoderX64& pp, Module* moduleToGen, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, TypeInfoFuncAttr* typeFunc)
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

void BackendSCBE::emitByteCodeCall(EncoderX64& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    int idxReg = 0;
    for (int idxParam = typeFuncBC->numReturnRegisters() - 1; idxParam >= 0; idxParam--, idxReg++)
    {
        switch (idxReg)
        {
        case 0:
            pp.emit_LoadAddress_Indirect(offsetRT, RDX, RDI);
            break;
        case 1:
            pp.emit_LoadAddress_Indirect(offsetRT + sizeof(Register), R8, RDI);
            break;
        }
    }

    uint32_t stackOffset = typeFuncBC->numReturnRegisters() * sizeof(Register);
    for (int idxParam = (int) pushRAParams.size() - 1; idxParam >= 0; idxParam--, idxReg++)
    {
        static const CPURegister idxToReg[4] = {RDX, R8, R9};

        // Pass by value
        stackOffset += sizeof(Register);
        if (idxReg <= 2)
        {
            pp.emit_Load64_Indirect(REG_OFFSET(pushRAParams[idxParam]), idxToReg[idxReg]);
        }
        else
        {
            pp.emit_Load64_Indirect(REG_OFFSET(pushRAParams[idxParam]), RAX);
            pp.emit_Store64_Indirect(stackOffset, RAX, RSP);
        }
    }
}

void BackendSCBE::emitByteCodeCallParameters(EncoderX64& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    // If the closure is assigned to a lambda, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal function)
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBC->isClosure())
    {
        auto reg = pushRAParams.back();
        pp.emit_Load64_Indirect(REG_OFFSET(reg), RAX);
        pp.emit_TestN(RAX, RAX, CPUBits::B64);

        // If not zero, jump to closure call
        pp.emit_LongJumpOp(JZ);
        pp.concat.addU32(0);
        auto seekPtrClosure = pp.concat.getSeekPtr() - 4;
        auto seekJmpClosure = pp.concat.totalCount();

        emitByteCodeCall(pp, typeFuncBC, offsetRT, pushRAParams);

        // Jump to after closure call
        pp.emit_LongJumpOp(JUMP);
        pp.concat.addU32(0);
        auto seekPtrAfterClosure = pp.concat.getSeekPtr() - 4;
        auto seekJmpAfterClosure = pp.concat.totalCount();

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