#include "pch.h"
#include "BackendX64.h"
#include "BackendX64_Macros.h"
#include "TypeManager.h"
#include "ByteCode.h"

void BackendX64::emitGetParam(X64Gen& pp, TypeInfoFuncAttr* typeFunc, int reg, int paramIdx, int sizeOf, int storeS4, int sizeStack, uint64_t toAdd, int deRefSize)
{
    const auto& cc = g_CallConv[typeFunc->callConv];

    int stackOffset = 0;

    // If this was a register, then get the value from storeS4 (where input registers have been saveed)
    // instead of value from the stack
    if (paramIdx < 4)
        stackOffset = storeS4 + regOffset(paramIdx);

    // Value from the caller stack
    // We need to add 8 because the call has pushed one register on the stack
    // We need to add 8 again, because of the first 'push edi' at the start of the function
    // Se we add 16 in total to get the offset of the parameter in the stack
    else
        stackOffset = 16 + sizeStack + regOffset(paramIdx);

    switch (sizeOf)
    {
    case 1:
        SWAG_ASSERT(!toAdd);
        pp.emit_Clear32(RAX);
        pp.emit_Load8_Indirect(stackOffset, RAX, RDI);
        pp.emit_Store32_Indirect(regOffset(reg), RAX, RDI);
        return;
    case 2:
        SWAG_ASSERT(!toAdd);
        pp.emit_Clear32(RAX);
        pp.emit_Load16_Indirect(stackOffset, RAX, RDI);
        pp.emit_Store32_Indirect(regOffset(reg), RAX, RDI);
        return;
    case 4:
        SWAG_ASSERT(!toAdd);
        pp.emit_Load32_Indirect(stackOffset, RAX, RDI);
        pp.emit_Store64_Indirect(regOffset(reg), RAX, RDI);
        return;
    }

    paramIdx       = typeFunc->registerIdxToParamIdx(paramIdx);
    auto typeParam = TypeManager::concreteReferenceType(typeFunc->parameters[paramIdx]->typeInfo);

    if (cc.structByRegister && typeParam->kind == TypeInfoKind::Struct && typeParam->sizeOf <= sizeof(void*))
        pp.emit_LoadAddress_Indirect(stackOffset, RAX, RDI);
    else
        pp.emit_Load64_Indirect(stackOffset, RAX, RDI);

    if (toAdd)
    {
        if (deRefSize && toAdd <= 0x7FFFFFFFF)
        {
            switch (deRefSize)
            {
            case 1:
                pp.emit_Load8_Indirect((uint32_t) toAdd, RAX, RAX);
                pp.emit_UnsignedExtend_8_To_64(RAX);
                break;
            case 2:
                pp.emit_Load16_Indirect((uint32_t) toAdd, RAX, RAX);
                pp.emit_UnsignedExtend_16_To_64(RAX);
                break;
            case 4:
                pp.emit_Load32_Indirect((uint32_t) toAdd, RAX, RAX);
                break;
            case 8:
                pp.emit_Load64_Indirect((uint32_t) toAdd, RAX, RAX);
                break;
            }

            deRefSize = false;
        }
        else
        {
            pp.emit_Load64_Immediate(toAdd, RCX);
            pp.emit_Op64(RCX, RAX, X64Op::ADD);
        }
    }

    if (deRefSize)
    {
        switch (deRefSize)
        {
        case 1:
            pp.emit_Load8_Indirect(0, RAX, RAX);
            pp.emit_UnsignedExtend_8_To_64(RAX);
            break;
        case 2:
            pp.emit_Load16_Indirect(0, RAX, RAX);
            pp.emit_UnsignedExtend_16_To_64(RAX);
            break;
        case 4:
            pp.emit_Load32_Indirect(0, RAX, RAX);
            break;
        case 8:
            pp.emit_Load64_Indirect(0, RAX, RAX);
            break;
        }
    }

    pp.emit_Store64_Indirect(regOffset(reg), RAX, RDI);
}

void BackendX64::emitCall(X64Gen& pp, const Utf8& funcName, ByteCodeInstruction* ip, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams, bool localCall)
{
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

    // Push parameters
    pp.emit_Call_Parameters(typeFuncBC, pushRAParams, offsetRT);

    auto& concat = pp.concat;

    // Dll imported function name will have "__imp_" before (imported mangled name)
    if (!localCall)
    {
        // Need to make a far call
        concat.addU8(0xFF); // call
        concat.addU8(0x15);

        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount() - pp.textSectionOffset;

        auto callSym      = getOrAddSymbol(pp, "__imp_" + funcName, CoffSymbolKind::Extern);
        reloc.symbolIndex = callSym->index;
        reloc.type        = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);
        concat.addU32(0);
    }
    else
    {
        emitCall(pp, funcName);
    }

    // Store result
    pp.emit_Call_Result(typeFuncBC, offsetRT);
}

void BackendX64::emitByteCodeCall(X64Gen& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
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
        static const uint8_t idxToReg[4] = {RDX, R8, R9};

        // Pass by value
        stackOffset += sizeof(Register);
        if (idxReg <= 2)
        {
            pp.emit_Load64_Indirect(regOffset(pushRAParams[idxParam]), idxToReg[idxReg], RDI);
        }
        else
        {
            pp.emit_Load64_Indirect(regOffset(pushRAParams[idxParam]), RAX, RDI);
            pp.emit_Store64_Indirect(stackOffset, RAX, RSP);
        }
    }
}

void BackendX64::emitByteCodeCallParameters(X64Gen& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    // If the closure is assigned to a lambda, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal function)
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBC->isClosure())
    {
        auto reg = pushRAParams.back();
        pp.emit_Load64_Indirect(regOffset(reg), RAX, RDI);
        pp.emit_Test64(RAX, RAX);

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