#include "pch.h"
#include "LLVM.h"
#include "LLVM_Macros.h"
#include "Module.h"
#include "Report.h"
#include "TypeManager.h"
#include "Workspace.h"

void LLVM::getReturnResult(llvm::LLVMContext&     context,
                           const BuildParameters& buildParameters,
                           TypeInfo*              returnType,
                           bool                   imm,
                           const Register&        reg,
                           llvm::AllocaInst*      allocR,
                           llvm::AllocaInst*      allocResult)
{
    const int   ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = *static_cast<LLVMEncoder*>(perThread[ct][precompileIndex]);
    auto&       builder         = *pp.builder;

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
        const auto llvmType = swagTypeToLLVMType(buildParameters, returnType);
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

void LLVM::createRet(const BuildParameters& buildParameters, const TypeInfoFuncAttr* typeFunc, TypeInfo* returnType, llvm::AllocaInst* allocResult)
{
    const int   ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = *static_cast<LLVMEncoder*>(perThread[ct][precompileIndex]);
    auto&       context         = *pp.llvmContext;
    auto&       builder         = *pp.builder;

    // Emit result
    if (!returnType->isVoid() && !CallConv::returnByAddress(typeFunc))
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
            const auto llvmType = swagTypeToLLVMType(buildParameters, returnType);
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

llvm::FunctionType* LLVM::getOrCreateFuncType(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFunc, bool closureToLambda)
{
    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;
    auto&      pp              = *static_cast<LLVMEncoder*>(perThread[ct][precompileIndex]);
    auto&      context         = *pp.llvmContext;

    // Already done ?
    if (closureToLambda)
    {
        const auto it = pp.mapFctTypeForeignClosure.find(typeFunc);
        if (it != pp.mapFctTypeForeignClosure.end())
            return it->second;
    }
    else
    {
        const auto it = pp.mapFctTypeForeign.find(typeFunc);
        if (it != pp.mapFctTypeForeign.end())
            return it->second;
    }

    VectorNative<llvm::Type*> params;
    llvm::Type*               llvmRealReturnType = swagTypeToLLVMType(buildParameters, typeFunc->returnType);
    const bool                returnByAddress    = CallConv::returnByAddress(typeFunc);
    const auto                returnType         = typeFunc->concreteReturnType();

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

    if (!typeFunc->parameters.empty())
    {
        auto numParams = typeFunc->parameters.size();

        // Variadic parameters are always first
        if (typeFunc->isFctVariadic())
        {
            params.push_back(PTR_I8_TY());
            params.push_back(I64_TY());
            SWAG_ASSERT(numParams);
            numParams--;
        }
        else if (typeFunc->isFctCVariadic())
        {
            SWAG_ASSERT(numParams);
            numParams--;
        }

        int idxFirst = 0;
        if (typeFunc->isClosure() && closureToLambda)
            idxFirst = 1;

        for (size_t i = idxFirst; i < numParams; i++)
        {
            auto param = TypeManager::concreteType(typeFunc->parameters[i]->typeInfo);
            if (param->isAutoConstPointerRef())
                param = TypeManager::concretePtrRef(param);

            if (CallConv::structParamByValue(typeFunc, param))
            {
                params.push_back(IX_TY(param->sizeOf * 8));
            }
            else if (param->isString() || param->isSlice())
            {
                auto cType = swagTypeToLLVMType(buildParameters, param);
                params.push_back(cType);
                params.push_back(I64_TY());
            }
            else if (param->isAny() || param->isInterface())
            {
                auto cType = swagTypeToLLVMType(buildParameters, param);
                params.push_back(cType);
                params.push_back(PTR_I8_TY());
            }
            else
            {
                auto cType = swagTypeToLLVMType(buildParameters, param);
                params.push_back(cType);
            }
        }
    }

    // Return value by copy is last
    if (returnByAddress)
        params.push_back(llvmRealReturnType);

    const auto result = llvm::FunctionType::get(llvmReturnType, {params.begin(), params.end()}, typeFunc->isFctCVariadic());

    if (closureToLambda)
        pp.mapFctTypeForeignClosure[typeFunc] = result;
    else
        pp.mapFctTypeForeign[typeFunc] = result;

    return result;
}

bool LLVM::emitGetParam(llvm::LLVMContext&     context,
                        const BuildParameters& buildParameters,
                        const llvm::Function*  func,
                        TypeInfoFuncAttr*      typeFunc,
                        uint32_t               rDest,
                        uint32_t               paramIdx,
                        llvm::AllocaInst*      allocR,
                        int                    sizeOf,
                        uint64_t               toAdd,
                        int                    deRefSize)
{
    const int   ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = *static_cast<LLVMEncoder*>(perThread[ct][precompileIndex]);
    auto&       builder         = *pp.builder;

    auto param = typeFunc->registerIdxToType(paramIdx);
    if (param->isAutoConstPointerRef())
        param = TypeManager::concretePtrRef(param);

    const auto arg = func->getArg(paramIdx);

    if (toAdd || deRefSize)
    {
        if (CallConv::structParamByValue(typeFunc, param))
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
        else if (CallConv::structParamByValue(typeFunc, param))
        {
            // Make a copy of the value on the stack, and get the address
            const auto allocR1 = builder.CreateAlloca(I64_TY(), builder.getInt32(1));
            allocR1->setAlignment(llvm::Align{16});

            const auto ra = builder.CreateIntCast(arg, I64_TY(), false);
            builder.CreateStore(ra, allocR1);
            const auto r0 = GEP64_PTR_PTR_I64(allocR, rDest);
            builder.CreateStore(allocR1, r0);
        }

        // This can be casted to an integer
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
                const auto ty = swagTypeToLLVMType(buildParameters, param);
                r0            = builder.CreatePointerCast(r0, ty->getPointerTo());
                builder.CreateStore(arg, r0);
            }
        }

        // Real type
        else
        {
            const auto ty = swagTypeToLLVMType(buildParameters, param);
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

bool LLVM::emitCallParameters(const BuildParameters&        buildParameters,
                              llvm::AllocaInst*             allocR,
                              llvm::AllocaInst*             allocRR,
                              TypeInfoFuncAttr*             typeFuncBC,
                              VectorNative<llvm::Value*>&   params,
                              const VectorNative<uint32_t>& pushParams,
                              const Vector<llvm::Value*>&   values,
                              bool                          closureToLambda)
{
    const int   ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = *static_cast<LLVMEncoder*>(perThread[ct][precompileIndex]);
    auto&       context         = *pp.llvmContext;
    auto&       builder         = *pp.builder;

    int numCallParams = static_cast<int>(typeFuncBC->parameters.size());
    int idxParam      = static_cast<int>(pushParams.size()) - 1;

    // Variadic are first
    if (typeFuncBC->isFctVariadic())
    {
        auto index = pushParams[idxParam--];
        params.push_back(builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index)));
        index = pushParams[idxParam--];
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        SWAG_ASSERT(numCallParams);
        numCallParams--;
    }
    else if (typeFuncBC->isFctCVariadic())
    {
        numCallParams--;
    }

    int idxFirst = 0;
    if (typeFuncBC->isClosure() && closureToLambda)
    {
        idxFirst = 1;
        idxParam--;
    }

    // All parameters
    for (int idxCall = idxFirst; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = TypeManager::concreteType(typeFuncBC->parameters[idxCall]->typeInfo);
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
            const auto llvmType = swagTypeToLLVMType(buildParameters, typePtr);
            params.push_back(builder.CreateLoad(llvmType, GEP64(allocR, index)));
        }
        else if (CallConv::structParamByValue(typeFuncBC, typeParam))
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
            const auto llvmType = swagTypeToLLVMType(buildParameters, typeParam);
            params.push_back(builder.CreateLoad(llvmType, GEP64(allocR, index)));
        }
        else
        {
            return Report::internalError(typeFuncBC->declNode, "emitCall, invalid param type");
        }
    }

    // Return by parameter
    if (CallConv::returnByAddress(typeFuncBC) && !CallConv::returnByStackAddress(typeFuncBC))
    {
        params.push_back(TO_PTR_I8(allocRR));
    }
    else if (CallConv::returnByAddress(typeFuncBC))
    {
        params.push_back(builder.CreateLoad(PTR_I8_TY(), allocRR));
    }

    // Add all C variadic parameters
    if (typeFuncBC->isFctCVariadic())
    {
        for (size_t i = typeFuncBC->numParamsRegisters(); i < pushParams.size(); i++)
        {
            const auto index = pushParams[idxParam--];
            params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        }
    }

    return true;
}

bool LLVM::emitCallReturnValue(const BuildParameters&  buildParameters,
                               llvm::AllocaInst*       allocRR,
                               const TypeInfoFuncAttr* typeFuncBC,
                               llvm::Value*            callResult) const
{
    const int   ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = *static_cast<LLVMEncoder*>(perThread[ct][precompileIndex]);
    auto&       context         = *pp.llvmContext;
    auto&       builder         = *pp.builder;

    const auto returnType = typeFuncBC->concreteReturnType();
    if (!returnType->isVoid() && !CallConv::returnByAddress(typeFuncBC))
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
                return Report::internalError(typeFuncBC->declNode, "emitCall, invalid return type");
            builder.CreateStore(callResult, r);
        }
        else if (returnType->isStruct())
        {
            builder.CreateStore(callResult, TO_PTR_I64(allocRR));
        }
        else
        {
            return Report::internalError(typeFuncBC->declNode, "emitCall, invalid return type");
        }
    }

    return true;
}

llvm::Value* LLVM::emitCall(const BuildParameters&        buildParameters,
                            const Utf8&                   funcName,
                            TypeInfoFuncAttr*             typeFuncBC,
                            llvm::AllocaInst*             allocR,
                            llvm::AllocaInst*             allocRR,
                            const VectorNative<uint32_t>& pushParams,
                            const Vector<llvm::Value*>&   values,
                            bool                          localCall)
{
    const int   ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = *static_cast<LLVMEncoder*>(perThread[ct][precompileIndex]);
    auto&       builder         = *pp.builder;
    auto&       modu            = *pp.llvmModule;

    // Get parameters
    VectorNative<llvm::Value*> params;
    emitCallParameters(buildParameters, allocR, allocRR, typeFuncBC, params, pushParams, values);

    // Make the call
    const auto typeF = getOrCreateFuncType(buildParameters, typeFuncBC);
    auto       func  = modu.getOrInsertFunction(funcName.c_str(), typeF);
    const auto F     = llvm::dyn_cast<llvm::Function>(func.getCallee());

    // Why this can be null ????
    if (F && !localCall)
        F->setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);

    return builder.CreateCall(func, {params.begin(), params.end()});
}

llvm::Value* LLVM::emitCall(const BuildParameters&      buildParameters,
                            const char*                 name,
                            llvm::AllocaInst*           allocR,
                            llvm::AllocaInst*           allocT,
                            const Vector<uint32_t>&     regs,
                            const Vector<llvm::Value*>& values)
{
    const auto typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(name);
    getOrCreateFuncType(buildParameters, typeFunc);

    // Invert regs
    VectorNative<uint32_t> pushRAParams;
    for (int i = static_cast<int>(regs.size()) - 1; i >= 0; i--)
        pushRAParams.push_back(regs[i]);
    Vector<llvm::Value*> pushVParams;
    for (int i = static_cast<int>(values.size()) - 1; i >= 0; i--)
        pushVParams.push_back(values[i]);

    return emitCall(buildParameters, name, typeFunc, allocR, allocT, pushRAParams, pushVParams, true);
}

void LLVM::emitByteCodeCallParameters(const BuildParameters&      buildParameters,
                                      llvm::AllocaInst*           allocR,
                                      llvm::AllocaInst*           allocRR,
                                      const llvm::AllocaInst*     allocT,
                                      VectorNative<llvm::Value*>& params,
                                      TypeInfoFuncAttr*           typeFuncBC,
                                      VectorNative<uint32_t>&     pushRAParams,
                                      const Vector<llvm::Value*>& values,
                                      bool                        closureToLambda)
{
    const int   ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = *static_cast<LLVMEncoder*>(perThread[ct][precompileIndex]);
    auto&       builder         = *pp.builder;
    auto&       context         = *pp.llvmContext;
    int         popRAidx        = static_cast<int>(pushRAParams.size()) - 1;
    int         numCallParams   = static_cast<int>(typeFuncBC->parameters.size());

    // Return value
    // Normal user case
    if (allocRR)
    {
        for (uint32_t j = 0; j < typeFuncBC->numReturnRegisters(); j++)
        {
            params.push_back(GEP64(allocRR, j));
        }
    }

    // Special case when calling an internal function
    else
    {
        for (uint32_t j = 0; j < typeFuncBC->numReturnRegisters(); j++)
        {
            const auto index = pushRAParams[popRAidx--];
            if (index == UINT32_MAX)
                params.push_back(values[popRAidx + 1]);
            else
                params.push_back(GEP64(allocR, index));
        }
    }

    // Two registers for variadic first
    if (typeFuncBC->isFctVariadic())
    {
        auto index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        numCallParams--;
    }
    else if (typeFuncBC->isFctCVariadic())
    {
        numCallParams--;
    }

    int idxFirst = 0;
    if (typeFuncBC->isClosure() && closureToLambda)
    {
        idxFirst = 1;
        popRAidx--;
    }

    for (int idxCall = idxFirst; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteType(typeParam);
        if (typeParam->isAutoConstPointerRef())
            typeParam = TypeManager::concretePtrRef(typeParam);

        for (uint32_t j = 0; j < typeParam->numRegisters(); j++)
        {
            const auto index = pushRAParams[popRAidx--];

            // By value
            if (typeParam->numRegisters() == 1)
            {
                const auto ty = swagTypeToLLVMType(buildParameters, typeParam);
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
    if (typeFuncBC->isFctCVariadic())
    {
        const auto numVariadics = popRAidx + 1;
        for (int idxCall = 0; idxCall < numVariadics; idxCall++)
        {
            const auto index = pushRAParams[popRAidx--];
            const auto v0    = builder.CreateLoad(I64_TY(), GEP64(allocR, index));
            params.push_back(v0);
        }
    }
}
