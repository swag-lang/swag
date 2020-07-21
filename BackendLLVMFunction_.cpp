#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "Job.h"
#include "Module.h"
#include "Workspace.h"
#include "OS.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "DataSegment.h"
#include "TypeManager.h"
#include "Ast.h"

BackendFunctionBodyJob* BackendLLVM::newFunctionJob()
{
    return g_Pool_backendLLVMFunctionBodyJob.alloc();
}

bool BackendLLVM::swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo, llvm::Type** llvmType)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& context = *perThread[ct][precompileIndex].context;

    typeInfo  = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);
    *llvmType = nullptr;

    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeInfoEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, typeInfoEnum->rawType, llvmType));
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typeInfoPointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);

        uint32_t ptrCount = typeInfoPointer->ptrCount;
        if (ptrCount != 1)
            return moduleToGen->internalError(format("swagTypeToLLVMType, invalid pointer type '%s'", typeInfo->name.c_str()));

        if (typeInfoPointer->finalType->kind == TypeInfoKind::Slice ||
            typeInfoPointer->finalType->kind == TypeInfoKind::Array ||
            typeInfoPointer->finalType->kind == TypeInfoKind::Struct ||
            typeInfoPointer->finalType->kind == TypeInfoKind::Interface ||
            typeInfoPointer->finalType->isNative(NativeTypeKind::Any) ||
            typeInfoPointer->finalType->isNative(NativeTypeKind::String) ||
            typeInfoPointer->finalType->kind == TypeInfoKind::Reference)
        {
            *llvmType = llvm::PointerType::getInt8PtrTy(context);
        }
        else if (typeInfoPointer->finalType->kind == TypeInfoKind::Native)
        {
            switch (typeInfoPointer->finalType->nativeType)
            {
            case NativeTypeKind::Bool:
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

bool BackendLLVM::emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    bool returnByCopy = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;

    // Real return type
    llvm::Type* llvmReturnType;
    llvm::Type* llvmRealReturnType;
    SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, typeFunc->returnType, &llvmRealReturnType));
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
            SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, param->typeInfo, &llvmType));
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
    llvm::Function*     func     = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name.c_str(), modu);
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
        auto rr0  = builder.CreateInBoundsGEP(allocRR, pp.cst0_i32);
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

    auto fcc = modu.getFunction(bc->callName().c_str());
    builder.CreateCall(fcc, args);

    // Return
    if (typeFunc->numReturnRegisters() && !returnByCopy)
    {
        auto rr0        = builder.CreateInBoundsGEP(allocRR, pp.cst0_i32);
        auto returnType = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM);

        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->kind == TypeInfoKind::Interface ||
            returnType->isNative(NativeTypeKind::Any) ||
            returnType->isNative(NativeTypeKind::String))
        {
            //*((void **) result) = rr0.pointer
            auto loadInst = builder.CreateLoad(rr0);
            auto arg0     = builder.CreatePointerCast(func->getArg((int) params.size() - 1), llvm::Type::getInt64PtrTy(context));
            builder.CreateStore(loadInst, arg0);

            //*((void **) result + 1) = rr1.pointer
            auto rr1  = builder.CreateInBoundsGEP(allocRR, pp.cst1_i32);
            auto arg1 = builder.CreateInBoundsGEP(arg0, pp.cst1_i32);
            loadInst  = builder.CreateLoad(rr1);
            builder.CreateStore(loadInst, arg1);

            builder.CreateRetVoid();
        }
        else if (returnType->kind == TypeInfoKind::Pointer)
        {
            auto loadInst = builder.CreateLoad(rr0);
            auto arg0     = builder.CreateCast(llvm::Instruction::CastOps::IntToPtr, loadInst, llvmReturnType);
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

bool BackendLLVM::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
    {
        if (buildParameters.compileType != BackendCompileType::Test)
            return true;
    }

    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp       = perThread[ct][precompileIndex];
    auto& context  = *pp.context;
    auto& builder  = *pp.builder;
    auto& modu     = *pp.module;
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
    llvm::Function*     func     = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, bc->callName().c_str(), modu);

    // Content
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(block);

    // Reserve registers
    llvm::AllocaInst* allocR = nullptr;
    if (bc->maxReservedRegisterRC)
        allocR = builder.CreateAlloca(llvm::Type::getInt64Ty(context), llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), bc->maxReservedRegisterRC));
    llvm::AllocaInst* allocRT = nullptr;
    if (bc->maxCallResults)
        allocRT = builder.CreateAlloca(llvm::Type::getInt64Ty(context), llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), bc->maxCallResults));

    // Stack
    llvm::AllocaInst* allocStack = nullptr;
    if (typeFunc->stackSize)
        allocStack = builder.CreateAlloca(llvm::Type::getInt64Ty(context), llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), typeFunc->stackSize));

#define CST_RA32 llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), ip->a.u32)
#define CST_RB32 llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), ip->b.u32)
#define CST_RC32 llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), ip->c.u32)
#define CST_RA64 llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), ip->a.u64)
#define CST_RB64 llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), ip->b.u64)
#define CST_RC64 llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), ip->c.u64)
#define TO_PTR_PTR(__r) builder.CreatePointerCast(__r, llvm::Type::getInt8PtrTy(context)->getPointerTo())
#define TO_PTR_I64(__r) builder.CreatePointerCast(__r, llvm::Type::getInt64PtrTy(context));
#define TO_PTR_I32(__r) builder.CreatePointerCast(__r, llvm::Type::getInt32PtrTy(context));
#define TO_PTR_I16(__r) builder.CreatePointerCast(__r, llvm::Type::getInt16PtrTy(context));
#define TO_PTR_I8(__r) builder.CreatePointerCast(__r, llvm::Type::getInt8PtrTy(context));
#define TO_PTR_F64(__r) builder.CreatePointerCast(__r, llvm::Type::getDoublePtrTy(context));
#define TO_PTR_F32(__r) builder.CreatePointerCast(__r, llvm::Type::getFloatPtrTy(context));

    // Generate bytecode
    int                    vaargsIdx = 0;
    auto                   ip        = bc->out;
    VectorNative<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
            continue;

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::CopySPtoBP:
            break;

        case ByteCodeOp::BoundCheckString:
            //concat.addStringFormat("swag_runtime_assert(r[%u].u32 <= r[%u].u32 + 1, \"%s\", %d, \": index out of range\");", ip->a.u32, ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            break;
        case ByteCodeOp::BoundCheck:
            //concat.addStringFormat("swag_runtime_assert(r[%u].u32 < r[%u].u32, \"%s\", %d, \": index out of range\");", ip->a.u32, ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            break;
        case ByteCodeOp::IncPointerVB32:
            //concat.addStringFormat("r[%u].pointer += %d;", ip->a.u32, ip->b.s32);
            break;
        case ByteCodeOp::IncPointer32:
            //concat.addStringFormat("r[%u].pointer = r[%u].pointer + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DecPointer32:
            //concat.addStringFormat("r[%u].pointer = r[%u].pointer - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DeRef8:
            //concat.addStringFormat("r[%u].u64 = *(swag_uint8_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef16:
            //concat.addStringFormat("r[%u].u64 = *(swag_uint16_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef32:
            //concat.addStringFormat("r[%u].u64 = *(swag_uint32_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRef64:
            //concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::DeRefPointer:
            //concat.addStringFormat("r[%u].pointer = *(swag_uint8_t**) (r[%u].pointer + %u);", ip->b.u32, ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::DeRefStringSlice:
            //concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (r[%u].pointer + 8); ", ip->b.u32, ip->a.u32);
            //concat.addStringFormat("r[%u].pointer = *(swag_uint8_t**) r[%u].pointer; ", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::MulRAVB:
            //concat.addStringFormat("r[%u].s64 *= %u;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::DivRAVB:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //concat.addStringFormat("swag_runtime_assert(r[%u].u32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].s64 /= %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::GetFromDataSeg8:
            //concat.addStringFormat("r[%u].u8 = *(swag_uint8_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromDataSeg16:
            //concat.addStringFormat("r[%u].u16 = *(swag_uint16_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromDataSeg32:
            //concat.addStringFormat("r[%u].u32 = *(swag_uint32_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromDataSeg64:
            //concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::GetFromBssSeg8:
            //concat.addStringFormat("r[%u].u8 = *(swag_uint8_t*) (__bssseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromBssSeg16:
            //concat.addStringFormat("r[%u].u16 = *(swag_uint16_t*) (__bssseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromBssSeg32:
            //concat.addStringFormat("r[%u].u32 = *(swag_uint32_t*) (__bssseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::GetFromBssSeg64:
            //concat.addStringFormat("r[%u].u64 = *(swag_uint64_t*) (__bssseg + %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearXVar:
            //concat.addStringFormat("swag_runtime_memset(r[%u].pointer, 0, r[%u].u32 * %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::SetZeroStack8:
            //CONCAT_STR_1(concat, "*(swag_uint8_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroStack16:
            //CONCAT_STR_1(concat, "*(swag_uint16_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroStack32:
            //CONCAT_STR_1(concat, "*(swag_uint32_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroStack64:
            //CONCAT_STR_1(concat, "*(swag_uint64_t*)(stack + ", ip->a.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroStackX:
            //concat.addStringFormat("swag_runtime_memset(stack + %u, 0, %u);", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::MakeDataSegPointer:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = __mutableseg + ", ip->b.u32, ";");
            break;
        case ByteCodeOp::MakeBssSegPointer:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = __bssseg + ", ip->b.u32, ";");
            break;
        case ByteCodeOp::MakeConstantSegPointer:
        {
            //concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) (__constantseg + %u); ", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r1 = builder.CreateInBoundsGEP(pp.constantSeg, {pp.cst0_i32, CST_RB32});
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeConstantSegPointerOC:
            //concat.addStringFormat("r[%u].pointer = __constantseg + %u; ", ip->a.u32, (uint32_t)(ip->c.u64 >> 32));
            //concat.addStringFormat("r[%u].u64 = %u;", ip->b.u32, (ip->c.u64) & 0xFFFFFFFF);
            break;

        case ByteCodeOp::MakePointerToStack:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = stack + ", ip->b.u32, ";");
            auto r0 = TO_PTR_PTR(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::GetFromStack8:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u8 = *(swag_uint8_t*) (stack + ", ip->b.u32, ");");
            auto r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromStack16:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u16 = *(swag_uint16_t*) (stack + ", ip->b.u32, ");");
            auto r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r1 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromStack32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 = *(swag_uint32_t*) (stack + ", ip->b.u32, ");");
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromStack64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = *(swag_uint64_t*) (stack + ", ip->b.u32, ");");
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::MemCpy:
            //concat.addStringFormat("swag_runtime_memcpy((void*) r[%u].pointer, (void*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::MemSet:
            //concat.addStringFormat("swag_runtime_memset((void*) r[%u].pointer, r[%u].u8, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::MemCmp:
            //concat.addStringFormat("r[%u].s32 = swag_runtime_memcmp((void*) r[%u].pointer, (void*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            break;

        case ByteCodeOp::CopyVC:
            //concat.addStringFormat("swag_runtime_memcpy(r[%u].pointer, r[%u].pointer, %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::CopyVBtoRA32:
        {
            //concat.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            builder.CreateStore(CST_RB32, r0);
            break;
        }
        case ByteCodeOp::CopyVBtoRA64:
        {
            //concat.addStringFormat("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64);
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            builder.CreateStore(CST_RB64, r0);
            break;
        }

        case ByteCodeOp::CopyRBtoRA:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRARBAddr:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (swag_uint8_t*) &r[", ip->b.u32, "];");
            break;

        case ByteCodeOp::ClearRA:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u64 = 0;");
            break;
        case ByteCodeOp::DecrementRA32:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32--;");
            break;
        case ByteCodeOp::IncrementRA32:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32++;");
            break;
        case ByteCodeOp::IncrementRA64:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u64++;");
            break;
        case ByteCodeOp::AddVBtoRA32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 += ", ip->b.u32, ";");
            break;

        case ByteCodeOp::SetAtPointer8:
        {
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) = r[", ip->b.u32, "].u8;");
            auto r0 = TO_PTR_PTR(builder.CreateInBoundsGEP(allocR, CST_RA32));
            r0      = builder.CreateLoad(r0);
            r0      = TO_PTR_I8(builder.CreateInBoundsGEP(r0, CST_RC32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocR, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::SetAtPointer16:
        {
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) = r[", ip->b.u32, "].u16;");
            auto r0 = TO_PTR_PTR(builder.CreateInBoundsGEP(allocR, CST_RA32));
            r0      = builder.CreateLoad(r0);
            r0      = TO_PTR_I16(builder.CreateInBoundsGEP(r0, CST_RC32));
            auto r1 = TO_PTR_I16(builder.CreateInBoundsGEP(allocR, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::SetAtPointer32:
        {
            //concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR(builder.CreateInBoundsGEP(allocR, CST_RA32));
            r0      = builder.CreateLoad(r0);
            r0      = TO_PTR_I32(builder.CreateInBoundsGEP(r0, CST_RC32));
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::SetAtPointer64:
        {
            //concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR(builder.CreateInBoundsGEP(allocR, CST_RA32));
            r0      = builder.CreateLoad(r0);
            r0      = TO_PTR_I64(builder.CreateInBoundsGEP(r0, CST_RC32));
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::SetZeroAtPointer8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroAtPointer16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroAtPointer32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroAtPointer64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            break;
        case ByteCodeOp::SetZeroAtPointerX:
            //concat.addStringFormat("swag_runtime_memset(r[%u].pointer, 0, %u);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::SetPointerAtPointer:
            //CONCAT_STR_2(concat, "*(void**)(r[", ip->a.u32, "].pointer) = r[", ip->b.u32, "].pointer;");
            break;

        case ByteCodeOp::BinOpPlusS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 + r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 + r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 + r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusF32:
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpPlusF64:
            //concat.addStringFormat("r[%u].f64= r[%u].f64 + r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpMinusS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 - r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 - r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 - r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusF32:
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 - r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpMinusF64:
            //concat.addStringFormat("r[%u].f64 = r[%u].f64 - r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpMulS32:
        {
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 * r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RC32));
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r2 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RB32));
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMulS64:
        {
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 * r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RC32));
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r2 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RB32));
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMulF32:
        {
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 * r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_F32(builder.CreateInBoundsGEP(allocR, CST_RC32));
            auto r1 = TO_PTR_F32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r2 = TO_PTR_F32(builder.CreateInBoundsGEP(allocR, CST_RB32));
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMulF64:
        {
            //concat.addStringFormat("r[%u].f64= r[%u].f64 * r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_F64(builder.CreateInBoundsGEP(allocR, CST_RC32));
            auto r1 = TO_PTR_F64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r2 = TO_PTR_F64(builder.CreateInBoundsGEP(allocR, CST_RB32));
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::XorS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 ^ r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 ^ r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 ^ r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::XorU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 ^ r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::ShiftLeftS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 << r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 << r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftLeftU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::ShiftRightS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 >> r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 >> r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ShiftRightU64VB:
            //concat.addStringFormat("r[%u].u64 >>= %u;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpModuloS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 %% r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 %% r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 %% r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpModuloU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 %% r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BinOpDivS32:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].s32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 / r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivS64:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].s64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 / r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivU32:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].u32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 / r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivU64:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].u64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 / r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF32:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].f32 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 / r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpDivF64:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].f64 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].f64 = r[%u].f64 / r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].u64;");
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            //CONCAT_STR_2(concat, "*(swag_float32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            //CONCAT_STR_2(concat, "*(swag_float64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].u64;");
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            //CONCAT_STR_2(concat, "*(swag_float32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            //CONCAT_STR_2(concat, "*(swag_float64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpPlusEqPointer:
            //CONCAT_STR_2(concat, "*(swag_uint8_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMinusEqPointer:
            //CONCAT_STR_2(concat, "*(swag_uint8_t**)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s32;");
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].u64;");
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            //CONCAT_STR_2(concat, "*(swag_float32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f32;");
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            //CONCAT_STR_2(concat, "*(swag_float64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f64;");
            break;

        case ByteCodeOp::AffectOpDivEqS8:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].s8, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_int8_t*)(r[%u].pointer) /= r[%u].s8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS16:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].s16, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_int16_t*)(r[%u].pointer) /= r[%u].s16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS32:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].s32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_int32_t*)(r[%u].pointer) /= r[%u].s32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqS64:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].s64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_int64_t*)(r[%u].pointer) /= r[%u].s64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU8:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].u8, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_uint8_t*)(r[%u].pointer) /= r[%u].u8;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU16:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].u16, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_uint16_t*)(r[%u].pointer) /= r[%u].u16;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU32:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].u32, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_uint32_t*)(r[%u].pointer) /= r[%u].u32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqU64:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //concat.addStringFormat("swag_runtime_assert(r[%u].u64, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_uint64_t*)(r[%u].pointer) /= r[%u].u64;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqF32:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //concat.addStringFormat("swag_runtime_assert(r[%u].f32 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_float32_t*)(r[%u].pointer) /= r[%u].f32;", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //concat.addStringFormat("swag_runtime_assert(r[%u].f64 != 0, \"%s\", %d, \": error: division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("*(swag_float64_t*)(r[%u].pointer) /= r[%u].f64;", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpAndEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpAndEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpAndEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpAndEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpOrEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpOrEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpOrEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpOrEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::AffectOpShiftLeftEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            break;

        case ByteCodeOp::AffectOpPercentEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpPercentEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpPercentEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpPercentEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpPercentEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpPercentEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpPercentEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpPercentEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::AffectOpXOrEqS8:
            //CONCAT_STR_2(concat, "*(swag_int8_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s8;");
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            //CONCAT_STR_2(concat, "*(swag_int16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            //CONCAT_STR_2(concat, "*(swag_int32_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s32;");
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            //CONCAT_STR_2(concat, "*(swag_int64_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s64;");
            break;
        case ByteCodeOp::AffectOpXOrEqU8:
            //CONCAT_STR_2(concat, "*(swag_uint8_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].u8;");
            break;
        case ByteCodeOp::AffectOpXOrEqU16:
            //CONCAT_STR_2(concat, "*(swag_uint16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].u16;");
            break;
        case ByteCodeOp::AffectOpXOrEqU32:
            //CONCAT_STR_2(concat, "*(swag_uint32_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].u32;");
            break;
        case ByteCodeOp::AffectOpXOrEqU64:
            //CONCAT_STR_2(concat, "*(swag_uint64_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].u64;");
            break;

        case ByteCodeOp::CompareOpGreaterS32:
            //concat.addStringFormat("r[%u].b = r[%u].s32 > r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            //concat.addStringFormat("r[%u].b = r[%u].s64 > r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 > r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 > r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            //concat.addStringFormat("r[%u].b = r[%u].f32 > r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            //concat.addStringFormat("r[%u].b = r[%u].f64 > r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpGreaterPointer:
            //concat.addStringFormat("r[%u].b = r[%u].pointer > r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpLowerS32:
            //concat.addStringFormat("r[%u].b = r[%u].s32 < r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerS64:
            //concat.addStringFormat("r[%u].b = r[%u].s64 < r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerU32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 < r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerU64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 < r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerF32:
            //concat.addStringFormat("r[%u].b = r[%u].f32 < r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerF64:
            //concat.addStringFormat("r[%u].b = r[%u].f64 < r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpLowerPointer:
            //concat.addStringFormat("r[%u].b = r[%u].pointer < r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CompareOpEqual8:
            //concat.addStringFormat("r[%u].b = r[%u].u8 == r[%u].u8;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual16:
            //concat.addStringFormat("r[%u].b = r[%u].u16 == r[%u].u16;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual32:
            //concat.addStringFormat("r[%u].b = r[%u].u32 == r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqual64:
            //concat.addStringFormat("r[%u].b = r[%u].u64 == r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualPointer:
            //concat.addStringFormat("r[%u].b = r[%u].pointer == r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualInterface:
            //concat.addStringFormat("r[%u].b = (((void **) r[%u].pointer)[0] == ((void **) r[%u].pointer)[0]) && (((void **) r[%u].pointer)[1] == ((void **) r[%u].pointer)[1]);", ip->c.u32, ip->a.u32, ip->b.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualBool:
            //concat.addStringFormat("r[%u].b = r[%u].b == r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CompareOpEqualString:
            //concat.addStringFormat("r[%u].b = swag_runtime_strcmp((const char*) r[%u].pointer, (const char*) r[%u].pointer, r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::IsNullString:
            //concat.addStringFormat("r[%u].b = r[%u].pointer == 0;", ip->b.u32, ip->a.u32);
            break;

        case ByteCodeOp::BinOpAnd:
            //concat.addStringFormat("r[%u].b = r[%u].b && r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BinOpOr:
            //concat.addStringFormat("r[%u].b = r[%u].b || r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::BitmaskAndS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 & r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 & r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 & r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskAndU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 & r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrS32:
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 | r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrS64:
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 | r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrU32:
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 | r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::BitmaskOrU64:
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 | r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::Jump:
            //CONCAT_FIXED_STR(concat, "goto _");
            //concat.addS32Str8(ip->a.s32 + i + 1);
            //concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfNotTrue:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].b) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            //concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfTrue:
            //CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].b) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            //concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfZero32:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            //concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfZero64:
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u64) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            //concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfNotZero32:
            //CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            //concat.addChar(';');
            break;
        case ByteCodeOp::JumpIfNotZero64:
            //CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u64) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            //concat.addChar(';');
            break;
        case ByteCodeOp::Ret:
            //return;
            builder.CreateRetVoid();
            break;

        case ByteCodeOp::IntrinsicPrintS64:
        {
            //CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].s64);");
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            builder.CreateCall(modu.getFunction("swag_runtime_print_i64"), {builder.CreateLoad(r0)});
            break;
        }
        case ByteCodeOp::IntrinsicPrintF64:
        {
            //CONCAT_STR_1(concat, "swag_runtime_print_f64(r[", ip->a.u32, "].f64);");
            auto r0 = TO_PTR_F64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            builder.CreateCall(modu.getFunction("swag_runtime_print_f64"), {builder.CreateLoad(r0)});
            break;
        }
        case ByteCodeOp::IntrinsicPrintString:
        {
            //swag_runtime_print_n((const char*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RB32));
            builder.CreateCall(modu.getFunction("swag_runtime_print_n"), {builder.CreateLoad(r0), builder.CreateLoad(r1)});
            break;
        }

        case ByteCodeOp::IntrinsicAssert:
            //concat.addStringFormat("swag_runtime_assert(r[%u].b, \"%s\", %d, 0);", ip->a.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            break;
        case ByteCodeOp::IntrinsicAssertCastAny:
            //CONCAT_FIXED_STR(concat, "{ ");
            //concat.addStringFormat("const char* typeTo = *(char**)r[%u].pointer; ", ip->b.u32);
            //concat.addStringFormat("const char* typeFrom = *(char**)r[%u].pointer; ", ip->c.u32);
            //concat.addStringFormat("swag_runtime_assert(r[%u].b, \"%s\", %d, \"invalid cast from 'any'\");", ip->a.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //CONCAT_FIXED_STR(concat, "}");
            break;
        case ByteCodeOp::IntrinsicTarget:
            //concat.addStringFormat("r[%u].pointer = (void*) 0;", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicAlloc:
            //concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) swag_runtime_malloc(r[%u].u32);", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::IntrinsicRealloc:
            //concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) swag_runtime_realloc(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        case ByteCodeOp::IntrinsicFree:
            //concat.addStringFormat("swag_runtime_free(r[%u].pointer);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicGetContext:
            //concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) swag_runtime_tlsGetValue(__process_infos.contextTlsId);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicSetContext:
            //concat.addStringFormat("swag_runtime_tlsSetValue(__process_infos.contextTlsId, r[%u].pointer);", ip->a.u32);
            break;
        case ByteCodeOp::IntrinsicArguments:
            //concat.addStringFormat("r[%u].pointer = __process_infos.arguments.addr;", ip->a.u32);
            //concat.addStringFormat("r[%u].u64 = __process_infos.arguments.count;", ip->b.u32);
            break;
        case ByteCodeOp::IntrinsicIsByteCode:
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].b = 0;");
            break;

        case ByteCodeOp::NegBool:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].b ? 0 : 1;");
            break;
        case ByteCodeOp::NegF32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = -r[", ip->a.u32, "].f32;");
            break;
        case ByteCodeOp::NegF64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = -r[", ip->a.u32, "].f64;");
            break;
        case ByteCodeOp::NegS32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = -r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::NegS64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = -r[", ip->a.u32, "].s64;");
            break;

        case ByteCodeOp::InvertS8:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s8 = ~r[", ip->a.u32, "].s8;");
            break;
        case ByteCodeOp::InvertS16:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = ~r[", ip->a.u32, "].s16;");
            break;
        case ByteCodeOp::InvertS32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = ~r[", ip->a.u32, "].s32;");
            break;
        case ByteCodeOp::InvertS64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = ~r[", ip->a.u32, "].s64;");
            break;
        case ByteCodeOp::InvertU8:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u8 = ~r[", ip->a.u32, "].u8;");
            break;
        case ByteCodeOp::InvertU16:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u16 = ~r[", ip->a.u32, "].u16;");
            break;
        case ByteCodeOp::InvertU32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 = ~r[", ip->a.u32, "].u32;");
            break;
        case ByteCodeOp::InvertU64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = ~r[", ip->a.u32, "].u64;");
            break;

        case ByteCodeOp::ClearMaskU32:
        {
            //concat.addStringFormat("r[%u].u32 &= 0x%x;", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::ClearMaskU64:
        {
            //concat.addStringFormat("r[%u].u64 &= 0x%llx;", ip->a.u32, ip->b.u64);
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r0), CST_RB64);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastBool8:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u8 ? 1 : 0;");
            break;
        case ByteCodeOp::CastBool16:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u16 ? 1 : 0;");
            break;
        case ByteCodeOp::CastBool32:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u32 ? 1 : 0;");
            break;
        case ByteCodeOp::CastBool64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u64 ? 1 : 0;");
            break;

        case ByteCodeOp::CastS8S16:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = (swag_int16_t) r[", ip->a.u32, "].s8;");
            auto r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), llvm::Type::getInt16Ty(context), true);
            r0      = TO_PTR_I16(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16S32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (swag_int32_t) r[", ip->a.u32, "].s16;");
            auto r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), llvm::Type::getInt32Ty(context), true);
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32S64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = (swag_int64_t) r[", ip->a.u32, "].s32;");
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), llvm::Type::getInt64Ty(context), true);
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].s32;");
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), llvm::Type::getFloatTy(context));
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS64F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].s64;");
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), llvm::Type::getFloatTy(context));
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS64F64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (swag_float64_t) r[", ip->a.u32, "].s64;");
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), llvm::Type::getDoubleTy(context));
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU32F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].u32;");
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), llvm::Type::getFloatTy(context));
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU64F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].u64;");
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), llvm::Type::getFloatTy(context));
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU64F64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (swag_float64_t) r[", ip->a.u32, "].u64;");
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), llvm::Type::getDoubleTy(context));
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF32S32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (swag_int32_t) r[", ip->a.u32, "].f32;");
            auto r0 = TO_PTR_F32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), llvm::Type::getInt32Ty(context));
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF32F64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (swag_float64_t) r[", ip->a.u32, "].f32;");
            auto r0 = TO_PTR_F32(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPExt, builder.CreateLoad(r0), llvm::Type::getDoubleTy(context));
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF64S64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = (swag_int64_t) r[", ip->a.u32, "].f64;");
            auto r0 = TO_PTR_F64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), llvm::Type::getInt64Ty(context));
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF64F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (swag_float32_t) r[", ip->a.u32, "].f64;");
            auto r0 = TO_PTR_F64(builder.CreateInBoundsGEP(allocR, CST_RA32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPTrunc, builder.CreateLoad(r0), llvm::Type::getFloatTy(context));
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CopyRCtoRR:
            //CONCAT_STR_2(concat, "*rr", ip->a.u32, " = r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRRtoRC:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = *rr", ip->b.u32, ";");
            break;
        case ByteCodeOp::CopyRCtoRRCall:
            // CONCAT_STR_2(concat, "rt[", ip->a.u32, "] = r[", ip->b.u32, "];");
            break;
        case ByteCodeOp::CopyRRtoRCCall:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = rt[", ip->b.u32, "];");
            break;
        case ByteCodeOp::GetFromStackParam64:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = *rp", ip->c.u32, ";");
            break;
        case ByteCodeOp::MakePointerToStackParam:
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (swag_uint8_t*) &rp", ip->c.u32, "->pointer;");
            break;
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            break;

        case ByteCodeOp::MinusToTrue:
            //concat.addStringFormat("r[%u].b = r[%u].s32 < 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::MinusZeroToTrue:
            //concat.addStringFormat("r[%u].b = r[%u].s32 <= 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::PlusToTrue:
            //concat.addStringFormat("r[%u].b = r[%u].s32 > 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;
        case ByteCodeOp::PlusZeroToTrue:
            //concat.addStringFormat("r[%u].b = r[%u].s32 >= 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            break;

        case ByteCodeOp::MakeLambda:
        {
            //auto funcBC = (ByteCode*) ip->b.pointer;
            //SWAG_ASSERT(funcBC);
            //concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &%s;", ip->a.u32, funcBC->callName().c_str());
        }
        break;

        case ByteCodeOp::MakeLambdaForeign:
        {
            //auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            //SWAG_ASSERT(funcNode);
            //SWAG_ASSERT(funcNode->attributeFlags & ATTRIBUTE_FOREIGN);
            //TypeInfoFuncAttr* typeFuncNode = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
            //ComputedValue     foreignValue;
            //typeFuncNode->attributes.getValue("swag.foreign", "function", foreignValue);
            //SWAG_ASSERT(!foreignValue.text.empty());
            //concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &%s;", ip->a.u32, foreignValue.text.c_str());
            break;
        }

        case ByteCodeOp::PushRAParam:
            //pushRAParams.push_back(ip->a.u32);
            break;
        case ByteCodeOp::CopySP:
            //concat.addStringFormat("r[%u].pointer = (swag_uint8_t*) &r[%u];", ip->a.u32, ip->c.u32);
            break;
        case ByteCodeOp::CopySPVaargs:
        {
            //concat.addStringFormat("swag_register_t vaargs%u[] = { 0, ", vaargsIdx);
            //int idxParam = (int) pushRAParams.size() - 1;
            //while (idxParam >= 0)
            //{
            //    concat.addStringFormat("r[%u], ", pushRAParams[idxParam]);
            //    idxParam--;
            //}

            //CONCAT_FIXED_STR(concat, "}; ");
            //concat.addStringFormat("r[%u].pointer = sizeof(swag_register_t) + (swag_uint8_t*) &vaargs%u; ", ip->a.u32, vaargsIdx);
            //concat.addStringFormat("vaargs%u[0].pointer = r[%u].pointer;", vaargsIdx, ip->a.u32);
            //vaargsIdx++;
            break;
        }

        case ByteCodeOp::LambdaCall:
        {
            //TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            //concat.addStringFormat("if(r[%u].u64 & 0x%llx) { ", ip->a.u32, SWAG_LAMBDA_MARKER);

            //CONCAT_STR_1(concat, "__process_infos.byteCodeRun(r[", ip->a.u32, "].pointer");
            //if (typeFuncBC->numReturnRegisters() + typeFuncBC->numParamsRegisters())
            //    concat.addChar(',');
            //addCallParameters(concat, typeFuncBC, pushRAParams);
            //CONCAT_FIXED_STR(concat, ");");

            //// Need to output the function prototype too
            //CONCAT_FIXED_STR(concat, "} else { ((void(*)(");
            //for (int j = 0; j < typeFuncBC->numReturnRegisters() + typeFuncBC->numParamsRegisters(); j++)
            //{
            //    if (j)
            //        concat.addChar(',');
            //    CONCAT_FIXED_STR(concat, "swag_register_t*");
            //}

            //CONCAT_FIXED_STR(concat, "))");

            //// Then the call
            //CONCAT_STR_1(concat, " r[", ip->a.u32, "].pointer)");

            //// Then the parameters
            //concat.addChar('(');
            //addCallParameters(concat, typeFuncBC, pushRAParams);
            //CONCAT_FIXED_STR(concat, "); }");
            //pushRAParams.clear();
        }
        break;

        case ByteCodeOp::LocalCall:
        {
            //auto              funcBC     = (ByteCode*) ip->a.pointer;
            //TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;
            //concat.addString(funcBC->callName());
            //concat.addChar('(');
            //addCallParameters(concat, typeFuncBC, pushRAParams);
            //pushRAParams.clear();
            //CONCAT_FIXED_STR(concat, ");");
        }
        break;

        case ByteCodeOp::ForeignCall:
            //SWAG_CHECK(emitForeignCall(concat, moduleToGen, ip, pushRAParams));
            break;

        default:
            moduleToGen->internalError(format("unknown instruction '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
            break;
        }
    }

    builder.CreateRetVoid();
    return true;
}
