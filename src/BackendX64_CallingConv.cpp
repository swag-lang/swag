#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64_Macros.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

bool BackendX64::emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;
    bool  debug           = buildParameters.buildCfg->backendDebugInformations;

    concat.align(16);
    uint32_t startAddress = concat.totalCount();

    node->computeFullNameForeign(true);

    // Symbol
    uint32_t symbolFuncIndex = getOrAddSymbol(pp, node->fullnameForeign, CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
    pp.directives += Utf8::format("/EXPORT:%s ", node->fullnameForeign.c_str());
    auto coffFct     = registerFunction(pp, node, symbolFuncIndex);
    coffFct->wrapper = true;

    if (debug)
    {
        DbgLine dbgLine;
        dbgLine.line       = node->exportForeignLine;
        dbgLine.byteOffset = 0;
        DbgLines dbgLines;
        dbgLines.sourceFile = node->sourceFile;
        dbgLines.dbgLines.push_back(dbgLine);
        coffFct->dbgLines.push_back(dbgLines);
    }

    auto numTotalRegs  = typeFunc->numTotalRegisters();
    auto numReturnRegs = typeFunc->numReturnRegisters();
    auto returnType    = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM | CONCRETE_FORCEALIAS);
    bool returnByCopy  = returnType->flags & TYPEINFO_RETURN_BY_COPY;

    // Storage for the registers
    uint32_t sizeStack = (uint32_t) numTotalRegs * sizeof(Register);

    // When return by copy, the register needs to contain the "address to the address" where
    // the copy will be stored. So we add one more temporary storage at the end of the stack,
    // after the registers, to store the result address.
    uint32_t offsetRetCopy = 0;
    if (returnByCopy)
    {
        offsetRetCopy = sizeStack;
        sizeStack += sizeof(Register);
    }

    // Prolog
    VectorNative<uint16_t> unwind;
    auto                   beforeProlog = concat.totalCount();
    auto                   sizeProlog   = 0;
    if (sizeStack)
    {
        BackendX64Inst::emit_Push(pp, RDI);
        sizeProlog       = concat.totalCount() - beforeProlog;
        uint16_t unwind0 = computeUnwindPushRDI(sizeProlog);

        MK_ALIGN16(sizeStack);
        sizeStack += 4 * sizeof(Register);
        BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, sizeStack);
        sizeProlog = concat.totalCount() - beforeProlog;
        computeUnwindStack(sizeStack, sizeProlog, unwind);
        BackendX64Inst::emit_Copy64(pp, RSP, RDI);

        // At the end because array must be sorted in 'offset in prolog' descending order
        unwind.push_back(unwind0);
    }
    else
    {
        BackendX64Inst::emit_Sub_Cst32_To_RSP(pp, 5 * sizeof(Register));
        sizeProlog = concat.totalCount() - beforeProlog;
        computeUnwindStack(5 * sizeof(Register), sizeProlog, unwind);
    }

    // Need to save the return address if needed
    // If the return address is in a register, then we store it at its "normal" place in the stack,
    // so that we will always get it by the stack address.
    // See :ReturnRegister2
    // If the return address is not a register, then it's already in the stack, at the right place.
    if (numReturnRegs == 2 && numTotalRegs <= 5)
    {
        SWAG_ASSERT(!returnByCopy);
        SWAG_ASSERT(sizeStack);
        int            offset    = (int) numTotalRegs - 2;
        static uint8_t x64regs[] = {RCX, RDX, R8, R9};
        BackendX64Inst::emit_Store64_Indirect(pp, sizeStack + 16 + (offset * 8), x64regs[offset], RDI);
    }

    // Set all registers
    for (int i = 0; i < numTotalRegs; i++)
    {
        if (i < numReturnRegs)
        {
            if (returnByCopy)
            {
                // Set the register to the temporary storage where we will store the result address
                BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetRetCopy, RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(i), RAX, RDI);
                // Store the result address in the temporary storage
                storeCDeclParamToRegister(pp, returnType, numTotalRegs - numReturnRegs, offsetRetCopy, sizeStack);
            }
            else
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(i), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(i), RAX, RDI);
            }
        }
        else
        {
            auto typeParam = typeFunc->registerIdxToType(i - numReturnRegs);
            storeCDeclParamToRegister(pp, typeParam, i - numReturnRegs, regOffset(i), sizeStack);
        }
    }

    // :CConvLocal
    for (int i = 0; i < numTotalRegs; i++)
    {
        if (i < numReturnRegs)
        {
            BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(i), RAX, RDI);
            storeRAXToCDeclParam(pp, nullptr, i);
        }
        else
        {
            auto typeParam = typeFunc->registerIdxToType(i - numReturnRegs);
            if (passByValue(typeParam))
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(i), RAX, RDI);
            else
                BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(i), RAX, RDI);
            storeRAXToCDeclParam(pp, typeParam, i);
        }
    }

    // Make the call
    emitCall(pp, bc->getCallName());

    // Return value
    // If we return by copy, then this has already been copied to the correct
    // destination address, so nothing to do.
    if (numReturnRegs && !returnByCopy)
    {
        if (numReturnRegs == 1)
        {
            if (returnType->isNative(NativeTypeKind::F32))
                BackendX64Inst::emit_LoadF32_Indirect(pp, 0, XMM0, RDI);
            else if (returnType->isNative(NativeTypeKind::F64))
                BackendX64Inst::emit_LoadF64_Indirect(pp, 0, XMM0, RDI);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, 0, RAX, RDI);
        }
        else if (numReturnRegs == 2)
        {
            // Get the results in rax & rcx
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(0), RAX, RDI);
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(1), RCX, RDI);

            // Get the pointer to store the result
            // We get the return address from the stack, it's always there, even if that address
            // has been passed by register because we have made a copy at the start
            // :ReturnRegister2
            SWAG_ASSERT(numTotalRegs >= 2);
            int offset = numTotalRegs - 2;
            offset *= sizeof(Register);
            offset += sizeStack + 16;
            BackendX64Inst::emit_Load64_Indirect(pp, offset, RDX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, 0, RAX, RDX);
            BackendX64Inst::emit_Store64_Indirect(pp, 8, RCX, RDX);
        }
    }

    // Epilogue, exit
    if (sizeStack)
    {
        BackendX64Inst::emit_Add_Cst32_To_RSP(pp, sizeStack);
        BackendX64Inst::emit_Pop(pp, RDI);
    }
    else
    {
        BackendX64Inst::emit_Add_Cst32_To_RSP(pp, 5 * sizeof(Register));
    }

    BackendX64Inst::emit_Ret(pp);

    uint32_t endAddress = concat.totalCount();
    registerFunction(coffFct, startAddress, endAddress, sizeProlog, unwind);
    return true;
}

void BackendX64::storeCDeclParamToRegister(X64PerThread& pp, TypeInfo* typeParam, int calleeIndex, int stackOffset, uint32_t sizeStack)
{
    if (calleeIndex < 4)
    {
        if (typeParam->isNativeFloat())
        {
            static uint8_t x64Reg[] = {XMM0, XMM1, XMM2, XMM3};
            BackendX64Inst::emit_StoreF64_Indirect(pp, stackOffset, x64Reg[calleeIndex], RDI);
        }
        else
        {
            static uint8_t x64Reg[] = {RCX, RDX, R8, R9};
            BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, x64Reg[calleeIndex], RDI);
        }
    }
    else
    {
        // Get parameter from the stack (aligned to 8 bytes)
        auto offset = calleeIndex * (int) sizeof(Register);
        offset += sizeStack;
        offset += 16;
        BackendX64Inst::emit_Load64_Indirect(pp, offset, RAX, RDI);

        // Store it in the register
        BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, RAX, RDI);
    }
}

void BackendX64::storeRAXToCDeclParam(X64PerThread& pp, TypeInfo* typeParam, int callerIndex)
{
    if (callerIndex < 4)
    {
        if (typeParam && typeParam->isNativeFloat())
        {
            static uint8_t x64Reg[] = {XMM0, XMM1, XMM2, XMM3};
            BackendX64Inst::emit_CopyF64(pp, RAX, x64Reg[callerIndex]);
        }
        else if (!typeParam)
        {
            static uint8_t x64Reg[] = {RCX, RDX, R8, R9};
            BackendX64Inst::emit_Copy64(pp, RAX, x64Reg[callerIndex]);
        }
        else
        {
            static uint8_t x64Reg[] = {RCX, RDX, R8, R9};
            switch (typeParam->sizeOf)
            {
            case 1:
                BackendX64Inst::emit_Clear64(pp, x64Reg[callerIndex]);
                BackendX64Inst::emit_Copy8(pp, RAX, x64Reg[callerIndex]);
                break;
            case 2:
                BackendX64Inst::emit_Clear64(pp, x64Reg[callerIndex]);
                BackendX64Inst::emit_Copy16(pp, RAX, x64Reg[callerIndex]);
                break;
            case 4:
                BackendX64Inst::emit_Copy32(pp, RAX, x64Reg[callerIndex]);
                break;
            default:
                BackendX64Inst::emit_Copy64(pp, RAX, x64Reg[callerIndex]);
                break;
            }
        }
    }
}

void BackendX64::emitLocalCallParameters(X64PerThread& pp, uint32_t sizeParamsStack, TypeInfoFuncAttr* typeFuncBC, uint32_t stackRR, const VectorNative<uint32_t>& pushRAParams, const VectorNative<pair<uint32_t, uint32_t>>& pushRVParams)
{
    uint32_t sizeStack = (uint32_t) (pushRAParams.size() * sizeof(Register));
    sizeStack += typeFuncBC->numReturnRegisters() * sizeof(Register);

    // pushRVParams are for variadics. It contains registers like pushRAParams, but also the sizeof of what needs to be pushed
    if (pushRVParams.size())
    {
        auto sizeOf = pushRVParams[0].second;
        sizeStack += (uint32_t) pushRVParams.size() * sizeOf;
    }

    auto offset = sizeStack;
    MK_ALIGN16(sizeStack);
    SWAG_ASSERT(sizeStack <= sizeParamsStack);

    for (int iParam = 0; iParam < pushRVParams.size(); iParam++)
    {
        auto sizeOf = pushRVParams[0].second;
        auto reg    = pushRVParams[iParam].first;
        offset -= sizeOf;
        switch (sizeOf)
        {
        case 1:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(reg), RAX, RDI);
            BackendX64Inst::emit_Store8_Indirect(pp, offset, RAX, RSP);
            break;
        case 2:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(reg), RAX, RDI);
            BackendX64Inst::emit_Store16_Indirect(pp, offset, RAX, RSP);
            break;
        case 4:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(reg), RAX, RDI);
            BackendX64Inst::emit_Store32_Indirect(pp, offset, RAX, RSP);
            break;
        case 8:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(reg), RAX, RDI);
            BackendX64Inst::emit_Store64_Indirect(pp, offset, RAX, RSP);
            break;
        }
    }

    int numReturnRegs = typeFuncBC->numReturnRegisters();
    //    int callerIndex   = (int) pushRAParams.size() + numReturnRegs - 1;

    // Return value
    int callerIndex = 0;
    for (int j = 0; j < numReturnRegs; j++)
    {
        BackendX64Inst::emit_LoadAddress_Indirect(pp, stackRR + regOffset(j), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, regOffset(j), RAX, RSP);

        // :CConvLocal
        storeRAXToCDeclParam(pp, nullptr, callerIndex);
    }

    // Emit all push params
    for (int iParam = 0; iParam < pushRAParams.size(); iParam++)
    {
        offset -= 8;
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[iParam]), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, offset, RAX, RSP);

        // :CConvLocal
        // auto typeParam = typeFuncBC->registerIdxToType(callerIndex - numReturnRegs);
        // if (!passByValue(typeParam))
        //     BackendX64Inst::emit_LoadAddress_Indirect(pp, offset, RAX, RSP);
        // storeRAXToCDeclParam(pp, typeParam, callerIndex);
        //callerIndex--;
    }
}

void BackendX64::emitParam(X64PerThread& pp, TypeInfoFuncAttr* typeFunc, int reg, int paramIdx, int sizeOf, int sizeStack)
{
    // We need to add 8 because the call has pushed one register on the stack
    // We need to add 8 again, because of the first 'push edi' at the start of the function
    // Se we add 16 in total to get the offset of the parameter in the stack
    switch (sizeOf)
    {
    case 1:
        BackendX64Inst::emit_Clear32(pp, RAX);
        BackendX64Inst::emit_Load8_Indirect(pp, 16 + sizeStack + regOffset(paramIdx) + regOffset(typeFunc->numReturnRegisters()), RAX, RDI);
        BackendX64Inst::emit_Store32_Indirect(pp, regOffset(reg), RAX, RDI);
        break;
    case 2:
        BackendX64Inst::emit_Clear32(pp, RAX);
        BackendX64Inst::emit_Load16_Indirect(pp, 16 + sizeStack + regOffset(paramIdx) + regOffset(typeFunc->numReturnRegisters()), RAX, RDI);
        BackendX64Inst::emit_Store32_Indirect(pp, regOffset(reg), RAX, RDI);
        break;
    case 4:
        BackendX64Inst::emit_Load32_Indirect(pp, 16 + sizeStack + regOffset(paramIdx) + regOffset(typeFunc->numReturnRegisters()), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, regOffset(reg), RAX, RDI);
        break;
    default:
        BackendX64Inst::emit_Load64_Indirect(pp, 16 + sizeStack + regOffset(paramIdx) + regOffset(typeFunc->numReturnRegisters()), RAX, RDI);
        BackendX64Inst::emit_Store64_Indirect(pp, regOffset(reg), RAX, RDI);
        break;
    }
}

void BackendX64::emitParamAddr(X64PerThread& pp, TypeInfoFuncAttr* typeFunc, int reg, int paramIdx, int sizeStack)
{
    // We need to add 8 because the call has pushed one register on the stack
    // We need to add 8 again, because of the first 'push edi' at the start of the function
    // Se we add 16 in total to get the offset of the parameter in the stack
    BackendX64Inst::emit_LoadAddress_Indirect(pp, 16 + sizeStack + regOffset(paramIdx) + regOffset(typeFunc->numReturnRegisters()), RAX, RDI);
    BackendX64Inst::emit_Store64_Indirect(pp, regOffset(reg), RAX, RDI);
}

bool BackendX64::emitForeignCall(X64PerThread& pp, Module* moduleToGen, ByteCodeInstruction* ip, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    auto              nodeFunc   = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

    // Get function name
    Utf8 funcName;
    auto foreignValue = typeFuncBC->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_function);
    if (foreignValue && !foreignValue->text.empty())
        funcName = foreignValue->text;
    else
        funcName = nodeFunc->token.text;

    // Push parameters
    SWAG_CHECK(emitForeignCallParameters(pp, moduleToGen, offsetRT, typeFuncBC, pushRAParams));

    auto& concat = pp.concat;

    // Need to make a far call
    concat.addU8(0xFF); // call
    concat.addU8(0x15);

    // Dll imported function name will have "__imp_" before (imported mangled name)
    CoffRelocation reloc;
    reloc.virtualAddress = concat.totalCount() - pp.textSectionOffset;
    auto callSym         = getOrAddSymbol(pp, "__imp_" + funcName, CoffSymbolKind::Extern);
    reloc.symbolIndex    = callSym->index;
    reloc.type           = IMAGE_REL_AMD64_REL32;
    pp.relocTableTextSection.table.push_back(reloc);
    concat.addU32(0);

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

bool BackendX64::emitForeignCallParameters(X64PerThread& pp, Module* moduleToGen, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushRAParams)
{
    int numCallParams = (int) typeFuncBC->parameters.size();

    VectorNative<uint32_t>  paramsRegisters;
    VectorNative<TypeInfo*> paramsTypes;

    int indexParam = (int) pushRAParams.size() - 1;

    // Variadic are first
    if (numCallParams)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters.back()->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic || typeParam->kind == TypeInfoKind::TypedVariadic)
        {
            auto index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);

            index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
            numCallParams--;
        }
    }

    // All parameters
    for (int i = 0; i < (int) numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[i]->typeInfo);

        auto index = pushRAParams[indexParam--];

        if (typeParam->kind == TypeInfoKind::Pointer ||
            typeParam->kind == TypeInfoKind::Struct ||
            typeParam->kind == TypeInfoKind::Lambda ||
            typeParam->kind == TypeInfoKind::Array)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
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
    auto returnType   = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    bool returnByCopy = returnType->flags & TYPEINFO_RETURN_BY_COPY;
    if (returnType->kind == TypeInfoKind::Slice ||
        returnType->kind == TypeInfoKind::Interface ||
        returnType->isNative(NativeTypeKind::Any) ||
        returnType->isNative(NativeTypeKind::String))
    {
        paramsRegisters.push_back(offsetRT);
        paramsTypes.push_back(g_TypeMgr->typeInfoUndefined);
    }
    else if (returnByCopy)
    {
        paramsRegisters.push_back(offsetRT);
        paramsTypes.push_back(g_TypeMgr->typeInfoUndefined);
    }

    // Set the first 4 parameters. Can be return register, or function parameter.
    for (int i = 0; i < min(4, (int) paramsRegisters.size()); i++)
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
            if (type->isNativeFloat())
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(r), regf4[i], RDI);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(r), reg4[i], RDI);
        }
    }

    // Store all parameters after 4 on the stack, with an offset of 4 * sizeof(uint64_t)
    // because the first 4 x uint64_t are for the first 4 parameters (even if they are passed in
    // registers, this is the x64 cdecl convention...)
    if (paramsRegisters.size() > 4)
    {
        uint32_t offsetStack = 4 * sizeof(uint64_t);
        for (int i = 4; i < (int) paramsRegisters.size(); i++)
        {
            // This is for a return value
            if (paramsTypes[i] == g_TypeMgr->typeInfoUndefined)
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

            // Push is always aligned
            offsetStack += 8;
        }
    }

    return true;
}

void BackendX64::emitByteCodeLambdaParams(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams)
{
    int idxReg = 0;
    for (int idxParam = typeFuncBC->numReturnRegisters() - 1; idxParam >= 0; idxParam--, idxReg++)
    {
        switch (idxReg)
        {
        case 0:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetRT, RDX, RDI);
            break;
        case 1:
            BackendX64Inst::emit_LoadAddress_Indirect(pp, offsetRT + sizeof(Register), R8, RDI);
            break;
        }
    }

    // When calling a bytecode callback in x64, we must respect the passByValue convention, even if it's not
    // the case internaly for local calls.
    uint32_t stackOffset = 0;
    for (int idxParam = (int) pushRAParams.size() - 1; idxParam >= 0; idxParam--, idxReg++)
    {
        auto typeParam = typeFuncBC->registerIdxToType(idxReg - typeFuncBC->numReturnRegisters());

        static const uint8_t idxToReg[4] = {RDX, R8, R9};

        // Pass by value
        if (passByValue(typeParam))
        {
            stackOffset += sizeof(Register);
            if (idxReg <= 2)
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[idxParam]), idxToReg[idxReg], RDI);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(pushRAParams[idxParam]), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, RAX, RSP);
            }
        }

        // Pass by register pointer
        else
        {
            stackOffset += sizeof(Register);
            if (idxReg <= 2)
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(pushRAParams[idxParam]), idxToReg[idxReg], RDI);
            }
            else
            {
                BackendX64Inst::emit_LoadAddress_Indirect(pp, regOffset(pushRAParams[idxParam]), RAX, RDI);
                BackendX64Inst::emit_Store64_Indirect(pp, stackOffset, RAX, RSP);
            }
        }
    }
}