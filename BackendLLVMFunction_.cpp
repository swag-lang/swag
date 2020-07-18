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
    llvm::Type* returnType;
    llvm::Type* realReturnType;
    SWAG_CHECK(swagTypeToLLVMType(moduleToGen, typeFunc->returnType, precompileIndex, &realReturnType));
    if (returnByCopy)
        returnType = llvm::Type::getVoidTy(context);
    else if (typeFunc->numReturnRegisters() > 1)
        returnType = llvm::Type::getVoidTy(context);
    else
        returnType = realReturnType;

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
        params.push_back(realReturnType);
    }

    // Public foreign name
    auto name = Ast::computeFullNameForeign(node, true);

    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, params, false);
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

    auto alloca = builder.CreateAlloca(llvm::Type::getInt64Ty(context), llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), n));

    // Affect registers
    int idx = typeFunc->numReturnRegisters();

    // Return by copy
    if (returnByCopy)
    {
        // rr0 = result
        auto rr0  = builder.CreateInBoundsGEP(alloca, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));
        auto cst0 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg(params.size() - 1), llvm::Type::getInt64Ty(context));
        builder.CreateStore(cst0, rr0);
    }

    auto numParams = typeFunc->parameters.size();

    // Variadic must be pushed first
    if (numParams)
    {
        auto param     = typeFunc->parameters.back();
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            auto rr0 = builder.CreateInBoundsGEP(alloca, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), idx));
            auto cst0 = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func->getArg(0), llvm::Type::getInt64Ty(context));
            builder.CreateStore(cst0, rr0);

            auto rr1 = builder.CreateInBoundsGEP(alloca, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), idx + 1));
            auto cst1 = builder.CreateCast(llvm::Instruction::CastOps::ZExt, func->getArg(1), llvm::Type::getInt64Ty(context));
            builder.CreateStore(cst1, rr1);

            idx += 2;
            numParams--;
        }
    }

    // Make the call
    vector<llvm::Value*> args;
    for (int i = 0; i < n; i++)
    {
        auto rr0 = builder.CreateInBoundsGEP(alloca, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), i));
        args.push_back(rr0);
    }

    auto fcc = modu->getFunction(bc->callName().c_str());
    builder.CreateCall(fcc, args);

    builder.CreateRetVoid();
    return true;
}
