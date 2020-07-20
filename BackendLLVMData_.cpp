#include "pch.h"
#include "BackendLLVM.h"
#include "DataSegment.h"
#include "Module.h"

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
    auto& context = *llvmContext[ct][precompileIndex];
    auto  modu    = llvmModule[ct][precompileIndex];

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
            new llvm::GlobalVariable(*modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__mutableseg");
        else
            new llvm::GlobalVariable(*modu, arrayType, true, llvm::GlobalValue::ExternalLinkage, constArray, "__constantseg");
    }

    return true;
}

bool BackendLLVM::emitInitDataSeg(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& context = *llvmContext[ct][precompileIndex];
    auto& builder = *llvmBuilder[ct][precompileIndex];
    auto  modu    = llvmModule[ct][precompileIndex];

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, "emitDataSeg", modu);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    builder.CreateRetVoid();
    return true;
}

bool BackendLLVM::emitInitConstantSeg(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& context = *llvmContext[ct][precompileIndex];
    auto& builder = *llvmBuilder[ct][precompileIndex];
    auto  modu    = llvmModule[ct][precompileIndex];

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, "emitConstantSeg", modu);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    builder.CreateRetVoid();
    return true;
}
