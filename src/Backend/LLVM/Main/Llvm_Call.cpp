#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/LLVM/Main/Llvm.h"
#include "Backend/LLVM/Main/Llvm_Macros.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

void Llvm::getReturnResult(LlvmEncoder& pp, TypeInfo* returnType, bool imm, const Register& reg, llvm::AllocaInst* allocR, llvm::AllocaInst* allocResult)
{
    auto& context = *pp.llvmContext;
    auto& builder = *pp.builder;

    llvm::Value* returnResult = nullptr;
    if (returnType->isNative())
    {
        switch (returnType->nativeType)
        {
            case NativeTypeKind::U8:
            case NativeTypeKind::S8:
            case NativeTypeKind::Bool:
                if (imm)
                    returnResult = builder.getInt8(reg.u8);
                else
                    returnResult = builder.CreateLoad(I8_TY(), GEP64(allocR, reg.u32));
                builder.CreateStore(returnResult, TO_PTR_I8(allocResult));
                break;
            case NativeTypeKind::U16:
            case NativeTypeKind::S16:
                if (imm)
                    returnResult = builder.getInt16(reg.u16);
                else
                    returnResult = builder.CreateLoad(I16_TY(), GEP64(allocR, reg.u32));
                builder.CreateStore(returnResult, TO_PTR_I16(allocResult));
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::S32:
            case NativeTypeKind::Rune:
                if (imm)
                    returnResult = builder.getInt32(reg.u32);
                else
                    returnResult = builder.CreateLoad(I32_TY(), GEP64(allocR, reg.u32));
                builder.CreateStore(returnResult, TO_PTR_I32(allocResult));
                break;
            case NativeTypeKind::U64:
            case NativeTypeKind::S64:
                if (imm)
                    returnResult = builder.getInt64(reg.u64);
                else
                    returnResult = builder.CreateLoad(I64_TY(), GEP64(allocR, reg.u32));
                builder.CreateStore(returnResult, allocResult);
                break;
            case NativeTypeKind::F32:
                if (imm)
                    returnResult = llvm::ConstantFP::get(F32_TY(), reg.f32);
                else
                    returnResult = builder.CreateLoad(F32_TY(), GEP64(allocR, reg.u32));
                builder.CreateStore(returnResult, TO_PTR_F32(allocResult));
                break;
            case NativeTypeKind::F64:
                if (imm)
                    returnResult = llvm::ConstantFP::get(F64_TY(), reg.f64);
                else
                    returnResult = builder.CreateLoad(F64_TY(), GEP64(allocR, reg.u32));
                builder.CreateStore(returnResult, TO_PTR_F64(allocResult));
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }
    }
    else if (returnType->isPointer() || returnType->isLambdaClosure())
    {
        const auto llvmType = getLlvmType(pp, returnType);
        if (imm)
            returnResult = builder.CreateIntToPtr(builder.getInt64(reg.u64), llvmType);
        else
            returnResult = builder.CreateIntToPtr(builder.CreateLoad(I64_TY(), GEP64(allocR, reg.u32)), llvmType);
        builder.CreateStore(returnResult, builder.CreatePointerCast(allocResult, llvmType->getPointerTo()));
    }
    // :ReturnStructByValue
    else if (returnType->isStruct())
    {
        SWAG_ASSERT(returnType->sizeOf <= sizeof(void*));
        if (imm)
            returnResult = builder.getInt64(reg.u64);
        else
            returnResult = builder.CreateLoad(I64_TY(), GEP64(allocR, reg.u32));
        builder.CreateStore(returnResult, allocResult);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void Llvm::emitRet(LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, TypeInfo* returnType, llvm::AllocaInst* allocResult)
{
    auto& context = *pp.llvmContext;
    auto& builder = *pp.builder;

    // Emit result
    if (!returnType->isVoid() && !typeFuncBc->returnByAddress())
    {
        if (returnType->isNative())
        {
            switch (returnType->nativeType)
            {
                case NativeTypeKind::U8:
                case NativeTypeKind::S8:
                case NativeTypeKind::Bool:
                    builder.CreateRet(builder.CreateLoad(I8_TY(), allocResult));
                    break;
                case NativeTypeKind::U16:
                case NativeTypeKind::S16:
                    builder.CreateRet(builder.CreateLoad(I16_TY(), allocResult));
                    break;
                case NativeTypeKind::U32:
                case NativeTypeKind::S32:
                case NativeTypeKind::Rune:
                    builder.CreateRet(builder.CreateLoad(I32_TY(), allocResult));
                    break;
                case NativeTypeKind::U64:
                case NativeTypeKind::S64:
                    builder.CreateRet(builder.CreateLoad(I64_TY(), allocResult));
                    break;
                case NativeTypeKind::F32:
                    builder.CreateRet(builder.CreateLoad(F32_TY(), allocResult));
                    break;
                case NativeTypeKind::F64:
                    builder.CreateRet(builder.CreateLoad(F64_TY(), allocResult));
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
        }
        else if (returnType->isPointer() || returnType->isLambdaClosure())
        {
            const auto llvmType = getLlvmType(pp, returnType);
            builder.CreateRet(builder.CreateLoad(llvmType, allocResult));
        }
        // :ReturnStructByValue
        else if (returnType->isStruct())
        {
            SWAG_ASSERT(returnType->sizeOf <= sizeof(void*));
            builder.CreateRet(builder.CreateLoad(I64_TY(), allocResult));
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }
    else
    {
        builder.CreateRetVoid();
    }
}

llvm::FunctionType* Llvm::getOrCreateFuncType(LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, bool closureToLambda)
{
    auto& context = *pp.llvmContext;

    // Already done ?
    if (closureToLambda)
    {
        const auto it = pp.mapFctTypeForeignClosure.find(typeFuncBc);
        if (it != pp.mapFctTypeForeignClosure.end())
            return it->second;
    }
    else
    {
        const auto it = pp.mapFctTypeForeign.find(typeFuncBc);
        if (it != pp.mapFctTypeForeign.end())
            return it->second;
    }

    VectorNative<llvm::Type*> params;
    llvm::Type*               llvmRealReturnType = getLlvmType(pp, typeFuncBc->returnType);
    const bool                returnByAddress    = typeFuncBc->returnByAddress();
    const auto                returnType         = typeFuncBc->concreteReturnType();

    llvm::Type* llvmReturnType = nullptr;
    if (returnByAddress)
    {
        llvmReturnType = VOID_TY();
    }
    // :ReturnStructByValue
    else if (returnType->isStruct())
    {
        SWAG_ASSERT(returnType->sizeOf <= sizeof(void*));
        llvmReturnType = I64_TY();
    }
    else
        llvmReturnType = llvmRealReturnType;

    if (!typeFuncBc->parameters.empty())
    {
        auto numParams = typeFuncBc->parameters.size();

        // Variadic parameters are always first
        if (typeFuncBc->isFctVariadic())
        {
            params.push_back(PTR_I8_TY());
            params.push_back(I64_TY());
            SWAG_ASSERT(numParams);
            numParams--;
        }
        else if (typeFuncBc->isFctCVariadic())
        {
            SWAG_ASSERT(numParams);
            numParams--;
        }

        int idxFirst = 0;
        if (typeFuncBc->isClosure() && closureToLambda)
            idxFirst = 1;

        for (uint32_t i = idxFirst; i < numParams; i++)
        {
            auto param = TypeManager::concreteType(typeFuncBc->parameters[i]->typeInfo);
            if (param->isAutoConstPointerRef())
                param = TypeManager::concretePtrRef(param);

            if (typeFuncBc->structParamByValue(param))
            {
                params.push_back(IX_TY(param->sizeOf * 8));
            }
            else if (param->isString() || param->isSlice())
            {
                auto cType = getLlvmType(pp, param);
                params.push_back(cType);
                params.push_back(I64_TY());
            }
            else if (param->isAny() || param->isInterface())
            {
                auto cType = getLlvmType(pp, param);
                params.push_back(cType);
                params.push_back(PTR_I8_TY());
            }
            else
            {
                auto cType = getLlvmType(pp, param);
                params.push_back(cType);
            }
        }
    }

    // Return value by copy is last
    if (returnByAddress)
        params.push_back(llvmRealReturnType);

    const auto result = llvm::FunctionType::get(llvmReturnType, {params.begin(), params.end()}, typeFuncBc->isFctCVariadic());

    if (closureToLambda)
        pp.mapFctTypeForeignClosure[typeFuncBc] = result;
    else
        pp.mapFctTypeForeign[typeFuncBc] = result;

    return result;
}

bool Llvm::emitGetParam(LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t rDest, uint32_t paramIdx, int sizeOf, uint64_t toAdd, int deRefSize)
{
    const auto allocR  = pp.allocR;
    auto&      builder = *pp.builder;
    auto&      context = *pp.llvmContext;

    auto param = typeFuncBc->registerIdxToType(paramIdx);
    if (param->isAutoConstPointerRef())
        param = TypeManager::concretePtrRef(param);

    const auto arg = pp.llvmFunc->getArg(paramIdx);

    if (toAdd || deRefSize)
    {
        if (typeFuncBc->structParamByValue(param))
        {
            auto ra = builder.CreateIntCast(arg, I64_TY(), false);

            // If this is a value, then we just have to shift it on the right with the given amount of bits
            if (deRefSize)
            {
                if (toAdd)
                    ra = builder.CreateLShr(ra, builder.getInt64(toAdd * 8));

                // We need to mask in order to dereference only the correct value
                if (param->sizeOf != static_cast<uint32_t>(deRefSize))
                {
                    switch (deRefSize)
                    {
                        case 1:
                            ra = builder.CreateAnd(ra, builder.getInt64(0xFF));
                            break;
                        case 2:
                            ra = builder.CreateAnd(ra, builder.getInt64(0xFFFF));
                            break;
                        case 4:
                            ra = builder.CreateAnd(ra, builder.getInt64(0xFFFFFFFF));
                            break;
                        default:
                            SWAG_ASSERT(false);
                            break;
                    }
                }

                const auto r0 = GEP64(allocR, rDest);
                builder.CreateStore(ra, r0);
            }

            // We need the pointer
            else
            {
                const auto allocR1 = builder.CreateAlloca(I64_TY(), builder.getInt32(1));
                allocR1->setAlignment(llvm::Align(8));
                builder.CreateStore(ra, allocR1);
                ra            = GEP8(allocR1, toAdd);
                const auto r0 = GEP64(allocR, rDest);
                builder.CreateStore(ra, r0);
            }

            return true;
        }

        llvm::Value* ra;
        if (arg->getType()->isPointerTy())
            ra = GEP8(arg, toAdd);
        else
            ra = GEP8(builder.CreateIntToPtr(arg, PTR_I8_TY()), toAdd);

        if (deRefSize)
        {
            llvm::Value* v1;
            switch (deRefSize)
            {
                case 1:
                    v1 = builder.CreateLoad(I8_TY(), ra);
                    ra = builder.CreateIntCast(v1, I64_TY(), false);
                    break;
                case 2:
                    v1 = builder.CreateLoad(I16_TY(), ra);
                    ra = builder.CreateIntCast(v1, I64_TY(), false);
                    break;
                case 4:
                    v1 = builder.CreateLoad(I32_TY(), ra);
                    ra = builder.CreateIntCast(v1, I64_TY(), false);
                    break;
                case 8:
                    ra = builder.CreateLoad(I64_TY(), ra);
                    break;
                default:
                    break;
            }

            const auto r0 = GEP64(allocR, rDest);
            builder.CreateStore(ra, r0);
        }
        else
        {
            const auto r0 = GEP64_PTR_PTR_I8(allocR, rDest);
            builder.CreateStore(ra, r0);
        }
    }
    else if (param->numRegisters() == 1)
    {
        // By convention, all remaining bits should be zero
        if (param->isNativeIntegerSigned() && param->sizeOf < sizeof(void*))
        {
            const auto r0 = GEP64(allocR, rDest);
            const auto ra = builder.CreateIntCast(arg, I64_TY(), true);
            builder.CreateStore(ra, r0);
        }
        else if (param->isNativeIntegerUnsigned() && param->sizeOf < sizeof(void*))
        {
            const auto r0 = GEP64(allocR, rDest);
            const auto ra = builder.CreateIntCast(arg, I64_TY(), false);
            builder.CreateStore(ra, r0);
        }

        // Struct by copy
        else if (typeFuncBc->structParamByValue(param))
        {
            // Make a copy of the value on the stack, and get the address
            const auto allocR1 = builder.CreateAlloca(I64_TY(), builder.getInt32(1));
            allocR1->setAlignment(llvm::Align{16});

            const auto ra = builder.CreateIntCast(arg, I64_TY(), false);
            builder.CreateStore(ra, allocR1);
            const auto r0 = GEP64_PTR_PTR_I64(allocR, rDest);
            builder.CreateStore(allocR1, r0);
        }

        // This can be cast to an integer
        else if (sizeOf)
        {
            auto r0 = GEP64(allocR, rDest);
            if (sizeOf == sizeof(void*))
            {
                builder.CreateStore(arg, r0);
            }
            else if (!arg->getType()->isFloatTy())
            {
                const auto ra = builder.CreateIntCast(arg, I64_TY(), false);
                builder.CreateStore(ra, r0);
            }
            else
            {
                const auto ty = getLlvmType(pp, param);
                r0            = builder.CreatePointerCast(r0, ty->getPointerTo());
                builder.CreateStore(arg, r0);
            }
        }

        // Real type
        else
        {
            const auto ty = getLlvmType(pp, param);
            auto       r0 = GEP64(allocR, rDest);
            r0            = builder.CreatePointerCast(r0, ty->getPointerTo());
            builder.CreateStore(arg, r0);
        }
    }
    else
    {
        if (arg->getType()->isPointerTy())
        {
            auto r0 = GEP64(allocR, rDest);
            r0      = builder.CreatePointerCast(r0, arg->getType()->getPointerTo());
            builder.CreateStore(arg, r0);
        }
        else
        {
            const auto r0 = GEP64(allocR, rDest);
            builder.CreateStore(arg, r0);
        }
    }

    return true;
}

bool Llvm::emitCallParameters(LlvmEncoder&                  pp,
                              const TypeInfoFuncAttr*       typeFuncBc,
                              llvm::AllocaInst*             allocR,
                              llvm::AllocaInst*             allocRR,
                              VectorNative<llvm::Value*>&   params,
                              const VectorNative<uint32_t>& pushParams,
                              const Vector<llvm::Value*>&   values,
                              bool                          closureToLambda)
{
    auto& context = *pp.llvmContext;
    auto& builder = *pp.builder;

    uint32_t numCallParams = typeFuncBc->parameters.size();
    uint32_t idxParam      = pushParams.size() - 1;

    // Variadic are first
    if (typeFuncBc->isFctVariadic())
    {
        auto index = pushParams[idxParam--];
        params.push_back(builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index)));
        index = pushParams[idxParam--];
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        SWAG_ASSERT(numCallParams);
        numCallParams--;
    }
    else if (typeFuncBc->isFctCVariadic())
    {
        numCallParams--;
    }

    int idxFirst = 0;
    if (typeFuncBc->isClosure() && closureToLambda)
    {
        idxFirst = 1;
        idxParam--;
    }

    // All parameters
    for (uint32_t idxCall = idxFirst; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = TypeManager::concreteType(typeFuncBc->parameters[idxCall]->typeInfo);
        if (typeParam->isAutoConstPointerRef())
            typeParam = TypeManager::concretePtrRef(typeParam);

        auto index = pushParams[idxParam--];

        if (index == UINT32_MAX)
        {
            params.push_back(values[idxParam + 1]);
            if (typeParam->numRegisters() > 1)
            {
                idxParam--;
                params.push_back(values[idxParam + 1]);
            }
        }
        else if (typeParam->isPointer())
        {
            const auto typePtr  = castTypeInfo<TypeInfoPointer>(typeParam, TypeInfoKind::Pointer);
            const auto llvmType = getLlvmType(pp, typePtr);
            params.push_back(builder.CreateLoad(llvmType, GEP64(allocR, index)));
        }
        else if (typeFuncBc->structParamByValue(typeParam))
        {
            const auto v0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index));
            params.push_back(builder.CreateLoad(IX_TY(typeParam->sizeOf * 8), v0));
        }
        else if (typeParam->isStruct() ||
                 typeParam->isLambdaClosure() ||
                 typeParam->isArray())
        {
            params.push_back(builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index)));
        }
        else if (typeParam->isString() ||
                 typeParam->isSlice())
        {
            params.push_back(builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index)));
            index = pushParams[idxParam--];
            params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        }
        else if (typeParam->isAny() ||
                 typeParam->isInterface())
        {
            params.push_back(builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index)));
            index = pushParams[idxParam--];
            params.push_back(builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index)));
        }
        else if (typeParam->isNative())
        {
            const auto llvmType = getLlvmType(pp, typeParam);
            params.push_back(builder.CreateLoad(llvmType, GEP64(allocR, index)));
        }
        else
        {
            return Report::internalError(typeFuncBc->declNode, "emitCall, invalid param type");
        }
    }

    // Return by parameter
    if (typeFuncBc->returnByAddress() && !typeFuncBc->returnByStackAddress())
    {
        params.push_back(TO_PTR_I8(allocRR));
    }
    else if (typeFuncBc->returnByAddress())
    {
        params.push_back(builder.CreateLoad(PTR_I8_TY(), allocRR));
    }

    // Add all C variadic parameters
    if (typeFuncBc->isFctCVariadic())
    {
        for (uint32_t i = typeFuncBc->numParamsRegisters(); i < pushParams.size(); i++)
        {
            const auto index = pushParams[idxParam--];
            params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        }
    }

    return true;
}

bool Llvm::emitCallResult(const LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, llvm::Value* callResult)
{
    const auto allocRR = pp.allocRR;
    auto&      context = *pp.llvmContext;
    auto&      builder = *pp.builder;

    const auto returnType = typeFuncBc->concreteReturnType();
    if (!returnType->isVoid() && !typeFuncBc->returnByAddress())
    {
        if (returnType->isPointer())
        {
            builder.CreateStore(TO_PTR_I8(callResult), TO_PTR_PTR_I8(allocRR));
        }
        else if (returnType->isNative())
        {
            llvm::Value* r = nullptr;
            switch (returnType->nativeType)
            {
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                case NativeTypeKind::Bool:
                case NativeTypeKind::Void:
                    r = TO_PTR_I8(allocRR);
                    break;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    r = TO_PTR_I16(allocRR);
                    break;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                    r = TO_PTR_I32(allocRR);
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                    r = TO_PTR_I64(allocRR);
                    break;
                case NativeTypeKind::F32:
                    r = TO_PTR_F32(allocRR);
                    break;
                case NativeTypeKind::F64:
                    r = TO_PTR_F64(allocRR);
                    break;
                default:
                    break;
            }

            if (!r)
                return Report::internalError(typeFuncBc->declNode, "emitCall, invalid return type");
            builder.CreateStore(callResult, r);
        }
        else if (returnType->isStruct())
        {
            builder.CreateStore(callResult, TO_PTR_I64(allocRR));
        }
        else
        {
            return Report::internalError(typeFuncBc->declNode, "emitCall, invalid return type");
        }
    }

    return true;
}

llvm::Value* Llvm::emitCall(LlvmEncoder&                  pp,
                            const Utf8&                   funcName,
                            const TypeInfoFuncAttr*       typeFuncBc,
                            llvm::AllocaInst*             allocR,
                            llvm::AllocaInst*             allocRR,
                            const VectorNative<uint32_t>& pushParams,
                            const Vector<llvm::Value*>&   values,
                            bool                          localCall)
{
    auto& builder = *pp.builder;
    auto& modu    = *pp.llvmModule;

    // Get parameters
    VectorNative<llvm::Value*> params;
    emitCallParameters(pp, typeFuncBc, allocR, allocRR, params, pushParams, values);

    // Make the call
    const auto typeF = getOrCreateFuncType(pp, typeFuncBc);
    auto       func  = modu.getOrInsertFunction(funcName.cstr(), typeF);
    const auto func1 = llvm::dyn_cast<llvm::Function>(func.getCallee());

    // Why this can be null ????
    if (func1 && !localCall)
        func1->setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);

    return builder.CreateCall(func, {params.begin(), params.end()});
}

llvm::Value* Llvm::emitCall(LlvmEncoder& pp, const Utf8& funcName, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const Vector<uint32_t>& regs, const Vector<llvm::Value*>& values)
{
    const auto typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(funcName);
    getOrCreateFuncType(pp, typeFunc);

    // Invert regs
    VectorNative<uint32_t> pushRAParams;
    for (uint32_t i = regs.size() - 1; i != UINT32_MAX; i--)
        pushRAParams.push_back(regs[i]);
    Vector<llvm::Value*> pushVParams;
    for (uint32_t i = values.size() - 1; i != UINT32_MAX; i--)
        pushVParams.push_back(values[i]);

    return emitCall(pp, funcName, typeFunc, allocR, allocT, pushRAParams, pushVParams, true);
}

void Llvm::emitByteCodeCallParameters(LlvmEncoder& pp, TypeInfoFuncAttr* typeFuncBc, VectorNative<llvm::Value*>& params, VectorNative<uint32_t>& pushRAParams, const Vector<llvm::Value*>& values, bool closureToLambda)
{
    const auto allocR  = pp.allocR;
    const auto allocT  = pp.allocT;
    const auto allocRR = pp.allocRR;
    auto&      builder = *pp.builder;
    auto&      context = *pp.llvmContext;

    uint32_t popRAidx      = pushRAParams.size() - 1;
    uint32_t numCallParams = typeFuncBc->parameters.size();

    // Return value
    // Normal user case
    if (allocRR)
    {
        for (uint32_t j = 0; j < typeFuncBc->numReturnRegisters(); j++)
        {
            params.push_back(GEP64(allocRR, j));
        }
    }

    // Special case when calling an internal function
    else
    {
        for (uint32_t j = 0; j < typeFuncBc->numReturnRegisters(); j++)
        {
            const auto index = pushRAParams[popRAidx--];
            if (index == UINT32_MAX)
                params.push_back(values[popRAidx + 1]);
            else
                params.push_back(GEP64(allocR, index));
        }
    }

    // Two registers for variadic first
    if (typeFuncBc->isFctVariadic())
    {
        auto index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        numCallParams--;
    }
    else if (typeFuncBc->isFctCVariadic())
    {
        numCallParams--;
    }

    uint32_t idxFirst = 0;
    if (typeFuncBc->isClosure() && closureToLambda)
    {
        idxFirst = 1;
        popRAidx--;
    }

    for (uint32_t idxCall = idxFirst; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = typeFuncBc->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteType(typeParam);
        if (typeParam->isAutoConstPointerRef())
            typeParam = TypeManager::concretePtrRef(typeParam);

        for (uint32_t j = 0; j < typeParam->numRegisters(); j++)
        {
            const auto index = pushRAParams[popRAidx--];

            // By value
            if (typeParam->numRegisters() == 1)
            {
                const auto ty = getLlvmType(pp, typeParam);
                if (index == UINT32_MAX)
                {
                    auto v0 = values[popRAidx + 1];
                    SWAG_ASSERT(v0);
                    params.push_back(v0);
                }
                else
                {
                    params.push_back(builder.CreateLoad(ty, GEP64(allocR, index)));
                }

                continue;
            }

            // By register value. If we have a value and not a register, store the value in a temporary register
            if (index == UINT32_MAX)
            {
                auto v0 = values[popRAidx + 1];
                SWAG_ASSERT(v0);
                if (allocT)
                {
                    if (v0->getType()->isPointerTy())
                        v0 = builder.CreatePtrToInt(v0, I64_TY());
                    else if (v0->getType()->isIntegerTy())
                        v0 = builder.CreateIntCast(v0, I64_TY(), false);
                    params.push_back(v0);
                }
                else
                {
                    params.push_back(v0);
                }
            }

            // By register value.
            else
            {
                const auto v0 = builder.CreateLoad(I64_TY(), GEP64(allocR, index));
                params.push_back(v0);
            }
        }
    }

    // C variadic arguments
    if (typeFuncBc->isFctCVariadic())
    {
        const auto numVariadics = popRAidx + 1;
        for (uint32_t idxCall = 0; idxCall < numVariadics; idxCall++)
        {
            const auto index = pushRAParams[popRAidx--];
            const auto v0    = builder.CreateLoad(I64_TY(), GEP64(allocR, index));
            params.push_back(v0);
        }
    }
}

void Llvm::emitLocalCall(LlvmEncoder& pp, llvm::Value*& resultFuncCall)
{
    const auto ip           = pp.ip;
    const auto callBc       = reinterpret_cast<ByteCode*>(ip->a.pointer);
    const auto typeFuncCall = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);
    resultFuncCall          = emitCall(pp, callBc->getCallNameFromDecl(), typeFuncCall, pp.allocR, pp.allocRR, pp.pushRAParams, {}, true);

    if (ip->op == ByteCodeOp::LocalCallPopRC)
    {
        emitTypedValueToRegister(pp, resultFuncCall, ip->d.u32);
    }

    pp.pushRAParams.clear();
    pp.pushRVParams.clear();
}

void Llvm::emitForeignCall(LlvmEncoder& pp, llvm::Value*& resultFuncCall)
{
    const auto ip           = pp.ip;
    const auto funcNode     = reinterpret_cast<AstFuncDecl*>(ip->a.pointer);
    const auto typeFuncCall = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);
    resultFuncCall          = emitCall(pp, funcNode->getFullNameForeignImport(), typeFuncCall, pp.allocR, pp.allocRR, pp.pushRAParams, {}, false);
    pp.pushRAParams.clear();
    pp.pushRVParams.clear();
}

bool Llvm::emitLambdaCall(LlvmEncoder& pp, llvm::Value*& resultFuncCall)
{
    const auto ip           = pp.ip;
    auto&      builder      = *pp.builder;
    auto&      context      = *pp.llvmContext;
    const auto typeFuncCall = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);

    const auto blockLambdaBC     = llvm::BasicBlock::Create(context, "", pp.llvmFunc);
    const auto blockLambdaNative = llvm::BasicBlock::Create(context, "", pp.llvmFunc);
    const auto blockNext         = llvm::BasicBlock::Create(context, "", pp.llvmFunc);

    {
        const auto v0 = builder.CreateLoad(I64_TY(), GEP64(pp.allocR, ip->a.u32));
        const auto v1 = builder.CreateAnd(v0, builder.getInt64(SWAG_LAMBDA_BC_MARKER));
        const auto v2 = builder.CreateIsNotNull(v1);
        builder.CreateCondBr(v2, blockLambdaBC, blockLambdaNative);
    }

    // Foreign
    //////////////////////////////
    builder.SetInsertPoint(blockLambdaNative);
    {
        llvm::FunctionType*        ft = nullptr;
        const auto                 v1 = builder.CreateLoad(I64_TY(), GEP64(pp.allocR, ip->a.u32));
        VectorNative<llvm::Value*> fctParams;
        emitCallParameters(pp, typeFuncCall, pp.allocR, pp.allocRR, fctParams, pp.pushRAParams, {});

        if (typeFuncCall->isClosure())
        {
            const auto blockLambda  = llvm::BasicBlock::Create(context, "", pp.llvmFunc);
            const auto blockClosure = llvm::BasicBlock::Create(context, "", pp.llvmFunc);

            // Test closure context pointer. If null, this is a lambda.
            // :VariadicAndClosure
            auto rc = pp.pushRAParams[pp.pushRAParams.size() - 1];
            if (typeFuncCall->isFctVariadic())
                rc = pp.pushRAParams[pp.pushRAParams.size() - 3];

            const auto v0 = builder.CreateLoad(I64_TY(), GEP64(pp.allocR, rc));
            const auto v2 = builder.CreateIsNotNull(v0);
            builder.CreateCondBr(v2, blockClosure, blockLambda);

            // Lambda call. We must eliminate the first parameter (closure context)
            builder.SetInsertPoint(blockLambda);
            VectorNative<llvm::Value*> fctParamsLocal;
            emitCallParameters(pp, typeFuncCall, pp.allocR, pp.allocRR, fctParamsLocal, pp.pushRAParams, {}, true);

            ft                 = getOrCreateFuncType(pp, typeFuncCall, true);
            const auto lPt     = llvm::PointerType::getUnqual(ft);
            const auto lR1     = builder.CreateIntToPtr(v1, lPt);
            const auto lResult = builder.CreateCall(ft, lR1, {fctParamsLocal.begin(), fctParamsLocal.end()});
            SWAG_CHECK(emitCallResult(pp, typeFuncCall, lResult));
            builder.CreateBr(blockNext);

            // Closure call. Normal call, as the type contains the first parameter.
            builder.SetInsertPoint(blockClosure);
            ft                 = getOrCreateFuncType(pp, typeFuncCall);
            const auto cPt     = llvm::PointerType::getUnqual(ft);
            const auto cR1     = builder.CreateIntToPtr(v1, cPt);
            const auto cResult = builder.CreateCall(ft, cR1, {fctParams.begin(), fctParams.end()});
            SWAG_CHECK(emitCallResult(pp, typeFuncCall, cResult));
            builder.CreateBr(blockNext);
        }
        else
        {
            ft                      = getOrCreateFuncType(pp, typeFuncCall);
            const auto pt           = llvm::PointerType::getUnqual(ft);
            const auto r1           = builder.CreateIntToPtr(v1, pt);
            const auto returnResult = builder.CreateCall(ft, r1, {fctParams.begin(), fctParams.end()});
            SWAG_CHECK(emitCallResult(pp, typeFuncCall, returnResult));
            builder.CreateBr(blockNext);
        }
    }

    // Bytecode
    //////////////////////////////
    builder.SetInsertPoint(blockLambdaBC);
    {
        const auto                 r0 = builder.CreateLoad(PTR_I8_TY(), GEP64(pp.allocR, ip->a.u32));
        VectorNative<llvm::Value*> fctParams;
        fctParams.push_front(r0);
        emitByteCodeCallParameters(pp, typeFuncCall, fctParams, pp.pushRAParams, {});

        const auto ra = builder.CreateInBoundsGEP(pp.processInfosTy, pp.processInfos, {builder.getInt32(0), builder.getInt32(4)});
        const auto r1 = builder.CreateLoad(PTR_I8_TY(), ra);
        const auto pt = llvm::PointerType::getUnqual(pp.bytecodeRunTy);
        const auto r2 = builder.CreatePointerCast(r1, pt);

        if (typeFuncCall->isClosure())
        {
            const auto blockLambda  = llvm::BasicBlock::Create(context, "", pp.llvmFunc);
            const auto blockClosure = llvm::BasicBlock::Create(context, "", pp.llvmFunc);

            // Test closure context pointer. If null, this is a lambda.
            const auto v0 = builder.CreateLoad(I64_TY(), GEP64(pp.allocR, pp.pushRAParams.back()));
            const auto v2 = builder.CreateIsNotNull(v0);
            builder.CreateCondBr(v2, blockClosure, blockLambda);

            // Lambda call. We must eliminate the first parameter (closure context)
            builder.SetInsertPoint(blockLambda);
            VectorNative<llvm::Value*> fctParamsLocal;
            fctParamsLocal.push_front(r0);
            emitByteCodeCallParameters(pp, typeFuncCall, fctParamsLocal, pp.pushRAParams, {}, true);
            builder.CreateCall(pp.bytecodeRunTy, r2, {fctParamsLocal.begin(), fctParamsLocal.end()});
            builder.CreateBr(blockNext);

            // Closure call. Normal call, as the type contains the first parameter.
            builder.SetInsertPoint(blockClosure);
            builder.CreateCall(pp.bytecodeRunTy, r2, {fctParams.begin(), fctParams.end()});
            builder.CreateBr(blockNext);
        }
        else
        {
            builder.CreateCall(pp.bytecodeRunTy, r2, {fctParams.begin(), fctParams.end()});
            builder.CreateBr(blockNext);
        }
    }

    builder.SetInsertPoint(blockNext);
    pp.pushRAParams.clear();
    pp.pushRVParams.clear();
    resultFuncCall = nullptr;
    return true;
}
