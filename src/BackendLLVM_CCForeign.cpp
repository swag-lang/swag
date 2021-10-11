#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "BackendLLVM_Macros.h"
#include "Module.h"
#include "ByteCode.h"
#include "TypeManager.h"
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

    auto numTotalRegs  = typeFunc->numTotalRegisters();
    auto numReturnRegs = typeFunc->numReturnRegisters();
    auto returnType    = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM | CONCRETE_FORCEALIAS);
    bool returnByCopy  = returnType->flags & TYPEINFO_RETURN_BY_COPY;

    // Storage for the registers
    auto allocRR = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(numTotalRegs));

    // Set all registers
    int argIdx = 0;
    for (int i = 0; i < numTotalRegs; i++)
    {
        if (i < numReturnRegs)
        {
            if (returnByCopy)
            {
                auto rr0  = TO_PTR_PTR_I8(allocRR);
                auto cst0 = TO_PTR_I8(func->getArg((int) func->arg_size() - 1));
                builder.CreateStore(cst0, rr0);
            }

            i = numReturnRegs - 1;
        }
        else
        {
            auto typeParam = typeFunc->registerIdxToType(i - numReturnRegs);
            auto rr0       = GEP_I32(allocRR, i);

            if (typeParam->kind == TypeInfoKind::Variadic || typeParam->kind == TypeInfoKind::TypedVariadic)
            {
                rr0       = TO_PTR_PTR_I8(rr0);
                auto cst0 = TO_PTR_I8(func->getArg(0));
                builder.CreateStore(cst0, rr0);
                auto rr1 = GEP_I32(allocRR, i + 1);
                builder.CreateStore(func->getArg(1), rr1);
                i += 1;
            }
            else if (typeParam->kind == TypeInfoKind::Pointer ||
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
                auto rr1 = GEP_I32(allocRR, i + 1);
                builder.CreateStore(func->getArg(argIdx + 1), rr1);
                i += 1;
            }
            else if (typeParam->isNative(NativeTypeKind::Any) ||
                     typeParam->kind == TypeInfoKind::Interface)
            {
                auto cst0 = TO_PTR_I8(func->getArg(argIdx));
                builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
                auto cst1 = TO_PTR_I8(func->getArg(argIdx + 1));
                auto rr1  = GEP_I32(allocRR, i + 1);
                builder.CreateStore(cst1, TO_PTR_PTR_I8(rr1));
                i += 1;
            }
            else if (typeParam->kind == TypeInfoKind::Native)
            {
                if (!passByValue(typeParam))
                {
                    auto r = TO_PTR_NATIVE(rr0, typeParam->nativeType);
                    builder.CreateStore(func->getArg(argIdx), r);
                }
            }
            else
            {
                return moduleToGen->internalError(Utf8::format("emitFuncWrapperPublic, invalid param type `%s`", typeParam->name.c_str()));
            }

            argIdx += typeParam->numRegisters();
        }
    }

    // Set all parameters
    VectorNative<llvm::Value*> args;
    for (int i = 0; i < numTotalRegs; i++)
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
                args.push_back(builder.CreateLoad(rr0));
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
            if (returnType->kind == TypeInfoKind::Pointer || returnType->kind == TypeInfoKind::Reference || returnType->kind == TypeInfoKind::Lambda)
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
            auto result0  = TO_PTR_I64(func->getArg((int) func->arg_size() - 1));
            builder.CreateStore(loadInst, result0);

            //*((void **) result + 1) = rr1.pointer
            auto rr1     = builder.CreateInBoundsGEP(allocRR, pp.cst1_i32);
            auto result1 = builder.CreateInBoundsGEP(result0, pp.cst1_i32);
            loadInst     = builder.CreateLoad(rr1);
            builder.CreateStore(loadInst, result1);

            builder.CreateRetVoid();
        }
    }
    else
    {
        builder.CreateRetVoid();
    }

    return true;
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
