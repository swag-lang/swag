#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "BackendLLVMFunctionMacros_.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Workspace.h"

#define OPEQ_OVERFLOW(__intr, __inst, __type)                                                                                       \
    if (module->mustEmitSafetyOF(ip->node))                                                                                         \
    {                                                                                                                               \
        auto vs = builder.CreateIntrinsic(llvm::Intrinsic::__intr, {builder.__type, builder.__type}, {builder.CreateLoad(r1), r2}); \
        auto v0 = builder.CreateExtractValue(vs, {0});                                                                              \
        auto v1 = builder.CreateExtractValue(vs, {1});                                                                              \
                                                                                                                                    \
        llvm::BasicBlock* blockOk  = llvm::BasicBlock::Create(context, "", func);                                                   \
        llvm::BasicBlock* blockErr = llvm::BasicBlock::Create(context, "", func);                                                   \
                                                                                                                                    \
        auto v2 = builder.CreateIsNull(v1);                                                                                         \
        builder.CreateCondBr(v2, blockOk, blockErr);                                                                                \
        builder.SetInsertPoint(blockErr);                                                                                           \
        emitInternalPanic(buildParameters, allocR, allocT, ip->node, "[safety] integer overflow");                                  \
        builder.CreateBr(blockOk);                                                                                                  \
        builder.SetInsertPoint(blockOk);                                                                                            \
        builder.CreateStore(v0, r1);                                                                                                \
    }                                                                                                                               \
    else                                                                                                                            \
    {                                                                                                                               \
        auto v0 = builder.__inst(builder.CreateLoad(r1), r2);                                                                       \
        builder.CreateStore(v0, r1);                                                                                                \
    }

#define OP_OVERFLOW(__intr, __inst, __cast, __type)                                                             \
    if (module->mustEmitSafetyOF(ip->node))                                                                     \
    {                                                                                                           \
        auto vs = builder.CreateIntrinsic(llvm::Intrinsic::__intr, {builder.__type, builder.__type}, {r1, r2}); \
        auto v0 = builder.CreateExtractValue(vs, {0});                                                          \
        auto v1 = builder.CreateExtractValue(vs, {1});                                                          \
                                                                                                                \
        llvm::BasicBlock* blockOk  = llvm::BasicBlock::Create(context, "", func);                               \
        llvm::BasicBlock* blockErr = llvm::BasicBlock::Create(context, "", func);                               \
                                                                                                                \
        auto v2 = builder.CreateIsNull(v1);                                                                     \
        builder.CreateCondBr(v2, blockOk, blockErr);                                                            \
        builder.SetInsertPoint(blockErr);                                                                       \
        emitInternalPanic(buildParameters, allocR, allocT, ip->node, "[safety] integer overflow");              \
        builder.CreateBr(blockOk);                                                                              \
        builder.SetInsertPoint(blockOk);                                                                        \
        builder.CreateStore(v0, __cast(r0));                                                                    \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
        auto v0 = builder.__inst(r1, r2);                                                                       \
        builder.CreateStore(v0, __cast(r0));                                                                    \
    }

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
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
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
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
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
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;

    typeInfo  = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS | CONCRETE_FORCEALIAS);
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
        auto pointedType     = TypeManager::concreteType(typeInfoPointer->pointedType);
        if (pointedType->isNative(NativeTypeKind::Void))
            *llvmType = llvm::Type::getInt8Ty(context);
        else
            SWAG_CHECK(swagTypeToLLVMType(buildParameters, moduleToGen, pointedType, llvmType));
        *llvmType = (*llvmType)->getPointerTo();
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice ||
        typeInfo->kind == TypeInfoKind::Array ||
        typeInfo->kind == TypeInfoKind::Struct ||
        typeInfo->kind == TypeInfoKind::Interface ||
        typeInfo->kind == TypeInfoKind::TypeSet ||
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
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
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

    // Total number of registers
    auto n = typeFunc->numReturnRegisters();
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        n += typeParam->numRegisters();
    }

    auto allocRR = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(n));

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
    int idx    = typeFunc->numReturnRegisters();
    int argIdx = 0;
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

    // Affect registers
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
                 typeParam->kind == TypeInfoKind::Interface ||
                 typeParam->kind == TypeInfoKind::TypeSet)
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
            return moduleToGen->internalError(format("emitFuncWrapperPublic, invalid param type '%s'", typeParam->name.c_str()));
        }

        idx += typeParam->numRegisters();
        argIdx += typeParam->numRegisters();
    }

    // Make the call
    VectorNative<llvm::Value*> args;
    for (int i = 0; i < n; i++)
    {
        if (i < typeFunc->numReturnRegisters())
        {
            auto rr0 = builder.CreateInBoundsGEP(allocRR, builder.getInt32(i));
            args.push_back(rr0);
        }
        else
        {
            auto typeParam = typeFunc->registerIdxToType(i - typeFunc->numReturnRegisters());
            if (passByValue(typeParam))
                args.push_back(func->getArg(i - typeFunc->numReturnRegisters()));
            else
            {
                auto rr0 = builder.CreateInBoundsGEP(allocRR, builder.getInt32(i));
                args.push_back(rr0);
            }
        }
    }

    auto fcc = modu.getFunction(bc->callName().c_str());
    builder.CreateCall(fcc, {args.begin(), args.end()});

    // Return
    if (typeFunc->numReturnRegisters() && !returnByCopy)
    {
        auto rr0        = allocRR;
        auto returnType = TypeManager::concreteType(typeFunc->returnType, CONCRETE_ALIAS | CONCRETE_ENUM | CONCRETE_FORCEALIAS);

        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->kind == TypeInfoKind::Interface ||
            returnType->kind == TypeInfoKind::TypeSet ||
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

void BackendLLVM::emitInternalPanic(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, AstNode* node, const char* message)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;

    // Filename
    llvm::Value* r1 = builder.CreateGlobalString(normalizePath(node->sourceFile->path).c_str());
    r1              = TO_PTR_I8(builder.CreateInBoundsGEP(r1, {pp.cst0_i32}));

    // Line & column
    auto r2 = builder.getInt32(node->token.startLocation.line);
    auto r3 = builder.getInt32(node->token.startLocation.column);

    // Message
    llvm::Value* r4;
    if (message)
    {
        r4 = builder.CreateGlobalString(message);
        r4 = TO_PTR_I8(builder.CreateInBoundsGEP(r4, {pp.cst0_i32}));
    }
    else
        r4 = builder.CreateIntToPtr(pp.cst0_i64, builder.getInt8PtrTy());

    localCall(buildParameters, allocR, allocT, "__panic", {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX}, {r1, r2, r3, r4});
}

void BackendLLVM::setFuncAttributes(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc, llvm::Function* func)
{
    llvm::AttrBuilder attr;

    if (!moduleToGen->mustOptimizeBK(bc->node))
        attr.addAttribute(llvm::Attribute::OptimizeNone);

    func->addAttributes(llvm::AttributeList::FunctionIndex, attr);
}

bool BackendLLVM::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC) && (buildParameters.compileType != BackendCompileType::Test))
        return true;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;
    auto  typeFunc        = bc->callType();
    bool  ok              = true;

    // Function prototype
    llvm::FunctionType* funcType = createFunctionTypeInternal(buildParameters, typeFunc);
    llvm::Function*     func     = (llvm::Function*) modu.getOrInsertFunction(bc->callName().c_str(), funcType).getCallee();
    setFuncAttributes(buildParameters, moduleToGen, bc, func);

    // No pointer aliasing, on all pointers. Is this correct ??
    // Note that without the NoAlias flag, some optims will not trigger (like vectorisation)
    for (int i = 0; i < func->arg_size(); i++)
    {
        auto arg = func->getArg(i);
        if (!arg->getType()->isPointerTy())
            continue;
        arg->addAttr(llvm::Attribute::NoAlias);
    }

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
    {
        allocStack = builder.CreateAlloca(builder.getInt8Ty(), builder.getInt32(typeFunc->stackSize));
        allocStack->setAlignment(llvm::Align(16));
    }

    // Reserve room to pass parameters to embedded intrinsics
    const int ALLOCT_NUM = 5;
    auto      allocT     = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(ALLOCT_NUM));

    // Debug infos
    if (pp.dbg && bc->node)
    {
        pp.dbg->startFunction(buildParameters, pp, bc, func, allocStack);
        pp.dbg->setLocation(pp.builder, bc, nullptr);
    }

    // Generate bytecode
    auto                                   ip = bc->out;
    VectorNative<pair<uint32_t, uint32_t>> pushRVParams;
    VectorNative<uint32_t>                 pushRAParams;
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
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::DebugNop:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocT, 0));
            builder.CreateStore(builder.getInt32(0), r0);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicAddS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAddS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAddS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAddS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Add, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicAndS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAndS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAndS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicAndS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::And, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicOrS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicOrS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicOrS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicOrS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Or, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicXorS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXorS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXorS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXorS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xor, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicXchgS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXchgS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I16(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXchgS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = TO_PTR_I32(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(v0, r3);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicXchgS64:
        {
            MK_BINOPEQ64_CAB();
            auto v0 = builder.CreateAtomicRMW(llvm::AtomicRMWInst::BinOp::Xchg, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent);
            auto r3 = GEP_I32(allocR, ip->c.u32);
            builder.CreateStore(v0, r3);
            break;
        }

        case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
        {
            auto r0   = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto ptr0 = builder.CreateLoad(r0);

            llvm::Value* r1 = MK_IMMB_8();
            llvm::Value* r2 = MK_IMMC_8();
            auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
            v0->setVolatile(true);
            auto v1 = builder.CreateExtractValue(v0, 0);

            auto r4 = TO_PTR_I8(GEP_I32(allocR, ip->d.u32));
            builder.CreateStore(v1, r4);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
        {
            auto r0   = TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto ptr0 = builder.CreateLoad(r0);

            llvm::Value* r1 = MK_IMMB_16();
            llvm::Value* r2 = MK_IMMC_16();
            auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
            v0->setVolatile(true);
            auto v1 = builder.CreateExtractValue(v0, 0);

            auto r4 = TO_PTR_I16(GEP_I32(allocR, ip->d.u32));
            builder.CreateStore(v1, r4);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
        {
            auto r0   = TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto ptr0 = builder.CreateLoad(r0);

            llvm::Value* r1 = MK_IMMB_32();
            llvm::Value* r2 = MK_IMMC_32();
            auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
            v0->setVolatile(true);
            auto v1 = builder.CreateExtractValue(v0, 0);

            auto r4 = TO_PTR_I32(GEP_I32(allocR, ip->d.u32));
            builder.CreateStore(v1, r4);
            break;
        }
        case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
        {
            auto r0   = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto ptr0 = builder.CreateLoad(r0);

            llvm::Value* r1 = MK_IMMB_64();
            llvm::Value* r2 = MK_IMMC_64();
            auto         v0 = builder.CreateAtomicCmpXchg(ptr0, r1, r2, llvm::AtomicOrdering::SequentiallyConsistent, llvm::AtomicOrdering::SequentiallyConsistent);
            v0->setVolatile(true);
            auto v1 = builder.CreateExtractValue(v0, 0);

            auto r4 = GEP_I32(allocR, ip->d.u32);
            builder.CreateStore(v1, r4);
            break;
        }

        case ByteCodeOp::IncPointer64:
        case ByteCodeOp::DecPointer64:
        {
            auto         r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32));
            auto         r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            llvm::Value* r2;
            if (ip->flags & BCI_IMM_B)
                r2 = builder.getInt64(ip->b.u64);
            else
                r2 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            if (ip->op == ByteCodeOp::DecPointer64)
                r2 = builder.CreateNeg(r2);
            auto r3 = builder.CreateInBoundsGEP(r1, r2);
            builder.CreateStore(r3, r0);
            break;
        }

        case ByteCodeOp::DeRef8:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto r1 = TO_PTR_I8(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(builder.CreateLoad(r1), r2);
            break;
        }
        case ByteCodeOp::DeRef16:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto r1 = TO_PTR_I16(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(builder.CreateLoad(r1), r2);
            break;
        }
        case ByteCodeOp::DeRef32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto r1 = TO_PTR_I32(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(builder.CreateLoad(r1), r2);
            break;
        }
        case ByteCodeOp::DeRef64:
        {
            auto r0 = GEP_I32(allocR, ip->b.u32);
            auto r1 = TO_PTR_I64(builder.CreateLoad(TO_PTR_PTR_I8(r0)));
            auto r2 = TO_PTR_I64(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(builder.CreateLoad(r1), r2);
            break;
        }
        case ByteCodeOp::DeRefPointer:
        {
            auto r0   = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto ptr  = builder.CreateLoad(r0);
            auto ptr8 = GEP_I32(ptr, ip->c.u32);
            auto v8   = builder.CreateLoad(TO_PTR_PTR_I8(ptr8));
            auto r1   = TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32));
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

        case ByteCodeOp::Mul64byVB64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.getInt64(ip->b.u64);
            builder.CreateStore(builder.CreateMul(builder.CreateLoad(r0), r1), r0);
            break;
        }
        case ByteCodeOp::Div64byVB64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.getInt64(ip->b.u64);
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
            builder.CreateMemSet(builder.CreateInBoundsGEP(allocStack, CST_RA32), pp.cst0_i8, ip->b.u32, llvm::Align{});
            break;
        }

        case ByteCodeOp::SetZeroAtPointer8:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i8, builder.CreateLoad(r0));
            break;
        }
        case ByteCodeOp::SetZeroAtPointer16:
        {
            auto r0 = TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i16, builder.CreateLoad(r0));
            break;
        }
        case ByteCodeOp::SetZeroAtPointer32:
        {
            auto r0 = TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i32, builder.CreateLoad(r0));
            break;
        }
        case ByteCodeOp::SetZeroAtPointer64:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(pp.cst0_i64, builder.CreateLoad(r0));
            break;
        }
        case ByteCodeOp::SetZeroAtPointer64OffVB32:
        {
            auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateInBoundsGEP(builder.CreateLoad(r0), CST_RB32);
            builder.CreateStore(pp.cst0_i64, v0);
            break;
        }
        case ByteCodeOp::SetZeroAtPointerX:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            builder.CreateMemSet(builder.CreateLoad(r0), pp.cst0_i8, ip->b.u64, llvm::Align{});
            break;
        }
        case ByteCodeOp::SetZeroAtPointerXRB:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r2 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            auto v2 = builder.CreateMul(r2, builder.getInt64(ip->c.u64));
            builder.CreateMemSet(builder.CreateLoad(r0), pp.cst0_i8, v2, llvm::Align{});
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

        case ByteCodeOp::GetFromStack8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::GetFromStack16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::GetFromStack32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r1), builder.getInt64Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::GetFromStack64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::GetFromStack64x2:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));
            builder.CreateStore(builder.CreateLoad(r1), r0);

            auto r2 = GEP_I32(allocR, ip->c.u32);
            auto r3 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RD32));
            builder.CreateStore(builder.CreateLoad(r3), r2);
            break;
        }

        case ByteCodeOp::IntrinsicStrCmp:
        {
            localCall(buildParameters, allocR, allocT, "@strcmp", {ip->d.u32, ip->a.u32, ip->b.u32, ip->c.u32, ip->d.u32}, {});
            break;
        }
        case ByteCodeOp::IntrinsicTypeCmp:
        {
            localCall(buildParameters, allocR, allocT, "@typecmp", {ip->d.u32, ip->a.u32, ip->b.u32, ip->c.u32}, {});
            break;
        }

        case ByteCodeOp::MemCpy8:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::MemCpy16:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::MemCpy32:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }
        case ByteCodeOp::MemCpy64:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateLoad(r1), r0);
            break;
        }

        case ByteCodeOp::MemCpyX:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateMemCpy(r0, llvm::Align{}, r1, llvm::Align{}, builder.getInt64(ip->c.u64));
            break;
        }

        case ByteCodeOp::IntrinsicMemCpy:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));

            if (ip->flags & BCI_IMM_C)
                builder.CreateMemCpy(r0, llvm::Align{}, r1, llvm::Align{}, builder.getInt64(ip->c.u64));
            else
            {
                auto r2 = builder.CreateLoad(GEP_I32(allocR, ip->c.u32));
                builder.CreateMemCpy(r0, llvm::Align{}, r1, llvm::Align{}, r2);
            }

            break;
        }

        case ByteCodeOp::IntrinsicMemMove:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r2 = builder.CreateLoad(GEP_I32(allocR, ip->c.u32));
            builder.CreateMemMove(r0, llvm::Align{}, r1, llvm::Align{}, r2);
            break;
        }

        case ByteCodeOp::IntrinsicMemSet:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            auto r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r2 = builder.CreateLoad(GEP_I32(allocR, ip->c.u32));
            builder.CreateMemSet(r0, r1, r2, llvm::Align{});
            break;
        }

        case ByteCodeOp::IntrinsicMemCmp:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r2 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->c.u32)));
            auto r3 = builder.CreateLoad(GEP_I32(allocR, ip->d.u32));
            builder.CreateStore(builder.CreateCall(pp.fn_memcmp, {r1, r2, r3}), r0);
            break;
        }

        case ByteCodeOp::IntrinsicCStrLen:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(builder.CreateCall(pp.fn_strlen, {r1}), r0);
            break;
        }

        case ByteCodeOp::IntrinsicAlloc:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(builder.CreateCall(pp.fn_malloc, {r1}), r0);
            break;
        }
        case ByteCodeOp::IntrinsicRealloc:
        {
            auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            auto r2 = builder.CreateLoad(GEP_I32(allocR, ip->c.u32));
            builder.CreateStore(builder.CreateCall(pp.fn_realloc, {r1, r2}), r0);
            break;
        }
        case ByteCodeOp::IntrinsicFree:
        {
            auto r0 = builder.CreateLoad(TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32)));
            builder.CreateCall(pp.fn_free, {r0});
            break;
        }

        case ByteCodeOp::IntrinsicInterfaceOf:
        {
            localCall(buildParameters, allocR, allocT, "@interfaceof", {ip->c.u32, ip->a.u32, ip->b.u32}, {});
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

        case ByteCodeOp::IncrementRA32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), pp.cst1_i32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::DecrementRA32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), pp.cst1_i32);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::IncrementRA64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), pp.cst1_i64);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::DecrementRA64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateSub(builder.CreateLoad(r0), pp.cst1_i64);
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
        case ByteCodeOp::Add64byVB64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateAdd(builder.CreateLoad(r0), CST_RB64);
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

        case ByteCodeOp::SetAtStackPointer8:
        {
            auto         r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt8(ip->b.u8);
            else
                r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtStackPointer16:
        {
            auto         r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt16(ip->b.u16);
            else
                r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtStackPointer32:
        {
            auto         r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt32(ip->b.u32);
            else
                r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::SetAtStackPointer64:
        {
            auto         r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32));
            llvm::Value* r1;
            if (ip->flags & BCI_IMM_B)
                r1 = builder.getInt64(ip->b.u64);
            else
                r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::SetAtStackPointer8x2:
        {
            {
                auto         r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RA32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_B)
                    r1 = builder.getInt8(ip->b.u8);
                else
                    r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));
                builder.CreateStore(r1, r0);
            }
            {
                auto         r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RC32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_D)
                    r1 = builder.getInt8(ip->d.u8);
                else
                    r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->d.u32)));
                builder.CreateStore(r1, r0);
            }
            break;
        }
        case ByteCodeOp::SetAtStackPointer16x2:
        {
            {
                auto         r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_B)
                    r1 = builder.getInt16(ip->b.u16);
                else
                    r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));
                builder.CreateStore(r1, r0);
            }
            {
                auto         r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RC32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_D)
                    r1 = builder.getInt16(ip->d.u16);
                else
                    r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->d.u32)));
                builder.CreateStore(r1, r0);
            }
            break;
        }
        case ByteCodeOp::SetAtStackPointer32x2:
        {
            {
                auto         r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_B)
                    r1 = builder.getInt32(ip->b.u32);
                else
                    r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));
                builder.CreateStore(r1, r0);
            }
            {
                auto         r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RC32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_D)
                    r1 = builder.getInt32(ip->d.u32);
                else
                    r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->d.u32)));
                builder.CreateStore(r1, r0);
            }
            break;
        }
        case ByteCodeOp::SetAtStackPointer64x2:
        {
            {
                auto         r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_B)
                    r1 = builder.getInt64(ip->b.u64);
                else
                    r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
                builder.CreateStore(r1, r0);
            }
            {
                auto         r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RC32));
                llvm::Value* r1;
                if (ip->flags & BCI_IMM_D)
                    r1 = builder.getInt64(ip->d.u64);
                else
                    r1 = builder.CreateLoad(GEP_I32(allocR, ip->d.u32));
                builder.CreateStore(r1, r0);
            }
            break;
        }

        case ByteCodeOp::BinOpPlusS32:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(sadd_with_overflow, CreateAdd, TO_PTR_I32, getInt32Ty());
            break;
        }
        case ByteCodeOp::BinOpPlusU32:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(uadd_with_overflow, CreateAdd, TO_PTR_I32, getInt32Ty());
            break;
        }
        case ByteCodeOp::BinOpPlusS64:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(sadd_with_overflow, CreateAdd, TO_PTR_I64, getInt64Ty());
            break;
        }
        case ByteCodeOp::BinOpPlusU64:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(uadd_with_overflow, CreateAdd, TO_PTR_I64, getInt64Ty());
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
            OP_OVERFLOW(ssub_with_overflow, CreateSub, TO_PTR_I32, getInt32Ty());
            break;
        }
        case ByteCodeOp::BinOpMinusU32:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(usub_with_overflow, CreateSub, TO_PTR_I32, getInt32Ty());
            break;
        }
        case ByteCodeOp::BinOpMinusS64:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(ssub_with_overflow, CreateSub, TO_PTR_I64, getInt64Ty());
            break;
        }
        case ByteCodeOp::BinOpMinusU64:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(usub_with_overflow, CreateSub, TO_PTR_I64, getInt64Ty());
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
            OP_OVERFLOW(smul_with_overflow, CreateMul, TO_PTR_I32, getInt32Ty());
            break;
        }
        case ByteCodeOp::BinOpMulU32:
        {
            MK_BINOP32_CAB();
            OP_OVERFLOW(umul_with_overflow, CreateMul, TO_PTR_I32, getInt32Ty());
            break;
        }
        case ByteCodeOp::BinOpMulS64:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(smul_with_overflow, CreateMul, TO_PTR_I64, getInt64Ty());
            break;
        }
        case ByteCodeOp::BinOpMulU64:
        {
            MK_BINOP64_CAB();
            OP_OVERFLOW(umul_with_overflow, CreateMul, TO_PTR_I64, getInt64Ty());
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
            OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, getInt8Ty());
            break;
        }
        case ByteCodeOp::AffectOpMinusEqU8:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(usub_with_overflow, CreateSub, getInt8Ty());
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS16:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, getInt16Ty());
            break;
        }
        case ByteCodeOp::AffectOpMinusEqU16:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(usub_with_overflow, CreateSub, getInt16Ty());
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS32:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, getInt32Ty());
            break;
        }
        case ByteCodeOp::AffectOpMinusEqU32:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(usub_with_overflow, CreateSub, getInt32Ty());
            break;
        }
        case ByteCodeOp::AffectOpMinusEqS64:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(ssub_with_overflow, CreateSub, getInt64Ty());
            break;
        }
        case ByteCodeOp::AffectOpMinusEqU64:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(usub_with_overflow, CreateSub, getInt64Ty());
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
            OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, getInt8Ty());
            break;
        }
        case ByteCodeOp::AffectOpPlusEqU8:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, getInt8Ty());
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS16:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, getInt16Ty());
            break;
        }
        case ByteCodeOp::AffectOpPlusEqU16:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, getInt16Ty());
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS32:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, getInt32Ty());
            break;
        }
        case ByteCodeOp::AffectOpPlusEqU32:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, getInt32Ty());
            break;
        }
        case ByteCodeOp::AffectOpPlusEqS64:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(sadd_with_overflow, CreateAdd, getInt64Ty());
            break;
        }
        case ByteCodeOp::AffectOpPlusEqU64:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(uadd_with_overflow, CreateAdd, getInt64Ty());
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

        case ByteCodeOp::AffectOpMulEqS8:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(smul_with_overflow, CreateMul, getInt8Ty());
            break;
        }
        case ByteCodeOp::AffectOpMulEqU8:
        {
            MK_BINOPEQ8_CAB();
            OPEQ_OVERFLOW(umul_with_overflow, CreateMul, getInt8Ty());
            break;
        }
        case ByteCodeOp::AffectOpMulEqS16:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(smul_with_overflow, CreateMul, getInt16Ty());
            break;
        }
        case ByteCodeOp::AffectOpMulEqU16:
        {
            MK_BINOPEQ16_CAB();
            OPEQ_OVERFLOW(umul_with_overflow, CreateMul, getInt16Ty());
            break;
        }
        case ByteCodeOp::AffectOpMulEqS32:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(smul_with_overflow, CreateMul, getInt32Ty());
            break;
        }
        case ByteCodeOp::AffectOpMulEqU32:
        {
            MK_BINOPEQ32_CAB();
            OPEQ_OVERFLOW(umul_with_overflow, CreateMul, getInt32Ty());
            break;
        }
        case ByteCodeOp::AffectOpMulEqS64:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(smul_with_overflow, CreateMul, getInt64Ty());
            break;
        }
        case ByteCodeOp::AffectOpMulEqU64:
        {
            MK_BINOPEQ64_CAB();
            OPEQ_OVERFLOW(umul_with_overflow, CreateMul, getInt64Ty());
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

        case ByteCodeOp::AffectOpShiftLeftEqU8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0));
            auto r2 = TO_PTR_I8(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqU16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0));
            auto r2 = TO_PTR_I16(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqU32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0));
            auto r2 = TO_PTR_I32(GEP_I32(allocR, ip->b.u32));
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpShiftLeftEqU64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0));
            auto r2 = GEP_I32(allocR, ip->b.u32);
            auto v0 = builder.CreateShl(builder.CreateLoad(r1), builder.CreateLoad(r2));
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

        case ByteCodeOp::AffectOpXorEqS8:
        {
            MK_BINOPEQ8_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXorEqS16:
        {
            MK_BINOPEQ16_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXorEqS32:
        {
            MK_BINOPEQ32_CAB();
            auto v0 = builder.CreateXor(builder.CreateLoad(r1), r2);
            builder.CreateStore(v0, r1);
            break;
        }
        case ByteCodeOp::AffectOpXorEqS64:
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

        case ByteCodeOp::CompareOpGreaterEqS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpSGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpSGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpUGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpUGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpUGE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpGreaterEqF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpUGE(r1, r2);
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

        case ByteCodeOp::CompareOpLowerEqS32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpSLE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqS64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpSLE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqU32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpULE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqU64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpULE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqF32:
        {
            MK_BINOPF32_CAB();
            auto v0 = builder.CreateFCmpULE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpLowerEqF64:
        {
            MK_BINOPF64_CAB();
            auto v0 = builder.CreateFCmpULE(r1, r2);
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

        case ByteCodeOp::CompareOpNotEqual8:
        {
            MK_BINOP8_CAB();
            auto v0 = builder.CreateICmpNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpNotEqual16:
        {
            MK_BINOP16_CAB();
            auto v0 = builder.CreateICmpNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpNotEqual32:
        {
            MK_BINOP32_CAB();
            auto v0 = builder.CreateICmpNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CompareOpNotEqual64:
        {
            MK_BINOP64_CAB();
            auto v0 = builder.CreateICmpNE(r1, r2);
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, r0);
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
        case ByteCodeOp::JumpIfZero8:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfZero16:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNull(builder.CreateLoad(r0));
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
        case ByteCodeOp::JumpIfNotZero8:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
            builder.CreateCondBr(b0, labelTrue, labelFalse);
            blockIsClosed = true;
            break;
        }
        case ByteCodeOp::JumpIfNotZero16:
        {
            auto labelTrue  = getOrCreateLabel(pp, func, i + ip->b.s32 + 1);
            auto labelFalse = getOrCreateLabel(pp, func, i + 1);
            auto r0         = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto b0         = builder.CreateIsNotNull(builder.CreateLoad(r0));
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
        {
            // Hack thanks to llvm in debug mode : we need to force the usage of function parameters until the very end of the function (i.e. each return),
            // otherwhise :
            // - parameters not used in the function body will be removed by llvm (even without optims activated !)
            // - a parameter will not be visible anymore ("optimized away") after its last usage.
            // So we force a read/write of each parameter just before the "ret" to avoid that debug mess.
            // RIDICULOUS !!
            //
            // If we request an optimized code, do not do that crap.
            bool isDebug = !buildParameters.buildCfg->backendOptimizeSpeed && !buildParameters.buildCfg->backendOptimizeSize;
            if (isDebug && buildParameters.buildCfg->backendDebugInformations)
            {
                auto r1 = GEP_I32(allocT, 0);
                for (int iparam = 0; iparam < typeFunc->parameters.size(); iparam++)
                {
                    SWAG_CHECK(storeLocalParam(buildParameters, func, typeFunc, iparam, r1));
                }
            }

            builder.CreateRetVoid();
            blockIsClosed = true;
            break;
        }

        case ByteCodeOp::IntrinsicPrintS64:
        case ByteCodeOp::IntrinsicPrintF64:
        {
            auto bcF = ((AstFuncDecl*) ip->node->resolvedSymbolOverload->node)->extension->bc;
            localCall(buildParameters, allocR, allocT, bcF->callName().c_str(), {ip->a.u32}, {});
            break;
        }
        case ByteCodeOp::IntrinsicPrintString:
        {
            auto bcF = ((AstFuncDecl*) ip->node->resolvedSymbolOverload->node)->extension->bc;
            localCall(buildParameters, allocR, allocT, bcF->callName().c_str(), {ip->a.u32, ip->b.u32}, {});
            break;
        }
        case ByteCodeOp::IntrinsicErrorMsg:
        {
            auto bcF = ((AstFuncDecl*) ip->node->resolvedSymbolOverload->node)->extension->bc;
            localCall(buildParameters, allocR, allocT, bcF->callName().c_str(), {ip->a.u32, ip->b.u32, ip->c.u32}, {});
            break;
        }
        case ByteCodeOp::IntrinsicPanic:
        {
            localCall(buildParameters, allocR, allocT, "@panic", {ip->a.u32, ip->b.u32, ip->c.u32}, {});
            break;
        }

        case ByteCodeOp::InternalInitStackTrace:
        {
            localCall(buildParameters, allocR, allocT, "__initStackTrace", {ip->a.u32}, {});
            break;
        }
        case ByteCodeOp::InternalStackTrace:
        {
            localCall(buildParameters, allocR, allocT, "__stackTrace", {ip->a.u32}, {});
            break;
        }
        case ByteCodeOp::InternalPanic:
        {
            emitInternalPanic(buildParameters, allocR, allocT, ip->node, (const char*) ip->d.pointer);
            break;
        }

        case ByteCodeOp::IntrinsicGetContext:
        {
            auto v0 = builder.CreateLoad(TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst1_i32})));
            localCall(buildParameters, allocR, allocT, "__tlsGetValue", {ip->a.u32, UINT32_MAX}, {0, v0});
            break;
        }
        case ByteCodeOp::IntrinsicSetContext:
        {
            auto v0 = builder.CreateLoad(TO_PTR_I64(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst1_i32})));
            localCall(buildParameters, allocR, allocT, "__tlsSetValue", {UINT32_MAX, ip->a.u32}, {v0, 0});
            break;
        }
        case ByteCodeOp::IntrinsicArguments:
        {
            localCall(buildParameters, allocR, allocT, "@args", {ip->a.u32, ip->b.u32}, {});
            break;
        }

        case ByteCodeOp::IntrinsicGetErr:
        {
            localCall(buildParameters, allocR, allocT, "__geterr", {ip->a.u32, ip->b.u32}, {});
            break;
        }
        case ByteCodeOp::IntrinsicSetErr:
        {
            localCall(buildParameters, allocR, allocT, "@seterr", {ip->a.u32, ip->b.u32}, {});
            break;
        }
        case ByteCodeOp::InternalPushErr:
        {
            localCall(buildParameters, allocR, allocT, "__pusherr", {}, {});
            break;
        }
        case ByteCodeOp::InternalPopErr:
        {
            localCall(buildParameters, allocR, allocT, "__poperr", {}, {});
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

        case ByteCodeOp::InvertU8:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertU16:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertU32:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateNot(builder.CreateLoad(r0));
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::InvertU64:
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

        case ByteCodeOp::CastInvBool8:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNull(builder.CreateLoad(TO_PTR_I8(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastInvBool16:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNull(builder.CreateLoad(TO_PTR_I16(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastInvBool32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNull(builder.CreateLoad(TO_PTR_I32(r0)));
            v0      = builder.CreateIntCast(v0, builder.getInt8Ty(), false);
            builder.CreateStore(v0, TO_PTR_I8(r0));
            break;
        }
        case ByteCodeOp::CastInvBool64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateIsNull(builder.CreateLoad(TO_PTR_I64(r0)));
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
        case ByteCodeOp::CastS8S32:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt32Ty(), true);
            r0      = TO_PTR_I32(r0);
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
        case ByteCodeOp::CastF32S32:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), builder.getInt32Ty());
            r0      = TO_PTR_I32(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastS8S64:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), true);
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16S64:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateIntCast(builder.CreateLoad(r0), builder.getInt64Ty(), true);
            r0      = TO_PTR_I64(r0);
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
        case ByteCodeOp::CastF64S64:
        {
            auto r0 = TO_PTR_F64(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPToSI, builder.CreateLoad(r0), builder.getInt64Ty());
            r0      = TO_PTR_I64(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CastS8F32:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16F32:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
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
        case ByteCodeOp::CastU8F32:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU16F32:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
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
        case ByteCodeOp::CastU64F32:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getFloatTy());
            r0      = TO_PTR_F32(r0);
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

        case ByteCodeOp::CastS8F64:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS16F64:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastS32F64:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::SIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
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
        case ByteCodeOp::CastU8F64:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU16F64:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }
        case ByteCodeOp::CastU32F64:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::UIToFP, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
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
        case ByteCodeOp::CastF32F64:
        {
            auto r0 = TO_PTR_F32(GEP_I32(allocR, ip->a.u32));
            auto v0 = builder.CreateCast(llvm::Instruction::CastOps::FPExt, builder.CreateLoad(r0), builder.getDoubleTy());
            r0      = TO_PTR_F64(r0);
            builder.CreateStore(v0, r0);
            break;
        }

        case ByteCodeOp::CopyRCtoRR:
        {
            auto r1 = MK_IMMA_64();
            builder.CreateStore(r1, func->getArg(0));
            break;
        }
        case ByteCodeOp::CopyRCtoRR2:
        {
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(r1, func->getArg(0));
            r1 = builder.CreateLoad(GEP_I32(allocR, ip->b.u32));
            builder.CreateStore(r1, func->getArg(1));
            break;
        }
        case ByteCodeOp::CopyRCtoRT:
        {
            auto r0 = GEP_I32(allocRR, 0);
            auto r1 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::CopyRRtoRC:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(func->getArg(0));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::CopyRTtoRC:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(GEP_I32(allocRR, 0));
            builder.CreateStore(r1, r0);
            break;
        }
        case ByteCodeOp::CopyRTtoRC2:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = builder.CreateLoad(GEP_I32(allocRR, 0));
            builder.CreateStore(r1, r0);
            r0 = GEP_I32(allocR, ip->b.u32);
            r1 = builder.CreateLoad(GEP_I32(allocRR, 1));
            builder.CreateStore(r1, r0);
            break;
        }

        case ByteCodeOp::GetFromStackParam64:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            SWAG_CHECK(storeLocalParam(buildParameters, func, typeFunc, ip->c.u32, r0));
            break;
        }

        case ByteCodeOp::MakeStackPointerParam:
        {
            auto r0     = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
            auto offArg = ip->c.u32 + typeFunc->numReturnRegisters();
            auto r1     = func->getArg(offArg);
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

        case ByteCodeOp::PushRVParam:
            pushRVParams.push_back({ip->a.u32, ip->b.u32});
            break;
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
            // We need to make all variadic parameters contiguous in stack, and point to that address
            // Two cases here :
            // pushRVParams has something: then we have a typed param, of 1/2/4 bytes each
            // pushRVParams is empty: we make an array of registers

            if (!pushRVParams.empty())
            {
                llvm::AllocaInst* allocVA = nullptr;
                auto              sizeOf  = pushRVParams[0].second;
                switch (sizeOf)
                {
                case 1:
                {
                    allocVA = builder.CreateAlloca(builder.getInt8Ty(), builder.getInt64(pushRVParams.size()));
                    auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
                    auto r1 = builder.CreateInBoundsGEP(allocVA, pp.cst0_i32);
                    builder.CreateStore(r1, r0);
                    break;
                }
                case 2:
                {
                    allocVA = builder.CreateAlloca(builder.getInt16Ty(), builder.getInt64(pushRVParams.size()));
                    auto r0 = TO_PTR_PTR_I16(GEP_I32(allocR, ip->a.u32));
                    auto r1 = builder.CreateInBoundsGEP(allocVA, pp.cst0_i32);
                    builder.CreateStore(r1, r0);
                    break;
                }
                case 4:
                {
                    allocVA = builder.CreateAlloca(builder.getInt32Ty(), builder.getInt64(pushRVParams.size()));
                    auto r0 = TO_PTR_PTR_I32(GEP_I32(allocR, ip->a.u32));
                    auto r1 = builder.CreateInBoundsGEP(allocVA, pp.cst0_i32);
                    builder.CreateStore(r1, r0);
                    break;
                }
                }

                SWAG_ASSERT(allocVA);
                int idx      = 0;
                int idxParam = (int) pushRVParams.size() - 1;
                while (idxParam >= 0)
                {
                    auto         reg = pushRVParams[idxParam].first;
                    auto         r0  = GEP_I32(allocVA, idx);
                    llvm::Value* r1  = nullptr;
                    switch (sizeOf)
                    {
                    case 1:
                        r1 = TO_PTR_I8(GEP_I32(allocR, reg));
                        break;
                    case 2:
                        r1 = TO_PTR_I16(GEP_I32(allocR, reg));
                        break;
                    case 4:
                        r1 = TO_PTR_I32(GEP_I32(allocR, reg));
                        break;
                    }

                    SWAG_ASSERT(r1);
                    builder.CreateStore(builder.CreateLoad(r1), r0);
                    idx++;
                    idxParam--;
                }
            }
            else
            {
                auto allocVA = builder.CreateAlloca(builder.getInt64Ty(), builder.getInt64(pushRAParams.size()));

                // In the pushRAParams array, we have first all the variadic registers
                //
                // And then, we have all normal parameters. So we start at pushRAParams.size() less the number of registers
                // used to pass the normal parameters.
                //
                // The number of normal parameters is deduced from the 'offset' of the CopySPVaargs instruction (ip->b.u32)
                int idx      = 0;
                int idxParam = (int) pushRAParams.size() - (ip->b.u32 / sizeof(Register)) - 1;
                while (idxParam >= 0)
                {
                    auto r0 = GEP_I32(allocVA, idx);
                    auto r1 = GEP_I32(allocR, pushRAParams[idxParam]);
                    builder.CreateStore(builder.CreateLoad(r1), r0);
                    idx++;
                    idxParam--;
                }

                auto r0 = TO_PTR_PTR_I64(GEP_I32(allocR, ip->a.u32));
                auto r1 = builder.CreateInBoundsGEP(allocVA, pp.cst0_i32);
                builder.CreateStore(r1, r0);
            }
            break;
        }

        case ByteCodeOp::MakeLambda:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->b.pointer, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode);
            TypeInfoFuncAttr* typeFuncNode = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

            if (funcNode->attributeFlags & ATTRIBUTE_FOREIGN)
            {
                ComputedValue foreignValue;
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
            }
            else if (funcNode->attributeFlags & ATTRIBUTE_CALLBACK)
            {
                funcNode->computeFullNameForeign(true);

                llvm::FunctionType* T;
                SWAG_CHECK(createFunctionTypeForeign(buildParameters, moduleToGen, typeFuncNode, &T));
                auto F = (llvm::Function*) modu.getOrInsertFunction(funcNode->fullnameForeign.c_str(), T).getCallee();

                auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
                builder.CreateStore(TO_PTR_I8(F), r0);

                auto v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
                auto v1 = builder.CreateOr(v0, builder.getInt64(SWAG_LAMBDA_FOREIGN_MARKER));
                builder.CreateStore(v1, GEP_I32(allocR, ip->a.u32));
            }
            else
            {
                auto funcBC = (ByteCode*) ip->c.pointer;
                SWAG_ASSERT(funcBC);
                auto typeFuncLambda = CastTypeInfo<TypeInfoFuncAttr>(funcBC->node->typeInfo, TypeInfoKind::FuncAttr);

                auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
                auto T  = createFunctionTypeInternal(buildParameters, typeFuncLambda);
                auto F  = (llvm::Function*) modu.getOrInsertFunction(funcBC->callName().c_str(), T).getCallee();
                builder.CreateStore(TO_PTR_I8(F), r0);
            }
            break;
        }

        case ByteCodeOp::IntrinsicMakeForeign:
        {
            auto v0 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));
            auto v1 = builder.CreateOr(v0, builder.getInt64(SWAG_LAMBDA_FOREIGN_MARKER));
            builder.CreateStore(v1, GEP_I32(allocR, ip->a.u32));
            break;
        }

        case ByteCodeOp::IntrinsicMakeCallback:
        {
            llvm::BasicBlock* blockLambdaBC      = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockLambdaForeign = llvm::BasicBlock::Create(context, "", func);
            llvm::BasicBlock* blockNext          = llvm::BasicBlock::Create(context, "", func);
            auto              v0                 = builder.CreateLoad(GEP_I32(allocR, ip->a.u32));

            {
                auto v1 = builder.CreateAnd(v0, builder.getInt64(SWAG_LAMBDA_BC_MARKER));
                auto v2 = builder.CreateIsNotNull(v1);
                builder.CreateCondBr(v2, blockLambdaBC, blockLambdaForeign);
            }

            builder.SetInsertPoint(blockLambdaBC);
            {
                auto r1 = builder.CreateLoad(TO_PTR_PTR_I8(builder.CreateInBoundsGEP(pp.processInfos, {pp.cst0_i32, pp.cst4_i32})));
                auto PT = llvm::PointerType::getUnqual(pp.makeCallbackTy);
                auto r2 = builder.CreatePointerCast(r1, PT);
                auto v2 = builder.CreateIntToPtr(v0, builder.getInt8PtrTy());
                auto v1 = builder.CreateCall(pp.makeCallbackTy, r2, {v2});
                auto r0 = TO_PTR_PTR_I8(GEP_I32(allocR, ip->a.u32));
                builder.CreateStore(v1, r0);
                builder.CreateBr(blockNext);
            }

            builder.SetInsertPoint(blockLambdaForeign);
            {
                auto v1 = builder.CreateAnd(v0, builder.getInt64(~SWAG_LAMBDA_FOREIGN_MARKER));
                auto r0 = GEP_I32(allocR, ip->a.u32);
                builder.CreateStore(v1, r0);
                builder.CreateBr(blockNext);
            }

            builder.SetInsertPoint(blockNext);
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
                getLocalCallParameters(buildParameters, allocR, allocRR, allocT, fctParams, typeFuncBC, pushRAParams, {});
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
                getLocalCallParameters(buildParameters, allocR, allocRR, allocT, fctParams, typeFuncBC, pushRAParams, {});
                builder.CreateCall(pp.bytecodeRunTy, r2, {fctParams.begin(), fctParams.end()});
                builder.CreateBr(blockNext);
            }

            builder.SetInsertPoint(blockNext);
            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

        case ByteCodeOp::IncSPPostCall:
            pushRAParams.clear();
            pushRVParams.clear();
            break;

        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            auto                       FT = createFunctionTypeInternal(buildParameters, typeFuncBC);
            VectorNative<llvm::Value*> fctParams;
            getLocalCallParameters(buildParameters, allocR, allocRR, allocT, fctParams, typeFuncBC, pushRAParams, {});
            builder.CreateCall(modu.getOrInsertFunction(funcBC->callName().c_str(), FT), {fctParams.begin(), fctParams.end()});
            pushRAParams.clear();
            pushRVParams.clear();
            break;
        }

        case ByteCodeOp::ForeignCall:
            SWAG_CHECK(emitForeignCall(buildParameters, allocR, allocRR, moduleToGen, ip, pushRAParams));
            pushRAParams.clear();
            pushRVParams.clear();
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
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicS8x2:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_8();
            auto r2 = MK_IMMC_8();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {builder.getInt8Ty(), builder.getInt8Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {builder.getInt8Ty(), builder.getInt8Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS16x2:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_16();
            auto r2 = MK_IMMC_16();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {builder.getInt16Ty(), builder.getInt16Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {builder.getInt16Ty(), builder.getInt16Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS32x2:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_32();
            auto r2 = MK_IMMC_32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {builder.getInt32Ty(), builder.getInt32Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {builder.getInt32Ty(), builder.getInt32Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicS64x2:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = MK_IMMB_64();
            auto r2 = MK_IMMC_64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smin, {builder.getInt64Ty(), builder.getInt64Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::smax, {builder.getInt64Ty(), builder.getInt64Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }
            break;
        }

        case ByteCodeOp::IntrinsicU8x2:
        {
            auto r0 = TO_PTR_I8(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_8();
            auto r2 = MK_IMMC_8();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {builder.getInt8Ty(), builder.getInt8Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {builder.getInt8Ty(), builder.getInt8Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicU16x2:
        {
            auto r0 = TO_PTR_I16(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_16();
            auto r2 = MK_IMMC_16();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {builder.getInt16Ty(), builder.getInt16Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {builder.getInt16Ty(), builder.getInt16Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicU32x2:
        {
            auto r0 = TO_PTR_I32(GEP_I32(allocR, ip->a.u32));
            auto r1 = MK_IMMB_32();
            auto r2 = MK_IMMC_32();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {builder.getInt32Ty(), builder.getInt32Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {builder.getInt32Ty(), builder.getInt32Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }
            break;
        }
        case ByteCodeOp::IntrinsicU64x2:
        {
            auto r0 = GEP_I32(allocR, ip->a.u32);
            auto r1 = MK_IMMB_64();
            auto r2 = MK_IMMC_64();
            switch ((TokenId) ip->d.u32)
            {
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umin, {builder.getInt64Ty(), builder.getInt64Ty()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::umax, {builder.getInt64Ty(), builder.getInt64Ty()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::minnum, {builder.getFloatTy(), builder.getFloatTy()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::maxnum, {builder.getFloatTy(), builder.getFloatTy()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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
            case TokenId::IntrinsicMin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::minnum, {builder.getDoubleTy(), builder.getDoubleTy()}, {r1, r2}), r0);
                break;
            case TokenId::IntrinsicMax:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::maxnum, {builder.getDoubleTy(), builder.getDoubleTy()}, {r1, r2}), r0);
                break;
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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
            case TokenId::IntrinsicSqrt:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicSin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, builder.getFloatTy(), r1), r0);
                break;
            case TokenId::IntrinsicCos:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, builder.getFloatTy(), r1), r0);
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
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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
            case TokenId::IntrinsicSqrt:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sqrt, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicSin:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::sin, builder.getDoubleTy(), r1), r0);
                break;
            case TokenId::IntrinsicCos:
                builder.CreateStore(builder.CreateIntrinsic(llvm::Intrinsic::cos, builder.getDoubleTy(), r1), r0);
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
            default:
                ok = false;
                moduleToGen->internalError(format("unknown intrinsic '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
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
                     param->kind == TypeInfoKind::Interface ||
                     param->kind == TypeInfoKind::TypeSet)
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
                 typeParam->kind == TypeInfoKind::Interface ||
                 typeParam->kind == TypeInfoKind::TypeSet)
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
        returnType->kind == TypeInfoKind::TypeSet ||
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
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;

    auto returnType = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    if (returnType != g_TypeMgr.typeInfoVoid)
    {
        if ((returnType->kind == TypeInfoKind::Slice) ||
            (returnType->kind == TypeInfoKind::Interface) ||
            (returnType->kind == TypeInfoKind::TypeSet) ||
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
    auto& pp              = *perThread[ct][precompileIndex];
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

bool BackendLLVM::storeLocalParam(const BuildParameters& buildParameters, llvm::Function* func, TypeInfoFuncAttr* typeFunc, int idx, llvm::Value* r0)
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
        r0 = builder.CreatePointerCast(r0, ty->getPointerTo());
        builder.CreateStore(arg, r0);
    }
    else
    {
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

    auto typeFuncBC = g_Workspace.runtimeModule->getRuntimeTypeFct(name);
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