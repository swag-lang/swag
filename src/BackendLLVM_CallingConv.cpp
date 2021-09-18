#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "BackendLLVM_Macros.h"
#include "Module.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "LanguageSpec.h"

bool BackendLLVM::emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    // Function type
    llvm::FunctionType* funcType = nullptr;
    SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFunc, &funcType));

    // Create function
    node->computeFullNameForeign(true);
    llvm::Function* func = (llvm::Function*) modu.getOrInsertFunction(node->fullnameForeign.c_str(), funcType).getCallee();
    func->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    // Content
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(block);

    // Debug infos
    if (pp.dbg)
        pp.dbg->startWrapperFunction(pp, bc, node, func);

    // Total number of registers to store results and parameters
    auto numTotalRegisters = typeFunc->numTotalRegisters();
    auto allocRR           = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(numTotalRegisters));

    // Set all registers

    // Return by copy
    auto returnType   = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM | CONCRETE_FORCEALIAS);
    bool returnByCopy = returnType->flags & TYPEINFO_RETURN_BY_COPY;
    if (returnByCopy)
    {
        // rr0 = result
        auto rr0  = TO_PTR_PTR_I8(allocRR);
        auto cst0 = TO_PTR_I8(func->getArg((int) func->arg_size() - 1));
        builder.CreateStore(cst0, rr0);
    }

    auto numParams = typeFunc->parameters.size();

    // Variadics
    auto numReturnRegs = typeFunc->numReturnRegisters();
    int  idx           = numReturnRegs;
    int  argIdx        = 0;
    if (numParams)
    {
        auto param     = typeFunc->parameters.back();
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic || typeParam->kind == TypeInfoKind::TypedVariadic)
        {
            auto rr0  = TO_PTR_PTR_I8(GEP_I32(allocRR, idx));
            auto cst0 = TO_PTR_I8(func->getArg(0));
            builder.CreateStore(cst0, rr0);

            auto rr1 = GEP_I32(allocRR, idx + 1);
            builder.CreateStore(func->getArg(1), rr1);

            idx += 2;
            argIdx += 2;
            numParams--;
        }
    }

    // Parameters
    for (int i = 0; i < numParams; i++)
    {
        auto param     = typeFunc->parameters[i];
        auto typeParam = TypeManager::concreteType(param->typeInfo);
        auto rr0       = GEP_I32(allocRR, idx);

        if (typeParam->kind == TypeInfoKind::Pointer ||
            typeParam->kind == TypeInfoKind::Lambda ||
            typeParam->kind == TypeInfoKind::Struct ||
            typeParam->kind == TypeInfoKind::Array ||
            typeParam->kind == TypeInfoKind::Reference)
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
        }
        else if (typeParam->isNative(NativeTypeKind::String) ||
                 typeParam->kind == TypeInfoKind::Slice)
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
            auto rr1 = GEP_I32(allocRR, idx + 1);
            builder.CreateStore(func->getArg(argIdx + 1), rr1);
        }
        else if (typeParam->isNative(NativeTypeKind::Any) ||
                 typeParam->kind == TypeInfoKind::Interface)
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
            auto cst1 = TO_PTR_I8(func->getArg(argIdx + 1));
            auto rr1  = GEP_I32(allocRR, idx + 1);
            builder.CreateStore(cst1, TO_PTR_PTR_I8(rr1));
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            if (!passByValue(typeParam))
            {
                auto r = TO_PTR_NATIVE(rr0, typeParam->nativeType);
                if (!r)
                    return moduleToGen->internalError("emitFuncWrapperPublic, invalid param type");
                builder.CreateStore(func->getArg(argIdx), r);
            }
        }
        else
        {
            return moduleToGen->internalError(Utf8::format("emitFuncWrapperPublic, invalid param type `%s`", typeParam->name.c_str()));
        }

        idx += typeParam->numRegisters();
        argIdx += typeParam->numRegisters();
    }

    // Set all parameters
    VectorNative<llvm::Value*> args;
    for (int i = 0; i < numTotalRegisters; i++)
    {
        if (i < numReturnRegs)
        {
            auto rr0 = builder.CreateInBoundsGEP(allocRR, builder.getInt32(i));
            args.push_back(rr0);
        }
        else
        {
            auto typeParam = typeFunc->registerIdxToType(i - numReturnRegs);
            if (passByValue(typeParam))
                args.push_back(func->getArg(i - numReturnRegs));
            else
            {
                auto rr0 = builder.CreateInBoundsGEP(allocRR, builder.getInt32(i));
                args.push_back(rr0);
            }
        }
    }

    // Make the call
    auto fcc = modu.getFunction(bc->getCallName().c_str());
    builder.CreateCall(fcc, {args.begin(), args.end()});

    // Return value
    // If we return by copy, then this has already been copied to the correct
    // destination address, so nothing to do.
    if (numReturnRegs && !returnByCopy)
    {
        if (numReturnRegs == 1)
        {
            if (returnType->kind == TypeInfoKind::Pointer || returnType->kind == TypeInfoKind::Reference)
            {
                auto loadInst = builder.CreateLoad(allocRR);
                auto arg0     = builder.CreateCast(llvm::Instruction::CastOps::IntToPtr, loadInst, funcType->getReturnType());
                builder.CreateRet(arg0);
            }
            else if (returnType->kind == TypeInfoKind::Native)
            {
                auto r        = TO_PTR_NATIVE(allocRR, returnType->nativeType);
                auto loadInst = builder.CreateLoad(r);
                builder.CreateRet(loadInst);
            }
            else
            {
                return moduleToGen->internalError(Utf8::format("emitFuncWrapperPublic, invalid return type `%s`", returnType->getDisplayName().c_str()));
            }
        }
        else if (numReturnRegs == 2)
        {
            //*((void **) result) = rr0.pointer
            auto loadInst = builder.CreateLoad(allocRR);
            auto arg0     = TO_PTR_I64(func->getArg((int) func->arg_size() - 1));
            builder.CreateStore(loadInst, arg0);

            //*((void **) result + 1) = rr1.pointer
            auto rr1  = builder.CreateInBoundsGEP(allocRR, pp.cst1_i32);
            auto arg1 = builder.CreateInBoundsGEP(arg0, pp.cst1_i32);
            loadInst  = builder.CreateLoad(rr1);
            builder.CreateStore(loadInst, arg1);

            builder.CreateRetVoid();
        }
    }
    else
    {
        builder.CreateRetVoid();
    }

    return true;
}

void BackendLLVM::getLocalCallParameters(const BuildParameters&      buildParameters,
                                         llvm::AllocaInst*           allocR,
                                         llvm::AllocaInst*           allocRR,
                                         llvm::AllocaInst*           allocT,
                                         VectorNative<llvm::Value*>& params,
                                         TypeInfoFuncAttr*           typeFuncBC,
                                         VectorNative<uint32_t>&     pushRAParams,
                                         const vector<llvm::Value*>& values)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    int   popRAidx        = (int) pushRAParams.size() - 1;
    int   numCallParams   = (int) typeFuncBC->parameters.size();

    if (allocRR)
    {
        for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
        {
            auto r0 = GEP_I32(allocRR, j);
            params.push_back(r0);
        }
    }
    else
    {
        for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];
            if (index == UINT32_MAX)
            {
                auto v0 = values[popRAidx + 1];
                SWAG_ASSERT(v0);
                params.push_back(v0);
            }
            else
            {
                auto r0 = GEP_I32(allocR, index);
                params.push_back(r0);
            }
        }
    }

    // 2 registers for variadics first
    if (typeFuncBC->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        auto index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        auto r0 = GEP_I32(allocR, index);
        params.push_back(r0);
        index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        r0 = GEP_I32(allocR, index);
        params.push_back(r0);
        numCallParams--;
    }

    int allocTidx = 0;
    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        for (int j = 0; j < typeParam->numRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];

            // By value
            if (passByValue(typeParam))
            {
                llvm::Type* ty;
                swagTypeToLLVMType(buildParameters, module, typeParam, &ty);
                if (index == UINT32_MAX)
                {
                    auto v0 = values[popRAidx + 1];
                    SWAG_ASSERT(v0);
                    params.push_back(v0);
                }
                else
                {
                    auto r0 = builder.CreatePointerCast(GEP_I32(allocR, index), ty->getPointerTo());
                    params.push_back(builder.CreateLoad(r0));
                }

                continue;
            }

            // By register pointer. If we have a value and not a register, store the value in a temporary register
            if (index == UINT32_MAX)
            {
                auto v0 = values[popRAidx + 1];
                SWAG_ASSERT(v0);
                if (allocT)
                {
                    auto p0 = GEP_I32(allocT, allocTidx++);
                    if (v0->getType()->isPointerTy())
                        v0 = builder.CreatePtrToInt(v0, builder.getInt64Ty());
                    else if (v0->getType()->isIntegerTy())
                        v0 = builder.CreateIntCast(v0, builder.getInt64Ty(), false);
                    builder.CreateStore(v0, p0);
                    params.push_back(p0);
                }
                else
                {
                    params.push_back(v0);
                }
            }
            // By register pointer.
            else
            {
                params.push_back(GEP_I32(allocR, index));
            }
        }
    }
}

llvm::FunctionType* BackendLLVM::createFunctionTypeInternal(const BuildParameters& buildParameters, int numParams)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;

    VectorNative<llvm::Type*> params;
    for (int i = 0; i < numParams; i++)
        params.push_back(llvm::Type::getInt64PtrTy(context));

    return llvm::FunctionType::get(llvm::Type::getVoidTy(context), {params.begin(), params.end()}, false);
}

llvm::FunctionType* BackendLLVM::createFunctionTypeInternal(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFuncBC)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;

    // Already done ?
    auto it = pp.mapFctTypeInternal.find(typeFuncBC);
    if (it != pp.mapFctTypeInternal.end())
        return it->second;

    // Registers to get the result
    VectorNative<llvm::Type*> params;
    for (int i = 0; i < typeFuncBC->numReturnRegisters(); i++)
        params.push_back(llvm::Type::getInt64PtrTy(context));

    // Variadics first
    int numParams = (int) typeFuncBC->parameters.size();
    if (typeFuncBC->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        params.push_back(llvm::Type::getInt64PtrTy(context));
        params.push_back(llvm::Type::getInt64PtrTy(context));
        numParams--;
    }

    for (int i = 0; i < numParams; i++)
    {
        auto param     = typeFuncBC->parameters[i];
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (passByValue(typeParam))
        {
            llvm::Type* ty;
            swagTypeToLLVMType(buildParameters, module, typeParam, &ty);
            params.push_back(ty);
        }
        else
        {
            for (int r = 0; r < typeParam->numRegisters(); r++)
                params.push_back(llvm::Type::getInt64PtrTy(context));
        }
    }

    auto result                       = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {params.begin(), params.end()}, false);
    pp.mapFctTypeInternal[typeFuncBC] = result;
    return result;
}

bool BackendLLVM::createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, llvm::FunctionType** result)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    // Already done ?
    auto it = pp.mapFctTypeForeign.find(typeFunc);
    if (it != pp.mapFctTypeForeign.end())
    {
        *result = it->second;
        return true;
    }

    VectorNative<llvm::Type*> params;
    llvm::Type*               llvmRealReturnType = nullptr;
    llvm::Type*               returnType         = nullptr;
    bool                      returnByCopy       = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;

    SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, typeFunc->returnType, &llvmRealReturnType));
    if (returnByCopy)
        returnType = builder.getVoidTy();
    else if (typeFunc->numReturnRegisters() <= 1)
        returnType = llvmRealReturnType;
    else
        returnType = builder.getVoidTy();

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

        llvm::Type* cType = nullptr;
        for (int i = 0; i < numParams; i++)
        {
            auto param = typeFunc->parameters[i]->typeInfo;

            SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, param, &cType));
            params.push_back(cType);

            // Additional parameter
            if (param->isNative(NativeTypeKind::String) ||
                param->kind == TypeInfoKind::Slice)
            {
                params.push_back(builder.getInt64Ty());
            }
            else if (param->isNative(NativeTypeKind::Any) ||
                     param->kind == TypeInfoKind::Interface)
            {
                params.push_back(builder.getInt8Ty()->getPointerTo());
            }
        }
    }

    // Return value
    if (typeFunc->numReturnRegisters() > 1 || returnByCopy)
    {
        params.push_back(llvmRealReturnType);
    }

    *result = llvm::FunctionType::get(returnType, {params.begin(), params.end()}, false);

    pp.mapFctTypeForeign[typeFunc] = *result;

    return true;
}

bool BackendLLVM::getForeignCallParameters(const BuildParameters&        buildParameters,
                                           llvm::AllocaInst*             allocR,
                                           llvm::AllocaInst*             allocRR,
                                           Module*                       moduleToGen,
                                           TypeInfoFuncAttr*             typeFuncBC,
                                           VectorNative<llvm::Value*>&   params,
                                           const VectorNative<uint32_t>& pushParams)
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

    // All parameters
    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[idxCall]->typeInfo);

        auto index = pushParams[idxParam--];

        if (typeParam->kind == TypeInfoKind::Pointer)
        {
            auto        typePtr = CastTypeInfo<TypeInfoPointer>(typeParam, TypeInfoKind::Pointer);
            llvm::Type* llvmType;
            SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, typePtr, &llvmType));
            llvmType = llvmType->getPointerTo();
            auto r0  = GEP_I32(allocR, index);
            auto r   = builder.CreatePointerCast(r0, llvmType);
            params.push_back(builder.CreateLoad(r));
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
                                  llvm::AllocaInst*             allocR,
                                  llvm::AllocaInst*             allocRR,
                                  Module*                       moduleToGen,
                                  ByteCodeInstruction*          ip,
                                  const VectorNative<uint32_t>& pushParams)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    auto              nodeFunc   = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

    // Get function name
    Utf8 funcName;
    auto foreignValue = typeFuncBC->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_function);
    if (foreignValue && !foreignValue->text.empty())
        funcName = foreignValue->text;
    else
        funcName = nodeFunc->token.text;

    // Get parameters
    VectorNative<llvm::Value*> params;
    getForeignCallParameters(buildParameters, allocR, allocRR, moduleToGen, typeFuncBC, params, pushParams);

    // Make the call
    llvm::FunctionType* typeF;
    SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFuncBC, &typeF));
    auto func = modu.getOrInsertFunction(funcName.c_str(), typeF);
    auto F    = llvm::dyn_cast<llvm::Function>(func.getCallee());

    // Why this can be null ????
    if (F)
        F->setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);

    auto result = builder.CreateCall(func, {params.begin(), params.end()});

    // Store result to rt0
    SWAG_CHECK(getForeignCallReturnValue(buildParameters, allocRR, moduleToGen, typeFuncBC, result));

    return true;
}

bool BackendLLVM::storeLocalParam(const BuildParameters& buildParameters, llvm::Function* func, TypeInfoFuncAttr* typeFunc, int idx, llvm::Value* r0, int sizeInBytes)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    auto param  = typeFunc->registerIdxToType(idx);
    auto offArg = idx + typeFunc->numReturnRegisters();
    auto arg    = func->getArg(offArg);

    if (passByValue(param))
    {
        llvm::Type* ty = nullptr;
        SWAG_CHECK(swagTypeToLLVMType(buildParameters, module, param, &ty));
        if (sizeInBytes)
        {
            builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), false), r0);
        }
        else
        {
            r0 = builder.CreatePointerCast(r0, ty->getPointerTo());
            builder.CreateStore(arg, r0);
        }
    }
    else
    {
        SWAG_ASSERT(sizeInBytes == 0);
        builder.CreateStore(builder.CreateLoad(arg), r0);
    }

    return true;
}

void BackendLLVM::localCall(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const char* name, const vector<uint32_t>& regs, const vector<llvm::Value*>& values)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    auto typeFuncBC = g_Workspace->runtimeModule->getRuntimeTypeFct(name);
    auto FT         = createFunctionTypeInternal(buildParameters, typeFuncBC);

    // Invert regs
    VectorNative<uint32_t> pushRAParams;
    for (int i = (int) regs.size() - 1; i >= 0; i--)
        pushRAParams.push_back(regs[i]);
    vector<llvm::Value*> pushVParams;
    for (int i = (int) values.size() - 1; i >= 0; i--)
        pushVParams.push_back(values[i]);

    VectorNative<llvm::Value*> fctParams;
    getLocalCallParameters(buildParameters, allocR, nullptr, allocT, fctParams, typeFuncBC, pushRAParams, pushVParams);

    builder.CreateCall(modu.getOrInsertFunction(name, FT), {fctParams.begin(), fctParams.end()});
}