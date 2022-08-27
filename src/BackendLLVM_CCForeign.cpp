#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "BackendLLVM_Macros.h"
#include "Module.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

bool BackendLLVM::createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, llvm::FunctionType** result, bool closureToLambda)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    // Already done ?
    if (closureToLambda)
    {
        auto it = pp.mapFctTypeForeignClosure.find(typeFunc);
        if (it != pp.mapFctTypeForeignClosure.end())
        {
            *result = it->second;
            return true;
        }
    }
    else
    {
        auto it = pp.mapFctTypeForeign.find(typeFunc);
        if (it != pp.mapFctTypeForeign.end())
        {
            *result = it->second;
            return true;
        }
    }

    VectorNative<llvm::Type*> params;
    llvm::Type*               llvmRealReturnType = nullptr;
    llvm::Type*               returnType         = nullptr;
    bool                      returnByCopy       = typeFunc->returnByCopy();

    SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, typeFunc->returnType, &llvmRealReturnType));
    if (returnByCopy)
        returnType = builder.getVoidTy();
    else
        returnType = llvmRealReturnType;

    if (typeFunc->parameters.size())
    {
        // Variadic parameters are always first
        auto numParams = typeFunc->parameters.size();
        if (numParams)
        {
            auto param = typeFunc->parameters.back();
            if (param->typeInfo->kind == TypeInfoKind::Variadic || param->typeInfo->kind == TypeInfoKind::TypedVariadic)
            {
                params.push_back(builder.getInt8Ty()->getPointerTo());
                params.push_back(builder.getInt64Ty());
                numParams--;
            }
        }

        int idxFirst = 0;
        if (typeFunc->isClosure() && closureToLambda)
            idxFirst = 1;

        llvm::Type* cType = nullptr;
        for (int i = idxFirst; i < numParams; i++)
        {
            auto param = TypeManager::concreteReferenceType(typeFunc->parameters[i]->typeInfo);

            // :StructByCopy
            if (param->kind == TypeInfoKind::Struct && param->sizeOf <= sizeof(void*))
            {
                params.push_back(builder.getIntNTy(param->sizeOf * 8));
            }
            else if (param->isNative(NativeTypeKind::String) || param->kind == TypeInfoKind::Slice)
            {
                SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, param, &cType));
                params.push_back(cType);
                params.push_back(builder.getInt64Ty());
            }
            else if (param->isNative(NativeTypeKind::Any) || param->kind == TypeInfoKind::Interface)
            {
                SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, param, &cType));
                params.push_back(cType);
                params.push_back(builder.getInt8Ty()->getPointerTo());
            }
            else
            {
                SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, param, &cType));
                params.push_back(cType);
            }
        }
    }

    // Return value by copy is last
    if (returnByCopy)
        params.push_back(llvmRealReturnType);

    *result = llvm::FunctionType::get(returnType, {params.begin(), params.end()}, false);

    if (closureToLambda)
        pp.mapFctTypeForeignClosure[typeFunc] = *result;
    else
        pp.mapFctTypeForeign[typeFunc] = *result;

    return true;
}

bool BackendLLVM::getForeignCallParameters(const BuildParameters&        buildParameters,
                                           llvm::AllocaInst*             allocR,
                                           llvm::AllocaInst*             allocRR,
                                           Module*                       moduleToGen,
                                           TypeInfoFuncAttr*             typeFuncBC,
                                           VectorNative<llvm::Value*>&   params,
                                           const VectorNative<uint32_t>& pushParams,
                                           const vector<llvm::Value*>&   values,
                                           bool                          closureToLambda)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;

    int numCallParams = (int) typeFuncBC->parameters.size();
    int idxParam      = (int) pushParams.size() - 1;

    // Variadic are first
    if (numCallParams)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters.back()->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic || typeParam->kind == TypeInfoKind::TypedVariadic)
        {
            auto index = pushParams[idxParam--];
            auto r0    = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));

            index   = pushParams[idxParam--];
            auto r1 = GEP_I32(allocR, index);
            params.push_back(builder.CreateLoad(r1));
            numCallParams--;
        }
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
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[idxCall]->typeInfo);
        auto index     = pushParams[idxParam--];

        if (index == UINT32_MAX)
        {
            params.push_back(values[idxParam + 1]);
            if (typeParam->numRegisters() > 1)
            {
                index = pushParams[idxParam--];
                params.push_back(values[idxParam + 1]);
            }
        }
        else if (typeParam->kind == TypeInfoKind::Pointer)
        {
            auto        typePtr = CastTypeInfo<TypeInfoPointer>(typeParam, TypeInfoKind::Pointer);
            llvm::Type* llvmType;
            SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, typePtr, &llvmType));
            llvmType = llvmType->getPointerTo();
            auto r0  = GEP_I32(allocR, index);
            auto r   = builder.CreatePointerCast(r0, llvmType);
            params.push_back(builder.CreateLoad(r));
        }
        // :StructByCopy
        else if (typeParam->kind == TypeInfoKind::Struct && typeParam->sizeOf <= sizeof(void*))
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            auto v0 = builder.CreateLoad(r0);
            params.push_back(builder.CreateLoad(TO_PTR_IX(v0, typeParam->sizeOf * 8)));
        }
        else if (typeParam->kind == TypeInfoKind::Struct ||
                 typeParam->kind == TypeInfoKind::Lambda ||
                 typeParam->kind == TypeInfoKind::Array)
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));
        }
        else if (typeParam->isNative(NativeTypeKind::String) ||
                 typeParam->kind == TypeInfoKind::Slice)
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));

            index   = pushParams[idxParam--];
            auto r1 = GEP_I32(allocR, index);
            params.push_back(builder.CreateLoad(r1));
        }
        else if (typeParam->isNative(NativeTypeKind::Any) ||
                 typeParam->kind == TypeInfoKind::Interface)
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));

            index   = pushParams[idxParam--];
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r1));
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            auto r0 = GEP_I32(allocR, index);
            auto r  = TO_PTR_NATIVE(r0, typeParam->nativeType);
            if (!r)
                return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid param native type");
            params.push_back(builder.CreateLoad(r));
        }
        else
        {
            return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid param type");
        }
    }

    // Return by parameter
    auto returnType = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    if (returnType->kind == TypeInfoKind::Slice ||
        returnType->kind == TypeInfoKind::Interface ||
        returnType->isNative(NativeTypeKind::Any) ||
        returnType->isNative(NativeTypeKind::String))
    {
        params.push_back(TO_PTR_I8(allocRR));
    }
    else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        params.push_back(builder.CreateLoad(TO_PTR_PTR_I8(allocRR)));
    }

    return true;
}

bool BackendLLVM::getForeignCallReturnValue(const BuildParameters& buildParameters,
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
        else if (returnType->kind == TypeInfoKind::Pointer)
        {
            builder.CreateStore(TO_PTR_I8(callResult), TO_PTR_PTR_I8(allocRR));
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            auto r = TO_PTR_NATIVE(allocRR, returnType->nativeType);
            if (!r)
                return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid return type");
            builder.CreateStore(callResult, r);
        }
        else
        {
            return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid return type");
        }
    }
    return true;
}

bool BackendLLVM::emitForeignCall(const BuildParameters&        buildParameters,
                                  Module*                       moduleToGen,
                                  const Utf8&                   funcName,
                                  TypeInfoFuncAttr*             typeFuncBC,
                                  llvm::AllocaInst*             allocR,
                                  llvm::AllocaInst*             allocRR,
                                  const VectorNative<uint32_t>& pushParams,
                                  const vector<llvm::Value*>&   values,
                                  bool                          localCall)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    // Get parameters
    VectorNative<llvm::Value*> params;
    getForeignCallParameters(buildParameters, allocR, allocRR, moduleToGen, typeFuncBC, params, pushParams, values);

    // Make the call
    llvm::FunctionType* typeF;
    SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFuncBC, &typeF));
    auto func = modu.getOrInsertFunction(funcName.c_str(), typeF);
    auto F    = llvm::dyn_cast<llvm::Function>(func.getCallee());

    // Why this can be null ????
    if (F && !localCall)
        F->setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);

    auto result = builder.CreateCall(func, {params.begin(), params.end()});

    // Store result to rt0
    SWAG_CHECK(getForeignCallReturnValue(buildParameters, allocRR, moduleToGen, typeFuncBC, result));

    return true;
}
