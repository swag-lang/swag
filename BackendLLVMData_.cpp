#include "pch.h"
#include "BackendLLVM.h"
#include "DataSegment.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendLLVM::emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment)
{
    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    // Generate extern vars
    if (precompileIndex)
    {
        return true;
    }

    // Generate content
    auto  segSize = dataSegment->buckets.size();
    auto& context = *perType[ct].llvmContext[precompileIndex];
    auto  modu    = perType[ct].llvmModule[precompileIndex];

    llvm::Type*      type      = llvm::Type::getInt8Ty(context);
    llvm::ArrayType* arrayType = llvm::ArrayType::get(type, dataSegment->totalCount);

    if (dataSegment == &module->bssSegment)
    {
        llvm::ConstantAggregateZero* constArray = llvm::ConstantAggregateZero::get(arrayType);
        new llvm::GlobalVariable(*modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__bssseg");
    }
    else
    {
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
            mutableSeg = new llvm::GlobalVariable(*modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__mutableseg");
        else
            constantSeg = new llvm::GlobalVariable(*modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__constantseg");
    }

    return true;
}

bool BackendLLVM::emitInitDataSeg(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& context = *perType[ct].llvmContext[precompileIndex];
    auto& builder = *perType[ct].llvmBuilder[precompileIndex];
    auto  modu    = perType[ct].llvmModule[precompileIndex];

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
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.destOffset)};
            auto dest               = builder.CreateInBoundsGEP(mutableSeg, indices0);
            dest                    = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            llvm::Value* indices1[] = {
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.srcOffset)};
            auto src = builder.CreateInBoundsGEP(mutableSeg, indices1);
            src      = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
            builder.CreateStore(src, dest);
        }
        else
        {
            llvm::Value* indices0[] = {
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.destOffset)};
            auto dest               = builder.CreateInBoundsGEP(mutableSeg, indices0);
            dest                    = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
            llvm::Value* indices1[] = {
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.srcOffset)};
            auto src = builder.CreateInBoundsGEP(constantSeg, indices1);
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

    auto& context = *perType[ct].llvmContext[precompileIndex];
    auto& builder = *perType[ct].llvmBuilder[precompileIndex];
    auto  modu    = perType[ct].llvmModule[precompileIndex];

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, "initConstantSeg", modu);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    for (auto& k : module->constantSegment.initPtr)
    {
        SWAG_ASSERT(k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Constant);
        llvm::Value* indices0[] = {
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.destOffset)};
        auto dest               = builder.CreateInBoundsGEP(constantSeg, indices0);
        dest                    = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
        llvm::Value* indices1[] = {
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.srcOffset)};
        auto src = builder.CreateInBoundsGEP(constantSeg, indices1);
        src      = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, src, llvm::Type::getInt64Ty(context));
        builder.CreateStore(src, dest);
    }

    for (auto& k : module->constantSegment.initFuncPtr)
    {
        llvm::Value* indices0[] = {
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), k.first)};
        auto dest = builder.CreateInBoundsGEP(constantSeg, indices0);
        dest      = builder.CreatePointerCast(dest, llvm::Type::getInt64PtrTy(context));
        auto F    = modu->getOrInsertFunction(k.second->callName().c_str(), fctType);
        auto src  = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, F.getCallee(), llvm::Type::getInt64Ty(context));
        builder.CreateStore(src, dest);
    }

    builder.CreateRetVoid();
    return true;
}
