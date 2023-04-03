#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVM_Macros.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "Report.h"

llvm::FunctionType* BackendLLVM::getOrCreateFuncType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, bool closureToLambda)
{
    const auto& cc              = g_CallConv[typeFunc->callConv];
    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       context         = *pp.context;

    // Already done ?
    if (closureToLambda)
    {
        auto it = pp.mapFctTypeForeignClosure.find(typeFunc);
        if (it != pp.mapFctTypeForeignClosure.end())
            return it->second;
    }
    else
    {
        auto it = pp.mapFctTypeForeign.find(typeFunc);
        if (it != pp.mapFctTypeForeign.end())
            return it->second;
    }

    VectorNative<llvm::Type*> params;
    llvm::Type*               returnType   = nullptr;
    bool                      returnByCopy = typeFunc->returnByCopy();

    llvm::Type* llvmRealReturnType = swagTypeToLLVMType(buildParameters, moduleToGen, typeFunc->returnType);
    if (returnByCopy)
        returnType = VOID_TY();
    else
        returnType = llvmRealReturnType;

    if (typeFunc->parameters.size())
    {
        auto numParams = typeFunc->parameters.size();

        // Variadic parameters are always first
        if (typeFunc->isVariadic())
        {
            params.push_back(PTR_I8_TY());
            params.push_back(I64_TY());
            SWAG_ASSERT(numParams);
            numParams--;
        }
        else if (typeFunc->isCVariadic())
        {
            SWAG_ASSERT(numParams);
            numParams--;
        }

        int idxFirst = 0;
        if (typeFunc->isClosure() && closureToLambda)
            idxFirst = 1;

        for (int i = idxFirst; i < numParams; i++)
        {
            auto param = TypeManager::concreteType(typeFunc->parameters[i]->typeInfo);
            if (param->isAutoConstPointerRef())
                param = TypeManager::concretePtrRef(param);

            if (cc.structByRegister && param->isStruct() && param->sizeOf <= sizeof(void*))
            {
                params.push_back(IX_TY(param->sizeOf * 8));
            }
            else if (param->isString() || param->isSlice())
            {
                auto cType = swagTypeToLLVMType(buildParameters, moduleToGen, param);
                params.push_back(cType);
                params.push_back(I64_TY());
            }
            else if (param->isAny() || param->isInterface())
            {
                auto cType = swagTypeToLLVMType(buildParameters, moduleToGen, param);
                params.push_back(cType);
                params.push_back(PTR_I8_TY());
            }
            else
            {
                auto cType = swagTypeToLLVMType(buildParameters, moduleToGen, param);
                params.push_back(cType);
            }
        }
    }

    // Return value by copy is last
    if (returnByCopy)
        params.push_back(llvmRealReturnType);

    auto result = llvm::FunctionType::get(returnType, {params.begin(), params.end()}, typeFunc->isCVariadic());

    if (closureToLambda)
        pp.mapFctTypeForeignClosure[typeFunc] = result;
    else
        pp.mapFctTypeForeign[typeFunc] = result;

    return result;
}

bool BackendLLVM::emitGetParam(llvm::LLVMContext&     context,
                               const BuildParameters& buildParameters,
                               llvm::Function*        func,
                               TypeInfoFuncAttr*      typeFunc,
                               int                    idx,
                               llvm::Value*           r0,
                               int                    sizeOf,
                               uint64_t               toAdd,
                               int                    deRefSize)
{
    const auto& cc              = g_CallConv[typeFunc->callConv];
    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       builder         = *pp.builder;

    auto param = typeFunc->registerIdxToType(idx);
    if (param->isAutoConstPointerRef())
        param = TypeManager::concretePtrRef(param);

    auto arg = func->getArg(idx);

    // First two parameters are occupied by the variadic slice
    if (typeFunc->isVariadic())
        idx += 2;

    if (toAdd || deRefSize)
    {
        llvm::Value* ra = nullptr;

        if (cc.structByRegister && param->isStruct() && param->sizeOf <= sizeof(void*))
        {
            ra = builder.CreateIntCast(arg, I64_TY(), false);

            // If this is a value, then we just have to shift it on the right with the given amount of bits
            if (deRefSize)
            {
                if (toAdd)
                    ra = builder.CreateLShr(ra, builder.getInt64(toAdd * 8));

                // We need to mask in order to derefence only the correct value
                if (param->sizeOf != (uint32_t) deRefSize)
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

                builder.CreateStore(ra, r0);
            }

            // We need the pointer
            else
            {
                auto allocR = builder.CreateAlloca(I64_TY(), builder.getInt32(1));
                allocR->setAlignment(llvm::Align(sizeof(void*)));
                builder.CreateStore(ra, allocR);
                ra = builder.CreateInBoundsGEP(I8_TY(), allocR, builder.getInt64(toAdd));
                builder.CreateStore(ra, r0);
            }

            return true;
        }

        if (param->numRegisters() == 1)
        {
            ra = builder.CreateInBoundsGEP(I8_TY(), arg, builder.getInt64(toAdd));
        }
        else
        {
            ra = builder.CreateIntToPtr(arg, PTR_I8_TY());
            ra = builder.CreateInBoundsGEP(I8_TY(), ra, builder.getInt64(toAdd));
        }

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
            }

            builder.CreateStore(ra, r0);
        }
        else
        {
            builder.CreateStore(ra, TO_PTR_PTR_I8(r0));
        }
    }
    else if (param->numRegisters() == 1)
    {
        // By convention, all remaining bits should be zero
        if (param->isNativeIntegerSigned() && param->sizeOf < sizeof(void*))
            builder.CreateStore(builder.CreateIntCast(arg, I64_TY(), true), r0);
        else if (param->isNativeIntegerUnsigned() && param->sizeOf < sizeof(void*))
            builder.CreateStore(builder.CreateIntCast(arg, I64_TY(), false), r0);

        // Struct by copy
        else if (cc.structByRegister && param->isStruct() && param->sizeOf <= sizeof(void*))
        {
            // Make a copy of the value on the stack, and return the address
            auto allocR = builder.CreateAlloca(I64_TY(), builder.getInt32(1));
            allocR->setAlignment(llvm::Align(sizeof(void*)));
            builder.CreateStore(builder.CreateIntCast(arg, I64_TY(), false), allocR);
            builder.CreateStore(allocR, TO_PTR_PTR_I64(r0));
        }

        // This can be casted to an integer
        else if (sizeOf)
        {
            if (sizeOf == sizeof(void*))
                builder.CreateStore(arg, r0);
            else
                builder.CreateStore(builder.CreateIntCast(arg, I64_TY(), false), r0);
        }

        // Real type
        else
        {
            auto ty = swagTypeToLLVMType(buildParameters, module, param);
            r0      = builder.CreatePointerCast(r0, ty->getPointerTo());
            builder.CreateStore(arg, r0);
        }
    }
    else
    {
        if (arg->getType()->isPointerTy())
            builder.CreateStore(arg, builder.CreatePointerCast(r0, arg->getType()->getPointerTo()));
        else
            builder.CreateStore(arg, r0);
    }

    return true;
}

bool BackendLLVM::emitCallParameters(const BuildParameters&        buildParameters,
                                     llvm::AllocaInst*             allocR,
                                     llvm::AllocaInst*             allocRR,
                                     Module*                       moduleToGen,
                                     TypeInfoFuncAttr*             typeFuncBC,
                                     VectorNative<llvm::Value*>&   params,
                                     const VectorNative<uint32_t>& pushParams,
                                     const Vector<llvm::Value*>&   values,
                                     bool                          closureToLambda)
{
    const auto& cc              = g_CallConv[typeFuncBC->callConv];
    int         ct              = buildParameters.compileType;
    int         precompileIndex = buildParameters.precompileIndex;
    auto&       pp              = *perThread[ct][precompileIndex];
    auto&       context         = *pp.context;
    auto&       builder         = *pp.builder;

    int numCallParams = (int) typeFuncBC->parameters.size();
    int idxParam      = (int) pushParams.size() - 1;

    // Variadic are first
    if (typeFuncBC->isVariadic())
    {
        auto index = pushParams[idxParam--];
        params.push_back(builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index)));
        index = pushParams[idxParam--];
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        SWAG_ASSERT(numCallParams);
        numCallParams--;
    }
    else if (typeFuncBC->isCVariadic())
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
                index = pushParams[idxParam--];
                params.push_back(values[idxParam + 1]);
            }
        }
        else if (typeParam->isPointer())
        {
            auto typePtr  = CastTypeInfo<TypeInfoPointer>(typeParam, TypeInfoKind::Pointer);
            auto llvmType = swagTypeToLLVMType(buildParameters, moduleToGen, typePtr);
            params.push_back(builder.CreateLoad(llvmType, GEP64(allocR, index)));
        }
        else if (cc.structByRegister && typeParam->isStruct() && typeParam->sizeOf <= sizeof(void*))
        {
            auto v0 = builder.CreateLoad(PTR_I8_TY(), GEP64(allocR, index));
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
            params.push_back(builder.CreateLoad(toNativeTy(context, typeParam->nativeType), GEP64(allocR, index)));
        }
        else
        {
            return Report::internalError(typeFuncBC->declNode, "emitCall, invalid param type");
        }
    }

    // Return by parameter
    auto returnType = TypeManager::concreteType(typeFuncBC->returnType);
    if (returnType->isSlice() ||
        returnType->isInterface() ||
        returnType->isAny() ||
        returnType->isString())
    {
        params.push_back(TO_PTR_I8(allocRR));
    }
    else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        params.push_back(builder.CreateLoad(PTR_I8_TY(), allocRR));
    }

    // Add all C variadic parameters
    if (typeFuncBC->isCVariadic())
    {
        for (int i = typeFuncBC->numParamsRegisters(); i < pushParams.size(); i++)
        {
            auto index = pushParams[idxParam--];
            params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        }
    }

    return true;
}

bool BackendLLVM::emitCallReturnValue(const BuildParameters& buildParameters,
                                      llvm::AllocaInst*      allocRR,
                                      Module*                moduleToGen,
                                      TypeInfoFuncAttr*      typeFuncBC,
                                      llvm::Value*           callResult)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;

    auto returnType = TypeManager::concreteType(typeFuncBC->returnType);
    if (returnType != g_TypeMgr->typeInfoVoid)
    {
        if ((returnType->isSlice()) ||
            (returnType->isInterface()) ||
            (returnType->isAny()) ||
            (returnType->isString()) ||
            (returnType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            // Return by parameter
        }
        else if (returnType->isPointer())
        {
            builder.CreateStore(TO_PTR_I8(callResult), TO_PTR_PTR_I8(allocRR));
        }
        else if (returnType->isNative())
        {
            auto r = TO_PTR_NATIVE(allocRR, returnType->nativeType);
            if (!r)
                return Report::internalError(typeFuncBC->declNode, "emitCall, invalid return type");
            builder.CreateStore(callResult, r);
        }
        else
        {
            return Report::internalError(typeFuncBC->declNode, "emitCall, invalid return type");
        }
    }

    return true;
}

llvm::Value* BackendLLVM::emitCall(const BuildParameters&        buildParameters,
                                   Module*                       moduleToGen,
                                   const Utf8&                   funcName,
                                   TypeInfoFuncAttr*             typeFuncBC,
                                   llvm::AllocaInst*             allocR,
                                   llvm::AllocaInst*             allocRR,
                                   const VectorNative<uint32_t>& pushParams,
                                   const Vector<llvm::Value*>&   values,
                                   bool                          localCall)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    // Get parameters
    VectorNative<llvm::Value*> params;
    emitCallParameters(buildParameters, allocR, allocRR, moduleToGen, typeFuncBC, params, pushParams, values);

    // Make the call
    auto typeF = getOrCreateFuncType(buildParameters, moduleToGen, typeFuncBC);
    auto func  = modu.getOrInsertFunction(funcName.c_str(), typeF);
    auto F     = llvm::dyn_cast<llvm::Function>(func.getCallee());

    // Why this can be null ????
    if (F && !localCall)
        F->setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);

    return builder.CreateCall(func, {params.begin(), params.end()});
}

llvm::Value* BackendLLVM::emitCall(const BuildParameters&      buildParameters,
                                   Module*                     moduleToGen,
                                   const char*                 name,
                                   llvm::AllocaInst*           allocR,
                                   llvm::AllocaInst*           allocT,
                                   const Vector<uint32_t>&     regs,
                                   const Vector<llvm::Value*>& values)
{
    auto                typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(name);
    llvm::FunctionType* FT;
    getOrCreateFuncType(buildParameters, moduleToGen, typeFunc, &FT);

    // Invert regs
    VectorNative<uint32_t> pushRAParams;
    for (int i = (int) regs.size() - 1; i >= 0; i--)
        pushRAParams.push_back(regs[i]);
    Vector<llvm::Value*> pushVParams;
    for (int i = (int) values.size() - 1; i >= 0; i--)
        pushVParams.push_back(values[i]);

    return emitCall(buildParameters, moduleToGen, name, typeFunc, allocR, allocT, pushRAParams, pushVParams, true);
}

void BackendLLVM::emitByteCodeCallParameters(const BuildParameters&      buildParameters,
                                             llvm::AllocaInst*           allocR,
                                             llvm::AllocaInst*           allocRR,
                                             llvm::AllocaInst*           allocT,
                                             VectorNative<llvm::Value*>& params,
                                             TypeInfoFuncAttr*           typeFuncBC,
                                             VectorNative<uint32_t>&     pushRAParams,
                                             const Vector<llvm::Value*>& values,
                                             bool                        closureToLambda)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    auto& context         = *pp.context;
    int   popRAidx        = (int) pushRAParams.size() - 1;
    int   numCallParams   = (int) typeFuncBC->parameters.size();

    // Return value
    // Normal user case
    if (allocRR)
    {
        for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
        {
            params.push_back(GEP64(allocRR, j));
        }
    }

    // Special case when calling an internal function
    else
    {
        for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];
            if (index == UINT32_MAX)
                params.push_back(values[popRAidx + 1]);
            else
                params.push_back(GEP64(allocR, index));
        }
    }

    // Two registers for variadics first
    if (typeFuncBC->isVariadic())
    {
        auto index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        params.push_back(builder.CreateLoad(I64_TY(), GEP64(allocR, index)));
        numCallParams--;
    }
    else if (typeFuncBC->isCVariadic())
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

        for (int j = 0; j < typeParam->numRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];

            // By value
            if (typeParam->numRegisters() == 1)
            {
                auto ty = swagTypeToLLVMType(buildParameters, module, typeParam);
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
                auto v0 = builder.CreateLoad(I64_TY(), GEP64(allocR, index));
                params.push_back(v0);
            }
        }
    }

    // C variadic arguments
    if (typeFuncBC->isCVariadic())
    {
        auto numVariadics = popRAidx + 1;
        for (int idxCall = 0; idxCall < numVariadics; idxCall++)
        {
            auto index = pushRAParams[popRAidx--];
            auto v0    = builder.CreateLoad(I64_TY(), GEP64(allocR, index));
            params.push_back(v0);
        }
    }
}