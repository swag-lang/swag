#include "pch.h"
#include "BackendLLVM.h"
#include "DataSegment.h"
#include "Module.h"

bool BackendLLVM::emitDataSegment(DataSegment* dataSegment, int preCompileIndex)
{
    if (!dataSegment->buckets.size())
        return true;

    auto segSize = dataSegment->buckets.size();

    int count = 0;
    for (int bucket = 0; bucket < segSize; bucket++)
        count += (int) dataSegment->buckets[bucket].count;
    if (!count)
        return true;

    llvm::Type*      type      = llvm::Type::getInt8Ty(llvmContext);
    llvm::ArrayType* arrayType = llvm::ArrayType::get(type, count);

    if (dataSegment == &module->bssSegment)
    {
        llvm::ConstantAggregateZero* constArray = llvm::ConstantAggregateZero::get(arrayType);
        new llvm::GlobalVariable(*llvmModule, arrayType, false, llvm::GlobalValue::InternalLinkage, constArray, "__bssseg");
    }
    else
    {
        std::vector<llvm::Constant*> values;
        values.reserve(count);
        for (int bucket = 0; bucket < segSize; bucket++)
        {
            int  bkCount = (int) dataSegment->buckets[bucket].count;
            auto pz      = dataSegment->buckets[bucket].buffer;
            while (bkCount--)
            {
                values.push_back(llvm::ConstantInt::get(llvmContext, llvm::APInt(8, *pz)));
                pz++;
            }
        }

        llvm::Constant* constArray = llvm::ConstantArray::get(arrayType, values);
        if (dataSegment == &module->mutableSegment)
            new llvm::GlobalVariable(*llvmModule, arrayType, false, llvm::GlobalValue::InternalLinkage, constArray, "__mutableseg");
        else
            new llvm::GlobalVariable(*llvmModule, arrayType, true, llvm::GlobalValue::InternalLinkage, constArray, "__constantseg");
    }

    return true;
}
