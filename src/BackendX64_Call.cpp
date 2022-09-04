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
    emitCallParameters(pp, offsetRT, typeFuncBC, pushRAParams);

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
    emitCallResult(pp, typeFuncBC, offsetRT);
}

void BackendX64::emitCallResult(X64Gen& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT)
{
    const auto& cc = g_CallConv[typeFuncBC->callConv];

    // Store result to rt0
    auto returnType = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    if (returnType != g_TypeMgr->typeInfoVoid)
    {
        if ((returnType->kind == TypeInfoKind::Slice) ||
            (returnType->kind == TypeInfoKind::Interface) ||
            (returnType->isNative(NativeTypeKind::Any)) ||
            (returnType->isNative(NativeTypeKind::String)) ||
            (returnType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            // Return by parameter
        }
        else if (cc.useReturnByRegisterFloat && returnType->isNativeFloat())
        {
            pp.emit_StoreF64_Indirect(offsetRT, cc.returnByRegisterFloat, RDI);
        }
        else
        {
            pp.emit_Store64_Indirect(offsetRT, cc.returnByRegisterInteger, RDI);
        }
    }
}

void BackendX64::emitCallParameters(X64Gen& pp, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopy)
{
    const auto& cc           = g_CallConv[typeFuncBC->callConv];
    auto        returnType   = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    bool        returnByCopy = returnType->flags & TYPEINFO_RETURN_BY_COPY;

    int callConvRegisters    = cc.byRegisterCount;
    int maxParamsPerRegister = (int) paramsRegisters.size();

    // Set the first N parameters. Can be return register, or function parameter.
    int i = 0;
    for (; i < min(callConvRegisters, maxParamsPerRegister); i++)
    {
        auto type = paramsTypes[i];
        auto r    = paramsRegisters[i];

        // This is a return register
        if (type == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopy)
                pp.emit_Load64_Immediate((uint64_t) retCopy, cc.byRegisterInteger[i]);
            else if (returnByCopy)
                pp.emit_Load64_Indirect(r, cc.byRegisterInteger[i], RDI);
            else
                pp.emit_LoadAddress_Indirect(r, cc.byRegisterInteger[i], RDI);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            // Pass struct in a register if small enough
            if (cc.structByRegister && type->kind == TypeInfoKind::Struct && type->sizeOf <= sizeof(void*))
            {
                pp.emit_Load64_Indirect(regOffset(r), RAX, RDI);
                switch (type->sizeOf)
                {
                case 1:
                    pp.emit_Load8_Indirect(0, cc.byRegisterInteger[i], RAX);
                    break;
                case 2:
                    pp.emit_Load16_Indirect(0, cc.byRegisterInteger[i], RAX);
                    break;
                case 4:
                    pp.emit_Load32_Indirect(0, cc.byRegisterInteger[i], RAX);
                    break;
                case 8:
                    pp.emit_Load64_Indirect(0, cc.byRegisterInteger[i], RAX);
                    break;
                }
            }
            else if (cc.useRegisterFloat && type->isNativeFloat())
            {
                pp.emit_LoadF64_Indirect(regOffset(r), cc.byRegisterFloat[i], RDI);
            }
            else
            {
                pp.emit_Load64_Indirect(regOffset(r), cc.byRegisterInteger[i], RDI);
            }
        }
    }

    // Store all parameters after N on the stack, with an offset of N * sizeof(uint64_t)
    uint32_t offsetStack = min(callConvRegisters, maxParamsPerRegister) * sizeof(uint64_t);
    for (; i < (int) paramsRegisters.size(); i++)
    {
        // This is a C variadic parameter
        if (i >= maxParamsPerRegister)
        {
            pp.emit_Load64_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
            pp.emit_Store64_Indirect(offsetStack, RAX, RSP);
        }

        // This is for a return value
        else if (paramsTypes[i] == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopy)
                pp.emit_Load64_Immediate((uint64_t) retCopy, RAX);
            else if (returnByCopy)
                pp.emit_Load64_Indirect(paramsRegisters[i], RAX, RDI);
            else
                pp.emit_LoadAddress_Indirect(paramsRegisters[i], RAX, RDI);
            pp.emit_Store64_Indirect(offsetStack, RAX, RSP);
        }

        // This is for a normal parameter
        else
        {
            auto sizeOf = paramsTypes[i]->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (paramsTypes[i]->kind == TypeInfoKind::Struct)
            {
                pp.emit_Load64_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);

                // Store the content of the struct in the stack
                if (cc.structByRegister && sizeOf <= sizeof(void*))
                {
                    switch (sizeOf)
                    {
                    case 1:
                        pp.emit_Load8_Indirect(0, RAX, RAX);
                        pp.emit_Store8_Indirect(offsetStack, RAX, RSP);
                        break;
                    case 2:
                        pp.emit_Load16_Indirect(0, RAX, RAX);
                        pp.emit_Store16_Indirect(offsetStack, RAX, RSP);
                        break;
                    case 4:
                        pp.emit_Load32_Indirect(0, RAX, RAX);
                        pp.emit_Store32_Indirect(offsetStack, RAX, RSP);
                        break;
                    case 8:
                        pp.emit_Load64_Indirect(0, RAX, RAX);
                        pp.emit_Store64_Indirect(offsetStack, RAX, RSP);
                        break;
                    }
                }

                // Store the address of the struct in the stack
                else
                {
                    pp.emit_Store64_Indirect(offsetStack, RAX, RSP);
                }
            }
            else
            {
                switch (sizeOf)
                {
                case 1:
                    pp.emit_Load8_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
                    pp.emit_Store8_Indirect(offsetStack, RAX, RSP);
                    break;
                case 2:
                    pp.emit_Load16_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
                    pp.emit_Store16_Indirect(offsetStack, RAX, RSP);
                    break;
                case 4:
                    pp.emit_Load32_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
                    pp.emit_Store32_Indirect(offsetStack, RAX, RSP);
                    break;
                case 8:
                    pp.emit_Load64_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
                    pp.emit_Store64_Indirect(offsetStack, RAX, RSP);
                    break;
                default:
                    SWAG_ASSERT(false);
                    return;
                }
            }
        }

        // Push is always aligned
        offsetStack += 8;
    }
}

void BackendX64::emitCallParameters(X64Gen& pp, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushRAParams, void* retCopy)
{
    int numCallParams = (int) typeFuncBC->parameters.size();

    VectorNative<uint32_t>  paramsRegisters;
    VectorNative<TypeInfo*> paramsTypes;

    int indexParam = (int) pushRAParams.size() - 1;

    // Variadic are first
    if (typeFuncBC->isVariadic())
    {
        auto index = pushRAParams[indexParam--];
        paramsRegisters.push_back(index);
        paramsTypes.push_back(g_TypeMgr->typeInfoU64);

        index = pushRAParams[indexParam--];
        paramsRegisters.push_back(index);
        paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        numCallParams--;
    }
    else if (typeFuncBC->isCVariadic())
    {
        numCallParams--;
    }

    // All parameters
    for (int i = 0; i < (int) numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[i]->typeInfo);

        auto index = pushRAParams[indexParam--];

        if (typeParam->kind == TypeInfoKind::Pointer ||
            typeParam->kind == TypeInfoKind::Lambda ||
            typeParam->kind == TypeInfoKind::Array)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else if (typeParam->kind == TypeInfoKind::Struct)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(typeParam);
        }
        else if (typeParam->kind == TypeInfoKind::Slice ||
                 typeParam->isNative(NativeTypeKind::String))
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
            index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else if (typeParam->isNative(NativeTypeKind::Any) ||
                 typeParam->kind == TypeInfoKind::Interface)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
            index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else
        {
            SWAG_ASSERT(typeParam->sizeOf <= sizeof(void*));
            paramsRegisters.push_back(index);
            paramsTypes.push_back(typeParam);
        }
    }

    // Return by parameter
    if (typeFuncBC->returnByCopy())
    {
        paramsRegisters.push_back(offsetRT);
        paramsTypes.push_back(g_TypeMgr->typeInfoUndefined);
    }

    // Add all C variadic parameters
    if (typeFuncBC->isCVariadic())
    {
        for (int i = typeFuncBC->numParamsRegisters(); i < pushRAParams.size(); i++)
        {
            auto index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
    }

    // If the closure is assigned to a lambda, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal function)
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBC->isClosure())
    {
        auto reg = paramsRegisters[0];
        pp.emit_Load64_Indirect(regOffset(reg), RAX, RDI);
        pp.emit_Test64(RAX, RAX);

        // If not zero, jump to closure call
        pp.emit_LongJumpOp(JZ);
        pp.concat.addU32(0);
        auto seekPtrClosure = pp.concat.getSeekPtr() - 4;
        auto seekJmpClosure = pp.concat.totalCount();

        emitCallParameters(pp, typeFuncBC, paramsRegisters, paramsTypes, retCopy);

        // Jump to after closure call
        pp.emit_LongJumpOp(JUMP);
        pp.concat.addU32(0);
        auto seekPtrAfterClosure = pp.concat.getSeekPtr() - 4;
        auto seekJmpAfterClosure = pp.concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = (uint8_t) (pp.concat.totalCount() - seekJmpClosure);

        paramsRegisters.erase(0);
        paramsTypes.erase(0);
        emitCallParameters(pp, typeFuncBC, paramsRegisters, paramsTypes, retCopy);

        *seekPtrAfterClosure = (uint8_t) (pp.concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emitCallParameters(pp, typeFuncBC, paramsRegisters, paramsTypes, retCopy);
    }
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