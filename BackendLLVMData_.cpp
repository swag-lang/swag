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
    auto& pp              = perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& modu            = *pp.module;

    llvm::Type*      type      = llvm::Type::getInt8Ty(context);
    llvm::ArrayType* arrayType = llvm::ArrayType::get(type, dataSegment->totalCount);

    // Generate extern vars
    if (precompileIndex)
    {
        if (dataSegment == &module->bssSegment)
            pp.bssSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__bs");
        else if (dataSegment == &module->mutableSegment)
            pp.mutableSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__ms");
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
        auto segSize = dataSegment->buckets.size();

        // Collect datas
        // Would be faster if done by 32 or 64 bits !
        std::vector<llvm::Constant*> values;
        values.reserve(dataSegment->totalCount);
        for (int bucket = 0; bucket < segSize; bucket++)
        {
            int  count = (int) dataSegment->buckets[bucket].count;
            auto pz    = dataSegment->buckets[bucket].buffer;
            while (count--)
            {
                values.push_back(llvm::ConstantInt::get(context, llvm::APInt(8, *pz)));
                pz++;
            }
        }

        // Create global variables
        llvm::Constant* constArray = llvm::ConstantArray::get(arrayType, values);
        if (dataSegment == &module->mutableSegment)
        {
            pp.mutableSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__ms");
            if (pp.dbg)
                pp.dbg->createGlobalVariablesForSegment(buildParameters, arrayType, pp.mutableSeg);
        }
        else
        {
            pp.constantSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__cs");
        }
    }

    return true;
}

bool BackendLLVM::emitInitDataSeg(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    SWAG_ASSERT(precompileIndex == 0);

    auto& pp      = perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, "initDataSeg", modu);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    for (auto& k : module->mutableSegment.initPtr)
    {
        auto kind = k.destSeg;

        if (kind == SegmentKind::Me || kind == SegmentKind::Data)
        {
            llvm::Value* indices0[] = {
                pp.cst0_i32,
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.destOffset)};
            auto dest               = builder.CreateInBoundsGEP(perThread[ct][precompileIndex].mutableSeg, indices0);
            dest                    = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            llvm::Value* indices1[] = {
                pp.cst0_i32,
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.srcOffset)};
            auto src = builder.CreateInBoundsGEP(perThread[ct][precompileIndex].mutableSeg, indices1);
            src      = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
            builder.CreateStore(src, dest);
        }
        else
        {
            llvm::Value* indices0[] = {
                pp.cst0_i32,
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.destOffset)};
            auto dest               = builder.CreateInBoundsGEP(perThread[ct][precompileIndex].mutableSeg, indices0);
            dest                    = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            llvm::Value* indices1[] = {
                pp.cst0_i32,
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.srcOffset)};
            auto src = builder.CreateInBoundsGEP(perThread[ct][precompileIndex].constantSeg, indices1);
            src      = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
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

    auto& pp      = perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, "initConstantSeg", modu);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    for (auto& k : module->constantSegment.initPtr)
    {
        SWAG_ASSERT(k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Constant);
        llvm::Value* indices0[] = {
            pp.cst0_i32,
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.destOffset)};
        auto dest               = builder.CreateInBoundsGEP(perThread[ct][precompileIndex].constantSeg, indices0);
        dest                    = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
        llvm::Value* indices1[] = {
            pp.cst0_i32,
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.srcOffset)};
        auto src = builder.CreateInBoundsGEP(perThread[ct][precompileIndex].constantSeg, indices1);
        src      = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
        builder.CreateStore(src, dest);
    }

    for (auto& k : module->constantSegment.initFuncPtr)
    {
        llvm::Value* indices0[] = {
            pp.cst0_i32,
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.first)};
        auto dest = builder.CreateInBoundsGEP(perThread[ct][precompileIndex].constantSeg, indices0);
        dest      = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
        auto F    = modu.getOrInsertFunction(k.second->callName().c_str(), fctType);
        auto src  = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, F.getCallee(), llvm::Type::getInt64Ty(context));
        builder.CreateStore(src, dest);
    }

    builder.CreateRetVoid();
    return true;
}
