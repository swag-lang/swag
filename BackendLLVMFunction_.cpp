#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"

#define CST_RA32 builder.getInt32(ip->a.u32)
#define CST_RB32 builder.getInt32(ip->b.u32)
#define CST_RC32 builder.getInt32(ip->c.u32)
#define CST_RD32 builder.getInt32(ip->d.u32)
#define CST_RA64 builder.getInt64(ip->a.u64)
#define CST_RB64 builder.getInt64(ip->b.u64)
#define CST_RC64 builder.getInt64(ip->c.u64)
#define GEP_I32(__data, __i32) (__i32 ? builder.CreateInBoundsGEP(__data, builder.getInt32(__i32)) : __data)
#define TO_PTR_PTR_I8(__r) builder.CreatePointerCast(__r, llvm::Type::getInt8PtrTy(context)->getPointerTo())
#define TO_PTR_PTR_I16(__r) builder.CreatePointerCast(__r, llvm::Type::getInt16PtrTy(context)->getPointerTo())
#define TO_PTR_PTR_I32(__r) builder.CreatePointerCast(__r, llvm::Type::getInt32PtrTy(context)->getPointerTo())
#define TO_PTR_PTR_I64(__r) builder.CreatePointerCast(__r, llvm::Type::getInt64PtrTy(context)->getPointerTo())
#define TO_PTR_PTR_F32(__r) builder.CreatePointerCast(__r, llvm::Type::getFloatPtrTy(context)->getPointerTo())
#define TO_PTR_PTR_F64(__r) builder.CreatePointerCast(__r, llvm::Type::getDoublePtrTy(context)->getPointerTo())
#define TO_PTR_PTR_PTR_I8(__r) builder.CreatePointerCast(__r, llvm::Type::getInt8PtrTy(context)->getPointerTo()->getPointerTo())
#define TO_PTR_I64(__r) builder.CreatePointerCast(__r, llvm::Type::getInt64PtrTy(context))
#define TO_PTR_I32(__r) builder.CreatePointerCast(__r, llvm::Type::getInt32PtrTy(context))
#define TO_PTR_I16(__r) builder.CreatePointerCast(__r, llvm::Type::getInt16PtrTy(context))
#define TO_PTR_I8(__r) builder.CreatePointerCast(__r, llvm::Type::getInt8PtrTy(context))
#define TO_PTR_F64(__r) builder.CreatePointerCast(__r, llvm::Type::getDoublePtrTy(context))
#define TO_PTR_F32(__r) builder.CreatePointerCast(__r, llvm::Type::getFloatPtrTy(context))
#define TO_BOOL(__r) builder.CreateIntCast(__r, llvm::Type::getInt1Ty(context), false)

inline llvm::Value* toPtrNative(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Value* v, NativeTypeKind k)
{
    switch (k)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        return TO_PTR_I8(v);
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        return TO_PTR_I16(v);
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        return TO_PTR_I32(v);
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        return TO_PTR_I64(v);
    case NativeTypeKind::F32:
        return TO_PTR_F32(v);
    case NativeTypeKind::F64:
        return TO_PTR_F64(v);
    }

    SWAG_ASSERT(false);
    return nullptr;
}

inline llvm::Value* toPtrPtrNative(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Value* v, NativeTypeKind k)
{
    switch (k)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
    case NativeTypeKind::Void:
        return TO_PTR_PTR_I8(v);
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        return TO_PTR_PTR_I16(v);
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        return TO_PTR_PTR_I32(v);
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        return TO_PTR_PTR_I64(v);
    case NativeTypeKind::F32:
        return TO_PTR_PTR_F32(v);
    case NativeTypeKind::F64:
        return TO_PTR_PTR_F64(v);
    }

    SWAG_ASSERT(false);
    return nullptr;
}

#define TO_PTR_NATIVE(__value, __kind) toPtrNative(context, builder, __value, __kind)
#define TO_PTR_PTR_NATIVE(__value, __kind) toPtrPtrNative(context, builder, __value, __kind)

BackendFunctionBodyJob* BackendLLVM::newFunctionJob()
{
    return g_Pool_backendLLVMFunctionBodyJob.alloc();
}

bool BackendLLVM::swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo, llvm::Type** llvmType)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& context         = *perThread[ct][precompileIndex].context;

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
        auto finalType       = TypeManager::concreteType(typeInfoPointer->finalType);

        uint32_t ptrCount = typeInfoPointer->ptrCount;
        if (ptrCount != 1)
            return moduleToGen->internalError(format("swagTypeToLLVMType, invalid pointer type '%s'", typeInfo->name.c_str()));

        if (finalType->kind == TypeInfoKind::Slice ||
            finalType->kind == TypeInfoKind::Array ||
            finalType->kind == TypeInfoKind::Struct ||
            finalType->kind == TypeInfoKind::Interface ||
            finalType->isNative(NativeTypeKind::Any) ||
            finalType->isNative(NativeTypeKind::String) ||
            finalType->kind == TypeInfoKind::Reference)
        {
            *llvmType = llvm::Type::getInt8PtrTy(context);
        }
        else if (finalType->kind == TypeInfoKind::Native)
        {
            switch (finalType->nativeType)
            {
            case NativeTypeKind::Bool:
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                *llvmType = llvm::Type::getInt8PtrTy(context);
                return true;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                *llvmType = llvm::Type::getInt16PtrTy(context);
                return true;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Char:
                *llvmType = llvm::Type::getInt32PtrTy(context);
                return true;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                *llvmType = llvm::Type::getInt64PtrTy(context);
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
            return moduleToGen->internalError(format("swagTypeToLLVMType, invalid type '%s'", finalType->name.c_str()));
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
        *llvmType = llvm::Type::getInt8PtrTy(context);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            *llvmType = llvm::Type::getInt8Ty(context);
            return true;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            *llvmType = llvm::Type::getInt8Ty(context);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            *llvmType = llvm::Type::getInt16Ty(context);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            *llvmType = llvm::Type::getInt32Ty(context);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            *llvmType = llvm::Type::getInt64Ty(context);
            return true;
        case NativeTypeKind::F32:
            *llvmType = llvm::Type::getFloatTy(context);
            return true;
        case NativeTypeKind::F64:
            *llvmType = llvm::Type::getDoubleTy(context);
            return true;
        case NativeTypeKind::Void:
            *llvmType = llvm::Type::getVoidTy(context);
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

    // Function type
    llvm::FunctionType* funcType = nullptr;
    SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFunc, &funcType));

    // Public foreign name
    auto name = Ast::computeFullNameForeign(node, true);

    // Create function
    llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name.c_str(), modu);
    func->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(block);

    // Total number of registers
    auto n = typeFunc->numReturnRegisters();
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        n += typeParam->numRegisters();
    }

    auto allocRR = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(n));

    // Affect registers
    int idx = typeFunc->numReturnRegisters();

    // Return by copy
    bool returnByCopy = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;
    if (returnByCopy)
    {
        // rr0 = result
        auto rr0  = TO_PTR_PTR_I8(allocRR);
        auto cst0 = TO_PTR_I8(func->getArg((int) func->arg_size() - 1));
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
            auto rr0  = TO_PTR_PTR_I8(GEP_I32(allocRR, idx));
            auto cst0 = TO_PTR_I8(func->getArg(0));
            builder.CreateStore(cst0, rr0);

            auto rr1 = TO_PTR_I32(GEP_I32(allocRR, idx + 1));
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
        auto rr0       = GEP_I32(allocRR, idx);

        if (typeParam->kind == TypeInfoKind::Pointer)
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
            auto rr1 = TO_PTR_I32(GEP_I32(allocRR, idx + 1));
            builder.CreateStore(func->getArg(argIdx + 1), rr1);
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
            auto cst1 = TO_PTR_I8(func->getArg(argIdx + 1));
            auto rr1  = GEP_I32(allocRR, idx + 1);
            builder.CreateStore(cst1, TO_PTR_PTR_I8(rr1));
        }
        else if (typeParam->kind == TypeInfoKind::Struct ||
                 typeParam->kind == TypeInfoKind::Array ||
                 typeParam->kind == TypeInfoKind::Interface)
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            auto r = TO_PTR_NATIVE(rr0, typeParam->nativeType);
            if (!r)
                return moduleToGen->internalError("emitFuncWrapperPublic, invalid param type");
            builder.CreateStore(func->getArg(argIdx), r);
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
        auto rr0 = builder.CreateInBoundsGEP(allocRR, builder.getInt32(i));
        args.push_back(rr0);
    }

    auto fcc = modu.getFunction(bc->callName().c_str());
    builder.CreateCall(fcc, args);

    // Return
    if (typeFunc->numReturnRegisters() && !returnByCopy)
    {
        auto rr0        = allocRR;
        auto returnType = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM);

        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->kind == TypeInfoKind::Interface ||
            returnType->isNative(NativeTypeKind::Any) ||
            returnType->isNative(NativeTypeKind::String))
        {
            //*((void **) result) = rr0.pointer
            auto loadInst = builder.CreateLoad(rr0);
            auto arg0     = TO_PTR_I64(func->getArg((int) func->arg_size() - 1));
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
            auto arg0     = builder.CreateCast(llvm::Instruction::CastOps::IntToPtr, loadInst, funcType->getReturnType());
            builder.CreateRet(arg0);
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            auto r = TO_PTR_NATIVE(rr0, returnType->nativeType);
            if (!r)
                return moduleToGen->internalError("emitFuncWrapperPublic, invalid return type");
            auto loadInst = builder.CreateLoad(r);
            builder.CreateRet(loadInst);
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

llvm::BasicBlock* BackendLLVM::getOrCreateLabel(const BuildParameters& buildParameters, llvm::Function* func, int32_t ip)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& context         = *pp.context;

    auto it = pp.labels.find(ip);
    if (it == pp.labels.end())
    {
        llvm::BasicBlock* label = llvm::BasicBlock::Create(context, format("%d", ip).c_str(), func);
        pp.labels[ip]           = label;
        return label;
    }

    return it->second;
}

void BackendLLVM::createAssert(const BuildParameters& buildParameters, llvm::Value* toTest, ByteCodeInstruction* ip, const char* msg)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    auto r1 = builder.CreateGlobalString(normalizePath(ip->node->sourceFile->path).c_str());
    auto v1 = TO_PTR_I8(builder.CreateInBoundsGEP(r1, {pp.cst0_i32, pp.cst0_i32}));
    auto r2 = builder.getInt32(ip->node->token.startLocation.line + 1);
    auto r3 = builder.CreateGlobalString(msg);
    auto v3 = TO_PTR_I8(builder.CreateInBoundsGEP(r3, {pp.cst0_i32, pp.cst0_i32}));
    builder.CreateCall(modu.getFunction("swag_runtime_assert"), {toTest, v1, r2, v3, g_CommandLine.devMode ? pp.cst1_i8 : pp.cst0_i8});
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
    bool  ok       = true;

    // Function prototype
    llvm::FunctionType* funcType = createFunctionTypeInternal(buildParameters, typeFunc);
    llvm::Function*     func     = (llvm::Function*) modu.getOrInsertFunction(bc->callName().c_str(), funcType).getCallee();

    // Content
    llvm::BasicBlock* block         = llvm::BasicBlock::Create(context, "entry", func);
    bool              blockIsClosed = false;
    builder.SetInsertPoint(block);
    pp.labels.clear();
    bc->markLabels();

    // Reserve registers
    llvm::AllocaInst* allocR = nullptr;
    if (bc->maxReservedRegisterRC)
        allocR = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt32(bc->maxReservedRegisterRC));
    llvm::AllocaInst* allocRT = nullptr;
    if (bc->maxCallResults)
        allocRT = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt32(bc->maxCallResults));

    // Stack
    llvm::AllocaInst* allocStack = nullptr;
    if (typeFunc->stackSize)
        allocStack = builder.CreateAlloca(builder.getInt8Ty(), builder.getInt32(typeFunc->stackSize));

    // Generate bytecode
    auto                   ip = bc->out;
    VectorNative<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
        {
            SWAG_ASSERT(!(ip->flags & BCI_JUMP_DEST));
            continue;
        }

        // If we are the destination of a jump, be sure we have a block, and from now insert into that block
        if ((ip->flags & BCI_JUMP_DEST) || blockIsClosed)
        {
            auto label = getOrCreateLabel(buildParameters, func, i);
            if (!blockIsClosed)
                builder.CreateBr(label); // Make a jump from previous block to this one
            else
                blockIsClosed = false; // Each block must be closed by a valid terminator instruction
            builder.SetInsertPoint(label);
        }

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::CopySPtoBP:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::BoundCheckString:
        {
            //concat.addStringFormat("swag_runtime_assert(r[%u].u32 <= r[%u].u32, \"%s\", %d, \": index out of range\");", ip->a.u32, ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            auto v0 = builder.CreateICmpULE(r0, r1);
            auto t0 = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            createAssert(buildParameters, t0, ip, "index out of range");
            break;
        }
        case ByteCodeOp::BoundCheck:
        {
            //concat.addStringFormat("swag_runtime_assert(r[%u].u32 < r[%u].u32, \"%s\", %d, \": index out of range\");", ip->a.u32, ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            auto v0 = builder.CreateICmpULT(r0, r1);
            auto t0 = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            createAssert(buildParameters, t0, ip, "index out of range");
            break;
        }

        case ByteCodeOp::IncPointerVB32:
        {
            //concat.addStringFormat("r[%u].pointer += %d;", ip->a.u32, ip->b.s32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(r0);
            auto r2 = builder.CreateInBoundsGEP(r1, CST_RB32);
            builder.CreateStore(r2, r0);
            break;
        }
        case ByteCodeOp::IncPointer32:
        {
            //concat.addStringFormat("r[%u].pointer = r[%u].pointer + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            auto r3 = builder.CreateInBoundsGEP(r1, r2);
            builder.CreateStore(r3, r0);
            break;
        }
        case ByteCodeOp::DecPointer32:
        {
            //concat.addStringFormat("r[%u].pointer = r[%u].pointer - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r2 = builder.CreateNeg(builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32))));
            auto r3 = builder.CreateInBoundsGEP(r1, r2);
            builder.CreateStore(r3, r0);
            break;
        }
        break;

        case ByteCodeOp::DeRef8:
        {
            //concat.addStringFormat("r[%u].u64 = *(__ui8_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::DeRef16:
        {
            //concat.addStringFormat("r[%u].u64 = *(__ui16_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I16(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::DeRef32:
        {
            //concat.addStringFormat("r[%u].u64 = *(__ui32_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I32(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::DeRef64:
        {
            //concat.addStringFormat("r[%u].u64 = *(__ui64_t*) r[%u].pointer;", ip->a.u32, ip->a.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::DeRefPointer:
        {
            //concat.addStringFormat("r[%u].pointer = *(__ui8_t**) (r[%u].pointer + %u);", ip->b.u32, ip->a.u32, ip->c.u32);
            auto r0   = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1   = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto ptr  = builder.CreateLoad(r0);
            auto ptr8 = builder.CreateInBoundsGEP(ptr, CST_RC32);
            auto v8   = builder.CreateLoad(TO_PTR_PTR_I8(ptr8));
            builder.CreateStore(v8, r1);
            break;
        }
        case ByteCodeOp::DeRefStringSlice:
        {
            //concat.addStringFormat("r[%u].u64 = *(__ui64_t*) (r[%u].pointer + 8); ", ip->b.u32, ip->a.u32);
            //concat.addStringFormat("r[%u].pointer = *(__ui8_t**) r[%u].pointer; ", ip->a.u32, ip->a.u32);
            auto r0   = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1   = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto ptr  = builder.CreateLoad(r0);
            auto ptr8 = builder.CreateInBoundsGEP(ptr, builder.getInt32(8));
            auto v0   = builder.CreateLoad(TO_PTR_PTR_I8(ptr));
            auto v8   = builder.CreateLoad(TO_PTR_PTR_I8(ptr8));
            builder.CreateStore(v8, r1);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::Mul64byVB32:
        {
            //concat.addStringFormat("r[%u].s64 *= %u;", ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = llvm::ConstantInt::get(builder.getInt64Ty(), ip->b.u32);
            builder.CreateStore(builder.CreateMul(builder.CreateLoad(r0), r1), r0);
            break;
        }
        case ByteCodeOp::Div64byVB32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //  concat.addStringFormat("swag_runtime_assert(r[%u].u32, \"%s\", %d, \"division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].s64 /= %u;", ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = llvm::ConstantInt::get(builder.getInt64Ty(), ip->b.u32);
            builder.CreateStore(builder.CreateUDiv(builder.CreateLoad(r0), r1), r0);
            break;
        }

        case ByteCodeOp::GetFromDataSeg8:
        {
            //concat.addStringFormat("r[%u].u8 = *(__ui8_t*) (__ms + %u);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(pp.mutableSeg, {pp.cst0_i32, CST_RB32}));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromDataSeg16:
        {
            //concat.addStringFormat("r[%u].u16 = *(__ui16_t*) (__ms + %u);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I16(builder.CreateInBoundsGEP(pp.mutableSeg, {pp.cst0_i32, CST_RB32}));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromDataSeg32:
        {
            //concat.addStringFormat("r[%u].u32 = *(__ui32_t*) (__ms + %u);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(pp.mutableSeg, {pp.cst0_i32, CST_RB32}));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromDataSeg64:
        {
            //concat.addStringFormat("r[%u].u64 = *(__ui64_t*) (__ms + %u);", ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(pp.mutableSeg, {pp.cst0_i32, CST_RB32}));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::GetFromBssSeg8:
        {
            //concat.addStringFormat("r[%u].u8 = *(__ui8_t*) (__bs + %u);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(pp.bssSeg, {pp.cst0_i32, CST_RB32}));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromBssSeg16:
        {
            //concat.addStringFormat("r[%u].u16 = *(__ui16_t*) (__bs + %u);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I16(builder.CreateInBoundsGEP(pp.bssSeg, {pp.cst0_i32, CST_RB32}));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromBssSeg32:
        {
            //concat.addStringFormat("r[%u].u32 = *(__ui32_t*) (__bs + %u);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(pp.bssSeg, {pp.cst0_i32, CST_RB32}));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromBssSeg64:
        {
            //concat.addStringFormat("r[%u].u64 = *(__ui64_t*) (__bs + %u);", ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(pp.bssSeg, {pp.cst0_i32, CST_RB32}));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::SetZeroStack8:
        {
            //CONCAT_STR_1(concat, "*(__ui8_t*)(stack + ", ip->a.u32, ") = 0;");
            auto r0 = builder.CreateInBoundsGEP(allocStack, CST_RA32);
            builder.CreateStore(pp.cst0_i8, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack16:
        {
            //CONCAT_STR_1(concat, "*(__ui16_t*)(stack + ", ip->a.u32, ") = 0;");
            auto r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i16, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack32:
        {
            //CONCAT_STR_1(concat, "*(__ui32_t*)(stack + ", ip->a.u32, ") = 0;");
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i32, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack64:
        {
            //CONCAT_STR_1(concat, "*(__ui64_t*)(stack + ", ip->a.u32, ") = 0;");
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i64, r0);
            break;
        }
        case ByteCodeOp::SetZeroStackX:
        {
            //concat.addStringFormat("swag_runtime_memset(stack + %u, 0, %u);", ip->a.u32, ip->b.u32);
            auto r0 = builder.CreateInBoundsGEP(allocStack, CST_RA32);
            builder.CreateCall(modu.getFunction("swag_runtime_memset"), {r0, pp.cst0_i32, builder.getInt64(ip->b.u32)});
            break;
        }

        case ByteCodeOp::SetZeroAtPointer8:
        {
            //CONCAT_STR_2(concat, "*(__ui8_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i8, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointer16:
        {
            //CONCAT_STR_2(concat, "*(__ui16_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            auto r0 = TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i16, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointer32:
        {
            //CONCAT_STR_2(concat, "*(__ui32_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            auto r0 = TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i32, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointer64:
        {
            //CONCAT_STR_2(concat, "*(__ui64_t*)(r[", ip->a.u32, "].pointer + ", ip->b.u32, ") = 0;");
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i64, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointerX:
        {
            //concat.addStringFormat("swag_runtime_memset(r[%u].pointer, 0, %u);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(r0);
            builder.CreateCall(modu.getFunction("swag_runtime_memset"), {r1, pp.cst0_i32, builder.getInt64(ip->b.u32)});
            break;
        }
        case ByteCodeOp::SetZeroAtPointerXRB:
        {
            //concat.addStringFormat("swag_runtime_memset(r[%u].pointer, 0, r[%u].u32 * %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            auto v0 = builder.CreateMul(r2, builder.getInt32(ip->c.u32));
            v0      = builder.CreateIntCast(v0, builder.getInt64Ty(), false);
            builder.CreateCall(modu.getFunction("swag_runtime_memset"), {r0, pp.cst0_i32, v0});
            break;
        }

        case ByteCodeOp::MakeDataSegPointer:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = __ms + ", ip->b.u32, ";");
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(pp.mutableSeg, {pp.cst0_i32, CST_RB32});
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeBssSegPointer:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = __bs + ", ip->b.u32, ";");
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(pp.bssSeg, {pp.cst0_i32, CST_RB32});
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeConstantSegPointer:
        {
            //concat.addStringFormat("r[%u].pointer = (__ui8_t*) (__cs + %u); ", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(pp.constantSeg, {pp.cst0_i32, CST_RB32});
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeConstantSegPointerOC:
        {
            //concat.addStringFormat("r[%u].pointer = __cs + %u; ", ip->a.u32, (uint32_t)(ip->c.u64 >> 32));
            //concat.addStringFormat("r[%u].u64 = %u;", ip->b.u32, (ip->c.u64) & 0xFFFFFFFF);
            auto r0     = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto offset = (uint32_t)(ip->c.u64 >> 32);
            auto r1     = builder.CreateInBoundsGEP(pp.constantSeg, {pp.cst0_i32, builder.getInt32(offset)});
            builder.CreateStore(r1, r0);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(llvm::ConstantInt::get(builder.getInt64Ty(), (ip->c.u64 & 0xFFFFFFFF)), r2);
            break;
        }

        case ByteCodeOp::MakePointerToStack:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = stack + ", ip->b.u32, ";");
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::GetFromStack8:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u8 = *(__ui8_t*) (stack + ", ip->b.u32, ");");
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromStack16:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u16 = *(__ui16_t*) (stack + ", ip->b.u32, ");");
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromStack32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 = *(__ui32_t*) (stack + ", ip->b.u32, ");");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromStack64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u64 = *(__ui64_t*) (stack + ", ip->b.u32, ");");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::MemCpyVC32:
        {
            //concat.addStringFormat("swag_runtime_memcpy(r[%u].pointer, r[%u].pointer, %u);", ip->a.u32, ip->b.u32, ip->c.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            r0      = builder.CreateLoad(r0);
            r1      = builder.CreateLoad(r1);
            builder.CreateCall(modu.getFunction("swag_runtime_memcpy"), {r0, r1, builder.getInt64(ip->c.u32)});
            break;
        }
        case ByteCodeOp::MemCpy:
        {
            //concat.addStringFormat("swag_runtime_memcpy(r[%u].pointer, r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            r0      = builder.CreateLoad(r0);
            r1      = builder.CreateLoad(r1);
            r2      = builder.CreateIntCast(builder.CreateLoad(r2), builder.getInt64Ty(), false);
            builder.CreateCall(modu.getFunction("swag_runtime_memcpy"), {r0, r1, r2});
            break;
        }
        case ByteCodeOp::MemSet:
        {
            //concat.addStringFormat("swag_runtime_memset(r[%u].pointer, r[%u].u8, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            r0      = builder.CreateLoad(r0);
            r1      = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt32Ty(), false);
            r2      = builder.CreateIntCast(builder.CreateLoad(r2), builder.getInt64Ty(), false);
            builder.CreateCall(modu.getFunction("swag_runtime_memset"), {r0, r1, r2});
            break;
        }
        case ByteCodeOp::MemCmp:
        {
            //concat.addStringFormat("r[%u].s32 = swag_runtime_memcmp(r[%u].pointer, r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r2 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r3 = TO_PTR_I32(builder.CreateInBoundsGEP(allocR, CST_RD32));
            r1      = builder.CreateLoad(r1);
            r2      = builder.CreateLoad(r2);
            r3      = builder.CreateIntCast(builder.CreateLoad(r3), builder.getInt64Ty(), false);
            builder.CreateStore(builder.CreateCall(modu.getFunction("swag_runtime_memcmp"), {r1, r2, r3}), r0);
            break;
        }

        case ByteCodeOp::CopyVBtoRA32:
        {
            //concat.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(CST_RB32, r0);
            break;
        }
        case ByteCodeOp::CopyVBtoRA64:
        {
            //concat.addStringFormat("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(CST_RB64, r0);
            break;
        }

        case ByteCodeOp::CopyRBtoRA:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = r[", ip->b.u32, "];");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::CopyRBAddrToRA:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (__ui8_t*) &r[", ip->b.u32, "];");
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::ClearRA:
        {
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u64 = 0;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(pp.cst0_i64, r0);
            break;
        }
        case ByteCodeOp::DecrementRA32:
        {
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32--;");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), pp.cst1_i32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::IncrementRA32:
        {
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].u32++;");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), pp.cst1_i32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::AddVBtoRA32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].u32 += ", ip->b.u32, ";");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::SetAtPointer8:
        {
            //concat.addStringFormat("*(__ui8_t*)(r[%u].pointer + %u) = r[%u].u8;", ip->a.u32, ip->c.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = builder.CreateLoad(r0);
            r0      = TO_PTR_I8(builder.CreateInBoundsGEP(r0, CST_RC32));
            auto r1 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::SetAtPointer16:
        {
            //concat.addStringFormat("*(__ui16_t*)(r[%u].pointer + %u) = r[%u].u16;", ip->a.u32, ip->c.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = builder.CreateLoad(r0);
            r0      = TO_PTR_I16(builder.CreateInBoundsGEP(r0, CST_RC32));
            auto r1 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::SetAtPointer32:
        {
            //concat.addStringFormat("*(__ui32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = builder.CreateLoad(r0);
            r0      = TO_PTR_I32(builder.CreateInBoundsGEP(r0, CST_RC32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::SetAtPointer64:
        {
            //concat.addStringFormat("*(__ui64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = builder.CreateLoad(r0);
            r0      = TO_PTR_I64(builder.CreateInBoundsGEP(r0, CST_RC32));
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::BinOpPlusS32:
        {
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 + r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpPlusS64:
        {
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 + r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpPlusF32:
        {
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpPlusF64:
        {
            //concat.addStringFormat("r[%u].f64= r[%u].f64 + r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpMinusS32:
        {
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 - r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMinusS64:
        {
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 - r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMinusF32:
        {
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 - r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMinusF64:
        {
            //concat.addStringFormat("r[%u].f64 = r[%u].f64 - r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpMulS32:
        {
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 * r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMulS64:
        {
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 * r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMulF32:
        {
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 * r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpMulF64:
        {
            //concat.addStringFormat("r[%u].f64= r[%u].f64 * r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpXorU32:
        {
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 ^ r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpXorU64:
        {
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 ^ r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpShiftLeftU32:
        {
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpShiftLeftU64:
        {
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto vb = builder.CreateIntCast(builder.CreateLoad(r2), builder.getInt64Ty(), false);
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), vb);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpShiftRightS32:
        {
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpShiftRightS64:
        {
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto vb = builder.CreateIntCast(builder.CreateLoad(r2), builder.getInt64Ty(), false);
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), vb);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpShiftRightU32:
        {
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpShiftRightU64:
        {
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto vb = builder.CreateIntCast(builder.CreateLoad(r2), builder.getInt64Ty(), false);
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), vb);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpShiftRightU64VB:
        {
            //concat.addStringFormat("r[%u].u64 >>= %u;", ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto vb = builder.CreateIntCast(CST_RB32, builder.getInt64Ty(), false);
            auto v0 = builder.CreateLShr(builder.CreateLoad(r0), vb);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpModuloS32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s32", ip->b.u32).c_str(), "modulo operand is zero");
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 %% r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpModuloS64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s64", ip->b.u32).c_str(), "modulo operand is zero");
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 %% r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpModuloU32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u32", ip->b.u32).c_str(), "modulo operand is zero");
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 %% r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpModuloU64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u64", ip->b.u32).c_str(), "modulo operand is zero");
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 %% r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpDivS32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].s32, \"%s\", %d, \"division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 / r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpDivS64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].s64, \"%s\", %d, \"division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 / r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpDivU32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].u32, \"%s\", %d, \"division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].u32 = r[%u].u32 / r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpDivU64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].u64, \"%s\", %d, \"division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].u64 = r[%u].u64 / r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpDivF32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].f32 != 0, \"%s\", %d, \"division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].f32 = r[%u].f32 / r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_F32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(builder.CreateFCmpUNE(r0, pp.cst0_f32), builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpDivF64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    concat.addStringFormat("swag_runtime_assert(r[%u].f64 != 0, \"%s\", %d, \"division by zero\");", ip->b.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            //concat.addStringFormat("r[%u].f64 = r[%u].f64 / r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_F64(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(builder.CreateFCmpUNE(r0, pp.cst0_f64), builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpAnd:
        {
            //concat.addStringFormat("r[%u].b = r[%u].b && r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpOr:
        {
            //concat.addStringFormat("r[%u].b = r[%u].b || r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpBitmaskAndS32:
        {
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 & r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpBitmaskAndS64:
        {
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 & r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpBitmaskOrS32:
        {
            //concat.addStringFormat("r[%u].s32 = r[%u].s32 & r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::BinOpBitmaskOrS64:
        {
            //concat.addStringFormat("r[%u].s64 = r[%u].s64 | r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = GEP_I32(allocR, ip->c.u32);
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::AffectOpMinusEqS8:
        {
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s8;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        break;
        case ByteCodeOp::AffectOpMinusEqS16:
        {
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s16;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS32:
        {
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS64:
        {
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqF32:
        {
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F32(r0));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqF64:
        {
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].f64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F64(r0));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFSub(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqS8:
        {
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s8;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS16:
        {
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s16;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS32:
        {
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS64:
        {
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqF32:
        {
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F32(r0));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqF64:
        {
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].f64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F64(r0));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqPointer:
        {
            //CONCAT_STR_2(concat, "*(__ui8_t**)(r[", ip->a.u32, "].pointer) += r[", ip->b.u32, "].s32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqPointer:
        {
            //CONCAT_STR_2(concat, "*(__ui8_t**)(r[", ip->a.u32, "].pointer) -= r[", ip->b.u32, "].s32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r1), builder.CreateNeg(builder.CreateLoad(r2)));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpMulEqS8:
        {
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s8;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS16:
        {
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s16;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS32:
        {
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS64:
        {
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqF32:
        {
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F32(r0));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqF64:
        {
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) *= r[", ip->b.u32, "].f64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F64(r0));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpDivEqS8:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s8", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s8;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS16:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s16", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s16;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s32", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s32;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s64", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].s64;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU8:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u8", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__ui8_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u8;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU16:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u16", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__ui16_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u16;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u32", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__ui32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u32;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u64", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__ui64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].u64;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqF32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].f32 != 0", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__f32_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].f32;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_F32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(builder.CreateFCmpUNE(r0, pp.cst0_f32), builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F32(r0));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqF64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].f64 != 0", ip->b.u32).c_str(), "division by zero");
            //CONCAT_STR_2(concat, "*(__f64_t*)(r[", ip->a.u32, "].pointer) /= r[", ip->b.u32, "].f64;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_F64(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(builder.CreateFCmpUNE(r0, pp.cst0_f64), builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "division by zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F64(r0));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpAndEqS8:
        {
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s8;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpAndEqS16:
        {
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s16;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpAndEqS32:
        {
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpAndEqS64:
        {
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) &= r[", ip->b.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpOrEqS8:
        {
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s8;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqS16:
        {
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s16;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqS32:
        {
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqS64:
        {
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) |= r[", ip->b.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpShiftLeftEqS8:
        {
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqS16:
        {
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqS32:
        {
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqS64:
        {
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) <<= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpShiftRightEqS8:
        {
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqS16:
        {
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqS32:
        {
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqS64:
        {
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqU8:
        {
            //CONCAT_STR_2(concat, "*(__ui8_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqU16:
        {
            //CONCAT_STR_2(concat, "*(__ui16_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqU32:
        {
            //CONCAT_STR_2(concat, "*(__ui32_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqU64:
        {
            //CONCAT_STR_2(concat, "*(__ui64_t*)(r[", ip->a.u32, "].pointer) >>= r[", ip->b.u32, "].u32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqS8:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s8", ip->b.u32).c_str(), "modulo operand is zero");
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s8;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS16:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s16", ip->b.u32).c_str(), "modulo operand is zero");
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s16;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s32", ip->b.u32).c_str(), "modulo operand is zero");
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s32;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].s64", ip->b.u32).c_str(), "modulo operand is zero");
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].s64;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU8:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u8", ip->b.u32).c_str(), "modulo operand is zero");
            //CONCAT_STR_2(concat, "*(__ui8_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u8;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU16:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u16", ip->b.u32).c_str(), "modulo operand is zero");
            //CONCAT_STR_2(concat, "*(__ui16_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u16;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU32:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u32", ip->b.u32).c_str(), "modulo operand is zero");
            //CONCAT_STR_2(concat, "*(__ui32_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u32;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU64:
        {
            //if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            //    MK_ASSERT(format("r[%u].u64", ip->b.u32).c_str(), "modulo operand is zero");
            //CONCAT_STR_2(concat, "*(__ui64_t*)(r[", ip->a.u32, "].pointer) %= r[", ip->b.u32, "].u64;");
            if (moduleToGen->buildParameters.target.debugDivZeroCheck || g_CommandLine.debug)
            {
                auto r0 = builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->b.u32)));
                auto t0 = builder.CreateIntCast(r0, builder.getInt8Ty(), false);
                createAssert(buildParameters, t0, ip, "modulo operand is zero");
            }

            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpXOrEqS8:
        {
            //CONCAT_STR_2(concat, "*(__i8_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s8;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXOrEqS16:
        {
            //CONCAT_STR_2(concat, "*(__i16_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s16;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXOrEqS32:
        {
            //CONCAT_STR_2(concat, "*(__i32_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s32;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXOrEqS64:
        {
            //CONCAT_STR_2(concat, "*(__i64_t*)(r[", ip->a.u32, "].pointer) ^= r[", ip->b.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::CompareOpGreaterS32:
        {
            //concat.addStringFormat("r[%u].b = r[%u].s32 > r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateICmpSGT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterS64:
        {
            //concat.addStringFormat("r[%u].b = r[%u].s64 > r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateICmpSGT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterU32:
        {
            //concat.addStringFormat("r[%u].b = r[%u].u32 > r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateICmpUGT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterU64:
        {
            //concat.addStringFormat("r[%u].b = r[%u].u64 > r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateICmpUGT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterF32:
        {
            //concat.addStringFormat("r[%u].b = r[%u].f32 > r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFCmpUGT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterF64:
        {
            //concat.addStringFormat("r[%u].b = r[%u].f64 > r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFCmpUGT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOpLowerS32:
        {
            //concat.addStringFormat("r[%u].b = r[%u].s32 < r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateICmpSLT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerS64:
        {
            //concat.addStringFormat("r[%u].b = r[%u].s64 < r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateICmpSLT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerU32:
        {
            //concat.addStringFormat("r[%u].b = r[%u].u32 < r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateICmpULT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerU64:
        {
            //concat.addStringFormat("r[%u].b = r[%u].u64 < r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateICmpULT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerF32:
        {
            //concat.addStringFormat("r[%u].b = r[%u].f32 < r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFCmpULT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerF64:
        {
            //concat.addStringFormat("r[%u].b = r[%u].f64 < r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFCmpULT(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOpEqual8:
        {
            //concat.addStringFormat("r[%u].b = r[%u].u8 == r[%u].u8;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateICmpEQ(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual16:
        {
            //concat.addStringFormat("r[%u].b = r[%u].u16 == r[%u].u16;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateICmpEQ(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual32:
        {
            //concat.addStringFormat("r[%u].b = r[%u].u32 == r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateICmpEQ(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual64:
        {
            //concat.addStringFormat("r[%u].b = r[%u].u64 == r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto r1 = GEP_I32(allocR, ip->a.u32);
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateICmpEQ(builder.CreateLoad(r1), builder.CreateLoad(r2));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqualInterface:
        {
            //concat.addStringFormat("r[%u].b = (((void **) r[%u].pointer)[0] == ((void **) r[%u].pointer)[0]) && (((void **) r[%u].pointer)[1] == ((void **) r[%u].pointer)[1]);", ip->c.u32, ip->a.u32, ip->b.u32, ip->a.u32, ip->b.u32);
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(GEP_I32(allocR, ip->b.u32)));
            auto v0 = builder.CreateICmpEQ(builder.CreateLoad(r0), builder.CreateLoad(r1));

            auto r2 = GEP_I32(r0, 1);
            auto r3 = GEP_I32(r1, 1);
            auto v1 = builder.CreateICmpEQ(builder.CreateLoad(r2), builder.CreateLoad(r3));

            auto v2 = builder.CreateIntCast(builder.CreateAnd(v0, v1), builder.getInt8Ty(), false);
            auto r4 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v2, r4);
            break;
        }
        case ByteCodeOp::CompareOpEqualString:
        {
            //concat.addStringFormat("r[%u].b = swag_runtime_strcmp((const char*) r[%u].pointer, (const char*) r[%u].pointer, r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            r0      = builder.CreateLoad(r0);
            r1      = builder.CreateLoad(r1);
            auto rs = builder.CreateLoad(r2);
            builder.CreateStore(builder.CreateCall(modu.getFunction("swag_runtime_strcmp"), {r0, r1, rs}), TO_PTR_I8(r2));
            break;
        }

        case ByteCodeOp::Jump:
        {
            //CONCAT_FIXED_STR(concat, "goto _");
            //concat.addS32Str8(ip->a.s32 + i + 1);
            auto label = getOrCreateLabel(buildParameters, func, i + ip->a.s32 + 1);
            builder.CreateBr(label);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotTrue:
        {
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].b) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            auto labelTrue  = getOrCreateLabel(buildParameters, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(buildParameters, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfTrue:
        {
            //CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].b) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            auto labelTrue  = getOrCreateLabel(buildParameters, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(buildParameters, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero32:
        {
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            auto labelTrue  = getOrCreateLabel(buildParameters, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(buildParameters, func, i + 1);
            auto r0         = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero64:
        {
            //CONCAT_STR_1(concat, "if(!r[", ip->a.u32, "].u64) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            auto labelTrue  = getOrCreateLabel(buildParameters, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(buildParameters, func, i + 1);
            auto r0         = GEP_I32(allocR, ip->a.u32);
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero32:
        {
            //CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u32) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            auto labelTrue  = getOrCreateLabel(buildParameters, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(buildParameters, func, i + 1);
            auto r0         = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero64:
        {
            //CONCAT_STR_1(concat, "if(r[", ip->a.u32, "].u64) goto _");
            //concat.addS32Str8(ip->b.s32 + i + 1);
            auto labelTrue  = getOrCreateLabel(buildParameters, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(buildParameters, func, i + 1);
            auto r0         = GEP_I32(allocR, ip->a.u32);
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::Ret:
            //return;
            builder.CreateRetVoid();
            blockIsClosed = true;
            break;

        case ByteCodeOp::IntrinsicPrintS64:
        {
            //CONCAT_STR_1(concat, "swag_runtime_print_i64(r[", ip->a.u32, "].s64);");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateCall(modu.getFunction("swag_runtime_print_i64"), {builder.CreateLoad(r0)});
            break;
        }
        case ByteCodeOp::IntrinsicPrintF64:
        {
            //CONCAT_STR_1(concat, "swag_runtime_print_f64(r[", ip->a.u32, "].f64);");
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            builder.CreateCall(modu.getFunction("swag_runtime_print_f64"), {builder.CreateLoad(r0)});
            break;
        }
        case ByteCodeOp::IntrinsicPrintString:
        {
            //swag_runtime_print_n((const char*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            builder.CreateCall(modu.getFunction("swag_runtime_print_n"), {builder.CreateLoad(r0), builder.CreateLoad(r1)});
            break;
        }

        case ByteCodeOp::IntrinsicAssert:
        {
            //concat.addStringFormat("swag_runtime_assert(r[%u].b, \"%s\", %d, 0);", ip->a.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            auto r0 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            createAssert(buildParameters, r0, ip, "");
            break;
        }
        case ByteCodeOp::IntrinsicAssertCastAny:
        {
            //concat.addStringFormat("swag_runtime_assert(r[%u].b, \"%s\", %d, \"invalid cast from 'any'\");", ip->a.u32, normalizePath(ip->node->sourceFile->path).c_str(), ip->node->token.startLocation.line + 1);
            auto r0 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            createAssert(buildParameters, r0, ip, "invalid cast from 'any'");
            break;
        }
        case ByteCodeOp::IntrinsicAlloc:
        {
            //concat.addStringFormat("r[%u].pointer = (__ui8_t*) swag_runtime_malloc(r[%u].u32);", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), false);
            auto a0 = builder.CreateCall(modu.getFunction("swag_runtime_malloc"), {v0});
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(a0, r1);
            break;
        }
        case ByteCodeOp::IntrinsicRealloc:
        {
            //concat.addStringFormat("r[%u].pointer = (__ui8_t*) swag_runtime_realloc(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32);
            auto v0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto v1 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            auto a0 = builder.CreateCall(modu.getFunction("swag_runtime_realloc"), {v0, v1});
            auto r2 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(a0, r2);
            break;
        }
        case ByteCodeOp::IntrinsicFree:
        {
            //concat.addStringFormat("swag_runtime_free(r[%u].pointer);", ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            builder.CreateCall(modu.getFunction("swag_runtime_free"), {v0});
            break;
        }
        case ByteCodeOp::IntrinsicGetContext:
        {
            //concat.addStringFormat("r[%u].pointer = (__ui8_t*) swag_runtime_tlsGetValue(__process_infos.contextTlsId);", ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst1_i32})));
            auto a0 = builder.CreateCall(modu.getFunction("swag_runtime_tlsGetValue"), {v0});
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(a0, r0);
            break;
        }
        case ByteCodeOp::IntrinsicSetContext:
        {
            //concat.addStringFormat("swag_runtime_tlsSetValue(__process_infos.contextTlsId, r[%u].pointer);", ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst1_i32})));
            auto v1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            builder.CreateCall(modu.getFunction("swag_runtime_tlsSetValue"), {v0, v1});
            break;
        }
        case ByteCodeOp::IntrinsicArguments:
        {
            //concat.addStringFormat("r[%u].pointer = __process_infos.arguments.addr;", ip->a.u32);
            //concat.addStringFormat("r[%u].u64 = __process_infos.arguments.count;", ip->b.u32);
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = GEP_I32(allocR, ip->b.u32);
            auto v0 = TO_PTR_PTR_I8(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst0_i32, pp.cst0_i32}));
            auto v1 = TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst0_i32, pp.cst1_i32}));
            builder.CreateStore(builder.CreateLoad(v0), r0);
            builder.CreateStore(builder.CreateLoad(v1), r1);
            break;
        }
        case ByteCodeOp::IntrinsicIsByteCode:
        {
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].b = 0;");
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i8, r0);
            break;
        }

        case ByteCodeOp::NegBool:
        {
            //CONCAT_STR_1(concat, "r[", ip->a.u32, "].b ^= 1;");
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateXor(builder.CreateLoad(r0), pp.cst1_i8);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegF32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = -r[", ip->a.u32, "].f32;");
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateFNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegF64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = -r[", ip->a.u32, "].f64;");
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateFNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegS32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = -r[", ip->a.u32, "].s32;");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegS64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = -r[", ip->a.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::InvertS8:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s8 = ~r[", ip->a.u32, "].s8;");
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertS16:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = ~r[", ip->a.u32, "].s16;");
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertS32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = ~r[", ip->a.u32, "].s32;");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertS64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = ~r[", ip->a.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::ClearMaskU32:
        {
            //concat.addStringFormat("r[%u].u32 &= 0x%x;", ip->a.u32, ip->b.u32);
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::ClearMaskU64:
        {
            //concat.addStringFormat("r[%u].u64 &= 0x%llx;", ip->a.u32, ip->b.u64);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateAnd(builder.CreateLoad(r0), CST_RB64);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastBool8:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u8 ? 1 : 0;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I8(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool16:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u16 ? 1 : 0;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I64(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u32 ? 1 : 0;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I32(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].b = r[", ip->a.u32, "].u64 ? 1 : 0;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I64(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }

        case ByteCodeOp::CastS8S16:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s16 = (__i16_t) r[", ip->a.u32, "].s8;");
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt16Ty(), true);
            r0      = TO_PTR_I16(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16S32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (__i32_t) r[", ip->a.u32, "].s16;");
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt32Ty(), true);
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32S64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = (__i64_t) r[", ip->a.u32, "].s32;");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), true);
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (__f32_t) r[", ip->a.u32, "].s32;");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS64F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (__f32_t) r[", ip->a.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS64F64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (__f64_t) r[", ip->a.u32, "].s64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU32F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (__f32_t) r[", ip->a.u32, "].u32;");
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU64F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (__f32_t) r[", ip->a.u32, "].u64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU64F64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (__f64_t) r[", ip->a.u32, "].u64;");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF32S32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s32 = (__i32_t) r[", ip->a.u32, "].f32;");
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), builder.getInt32Ty());
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF32F64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f64 = (__f64_t) r[", ip->a.u32, "].f32;");
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPExt, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF64S64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].s64 = (__i64_t) r[", ip->a.u32, "].f64;");
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), builder.getInt64Ty());
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF64F32:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].f32 = (__f32_t) r[", ip->a.u32, "].f64;");
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPTrunc, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CopyRCtoRR:
        {
            //CONCAT_STR_2(concat, "*rr", ip->a.u32, " = r[", ip->b.u32, "];");
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, func->getArg(ip->a.u32));
            break;
        }
        case ByteCodeOp::CopyRRtoRC:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = *rr", ip->b.u32, ";");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(func->getArg(ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::CopyRCtoRRCall:
        {
            // CONCAT_STR_2(concat, "rt[", ip->a.u32, "] = r[", ip->b.u32, "];");
            auto r0 = GEP_I32(allocRT, ip->a.u32);
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::CopyRRtoRCCall:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = rt[", ip->b.u32, "];");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(GEP_I32(allocRT, ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::GetFromStackParam64:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "] = *rp", ip->c.u32, ";");
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(func->getArg(ip->c.u32 + typeFunc->numReturnRegisters()));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakePointerToStackParam:
        {
            //CONCAT_STR_2(concat, "r[", ip->a.u32, "].pointer = (__ui8_t*) &rp", ip->c.u32, "->pointer;");
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = func->getArg(ip->c.u32 + typeFunc->numReturnRegisters());
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::LowerZeroToTrue:
        {
            //concat.addStringFormat("r[%u].b = r[%u].s32 < 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::LowerEqZeroToTrue:
        {
            //concat.addStringFormat("r[%u].b = r[%u].s32 <= 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSLE(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::GreaterZeroToTrue:
        {
            //concat.addStringFormat("r[%u].b = r[%u].s32 > 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::GreaterEqZeroToTrue:
        {
            //concat.addStringFormat("r[%u].b = r[%u].s32 >= 0 ? 1 : 0;", ip->a.u32, ip->a.u32);
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSGE(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            break;

        case ByteCodeOp::CopySP:
        {
            //concat.addStringFormat("r[%u].pointer = (__ui8_t*) &r[%u];", ip->a.u32, ip->c.u32);
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::CopySPVaargs:
        {
            //concat.addStringFormat("__r_t vaargs%u[] = { 0, ", vaargsIdx);
            //int idxParam = (int) pushRAParams.size() - 1;
            //while (idxParam >= 0)
            //{
            //    concat.addStringFormat("r[%u], ", pushRAParams[idxParam]);
            //    idxParam--;
            //}
            auto allocVA = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(pushRAParams.size() + 1));

            int idx      = 1;
            int idxParam = (int) pushRAParams.size() - 1;
            while (idxParam >= 0)
            {
                auto r0 = GEP_I32(allocVA, idx);
                auto r1 = GEP_I32(allocR, pushRAParams[idxParam]);
                builder.CreateStore(builder.CreateLoad(r1), r0);
                idx++;
                idxParam--;
            }

            //concat.addStringFormat("r[%u].pointer = sizeof(__r_t) + (__ui8_t*) &vaargs%u; ", ip->a.u32, vaargsIdx);
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(allocVA, pp.cst1_i32);
            builder.CreateStore(r1, r0);

            //concat.addStringFormat("vaargs%u[0].pointer = r[%u].pointer;", vaargsIdx, ip->a.u32);
            auto r2 = TO_PTR_PTR_I64(allocVA);
            builder.CreateStore(r1, r2);
            break;
        }

        case ByteCodeOp::MakeLambda:
        {
            auto funcBC = (ByteCode*) ip->b.pointer;
            SWAG_ASSERT(funcBC);
            auto typeFuncLambda = CastTypeInfo<TypeInfoFuncAttr>(funcBC->node->typeInfo, TypeInfoKind::FuncAttr);

            //concat.addStringFormat("r[%u].pointer = (__ui8_t*) &%s;", ip->a.u32, funcBC->callName().c_str());
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto T  = createFunctionTypeInternal(buildParameters, typeFuncLambda);
            auto F  = (llvm::Function*) modu.getOrInsertFunction(funcBC->callName().c_str(), T).getCallee();
            builder.CreateStore(TO_PTR_I8(F), r0);
            break;
        }
        case ByteCodeOp::MakeLambdaForeign:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode);
            SWAG_ASSERT(funcNode->attributeFlags & ATTRIBUTE_FOREIGN);
            TypeInfoFuncAttr* typeFuncNode = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
            ComputedValue     foreignValue;
            typeFuncNode->attributes.getValue("swag.foreign", "function", foreignValue);
            SWAG_ASSERT(!foreignValue.text.empty());

            llvm::FunctionType* T;
            SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFuncNode, &T));
            auto F = (llvm::Function*) modu.getOrInsertFunction(foreignValue.text.c_str(), T).getCallee();

            //concat.addStringFormat("r[%u].pointer = (__ui8_t*) &%s;", ip->a.u32, foreignValue.text.c_str());
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(TO_PTR_I8(F), r0);
            break;
        }

        case ByteCodeOp::LambdaCall:
        {
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            llvm::BasicBlock* blockLambdaBC = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockLambdaNT = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockNext     = llvm::BasicBlock::Create(context, "", func);

            vector<llvm::Value*> fctParams;
            getCallParameters(buildParameters, allocR, allocRT, fctParams, typeFuncBC, pushRAParams);

            //concat.addStringFormat("if(r[%u].u64 & 0x%llx) { ", ip->a.u32, SWAG_LAMBDA_MARKER);
            {
                auto v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
                auto v1 = builder.CreateAnd(v0, builder.getInt64(SWAG_LAMBDA_MARKER));
                auto v2 = builder.CreateIsNull(v1);
                builder.CreateCondBr(v2, blockLambdaNT, blockLambdaBC);
            }

            builder.SetInsertPoint(blockLambdaNT);
            //CONCAT_STR_1(concat, " r[", ip->a.u32, "].pointer)");
            {
                auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
                auto FT = createFunctionTypeInternal(buildParameters, typeFuncBC);
                auto PT = llvm::PointerType::getUnqual(FT);
                auto r1 = builder.CreatePointerCast(r0, PT);
                builder.CreateCall(r1, fctParams);
                builder.CreateBr(blockNext);
            }

            builder.SetInsertPoint(blockLambdaBC);
            //CONCAT_STR_1(concat, "__process_infos.byteCodeRun(r[", ip->a.u32, "].pointer");
            {
                auto v0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
                fctParams.insert(fctParams.begin(), v0);
                auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst3_i32})));
                auto PT = llvm::PointerType::getUnqual(pp.bytecodeRunTy);
                auto r2 = builder.CreatePointerCast(r1, PT);
                builder.CreateCall(r2, fctParams);
                builder.CreateBr(blockNext);
            }

            builder.SetInsertPoint(blockNext);
            pushRAParams.clear();
            break;
        }

        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            auto                 FT = createFunctionTypeInternal(buildParameters, typeFuncBC);
            vector<llvm::Value*> fctParams;
            getCallParameters(buildParameters, allocR, allocRT, fctParams, typeFuncBC, pushRAParams);
            builder.CreateCall(modu.getOrInsertFunction(funcBC->callName().c_str(), FT), fctParams);

            pushRAParams.clear();
            break;
        }

        case ByteCodeOp::ForeignCall:
            SWAG_CHECK(emitForeignCall(buildParameters, allocR, allocRT, moduleToGen, ip, pushRAParams));
            break;

        default:
            ok = false;
            moduleToGen->internalError(format("unknown instruction '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
            break;
        }
    }

    if (!blockIsClosed)
        builder.CreateRetVoid();
    //SWAG_ASSERT(blockIsClosed);
    return ok;
}

void BackendLLVM::getCallParameters(const BuildParameters&  buildParameters,
                                    llvm::AllocaInst*       allocR,
                                    llvm::AllocaInst*       allocRT,
                                    vector<llvm::Value*>&   params,
                                    TypeInfoFuncAttr*       typeFuncBC,
                                    VectorNative<uint32_t>& pushRAParams)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
    {
        auto r0 = GEP_I32(allocRT, j);
        params.push_back(r0);
    }

    int popRAidx      = (int) pushRAParams.size() - 1;
    int numCallParams = (int) typeFuncBC->parameters.size();
    for (int idxCall = numCallParams - 1; idxCall >= 0; idxCall--)
    {
        auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        for (int j = 0; j < typeParam->numRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];
            auto r0    = GEP_I32(allocR, index);
            params.push_back(r0);
        }
    }
}

llvm::FunctionType* BackendLLVM::createFunctionTypeInternal(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFuncBC)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& context         = *perThread[ct][precompileIndex].context;

    vector<llvm::Type*> params;

    // Registers to get the result
    for (int i = 0; i < typeFuncBC->numReturnRegisters(); i++)
        params.push_back(llvm::Type::getInt64PtrTy(context));

    // Registers for parameters
    for (auto param : typeFuncBC->parameters)
    {
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        for (int i = 0; i < typeParam->numRegisters(); i++)
            params.push_back(llvm::Type::getInt64PtrTy(context));
    }

    return llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false);
}

bool BackendLLVM::createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, llvm::FunctionType** result)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    vector<llvm::Type*> params;

    llvm::Type* llvmRealReturnType = nullptr;
    llvm::Type* returnType         = nullptr;
    bool        returnByCopy       = typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY;

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
            if (param->typeInfo->kind == TypeInfoKind::Variadic)
            {
                params.push_back(builder.getInt8Ty()->getPointerTo());
                params.push_back(builder.getInt32Ty());
                numParams--;
            }
        }

        llvm::Type* cType = nullptr;
        for (int i = 0; i < numParams; i++)
        {
            auto param = typeFunc->parameters[i];

            SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, param->typeInfo, &cType));
            params.push_back(cType);

            if (param->typeInfo->kind == TypeInfoKind::Slice || param->typeInfo->isNative(NativeTypeKind::String))
            {
                params.push_back(builder.getInt32Ty());
            }
            else if (param->typeInfo->isNative(NativeTypeKind::Any))
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

    *result = llvm::FunctionType::get(returnType, params, false);
    return true;
}

bool BackendLLVM::emitForeignCall(const BuildParameters&  buildParameters,
                                  llvm::AllocaInst*       allocR,
                                  llvm::AllocaInst*       allocRT,
                                  Module*                 moduleToGen,
                                  ByteCodeInstruction*    ip,
                                  VectorNative<uint32_t>& pushParams)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    auto              nodeFunc   = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

    // Get function name
    ComputedValue foreignValue;
    Utf8          funcName;
    if (typeFuncBC->attributes.getValue("swag.foreign", "function", foreignValue) && !foreignValue.text.empty())
        funcName = foreignValue.text;
    else
        funcName = nodeFunc->name;

    int                  numCallParams = (int) typeFuncBC->parameters.size();
    vector<llvm::Value*> params;

    // Variadic are first
    if (numCallParams)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters.back()->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            auto index = pushParams.back();
            pushParams.pop_back();
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            //CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
            params.push_back(builder.CreateLoad(r0));

            index = pushParams.back();
            pushParams.pop_back();
            //CONCAT_STR_1(concat, ", r[", index, "].u32");
            auto r1 = TO_PTR_I32(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r1));
            numCallParams--;
        }
    }

    // All parameters
    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[idxCall]->typeInfo);

        auto index = pushParams.back();
        pushParams.pop_back();

        if (typeParam->kind == TypeInfoKind::Pointer)
        {
            //CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
            auto typePtr = CastTypeInfo<TypeInfoPointer>(typeParam, TypeInfoKind::Pointer);
            if (typePtr->ptrCount != 1)
                return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid pointer count");
            auto finalType = TypeManager::concreteType(typePtr->finalType);
            if (finalType->kind == TypeInfoKind::Native)
            {
                auto r0 = GEP_I32(allocR, index);
                auto r  = TO_PTR_PTR_NATIVE(r0, finalType->nativeType);
                if (!r)
                    return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid pointer param type");
                params.push_back(builder.CreateLoad(r));
            }
            else if (finalType->kind == TypeInfoKind::Struct ||
                     finalType->kind == TypeInfoKind::Interface ||
                     finalType->kind == TypeInfoKind::Array)
            {
                auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
                params.push_back(builder.CreateLoad(r0));
            }
            else
            {
                return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid pointer param type");
            }
        }
        else if (typeParam->kind == TypeInfoKind::Struct ||
                 typeParam->kind == TypeInfoKind::Interface ||
                 typeParam->kind == TypeInfoKind::Array)
        {
            //CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            //CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));

            index = pushParams.back();
            pushParams.pop_back();
            //CONCAT_STR_1(concat, ", r[", index, "].u32");
            auto r1 = TO_PTR_I32(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r1));
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
        {
            //CONCAT_STR_1(concat, "(void*)r[", index, "].pointer");
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));

            index = pushParams.back();
            pushParams.pop_back();
            //CONCAT_STR_1(concat, ", (void*)r[", index, "].pointer");
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r1));
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            //CONCAT_STR_1(concat, "r[", index, "]");
            auto r0 = GEP_I32(allocR, index);
            auto r  = TO_PTR_NATIVE(r0, typeParam->nativeType);
            if (!r)
                return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid param native type");
            params.push_back(builder.CreateLoad(r));
        }
        else
        {
            return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid param type");
        }
    }

    // Return by parameter
    auto returnType = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    if (returnType->kind == TypeInfoKind::Slice ||
        returnType->isNative(NativeTypeKind::Any) ||
        returnType->isNative(NativeTypeKind::String))
    {
        //CONCAT_FIXED_STR(concat, "&rt[0]");
        params.push_back(TO_PTR_I8(allocRT));
    }
    else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        //CONCAT_FIXED_STR(concat, "rt[0].pointer");
        params.push_back(builder.CreateLoad(TO_PTR_PTR_I8(allocRT)));
    }

    // Make the call
    llvm::FunctionType* typeF;
    SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFuncBC, &typeF));
    auto result = builder.CreateCall(modu.getOrInsertFunction(funcName.c_str(), typeF), params);

    // Store result to rt0
    if (returnType != g_TypeMgr.typeInfoVoid)
    {
        if ((returnType->kind == TypeInfoKind::Slice) ||
            (returnType->isNative(NativeTypeKind::Any)) ||
            (returnType->isNative(NativeTypeKind::String)) ||
            (returnType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            // Return by parameter
        }
        else if (returnType->kind == TypeInfoKind::Pointer)
        {
            //CONCAT_FIXED_STR(concat, "rt[0].pointer = (__ui8_t*) ");
            builder.CreateStore(result, TO_PTR_PTR_I8(allocRT));
        }
        else if (returnType->kind == TypeInfoKind::Native)
        {
            auto r = TO_PTR_NATIVE(allocRT, returnType->nativeType);
            if (!r)
                return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid return type");
            builder.CreateStore(result, r);
        }
        else
        {
            return moduleToGen->internalError(ip->node, ip->node->token, "emitForeignCall, invalid return type");
        }
    }

    return true;
}