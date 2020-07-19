#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "Job.h"
#include "Module.h"
#include "Workspace.h"
#include "OS.h"
#include "ByteCode.h"
#include "DataSegment.h"
#include "TypeManager.h"
#include "Ast.h"

BackendFunctionBodyJob* BackendLLVM::newFunctionJob()
{
    return g_Pool_backendLLVMFunctionBodyJob.alloc();
}

bool BackendLLVM::swagTypeToLLVMType(Module* moduleToGen, TypeInfo* typeInfo, int precompileIndex, llvm::Type** llvmType)
{
    auto& context = *llvmContext[precompileIndex];

    typeInfo  = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);
    *llvmType = nullptr;

    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeInfoEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        SWAG_CHECK(swagTypeToLLVMType(moduleToGen, typeInfoEnum->rawType, precompileIndex, llvmType));
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typeInfoPointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);

        uint32_t ptrCount = typeInfoPointer->ptrCount;
        if (typeInfoPointer->finalType->kind == TypeInfoKind::Struct ||
            typeInfoPointer->finalType->isNative(NativeTypeKind::Any) ||
            typeInfoPointer->finalType->kind == TypeInfoKind::Slice)
            ptrCount--;

        if (ptrCount == 0)
        {
            SWAG_CHECK(swagTypeToLLVMType(moduleToGen, typeInfoPointer->finalType, precompileIndex, llvmType));
        }
        else
        {
            if (ptrCount != 1)
                return moduleToGen->internalError(format("swagTypeToLLVMType, invalid pointer type '%s'", typeInfo->name.c_str()));

            if (typeInfo->kind == TypeInfoKind::Slice ||
                typeInfo->kind == TypeInfoKind::Array ||
                typeInfo->kind == TypeInfoKind::Struct ||
                typeInfo->kind == TypeInfoKind::Interface ||
                typeInfo->isNative(NativeTypeKind::Any) ||
                typeInfo->isNative(NativeTypeKind::String) ||
                typeInfo->kind == TypeInfoKind::Reference)
            {
                *llvmType = llvm::PointerType::getVoidTy(context);
            }
            else if (typeInfo->kind == TypeInfoKind::Native)
            {
                switch (typeInfoPointer->finalType->nativeType)
                {
                case NativeTypeKind::Bool:
                    *llvmType = llvm::PointerType::getInt8PtrTy(context);
                    return true;
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                    *llvmType = llvm::PointerType::getInt8PtrTy(context);
                    return true;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    *llvmType = llvm::PointerType::getInt16PtrTy(context);
                    return true;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                case NativeTypeKind::Char:
                    *llvmType = llvm::PointerType::getInt32PtrTy(context);
                    return true;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                    *llvmType = llvm::PointerType::getInt64PtrTy(context);
                    return true;
                case NativeTypeKind::F32:
                    *llvmType = llvm::Type::getFloatPtrTy(context);
                    return true;
                case NativeTypeKind::F64:
                    *llvmType = llvm::Type::getDoublePtrTy(context);
                    return true;
                case NativeTypeKind::Void:
                    *llvmType = llvm::Type::getInt8PtrTy(context);
                    return true;
                }
            }
            else
            {
                return moduleToGen->internalError(format("swagTypeToLLVMType, invalid type '%s'", typeInfo->name.c_str()));
            }
        }

        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice ||
        typeInfo->kind == TypeInfoKind::Array ||
        typeInfo->kind == TypeInfoKind::Struct ||
        typeInfo->kind == TypeInfoKind::Interface ||
        typeInfo->isNative(NativeTypeKind::Any) ||
        typeInfo->isNative(NativeTypeKind::String) ||
        typeInfo->kind == TypeInfoKind::Reference)
    {
        *llvmType = llvm::PointerType::getInt8PtrTy(context);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            *llvmType = llvm::IntegerType::getInt8Ty(context);
            return true;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            *llvmType = llvm::IntegerType::getInt8Ty(context);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            *llvmType = llvm::IntegerType::getInt16Ty(context);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            *llvmType = llvm::IntegerType::getInt32Ty(context);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            *llvmType = llvm::IntegerType::getInt64Ty(context);
            return true;
        case NativeTypeKind::F32:
            *llvmType = llvm::Type::getFloatTy(context);
            return true;
        case NativeTypeKind::F64:
            *llvmType = llvm::Type::getDoubleTy(context);
            return true;
        case NativeTypeKind::Void:
            *llvmType = llvm::IntegerType::getVoidTy(context);
            return true;
        }
    }

    return moduleToGen->internalError(format("swagTypeToLLVMType, invalid type '%s'", typeInfo->name.c_str()));
}

bool BackendLLVM::emitFunctionBody(Module* moduleToGen, ByteCode* bc, int precompileIndex)
{
    auto& context  = *llvmContext[precompileIndex];
    auto& builder  = *llvmBuilder[precompileIndex];
    auto  typeFunc = bc->callType();

    // One int64 per register
    vector<llvm::Type*> params;
    for (int i = 0; i < typeFunc->numReturnRegisters(); i++)
        params.push_back(llvm::Type::getInt64PtrTy(context));
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        for (int i = 0; i < typeParam->numRegisters(); i++)
            params.push_back(llvm::Type::getInt64PtrTy(context));
    }

    // Function prototype
    llvm::FunctionType* funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false);
    llvm::Function*     func     = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, bc->callName().c_str(), llvmModule[precompileIndex]);

    // Content
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(block);
    builder.CreateRetVoid();

    return true;
}

bool BackendLLVM::emitFuncWrapperPublic(Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc, int precompileIndex)
{
    auto& context = *llvmContext[precompileIndex];
    auto& builder = *llvmBuilder[precompileIndex];
    auto  modu    = llvmModule[precompileIndex];

    bool returnByCopy = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;

    // Real return type
    llvm::Type* llvmReturnType;
    llvm::Type* llvmRealReturnType;
    SWAG_CHECK(swagTypeToLLVMType(moduleToGen, typeFunc->returnType, precompileIndex, &llvmRealReturnType));
    if (returnByCopy)
        llvmReturnType = llvm::Type::getVoidTy(context);
    else if (typeFunc->numReturnRegisters() > 1)
        llvmReturnType = llvm::Type::getVoidTy(context);
    else
        llvmReturnType = llvmRealReturnType;

    // Real parameters
    std::vector<llvm::Type*> params;
    if (node->parameters)
    {
        // Variadic parameters are always first
        auto numParams = node->parameters->childs.size();
        if (numParams)
        {
            auto param = node->parameters->childs.back();
            if (param->typeInfo->kind == TypeInfoKind::Variadic)
            {
                params.push_back(llvm::Type::getInt8PtrTy(context));
                params.push_back(llvm::Type::getInt32Ty(context));
                numParams--;
            }
        }

        for (int i = 0; i < numParams; i++)
        {
            auto param = node->parameters->childs[i];

            llvm::Type* llvmType;
            SWAG_CHECK(swagTypeToLLVMType(moduleToGen, param->typeInfo, precompileIndex, &llvmType));
            params.push_back(llvmType);

            if (param->typeInfo->kind == TypeInfoKind::Slice || param->typeInfo->isNative(NativeTypeKind::String))
                params.push_back(llvm::Type::getInt32Ty(context));
            else if (param->typeInfo->isNative(NativeTypeKind::Any))
                params.push_back(llvm::Type::getInt8PtrTy(context));
        }
    }

    // Return value
    if (typeFunc->numReturnRegisters() > 1 || returnByCopy)
    {
        params.push_back(llvmRealReturnType);
    }

    // Public foreign name
    auto name = Ast::computeFullNameForeign(node, true);

    llvm::FunctionType* funcType = llvm::FunctionType::get(llvmReturnType, params, false);
    llvm::Function*     func     = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name.c_str(), llvmModule[precompileIndex]);
    func->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(block);

    // Result registers
    auto n = typeFunc->numReturnRegisters();
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        n += typeParam->numRegisters();
    }

    auto allocRR = builder.CreateAlloca(llvm::Type::getInt64Ty(context), llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), n));

    // Affect registers
    int idx = typeFunc->numReturnRegisters();

    // Return by copy
    if (returnByCopy)
    {
        // rr0 = result
        auto rr0  = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));
        auto cst0 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg((int32_t) params.size() - 1), llvm::Type::getInt64Ty(context));
        builder.CreateStore(cst0, rr0);
    }

    auto numParams = typeFunc->parameters.size();

    // Variadic must be pushed first
    int argIdx = 0;
    if (numParams)
    {
        auto param     = typeFunc->parameters.back();
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            auto rr0  = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), idx));
            auto cst0 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg(0), llvm::Type::getInt64Ty(context));
            builder.CreateStore(cst0, rr0);

            auto rr1 = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), idx + 1));
            rr1      = builder.CreatePointerCast(rr1, llvm::Type::getInt32PtrTy(context));
            builder.CreateStore(func->getArg(1), rr1);

            idx += 2;
            argIdx += 2;
            numParams--;
        }
    }

    for (int i = 0; i < numParams; i++)
    {
        auto param     = typeFunc->parameters[i];
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        auto rr0       = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), idx));
        if (typeParam->kind == TypeInfoKind::Pointer)
        {
            auto cst0 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg(argIdx), llvm::Type::getInt64Ty(context));
            builder.CreateStore(cst0, rr0);
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            auto cst0 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg(argIdx), llvm::Type::getInt64Ty(context));
            builder.CreateStore(cst0, rr0);
            auto rr1 = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), idx + 1));
            rr1      = builder.CreatePointerCast(rr1, llvm::Type::getInt32PtrTy(context));
            builder.CreateStore(func->getArg(argIdx + 1), rr1);
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String) || typeParam->isNative(NativeTypeKind::Any))
        {
            auto cst0 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg(argIdx), llvm::Type::getInt64Ty(context));
            builder.CreateStore(cst0, rr0);
            auto cst1 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg(argIdx + 1), llvm::Type::getInt64Ty(context));
            auto rr1  = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), idx + 1));
            builder.CreateStore(cst1, rr1);
        }
        else if (typeParam->kind == TypeInfoKind::Struct ||
                 typeParam->kind == TypeInfoKind::Array ||
                 typeParam->kind == TypeInfoKind::Interface)
        {
            auto cst0 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg(argIdx), llvm::Type::getInt64Ty(context));
            builder.CreateStore(cst0, rr0);
            break;
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            switch (typeParam->nativeType)
            {
            case NativeTypeKind::U8:
            case NativeTypeKind::S8:
            case NativeTypeKind::Bool:
            {
                rr0 = builder.CreatePointerCast(rr0, llvm::Type::getInt8PtrTy(context));
                builder.CreateStore(func->getArg(argIdx), rr0);
                break;
            }
            case NativeTypeKind::U16:
            case NativeTypeKind::S16:
            {
                rr0 = builder.CreatePointerCast(rr0, llvm::Type::getInt16PtrTy(context));
                builder.CreateStore(func->getArg(argIdx), rr0);
                break;
            }
            case NativeTypeKind::U32:
            case NativeTypeKind::S32:
            case NativeTypeKind::Char:
            {
                rr0 = builder.CreatePointerCast(rr0, llvm::Type::getInt32PtrTy(context));
                builder.CreateStore(func->getArg(argIdx), rr0);
                break;
            }
            case NativeTypeKind::U64:
            case NativeTypeKind::S64:
            {
                builder.CreateStore(func->getArg(argIdx), rr0);
                break;
            }
            case NativeTypeKind::F32:
            {
                rr0 = builder.CreatePointerCast(rr0, llvm::Type::getFloatPtrTy(context));
                builder.CreateStore(func->getArg(argIdx), rr0);
                break;
            }
            case NativeTypeKind::F64:
            {
                rr0 = builder.CreatePointerCast(rr0, llvm::Type::getDoublePtrTy(context));
                builder.CreateStore(func->getArg(argIdx), rr0);
                break;
            }
            default:
                return moduleToGen->internalError("emitFuncWrapperPublic, invalid param type");
            }
        }
        else
        {
            return moduleToGen->internalError(format("emitFuncWrapperPublic, invalid param type '%s'", typeParam->name.c_str()));
        }

        idx += typeParam->numRegisters();
        argIdx += typeParam->numRegisters();
    }

    // Make the call
    vector<llvm::Value*> args;
    for (int i = 0; i < n; i++)
    {
        auto rr0 = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), i));
        args.push_back(rr0);
    }

    auto fcc = modu->getFunction(bc->callName().c_str());
    builder.CreateCall(fcc, args);

    // Return
    if (typeFunc->numReturnRegisters() && !returnByCopy)
    {
        auto rr0        = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));
        auto returnType = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM);

        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->kind == TypeInfoKind::Interface ||
            returnType->isNative(NativeTypeKind::Any) ||
            returnType->isNative(NativeTypeKind::String))
        {
            //*((void **) result) = rr0.pointer
            auto loadInst = builder.CreateLoad(rr0);
            auto arg0 = builder.CreatePointerCast(func->getArg((int)params.size() - 1), llvm::Type::getInt64PtrTy(context));
            builder.CreateStore(loadInst, arg0);

            //*((void **) result + 1) = rr1.pointer
            auto rr1 = builder.CreateInBoundsGEP(allocRR, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 1));
            auto arg1 = builder.CreateInBoundsGEP(arg0, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 1));
            loadInst = builder.CreateLoad(rr1);
            builder.CreateStore(loadInst, arg1);

            builder.CreateRetVoid();
        }
        else if (returnType->kind == TypeInfoKind::Pointer)
        {
            auto loadInst = builder.CreateLoad(rr0);
            auto arg0 = builder.CreateCast(llvm::Instruction::CastOps::IntToPtr, loadInst, llvmReturnType);
            builder.CreateRet(arg0);
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            switch (returnType->nativeType)
            {
            case NativeTypeKind::U8:
            case NativeTypeKind::S8:
            case NativeTypeKind::Bool:
            {
                rr0           = builder.CreatePointerCast(rr0, llvm::Type::getInt8PtrTy(context));
                auto loadInst = builder.CreateLoad(rr0);
                builder.CreateRet(loadInst);
                break;
            }
            case NativeTypeKind::U16:
            case NativeTypeKind::S16:
            {
                rr0           = builder.CreatePointerCast(rr0, llvm::Type::getInt16PtrTy(context));
                auto loadInst = builder.CreateLoad(rr0);
                builder.CreateRet(loadInst);
                break;
            }
            case NativeTypeKind::U32:
            case NativeTypeKind::S32:
            case NativeTypeKind::Char:
            {
                rr0           = builder.CreatePointerCast(rr0, llvm::Type::getInt32PtrTy(context));
                auto loadInst = builder.CreateLoad(rr0);
                builder.CreateRet(loadInst);
                break;
            }
            case NativeTypeKind::U64:
            case NativeTypeKind::S64:
            {
                auto loadInst = builder.CreateLoad(rr0);
                builder.CreateRet(loadInst);
                break;
            }
            case NativeTypeKind::F32:
            {
                rr0           = builder.CreatePointerCast(rr0, llvm::Type::getFloatPtrTy(context));
                auto loadInst = builder.CreateLoad(rr0);
                builder.CreateRet(loadInst);
                break;
            }
            case NativeTypeKind::F64:
            {
                rr0           = builder.CreatePointerCast(rr0, llvm::Type::getDoublePtrTy(context));
                auto loadInst = builder.CreateLoad(rr0);
                builder.CreateRet(loadInst);
                break;
            }
            default:
                return moduleToGen->internalError("emitFuncWrapperPublic, invalid return type");
            }
        }
        else
        {
            return moduleToGen->internalError(format("emitFuncWrapperPublic, invalid return type '%s'", returnType->name.c_str()));
        }
    }
    else
    {
        builder.CreateRetVoid();
    }

    return true;
}
