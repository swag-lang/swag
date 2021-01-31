#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMDbg.h"
#include "DataSegment.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendLLVM::emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment)
{
    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& modu            = *pp.module;

    llvm::Type* type       = llvm::Type::getInt64Ty(context);
    auto        totalCount = dataSegment->totalCount / 8;
    if (totalCount * 8 != dataSegment->totalCount)
        totalCount++;
    llvm::ArrayType* arrayType = llvm::ArrayType::get(type, totalCount);

    // Generate extern vars
    if (precompileIndex)
    {
        if (dataSegment == &module->bssSegment)
            pp.bssSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__bs");
        else if (dataSegment == &module->mutableSegment)
            pp.mutableSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__ms");
        else if (dataSegment == &module->typeSegment)
            pp.typeSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__ts");
        else
            pp.constantSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__cs");
        return true;
    }

    // Generate content
    if (dataSegment == &module->bssSegment)
    {
        llvm::ConstantAggregateZero* constArray = llvm::ConstantAggregateZero::get(arrayType);
        pp.bssSeg                               = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__bs");
        if (pp.dbg)
            pp.dbg->createGlobalVariablesForSegment(buildParameters, arrayType, pp.bssSeg);
    }
    else
    {
        dataSegment->applyPatchPtr();

        // Collect datas
        VectorNative<llvm::Constant*> values;
        values.reserve(totalCount);

        uint64_t          value = 0;
        DataSegment::Seek seek;
        while (dataSegment->readU64(seek, value))
        {
            values.push_back(llvm::ConstantInt::get(context, llvm::APInt(64, value)));
        }

        SWAG_ASSERT(totalCount == values.size());

        // Create global variables
        llvm::Constant* constArray = llvm::ConstantArray::get(arrayType, {values.begin(), values.end()});
        if (dataSegment == &module->mutableSegment)
        {
            pp.mutableSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__ms");
            if (pp.dbg)
                pp.dbg->createGlobalVariablesForSegment(buildParameters, arrayType, pp.mutableSeg);
        }
        else if (dataSegment == &module->typeSegment)
        {
            pp.typeSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__ts");
        }
        else
        {
            pp.constantSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__cs");
            if (pp.dbg)
                pp.dbg->createGlobalVariablesForSegment(buildParameters, arrayType, pp.constantSeg);
        }
    }

    return true;
}

bool BackendLLVM::emitInitMutableSeg(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    SWAG_ASSERT(precompileIndex == 0);

    auto& pp      = *perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, "initMutableSeg", modu);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    auto ms = pp.mutableSeg;
    auto cs = pp.constantSeg;
    auto ts = pp.typeSeg;
    for (auto& k : module->mutableSegment.initPtr)
    {
        if (k.fromSegment == SegmentKind::Constant)
        {
            auto dest = builder.CreateInBoundsGEP(TO_PTR_I8(ms), builder.getInt64(k.patchOffset));
            dest      = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            auto src  = builder.CreateInBoundsGEP(TO_PTR_I8(cs), builder.getInt64(k.srcOffset));
            src       = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
            builder.CreateStore(src, dest);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Type);
            auto dest = builder.CreateInBoundsGEP(TO_PTR_I8(ms), builder.getInt64(k.patchOffset));
            dest      = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            auto src  = builder.CreateInBoundsGEP(TO_PTR_I8(ts), builder.getInt64(k.srcOffset));
            src       = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
            builder.CreateStore(src, dest);
        }
    }

    builder.CreateRetVoid();
    return true;
}

bool BackendLLVM::emitInitTypeSeg(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    SWAG_ASSERT(precompileIndex == 0);

    auto& pp      = *perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, "initTypeSeg", modu);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    auto ts = pp.typeSeg;
    auto cs = pp.constantSeg;
    for (auto& k : module->typeSegment.initPtr)
    {
        if (k.fromSegment == SegmentKind::Me)
        {
            auto dest = builder.CreateInBoundsGEP(TO_PTR_I8(ts), builder.getInt64(k.patchOffset));
            dest      = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            auto src  = builder.CreateInBoundsGEP(TO_PTR_I8(ts), builder.getInt64(k.srcOffset));
            src       = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
            builder.CreateStore(src, dest);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Constant);
            auto dest = builder.CreateInBoundsGEP(TO_PTR_I8(ts), builder.getInt64(k.patchOffset));
            dest      = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            auto src  = builder.CreateInBoundsGEP(TO_PTR_I8(cs), builder.getInt64(k.srcOffset));
            src       = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
            builder.CreateStore(src, dest);
        }
    }

    builder.CreateRetVoid();
    return true;
}

bool BackendLLVM::emitInitConstantSeg(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    SWAG_ASSERT(precompileIndex == 0);

    auto& pp      = *perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, "initConstantSeg", modu);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    auto cs = pp.constantSeg;
    auto ts = pp.typeSeg;
    for (auto& k : module->constantSegment.initPtr)
    {
        if (k.fromSegment == SegmentKind::Me || k.fromSegment == SegmentKind::Constant)
        {
            auto dest = builder.CreateInBoundsGEP(TO_PTR_I8(cs), builder.getInt64(k.patchOffset));
            dest      = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            auto src  = builder.CreateInBoundsGEP(TO_PTR_I8(cs), builder.getInt64(k.srcOffset));
            src       = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
            builder.CreateStore(src, dest);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Type);
            auto dest = builder.CreateInBoundsGEP(TO_PTR_I8(cs), builder.getInt64(k.patchOffset));
            dest      = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            auto src  = builder.CreateInBoundsGEP(TO_PTR_I8(ts), builder.getInt64(k.srcOffset));
            src       = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
            builder.CreateStore(src, dest);
        }
    }

    for (auto& k : module->constantSegment.initFuncPtr)
    {
        auto dest      = builder.CreateInBoundsGEP(TO_PTR_I8(cs), builder.getInt64(k.first));
        dest           = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
        auto relocType = k.second.second;
        auto F         = modu.getOrInsertFunction(k.second.first.c_str(), fctType);
        auto src       = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, F.getCallee(), llvm::Type::getInt64Ty(context));
        if (relocType == DataSegment::RelocType::Foreign)
            src = builder.CreateOr(src, builder.getInt64(SWAG_LAMBDA_FOREIGN_MARKER));
        builder.CreateStore(src, dest);
    }

    builder.CreateRetVoid();
    return true;
}
