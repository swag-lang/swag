#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "BackendLLVM_Macros.h"
#include "Module.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "Workspace.h"

llvm::FunctionType* BackendLLVM::createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, bool closureToLambda)
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
                auto cType = swagTypeToLLVMType(buildParameters, moduleToGen, param);
                params.push_back(cType);
                params.push_back(builder.getInt64Ty());
            }
            else if (param->isNative(NativeTypeKind::Any) || param->kind == TypeInfoKind::Interface)
            {
                auto cType = swagTypeToLLVMType(buildParameters, moduleToGen, param);
                params.push_back(cType);
                params.push_back(builder.getInt8Ty()->getPointerTo());
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

    auto result = llvm::FunctionType::get(returnType, {params.begin(), params.end()}, false);

    if (closureToLambda)
        pp.mapFctTypeForeignClosure[typeFunc] = result;
    else
        pp.mapFctTypeForeign[typeFunc] = result;

    return result;
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
            auto typePtr  = CastTypeInfo<TypeInfoPointer>(typeParam, TypeInfoKind::Pointer);
            auto llvmType = swagTypeToLLVMType(buildParameters, moduleToGen, typePtr);
            llvmType      = llvmType->getPointerTo();
            auto r0       = GEP_I32(allocR, index);
            auto r        = builder.CreatePointerCast(r0, llvmType);
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

llvm::Value* BackendLLVM::emitForeignCall(const BuildParameters&        buildParameters,
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
    auto typeF = createFunctionTypeForeign(buildParameters, moduleToGen, typeFuncBC);
    auto func  = modu.getOrInsertFunction(funcName.c_str(), typeF);
    auto F     = llvm::dyn_cast<llvm::Function>(func.getCallee());

    // Why this can be null ????
    if (F && !localCall)
        F->setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);

    return builder.CreateCall(func, {params.begin(), params.end()});

    // Store result to rt0
    // SWAG_CHECK(getForeignCallReturnValue(buildParameters, allocRR, moduleToGen, typeFuncBC, result));
    // return true;
}

llvm::Value* BackendLLVM::localCall(const BuildParameters&      buildParameters,
                                    Module*                     moduleToGen,
                                    const char*                 name,
                                    llvm::AllocaInst*           allocR,
                                    llvm::AllocaInst*           allocT,
                                    const vector<uint32_t>&     regs,
                                    const vector<llvm::Value*>& values)
{
    auto                typeFunc = g_Workspace->runtimeModule->getRuntimeTypeFct(name);
    llvm::FunctionType* FT;
    createFunctionTypeForeign(buildParameters, moduleToGen, typeFunc, &FT);

    // Invert regs
    VectorNative<uint32_t> pushRAParams;
    for (int i = (int) regs.size() - 1; i >= 0; i--)
        pushRAParams.push_back(regs[i]);
    vector<llvm::Value*> pushVParams;
    for (int i = (int) values.size() - 1; i >= 0; i--)
        pushVParams.push_back(values[i]);

    return emitForeignCall(buildParameters, moduleToGen, name, typeFunc, allocR, allocT, pushRAParams, pushVParams, true);
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
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    auto param = typeFunc->registerIdxToType(idx);
    auto arg   = func->getArg(idx);

    // First two parameters are occupied by the variadic slice
    if (typeFunc->isVariadic())
        idx += 2;

    if (toAdd || deRefSize)
    {
        llvm::Value* ra;
        if (passByValue(param))
            ra = builder.CreateInBoundsGEP(TO_PTR_I8(arg), builder.getInt64(toAdd));
        else
        {
            ra = builder.CreateIntToPtr(arg, builder.getInt8PtrTy());
            ra = builder.CreateInBoundsGEP(ra, builder.getInt64(toAdd));
        }
        if (deRefSize)
        {
            llvm::Value* v1;
            switch (deRefSize)
            {
            case 1:
                v1 = builder.CreateLoad(TO_PTR_I8(ra));
                ra = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
                break;
            case 2:
                v1 = builder.CreateLoad(TO_PTR_I16(ra));
                ra = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
                break;
            case 4:
                v1 = builder.CreateLoad(TO_PTR_I32(ra));
                ra = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
                break;
            case 8:
                ra = builder.CreateLoad(TO_PTR_I64(ra));
                break;
            }

            builder.CreateStore(ra, r0);
        }
        else
        {
            builder.CreateStore(ra, TO_PTR_PTR_I8(r0));
        }
    }
    else if (passByValue(param))
    {
        // By convention, all remaining bits should be zero
        if (param->isNativeIntegerSigned() && param->sizeOf < sizeof(void*))
            builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), true), r0);
        else if (param->isNativeIntegerUnsigned() && param->sizeOf < sizeof(void*))
            builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), false), r0);
        // :StructByCopy
        else if (param->kind == TypeInfoKind::Struct && param->sizeOf <= sizeof(void*))
            builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), false), r0);

        // This can be casted to an integer
        else if (sizeOf)
        {
            if (sizeOf == sizeof(void*))
                builder.CreateStore(arg, r0);
            else
                builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), false), r0);
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
