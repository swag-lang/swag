#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "BackendLLVMFunctionMacros_.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"
#include "OS.h"

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
        typeInfo->kind == TypeInfoKind::Lambda ||
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
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    // Function type
    llvm::FunctionType* funcType = nullptr;
    SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFunc, &funcType));

    // Create function
    node->computeFullNameForeign(true);
    llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node->fullnameForeign.c_str(), modu);
    func->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(block);

    if (pp.dbg)
    {
        pp.dbg->startWrapperFunction(pp, bc, node, func);
        builder.SetCurrentDebugLocation(llvm::DebugLoc::get(0, 0, func->getSubprogram()));
    }

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
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
            auto rr1 = TO_PTR_I32(GEP_I32(allocRR, idx + 1));
            builder.CreateStore(func->getArg(argIdx + 1), rr1);
        }
        else if (typeParam->isNative(NativeTypeKind::Any) || typeParam->kind == TypeInfoKind::Interface)
        {
            auto cst0 = TO_PTR_I8(func->getArg(argIdx));
            builder.CreateStore(cst0, TO_PTR_PTR_I8(rr0));
            auto cst1 = TO_PTR_I8(func->getArg(argIdx + 1));
            auto rr1  = GEP_I32(allocRR, idx + 1);
            builder.CreateStore(cst1, TO_PTR_PTR_I8(rr1));
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
    VectorNative<llvm::Value*> args;
    for (int i = 0; i < n; i++)
    {
        auto rr0 = builder.CreateInBoundsGEP(allocRR, builder.getInt32(i));
        args.push_back(rr0);
    }

    auto fcc = modu.getFunction(bc->callName().c_str());
    builder.CreateCall(fcc, {args.begin(), args.end()});

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
        else if (returnType->kind == TypeInfoKind::Pointer || returnType->kind == TypeInfoKind::Reference)
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

llvm::BasicBlock* BackendLLVM::getOrCreateLabel(LLVMPerThread& pp, llvm::Function* func, int32_t ip)
{
    auto& context = *pp.context;

    auto it = pp.labels.find(ip);
    if (it == pp.labels.end())
    {
        llvm::BasicBlock* label = llvm::BasicBlock::Create(context, format("%d", ip).c_str(), func);
        pp.labels[ip]           = label;
        return label;
    }

    return it->second;
}

bool BackendLLVM::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC) && (buildParameters.compileType != BackendCompileType::Test))
        return true;

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
    llvm::AllocaInst* allocRR = nullptr;
    if (bc->maxCallResults)
        allocRR = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt32(bc->maxCallResults));

    // Stack
    llvm::AllocaInst* allocStack = nullptr;
    if (typeFunc->stackSize)
        allocStack = builder.CreateAlloca(builder.getInt8Ty(), builder.getInt32(typeFunc->stackSize));

    if (pp.dbg && bc->node)
    {
        pp.dbg->startFunction(pp, bc, func, allocStack);
        pp.dbg->setLocation(pp.builder, bc, nullptr);
    }

    // Generate bytecode
    auto                   ip = bc->out;
    VectorNative<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
            continue;

        // If we are the destination of a jump, be sure we have a block, and from now insert into that block
        if ((ip->flags & BCI_JUMP_DEST) || blockIsClosed)
        {
            auto label = getOrCreateLabel(pp, func, i);
            if (!blockIsClosed)
                builder.CreateBr(label); // Make a jump from previous block to this one
            else
                blockIsClosed = false; // Each block must be closed by a valid terminator instruction
            builder.SetInsertPoint(label);
        }

        if (pp.dbg && bc->node)
            pp.dbg->setLocation(pp.builder, bc, ip);

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::Nop:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::DecSPBP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::IncSPPostCall:
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::IncPointer32:
        case ByteCodeOp::DecPointer32:
        {
            auto         r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto         r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            llvm::Value* r2;
            if (ip->flags & BCI_IMM_B)
                r2 = builder.getInt32(ip->b.u32);
            else
                r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            if (ip->op == ByteCodeOp::DecPointer32)
                r2 = builder.CreateNeg(r2);
            auto r3 = builder.CreateInBoundsGEP(r1, r2);
            builder.CreateStore(r3, r0);
            break;
        }

        case ByteCodeOp::DeRef8:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::DeRef16:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I16(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::DeRef32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I32(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::DeRef64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::DeRefPointer:
        {
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
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = llvm::ConstantInt::get(builder.getInt64Ty(), ip->b.u32);
            builder.CreateStore(builder.CreateMul(builder.CreateLoad(r0), r1), r0);
            break;
        }
        case ByteCodeOp::Div64byVB32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = llvm::ConstantInt::get(builder.getInt64Ty(), ip->b.u32);
            builder.CreateStore(builder.CreateUDiv(builder.CreateLoad(r0), r1), r0);
            break;
        }

        case ByteCodeOp::GetFromMutableSeg64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(TO_PTR_I8(pp.mutableSeg), CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::GetFromBssSeg64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(TO_PTR_I8(pp.bssSeg), CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::SetZeroStack8:
        {
            auto r0 = builder.CreateInBoundsGEP(allocStack, CST_RA32);
            builder.CreateStore(pp.cst0_i8, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack16:
        {
            auto r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i16, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack32:
        {
            auto r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i32, r0);
            break;
        }
        case ByteCodeOp::SetZeroStack64:
        {
            auto r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            builder.CreateStore(pp.cst0_i64, r0);
            break;
        }
        case ByteCodeOp::SetZeroStackX:
        {
            auto r0 = builder.CreateInBoundsGEP(allocStack, CST_RA32);
            builder.CreateMemSet(r0, pp.cst0_i8, ip->b.u32, llvm::MaybeAlign(0));
            break;
        }

        case ByteCodeOp::SetZeroAtPointer8:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i8, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointer16:
        {
            auto r0 = TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i16, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointer32:
        {
            auto r0 = TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i32, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointer64:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i64, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointerX:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(r0);
            builder.CreateMemSet(r1, pp.cst0_i8, ip->b.u32, llvm::MaybeAlign(0));
            break;
        }
        case ByteCodeOp::SetZeroAtPointerXRB:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            auto v0 = builder.CreateMul(r2, builder.getInt32(ip->c.u32));
            v0      = builder.CreateIntCast(v0, builder.getInt64Ty(), false);
            builder.CreateMemSet(r0, pp.cst0_i8, v0, llvm::MaybeAlign(0));
            break;
        }

        case ByteCodeOp::MakeMutableSegPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.mutableSeg), CST_RB32);
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeBssSegPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.bssSeg), CST_RB32);
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeConstantSegPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.constantSeg), CST_RB32);
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeTypeSegPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(TO_PTR_I8(pp.typeSeg), CST_RB32);
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::MakeStackPointer:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::GetFromStack64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::MemCpy:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            r0      = builder.CreateLoad(r0);
            r1      = builder.CreateLoad(r1);

            llvm::Value* r2;
            if (ip->flags & BCI_IMM_C)
                r2 = builder.getInt64(ip->c.u32);
            else
            {
                r2 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
                r2 = builder.CreateIntCast(builder.CreateLoad(r2), builder.getInt64Ty(), false);
            }

            builder.CreateMemCpy(r0, llvm::MaybeAlign(0), r1, llvm::MaybeAlign(0), r2);
            break;
        }
        case ByteCodeOp::MemSet:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            r0      = builder.CreateLoad(r0);
            r1      = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt8Ty(), false);
            r2      = builder.CreateIntCast(builder.CreateLoad(r2), builder.getInt64Ty(), false);
            builder.CreateMemSet(r0, r1, r2, llvm::MaybeAlign(0));
            break;
        }
        case ByteCodeOp::MemCmp:
        {
            auto rr = GEP_I32(allocR, ip->a.u32);
            auto r0 = GEP_I32(allocR, ip->b.u32);
            auto r1 = GEP_I32(allocR, ip->c.u32);
            auto r2 = GEP_I32(allocR, ip->d.u32);
            builder.CreateCall(modu.getFunction("@memcmp"), {rr, r0, r1, r2});
            break;
        }

        case ByteCodeOp::IntrinsicMkInterface:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r2 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            r0      = builder.CreateLoad(r0);
            r1      = builder.CreateLoad(r1);
            builder.CreateStore(builder.CreateCall(modu.getFunction("swag_runtime_interfaceof"), {r0, r1}), r2);
            break;
        }

        case ByteCodeOp::SetImmediate32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(CST_RB32, r0);
            break;
        }
        case ByteCodeOp::SetImmediate64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(CST_RB64, r0);
            break;
        }

        case ByteCodeOp::CopyRBtoRA:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::CopyRBAddrToRA:
        case ByteCodeOp::CopyRBAddrToRA2:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = GEP_I32(allocR, ip->b.u32);
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::ClearRA:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateStore(pp.cst0_i64, r0);
            break;
        }
        case ByteCodeOp::DecrementRA32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), pp.cst1_i32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::IncrementRA32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), pp.cst1_i32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::Add32byVB32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::SetAtPointer8:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            r0      = builder.CreateLoad(r0);
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt8(ip->b.u8);
            else
                r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtPointer16:
        {
            auto r0 = TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32));
            r0      = builder.CreateLoad(r0);
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt16(ip->b.u16);
            else
                r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtPointer32:
        {
            auto r0 = TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32));
            r0      = builder.CreateLoad(r0);
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt32(ip->b.u32);
            else
                r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtPointer64:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            r0      = builder.CreateLoad(r0);
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt64(ip->b.u64);
            else
                r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::BinOpPlusS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateAdd(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpPlusS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateAdd(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpPlusF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFAdd(r1, r2);
            builder.CreateStore(v0, TO_PTR_F32(r0));
            break;
        }
        case ByteCodeOp::BinOpPlusF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFAdd(r1, r2);
            builder.CreateStore(v0, TO_PTR_F64(r0));
            break;
        }

        case ByteCodeOp::BinOpMinusS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateSub(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpMinusS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateSub(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpMinusF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFSub(r1, r2);
            builder.CreateStore(v0, TO_PTR_F32(r0));
            break;
        }
        case ByteCodeOp::BinOpMinusF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFSub(r1, r2);
            builder.CreateStore(v0, TO_PTR_F64(r0));
            break;
        }

        case ByteCodeOp::BinOpMulS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateMul(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpMulS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateMul(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpMulF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFMul(r1, r2);
            builder.CreateStore(v0, TO_PTR_F32(r0));
            break;
        }
        case ByteCodeOp::BinOpMulF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFMul(r1, r2);
            builder.CreateStore(v0, TO_PTR_F64(r0));
            break;
        }

        case ByteCodeOp::BinOpXorU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateXor(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpXorU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateXor(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

        case ByteCodeOp::BinOpShiftLeftU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateShl(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpShiftLeftU64:
        {
            MK_BINOP64_CAB();
            auto vb = builder.CreateIntCast(r2, builder.getInt32Ty(), false);
            vb      = builder.CreateIntCast(r2, builder.getInt64Ty(), false);
            auto v0 = builder.CreateShl(r1, vb);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

        case ByteCodeOp::BinOpShiftRightS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateAShr(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpShiftRightS64:
        {
            MK_BINOP64_CAB();
            auto vb = builder.CreateIntCast(r2, builder.getInt32Ty(), false);
            vb      = builder.CreateIntCast(r2, builder.getInt64Ty(), false);
            auto v0 = builder.CreateAShr(r1, vb);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpShiftRightU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateLShr(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpShiftRightU64:
        {
            MK_BINOP64_CAB();
            auto vb = builder.CreateIntCast(r2, builder.getInt32Ty(), false);
            vb      = builder.CreateIntCast(r2, builder.getInt64Ty(), false);
            auto v0 = builder.CreateLShr(r1, vb);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpShiftRightU64VB:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto vb = builder.CreateIntCast(CST_RB32, builder.getInt64Ty(), false);
            auto v0 = builder.CreateLShr(builder.CreateLoad(r0), vb);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::BinOpModuloS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateSRem(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpModuloS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateSRem(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpModuloU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateURem(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpModuloU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateURem(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

        case ByteCodeOp::BinOpDivS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateSDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpDivS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateSDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpDivU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateUDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpDivU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateUDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }
        case ByteCodeOp::BinOpDivF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_F32(r0));
            break;
        }
        case ByteCodeOp::BinOpDivF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFDiv(r1, r2);
            builder.CreateStore(v0, TO_PTR_F64(r0));
            break;
        }

        case ByteCodeOp::BinOpBitmaskAndS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateAnd(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpBitmaskAndS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateAnd(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

        case ByteCodeOp::BinOpBitmaskOrS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateOr(r1, r2);
            builder.CreateStore(v0, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::BinOpBitmaskOrS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateOr(r1, r2);
            builder.CreateStore(v0, TO_PTR_I64(r0));
            break;
        }

        case ByteCodeOp::AffectOpMinusEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        break;
        case ByteCodeOp::AffectOpMinusEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateSub(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqF32:
        {
            MK_BINOPEQF32_CAB();
            auto v0 = builder.CreateFSub(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqF64:
        {
            MK_BINOPEQF64_CAB();
            auto v0 = builder.CreateFSub(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAdd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqF32:
        {
            MK_BINOPEQF32_CAB();
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpPlusEqF64:
        {
            MK_BINOPEQF64_CAB();
            auto v0 = builder.CreateFAdd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpPlusEqPointer:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMinusEqPointer:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r1), builder.CreateNeg(builder.CreateLoad(r2)));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpMulEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateMul(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqF32:
        {
            MK_BINOPEQF32_CAB();
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpMulEqF64:
        {
            MK_BINOPEQF64_CAB();
            auto v0 = builder.CreateFMul(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpDivEqS8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqS64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateSDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqU64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateUDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqF32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F32(r0));
            auto r2 = TO_PTR_F32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpDivEqF64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_F64(r0));
            auto r2 = TO_PTR_F64(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateFDiv(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpAndEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpAndEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpAndEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpAndEqS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAnd(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpOrEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpOrEqS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateOr(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpShiftLeftEqS8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqS16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqS32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqS64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpShiftRightEqS8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqS16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqS32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqS64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateAShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqU8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqU16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqU32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftRightEqU64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateLShr(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpModuloEqS8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqS64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateSRem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpModuloEqU64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateURem(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::AffectOpXOrEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXOrEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXOrEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXOrEqS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }

        case ByteCodeOp::CompareOpGreaterS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpSGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpSGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpUGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpUGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpUGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpUGT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOpLowerS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpSLT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpSLT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpULT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpULT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpULT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpULT(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CompareOp3WayS32:
        case ByteCodeOp::CompareOp3WayU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateSub(r1, r2);
            auto v1 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cst0_i32), builder.getInt32Ty(), false);
            auto v2 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cst0_i32), builder.getInt32Ty(), false);
            auto v3 = builder.CreateSub(v1, v2);
            builder.CreateStore(v3, TO_PTR_I32(r0));
            break;
        }

        case ByteCodeOp::CompareOp3WayU64:
        case ByteCodeOp::CompareOp3WayS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateSub(r1, r2);
            auto v1 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cst0_i64), builder.getInt32Ty(), false);
            auto v2 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cst0_i64), builder.getInt32Ty(), false);
            auto v3 = builder.CreateSub(v1, v2);
            builder.CreateStore(v3, TO_PTR_I32(r0));
            break;
        }

        case ByteCodeOp::CompareOp3WayF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFSub(r1, r2);
            auto v1 = builder.CreateIntCast(builder.CreateFCmpUGT(v0, pp.cst0_f32), builder.getInt32Ty(), false);
            auto v2 = builder.CreateIntCast(builder.CreateFCmpULT(v0, pp.cst0_f32), builder.getInt32Ty(), false);
            auto v3 = builder.CreateSub(v1, v2);
            builder.CreateStore(v3, TO_PTR_I32(r0));
            break;
        }
        case ByteCodeOp::CompareOp3WayF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFSub(r1, r2);
            auto v1 = builder.CreateIntCast(builder.CreateFCmpUGT(v0, pp.cst0_f64), builder.getInt32Ty(), false);
            auto v2 = builder.CreateIntCast(builder.CreateFCmpULT(v0, pp.cst0_f64), builder.getInt32Ty(), false);
            auto v3 = builder.CreateSub(v1, v2);
            builder.CreateStore(v3, TO_PTR_I32(r0));
            break;
        }

        case ByteCodeOp::CompareOpEqual8:
        {
            MK_BINOP8_CAB();
            auto v0 = builder.CreateICmpEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual16:
        {
            MK_BINOP16_CAB();
            auto v0 = builder.CreateICmpEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqual64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpEQ(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpEqualString:
        {
            auto rr  = GEP_I32(allocR, ip->c.u32);
            auto r0  = GEP_I32(allocR, ip->a.u32);
            auto r1  = GEP_I32(allocR, ip->b.u32);
            auto r2  = GEP_I32(allocR, ip->c.u32);
            auto r3  = GEP_I32(allocR, ip->d.u32);
            builder.CreateCall(modu.getFunction("@strcmp"), {rr, r0, r1, r2, r3});
            break;
        }
        case ByteCodeOp::CompareOpEqualTypeInfo:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r2 = builder.getInt32(ip->c.u32);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->d.u32));
            r0      = builder.CreateLoad(r0);
            r1      = builder.CreateLoad(r1);
            builder.CreateStore(builder.CreateCall(modu.getFunction("swag_runtime_compareType"), {r0, r1, r2}), r3);
            break;
        }

        case ByteCodeOp::TestNotZero8:
        {
            auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt8(ip->b.u8);
            else
                r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto b0 = builder.CreateIsNotNull(r1);
            builder.CreateStore(builder.CreateIntCast(b0, builder.getInt8Ty(), false), r0);
            break;
        }
        case ByteCodeOp::TestNotZero16:
        {
            auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt16(ip->b.u16);
            else
                r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
            auto b0 = builder.CreateIsNotNull(r1);
            builder.CreateStore(builder.CreateIntCast(b0, builder.getInt8Ty(), false), r0);
            break;
        }
        case ByteCodeOp::TestNotZero32:
        {
            auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt32(ip->b.u32);
            else
                r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            auto b0 = builder.CreateIsNotNull(r1);
            builder.CreateStore(builder.CreateIntCast(b0, builder.getInt8Ty(), false), r0);
            break;
        }
        case ByteCodeOp::TestNotZero64:
        {
            auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt64(ip->b.u64);
            else
                r1 = builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->b.u32)));
            auto b0 = builder.CreateIsNotNull(r1);
            builder.CreateStore(builder.CreateIntCast(b0, builder.getInt8Ty(), false), r0);
            break;
        }

        case ByteCodeOp::Jump:
        {
            auto label = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            builder.CreateBr(label);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfFalse:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfTrue:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = GEP_I32(allocR, ip->a.u32);
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero32:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero64:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = GEP_I32(allocR, ip->a.u32);
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::Ret:
            builder.CreateRetVoid();
            blockIsClosed = true;
            break;

        case ByteCodeOp::IntrinsicPrintS64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            builder.CreateCall(modu.getFunction("swag_runtime_print_i64"), {builder.CreateLoad(r0)});
            break;
        }
        case ByteCodeOp::IntrinsicPrintF64:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            builder.CreateCall(modu.getFunction("swag_runtime_print_f64"), {builder.CreateLoad(r0)});
            break;
        }
        case ByteCodeOp::IntrinsicPrintString:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            builder.CreateCall(modu.getFunction("swag_runtime_print_n"), {builder.CreateLoad(r0), builder.CreateLoad(r1)});
            break;
        }
        case ByteCodeOp::IntrinsicError:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateCall(modu.getFunction("swag_runtime_error"), {builder.CreateLoad(r0), builder.CreateLoad(r1), builder.CreateLoad(r2)});
            break;
        }

        case ByteCodeOp::IntrinsicAssert:
        {
            llvm::Value* r0;
            if (ip->flags & BCI_IMM_A)
                r0 = builder.getInt8(ip->a.b);
            else
                r0 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto         r1 = builder.CreateGlobalString(normalizePath(ip->node->sourceFile->path).c_str());
            auto         v1 = TO_PTR_I8(builder.CreateInBoundsGEP(r1, {pp.cst0_i32, pp.cst0_i32}));
            auto         r2 = builder.getInt64(ip->node->token.startLocation.line | ((uint64_t) ip->node->token.startLocation.column << 32));
            llvm::Value* r3;
            if (ip->d.pointer)
            {
                r3 = builder.CreateGlobalString((const char*) ip->d.pointer);
                r3 = TO_PTR_I8(builder.CreateInBoundsGEP(r3, {pp.cst0_i32}));
            }
            else
                r3 = builder.CreateIntToPtr(pp.cst0_i64, builder.getInt8PtrTy());
            builder.CreateCall(modu.getFunction("swag_runtime_assert"), {r0, v1, r2, r3});
            break;
        }
        case ByteCodeOp::IntrinsicAlloc:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), false);
            auto a0 = builder.CreateCall(pp.fn_malloc, {v0});
            auto r1 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(a0, r1);
            break;
        }
        case ByteCodeOp::IntrinsicRealloc:
        {
            auto v0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r1 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            auto v1 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            auto a0 = builder.CreateCall(pp.fn_realloc, {v0, v1});
            auto r2 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(a0, r2);
            break;
        }
        case ByteCodeOp::IntrinsicFree:
        {
            auto v0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            builder.CreateCall(pp.fn_free, {v0});
            break;
        }
        case ByteCodeOp::IntrinsicGetContext:
        {
            auto v0 = builder.CreateLoad(TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst1_i32})));
            auto a0 = builder.CreateCall(modu.getFunction("swag_runtime_tlsGetValue"), {v0});
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(a0, r0);
            break;
        }
        case ByteCodeOp::IntrinsicSetContext:
        {
            auto v0 = builder.CreateLoad(TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst1_i32})));
            auto v1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            builder.CreateCall(modu.getFunction("swag_runtime_tlsSetValue"), {v0, v1});
            break;
        }
        case ByteCodeOp::IntrinsicArguments:
        {
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
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i8, r0);
            break;
        }
        case ByteCodeOp::IntrinsicCompiler:
        {
            auto r0 = TO_PTR_I64(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i64, r0);
            auto r1 = TO_PTR_I64(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(pp.cst0_i64, r1);
            break;
        }

        case ByteCodeOp::NegBool:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateXor(builder.CreateLoad(r0), pp.cst1_i8);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegF32:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateFNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegF64:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateFNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegS32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::NegS64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateNeg(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::InvertS8:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertS16:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertS32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertS64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::ClearMaskU32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAnd(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::ClearMaskU64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateAnd(builder.CreateLoad(r0), CST_RB64);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastBool8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I8(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I16(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I32(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastBool64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNotNull(builder.CreateLoad(TO_PTR_I64(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }

        case ByteCodeOp::CastS8S16:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt16Ty(), true);
            r0      = TO_PTR_I16(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16S32:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt32Ty(), true);
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32S64:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), true);
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32F32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS64F32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS64F64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU32F32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU64F64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF32S32:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), builder.getInt32Ty());
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF32F64:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPExt, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF64S64:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), builder.getInt64Ty());
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastF64F32:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPTrunc, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CopyRCtoRR:
        {
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, func->getArg(ip->a.u32));
            break;
        }
        case ByteCodeOp::CopyRCtoRT:
        {
            auto r0 = GEP_I32(allocRR, ip->a.u32);
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::CopyRRtoRC:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(func->getArg(ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::CopyRTtoRC:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(GEP_I32(allocRR, ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::GetFromStackParam64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(func->getArg(ip->c.u32 + typeFunc->numReturnRegisters()));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::MakeStackPointerParam:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = func->getArg(ip->c.u32 + typeFunc->numReturnRegisters());
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::LowerZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSLT(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::LowerEqZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSLE(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::GreaterZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSGT(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::GreaterEqZeroToTrue:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateLoad(TO_PTR_I32(r0));
            auto a0 = builder.CreateIntCast(builder.CreateICmpSGE(v0, pp.cst0_i32), builder.getInt8Ty(), false);
            builder.CreateStore(a0, TO_PTR_I8(r0));
            break;
        }

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            break;
        case ByteCodeOp::PushRAParam2:
            pushRAParams.push_back(ip->a.u32);
            pushRAParams.push_back(ip->b.u32);
            break;
        case ByteCodeOp::PushRAParam3:
            pushRAParams.push_back(ip->a.u32);
            pushRAParams.push_back(ip->b.u32);
            pushRAParams.push_back(ip->c.u32);
            break;
        case ByteCodeOp::PushRAParam4:
            pushRAParams.push_back(ip->a.u32);
            pushRAParams.push_back(ip->b.u32);
            pushRAParams.push_back(ip->c.u32);
            pushRAParams.push_back(ip->d.u32);
            break;
        case ByteCodeOp::CopySP:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::CopySPVaargs:
        {
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

            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateInBoundsGEP(allocVA, pp.cst1_i32);
            builder.CreateStore(r1, r0);

            auto r2 = TO_PTR_PTR_I64(allocVA);
            builder.CreateStore(r1, r2);
            break;
        }

        case ByteCodeOp::MakeLambda:
        {
            auto funcBC = (ByteCode*) ip->b.pointer;
            SWAG_ASSERT(funcBC);
            auto typeFuncLambda = CastTypeInfo<TypeInfoFuncAttr>(funcBC->node->typeInfo, TypeInfoKind::FuncAttr);

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

            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(TO_PTR_I8(F), r0);

            auto v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
            auto v1 = builder.CreateOr(v0, builder.getInt64(SWAG_LAMBDA_FOREIGN_MARKER));
            builder.CreateStore(v1, GEP_I32(allocR, ip->a.u32));
            break;
        }

        case ByteCodeOp::LambdaCall:
        {
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            llvm::BasicBlock* blockLambdaBC             = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockLambdaLocalOrForeign = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockLambdaForeign        = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockLambdaLocal          = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockNext                 = llvm::BasicBlock::Create(context, "", func);

            {
                auto v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
                auto v1 = builder.CreateAnd(v0, builder.getInt64(SWAG_LAMBDA_BC_MARKER));
                auto v2 = builder.CreateIsNotNull(v1);
                builder.CreateCondBr(v2, blockLambdaBC, blockLambdaLocalOrForeign);
            }

            builder.SetInsertPoint(blockLambdaLocalOrForeign);
            {
                auto v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
                auto v1 = builder.CreateAnd(v0, builder.getInt64(SWAG_LAMBDA_FOREIGN_MARKER));
                auto v2 = builder.CreateIsNotNull(v1);
                builder.CreateCondBr(v2, blockLambdaForeign, blockLambdaLocal);
            }

            builder.SetInsertPoint(blockLambdaForeign);
            {
                auto                v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
                auto                v1 = builder.CreateAnd(v0, builder.getInt64(~SWAG_LAMBDA_FOREIGN_MARKER));
                llvm::FunctionType* FT;
                SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFuncBC, &FT));
                auto                       PT = llvm::PointerType::getUnqual(FT);
                auto                       r1 = builder.CreateIntToPtr(v1, PT);
                VectorNative<llvm::Value*> fctParams;
                getForeignCallParameters(buildParameters, allocR, allocRR, moduleToGen, typeFuncBC, fctParams, pushRAParams);
                auto result = builder.CreateCall(FT, r1, {fctParams.begin(), fctParams.end()});
                SWAG_CHECK(getForeignCallReturnValue(buildParameters, allocRR, moduleToGen, typeFuncBC, result));
                builder.CreateBr(blockNext);
            }

            builder.SetInsertPoint(blockLambdaLocal);
            {
                auto                       r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
                auto                       FT = createFunctionTypeInternal(buildParameters, typeFuncBC);
                auto                       PT = llvm::PointerType::getUnqual(FT);
                auto                       r1 = builder.CreatePointerCast(r0, PT);
                VectorNative<llvm::Value*> fctParams;
                getLocalCallParameters(buildParameters, allocR, allocRR, fctParams, typeFuncBC, pushRAParams);
                builder.CreateCall(FT, r1, {fctParams.begin(), fctParams.end()});
                builder.CreateBr(blockNext);
            }

            builder.SetInsertPoint(blockLambdaBC);
            {
                auto                       v0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
                VectorNative<llvm::Value*> fctParams;
                fctParams.push_front(v0);
                auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst3_i32})));
                auto PT = llvm::PointerType::getUnqual(pp.bytecodeRunTy);
                auto r2 = builder.CreatePointerCast(r1, PT);
                getLocalCallParameters(buildParameters, allocR, allocRR, fctParams, typeFuncBC, pushRAParams);
                builder.CreateCall(pp.bytecodeRunTy, r2, {fctParams.begin(), fctParams.end()});
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

            auto                       FT = createFunctionTypeInternal(buildParameters, typeFuncBC);
            VectorNative<llvm::Value*> fctParams;
            getLocalCallParameters(buildParameters, allocR, allocRR, fctParams, typeFuncBC, pushRAParams);
            builder.CreateCall(modu.getOrInsertFunction(funcBC->callName().c_str(), FT), {fctParams.begin(), fctParams.end()});

            pushRAParams.clear();
            break;
        }

        case ByteCodeOp::ForeignCall:
            SWAG_CHECK(emitForeignCall(buildParameters, allocR, allocRR, moduleToGen, ip, pushRAParams));
            break;

        case ByteCodeOp::IntrinsicS8x1:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_8();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {builder.getInt8Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS16x1:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_16();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {builder.getInt16Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS32x1:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {builder.getInt32Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS64x1:
        {
            auto r0 = TO_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::abs, {builder.getInt64Ty(), builder.getInt1Ty()}, {r1, pp.cst0_i1}), r0);
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF32x2:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_F32();
            auto r2 = MK_IMMC_F32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicPow:
                builder.CreateStore(builder.CreateCall(pp.fn_powf32, {r1, r2}), r0);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicF64x2:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_F64();
            auto r2 = MK_IMMC_F64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicPow:
                builder.CreateStore(builder.CreateCall(pp.fn_powf64, {r1, r2}), r0);
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicF32x1:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_F32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicSin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicCos:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicTan:
                builder.CreateStore(builder.CreateCall(pp.fn_tanf32, r1), r0);
                break;
            case TokenId::IntrinsicSinh:
                builder.CreateStore(builder.CreateCall(pp.fn_sinhf32, r1), r0);
                break;
            case TokenId::IntrinsicCosh:
                builder.CreateStore(builder.CreateCall(pp.fn_coshf32, r1), r0);
                break;
            case TokenId::IntrinsicTanh:
                builder.CreateStore(builder.CreateCall(pp.fn_tanhf32, r1), r0);
                break;
            case TokenId::IntrinsicASin:
                builder.CreateStore(builder.CreateCall(pp.fn_asinf32, r1), r0);
                break;
            case TokenId::IntrinsicACos:
                builder.CreateStore(builder.CreateCall(pp.fn_acosf32, r1), r0);
                break;
            case TokenId::IntrinsicATan:
                builder.CreateStore(builder.CreateCall(pp.fn_atanf32, r1), r0);
                break;
            case TokenId::IntrinsicLog:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog2:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log2, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog10:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log10, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicFloor:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::floor, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicCeil:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ceil, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicTrunc:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::trunc, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicRound:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::round, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fabs, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicExp:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicExp2:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp2, builder.getFloatTy(), r1), r0);
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicF64x1:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_F64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicSqrt:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicSin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicCos:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicTan:
                builder.CreateStore(builder.CreateCall(pp.fn_tanf64, r1), r0);
                break;
            case TokenId::IntrinsicSinh:
                builder.CreateStore(builder.CreateCall(pp.fn_sinhf64, r1), r0);
                break;
            case TokenId::IntrinsicCosh:
                builder.CreateStore(builder.CreateCall(pp.fn_coshf64, r1), r0);
                break;
            case TokenId::IntrinsicTanh:
                builder.CreateStore(builder.CreateCall(pp.fn_tanhf64, r1), r0);
                break;
            case TokenId::IntrinsicASin:
                builder.CreateStore(builder.CreateCall(pp.fn_asinf64, r1), r0);
                break;
            case TokenId::IntrinsicACos:
                builder.CreateStore(builder.CreateCall(pp.fn_acosf64, r1), r0);
                break;
            case TokenId::IntrinsicATan:
                builder.CreateStore(builder.CreateCall(pp.fn_atanf64, r1), r0);
                break;
            case TokenId::IntrinsicLog:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog2:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log2, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicLog10:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::log10, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicFloor:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::floor, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicCeil:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::ceil, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicTrunc:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::trunc, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicRound:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::round, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicAbs:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::fabs, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicExp:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicExp2:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::exp2, builder.getDoubleTy(), r1), r0);
                break;
            }
            break;
        }

        default:
            ok = false;
            moduleToGen->internalError(format("unknown instruction '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
            break;
        }
    }

    if (!blockIsClosed)
        builder.CreateRetVoid();
    return ok;
}

void BackendLLVM::getLocalCallParameters(const BuildParameters&      buildParameters,
                                         llvm::AllocaInst*           allocR,
                                         llvm::AllocaInst*           allocRR,
                                         VectorNative<llvm::Value*>& params,
                                         TypeInfoFuncAttr*           typeFuncBC,
                                         VectorNative<uint32_t>&     pushRAParams)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
    {
        auto r0 = GEP_I32(allocRR, j);
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

    VectorNative<llvm::Type*> params;

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

    return llvm::FunctionType::get(llvm::Type::getVoidTy(context), {params.begin(), params.end()}, false);
}

bool BackendLLVM::createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, llvm::FunctionType** result)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    VectorNative<llvm::Type*> params;

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

            // Additional parameter
            if (param->typeInfo->kind == TypeInfoKind::Slice || param->typeInfo->isNative(NativeTypeKind::String))
                params.push_back(builder.getInt32Ty());
            else if (param->typeInfo->isNative(NativeTypeKind::Any) || param->typeInfo->kind == TypeInfoKind::Interface)
                params.push_back(builder.getInt8Ty()->getPointerTo());
        }
    }

    // Return value
    if (typeFunc->numReturnRegisters() > 1 || returnByCopy)
    {
        params.push_back(llvmRealReturnType);
    }

    *result = llvm::FunctionType::get(returnType, {params.begin(), params.end()}, false);
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
    auto& pp              = perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;

    int numCallParams = (int) typeFuncBC->parameters.size();
    int idxParam      = (int) pushParams.size() - 1;

    // Variadic are first
    if (numCallParams)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters.back()->typeInfo);
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            auto index = pushParams[idxParam--];
            auto r0    = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));

            index   = pushParams[idxParam--];
            auto r1 = TO_PTR_I32(GEP_I32(allocR, index));
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
            auto typePtr = CastTypeInfo<TypeInfoPointer>(typeParam, TypeInfoKind::Pointer);
            if (typePtr->ptrCount != 1)
                return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid pointer count");
            auto finalType = TypeManager::concreteType(typePtr->finalType);
            if (finalType->kind == TypeInfoKind::Native)
            {
                auto r0 = GEP_I32(allocR, index);
                auto r  = TO_PTR_PTR_NATIVE(r0, finalType->nativeType);
                if (!r)
                    return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid pointer param type");
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
                return moduleToGen->internalError(typeFuncBC->declNode, typeFuncBC->declNode->token, "emitForeignCall, invalid pointer param type");
            }
        }
        else if (typeParam->kind == TypeInfoKind::Struct ||
                 typeParam->kind == TypeInfoKind::Interface ||
                 typeParam->kind == TypeInfoKind::Lambda ||
                 typeParam->kind == TypeInfoKind::Array)
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));
        }
        else if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r0));

            index   = pushParams[idxParam--];
            auto r1 = TO_PTR_I32(GEP_I32(allocR, index));
            params.push_back(builder.CreateLoad(r1));
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
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
        returnType->isNative(NativeTypeKind::Any) ||
        returnType->isNative(NativeTypeKind::String))
    {
        //CONCAT_FIXED_STR(concat, "&rt[0]");
        params.push_back(TO_PTR_I8(allocRR));
    }
    else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        //CONCAT_FIXED_STR(concat, "rt[0].pointer");
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
    auto& pp              = perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;

    auto returnType = TypeManager::concreteReferenceType(typeFuncBC->returnType);
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
            //CONCAT_FIXED_STR(concat, "rt[0].pointer = (__u8_t*) ");
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
    auto& pp              = perThread[ct][precompileIndex];
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

    // Get parameters
    VectorNative<llvm::Value*> params;
    getForeignCallParameters(buildParameters, allocR, allocRR, moduleToGen, typeFuncBC, params, pushParams);

    // Make the call
    llvm::FunctionType* typeF;
    SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFuncBC, &typeF));
    auto func = modu.getOrInsertFunction(funcName.c_str(), typeF);
    auto F    = llvm::dyn_cast<llvm::Function>(func.getCallee());
    F->setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);
    auto result = builder.CreateCall(func, {params.begin(), params.end()});

    // Store result to rt0
    SWAG_CHECK(getForeignCallReturnValue(buildParameters, allocRR, moduleToGen, typeFuncBC, result));

    return true;
}