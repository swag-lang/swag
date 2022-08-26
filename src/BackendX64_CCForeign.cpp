#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64_Macros.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

void BackendX64::emitGetParam(X64PerThread& pp, TypeInfoFuncAttr* typeFunc, int reg, int paramIdx, int sizeOf, int storeS4, int sizeStack, uint64_t toAdd, int deRefSize)
{
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
        BackendX64Inst::emit_Clear32(pp, RAX);
        BackendX64Inst::emit_Load8_Indirect(pp, stackOffset, RAX, RDI);
        BackendX64Inst::emit_Store32_Indirect(pp, regOffset(reg), RAX, RDI);
        return;
    case 2:
        SWAG_ASSERT(!toAdd);
        BackendX64Inst::emit_Clear32(pp, RAX);
        BackendX64Inst::emit_Load16_Indirect(pp, stackOffset, RAX, RDI);
        BackendX64Inst::emit_Store32_Indirect(pp, regOffset(reg), RAX, RDI);
        return;
    case 4:
        SWAG_ASSERT(!toAdd);
        BackendX64Inst::emit_Load32_Indirect(pp, stackOffset, RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, regOffset(reg), RAX, RDI);
        return;
    }

    paramIdx       = typeFunc->registerIdxToParamIdx(paramIdx);
    auto typeParam = TypeManager::concreteReferenceType(typeFunc->parameters[paramIdx]->typeInfo);

    // :StructByCopy
    if (typeParam->kind == TypeInfoKind::Struct && typeParam->sizeOf <= sizeof(void*))
        BackendX64Inst::emit_LoadAddress_Indirect(pp, stackOffset, RAX, RDI);
    else
        BackendX64Inst::emit_Load64_Indirect(pp, stackOffset, RAX, RDI);

    if (toAdd)
    {
        if (deRefSize && toAdd <= 0x7FFFFFFFF)
        {
            switch (deRefSize)
            {
            case 1:
                BackendX64Inst::emit_Load8_Indirect(pp, (uint32_t) toAdd, RAX, RAX);
                BackendX64Inst::emit_UnsignedExtend_8_To_64(pp, RAX);
                break;
            case 2:
                BackendX64Inst::emit_Load16_Indirect(pp, (uint32_t) toAdd, RAX, RAX);
                BackendX64Inst::emit_UnsignedExtend_16_To_64(pp, RAX);
                break;
            case 4:
                BackendX64Inst::emit_Load32_Indirect(pp, (uint32_t) toAdd, RAX, RAX);
                break;
            case 8:
                BackendX64Inst::emit_Load64_Indirect(pp, (uint32_t) toAdd, RAX, RAX);
                break;
            }

            deRefSize = false;
        }
        else
        {
            BackendX64Inst::emit_Load64_Immediate(pp, toAdd, RCX);
            BackendX64Inst::emit_Op64(pp, RCX, RAX, X64Op::ADD);
        }
    }

    if (deRefSize)
    {
        switch (deRefSize)
        {
        case 1:
            BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_UnsignedExtend_8_To_64(pp, RAX);
            break;
        case 2:
            BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
            BackendX64Inst::emit_UnsignedExtend_16_To_64(pp, RAX);
            break;
        case 4:
            BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
            break;
        case 8:
            BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
            break;
        }
    }

    BackendX64Inst::emit_Store64_Indirect(pp, regOffset(reg), RAX, RDI);
}

bool BackendX64::emitForeignCall(X64PerThread& pp, Module* moduleToGen, const Utf8& funcName, ByteCodeInstruction* ip, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams, bool localCall)
{
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

    // Push parameters
    SWAG_CHECK(emitForeignCallParameters(pp, moduleToGen, offsetRT, typeFuncBC, pushRAParams));

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
    emitForeignCallResult(pp, typeFuncBC, offsetRT);
    return true;
}

void BackendX64::emitForeignCallResult(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT)
{
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
        else if (returnType->isNativeFloat())
        {
            BackendX64Inst::emit_StoreF64_Indirect(pp, offsetRT, XMM0, RDI);
        }
        else
        {
            BackendX64Inst::emit_Store64_Indirect(pp, offsetRT, RAX, RDI);
        }
    }
}

bool BackendX64::emitForeignFctCall(X64PerThread& pp, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes)
{
    auto returnType   = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    bool returnByCopy = returnType->flags & TYPEINFO_RETURN_BY_COPY;

    int callConvRegisters    = 4;
    int maxParamsPerRegister = (int) paramsRegisters.size();

    // Pass by stack
    if (typeFuncBC->isCVariadic())
    {
        maxParamsPerRegister = typeFuncBC->numParamsRegisters();
    }

    // Set the first 4 parameters. Can be return register, or function parameter.
    int i = 0;
    for (; i < min(callConvRegisters, maxParamsPerRegister); i++)
    {
        auto type = paramsTypes[i];
        auto r    = paramsRegisters[i];

        static const uint8_t reg4[]  = {RCX, RDX, R8, R9};
        static const uint8_t regf4[] = {XMM0, XMM1, XMM2, XMM3};

        // This is a return register
        if (type == g_TypeMgr->typeInfoUndefined)
        {
            if (returnByCopy)
                BackendX64Inst::emit_Load64_Indirect(pp, r, reg4[i], RDI);
            else
                BackendX64Inst::emit_LoadAddress_Indirect(pp, r, reg4[i], RDI);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            // Pass struct in a register if small enough
            // :StructByCopy
            if (type->kind == TypeInfoKind::Struct && type->sizeOf <= sizeof(void*))
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(r), RAX, RDI);
                switch (type->sizeOf)
                {
                case 1:
                    BackendX64Inst::emit_Load8_Indirect(pp, 0, reg4[i], RAX);
                    break;
                case 2:
                    BackendX64Inst::emit_Load16_Indirect(pp, 0, reg4[i], RAX);
                    break;
                case 4:
                    BackendX64Inst::emit_Load32_Indirect(pp, 0, reg4[i], RAX);
                    break;
                case 8:
                    BackendX64Inst::emit_Load64_Indirect(pp, 0, reg4[i], RAX);
                    break;
                }
            }
            else if (type->isNativeFloat())
            {
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(r), regf4[i], RDI);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(r), reg4[i], RDI);
            }
        }
    }

    // Store all parameters after 4 on the stack, with an offset of 4 * sizeof(uint64_t)
    // because the first 4 x uint64_t are for the first 4 parameters (even if they are passed in
    // registers, this is the x64 cdecl convention...)
    // uint32_t offsetStack = min(callConvRegisters, maxParamsPerRegister) * sizeof(uint64_t);
    uint32_t offsetStack = 4 * sizeof(uint64_t);
    for (; i < (int) paramsRegisters.size(); i++)
    {
        // This is a C variadic parameter
        if (i >= maxParamsPerRegister)
        {
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);
        }

        // This is for a return value
        else if (paramsTypes[i] == g_TypeMgr->typeInfoUndefined)
        {
            if (returnByCopy)
                BackendX64Inst::emit_Load64_Indirect(pp, paramsRegisters[i], RAX, RDI);
            else
                BackendX64Inst::emit_LoadAddress_Indirect(pp, paramsRegisters[i], RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);
        }

        // This is for a normal parameter
        else
        {
            auto sizeOf = paramsTypes[i]->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (paramsTypes[i]->kind == TypeInfoKind::Struct)
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);

                // :StructByCopy
                // Store the content of the struct in the stack
                if (sizeOf <= sizeof(void*))
                {
                    switch (sizeOf)
                    {
                    case 1:
                        BackendX64Inst::emit_Load8_Indirect(pp, 0, RAX, RAX);
                        BackendX64Inst::emit_Store8_Indirect(pp, offsetStack, RAX, RSP);
                        break;
                    case 2:
                        BackendX64Inst::emit_Load16_Indirect(pp, 0, RAX, RAX);
                        BackendX64Inst::emit_Store16_Indirect(pp, offsetStack, RAX, RSP);
                        break;
                    case 4:
                        BackendX64Inst::emit_Load32_Indirect(pp, 0, RAX, RAX);
                        BackendX64Inst::emit_Store32_Indirect(pp, offsetStack, RAX, RSP);
                        break;
                    case 8:
                        BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RAX);
                        BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);
                        break;
                    }
                }

                // Store the address of the struct in the stack
                else
                {
                    BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);
                }
            }
            else
            {
                switch (sizeOf)
                {
                case 1:
                    BackendX64Inst::emit_Load8_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    BackendX64Inst::emit_Store8_Indirect(pp, offsetStack, RAX, RSP);
                    break;
                case 2:
                    BackendX64Inst::emit_Load16_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    BackendX64Inst::emit_Store16_Indirect(pp, offsetStack, RAX, RSP);
                    break;
                case 4:
                    BackendX64Inst::emit_Load32_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    BackendX64Inst::emit_Store32_Indirect(pp, offsetStack, RAX, RSP);
                    break;
                case 8:
                    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(paramsRegisters[i]), RAX, RDI);
                    BackendX64Inst::emit_Store64_Indirect(pp, offsetStack, RAX, RSP);
                    break;
                default:
                    return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid parameter type");
                }
            }
        }

        // Push is always aligned
        offsetStack += 8;
    }

    return true;
}

bool BackendX64::emitForeignCallParameters(X64PerThread& pp, Module* moduleToGen, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushRAParams)
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
            if (typeParam->sizeOf > sizeof(void*))
                return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid parameter type");
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
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(reg), RAX, RDI);
        BackendX64Inst::emit_Test64(pp, RAX, RAX);

        // If not zero, jump to closure call
        BackendX64Inst::emit_LongJumpOp(pp, BackendX64Inst::JZ);
        pp.concat.addU32(0);
        auto seekPtrClosure = pp.concat.getSeekPtr() - 4;
        auto seekJmpClosure = pp.concat.totalCount();

        SWAG_CHECK(emitForeignFctCall(pp, moduleToGen, typeFuncBC, paramsRegisters, paramsTypes));

        // Jump to after closure call
        BackendX64Inst::emit_LongJumpOp(pp, BackendX64Inst::JUMP);
        pp.concat.addU32(0);
        auto seekPtrAfterClosure = pp.concat.getSeekPtr() - 4;
        auto seekJmpAfterClosure = pp.concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = (uint8_t) (pp.concat.totalCount() - seekJmpClosure);

        paramsRegisters.erase(0);
        paramsTypes.erase(0);
        SWAG_CHECK(emitForeignFctCall(pp, moduleToGen, typeFuncBC, paramsRegisters, paramsTypes));

        *seekPtrAfterClosure = (uint8_t) (pp.concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        SWAG_CHECK(emitForeignFctCall(pp, moduleToGen, typeFuncBC, paramsRegisters, paramsTypes));
    }

    return true;
}
