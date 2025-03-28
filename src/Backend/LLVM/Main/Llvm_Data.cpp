#include "pch.h"
#include "Backend/LLVM/Debug/LlvmDebug.h"
#include "Backend/LLVM/Main/Llvm.h"
#include "Backend/LLVM/Main/Llvm_Macros.h"
#include "Wmf/Module.h"

bool Llvm::emitDataSegment(LlvmEncoder& pp, DataSegment* dataSegment)
{
    if (dataSegment->buckets.empty())
        return true;
    if (!dataSegment->totalCount)
        return true;

    const auto& buildParameters = pp.buildParams;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto  module          = pp.module;
    auto&       context         = *pp.llvmContext;
    auto&       modu            = *pp.llvmModule;

    llvm::Type* type       = I64_TY();
    auto        totalCount = dataSegment->totalCount / 8;
    if (totalCount * 8 != dataSegment->totalCount)
        totalCount++;
    llvm::ArrayType* arrayType = llvm::ArrayType::get(type, totalCount);

    // Generate extern vars
    if (precompileIndex)
    {
        if (dataSegment == &module->bssSegment)
        {
            pp.bssSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__bs");
            pp.bssSeg->setAlignment(llvm::Align(16));
        }
        else if (dataSegment == &module->mutableSegment)
        {
            pp.mutableSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__ms");
            pp.mutableSeg->setAlignment(llvm::Align(16));
        }
        else if (dataSegment == &module->tlsSegment)
        {
            pp.tlsSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "__tls");
            pp.tlsSeg->setAlignment(llvm::Align(16));
        }
        else
        {
            pp.constantSeg = new llvm::GlobalVariable(modu, arrayType, true, llvm::GlobalValue::ExternalLinkage, nullptr, "__cs");
            pp.constantSeg->setAlignment(llvm::Align(16));
        }

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
            pp.mutableSeg->setAlignment(llvm::Align(16));
            if (pp.dbg)
                pp.dbg->createGlobalVariablesForSegment(buildParameters, arrayType, pp.mutableSeg);
        }
        else if (dataSegment == &module->tlsSegment)
        {
            pp.tlsSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__tls");
            pp.tlsSeg->setAlignment(llvm::Align(16));
            if (pp.dbg)
                pp.dbg->createGlobalVariablesForSegment(buildParameters, arrayType, pp.tlsSeg);
        }
        else
        {
            pp.constantSeg = new llvm::GlobalVariable(modu, arrayType, false, llvm::GlobalValue::ExternalLinkage, constArray, "__cs");
            pp.constantSeg->setAlignment(llvm::Align(16));
            if (pp.dbg)
                pp.dbg->createGlobalVariablesForSegment(buildParameters, arrayType, pp.constantSeg);
        }
    }

    return true;
}

bool Llvm::emitInitSeg(const LlvmEncoder& pp, DataSegment* dataSegment, SegmentKind me)
{
    auto& context = *pp.llvmContext;
    auto& builder = *pp.builder;
    auto& modu    = *pp.llvmModule;

    const auto            fctType = llvm::FunctionType::get(VOID_TY(), false);
    const char*           name    = nullptr;
    llvm::GlobalVariable* gVar    = nullptr;
    switch (me)
    {
        case SegmentKind::Data:
            name = "__initMutableSeg";
            gVar = pp.mutableSeg;
            break;
        case SegmentKind::Constant:
            name = "__initConstantSeg";
            gVar = pp.constantSeg;
            break;
        case SegmentKind::Tls:
            name = "__initTlsSeg";
            gVar = pp.tlsSeg;
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    llvm::Function*   fct   = llvm::Function::Create(fctType, llvm::Function::InternalLinkage, name, modu);
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(block);

    for (const auto& k : dataSegment->initPtr)
    {
        auto fromSegment = k.fromSegment;
        if (fromSegment == SegmentKind::Me)
            fromSegment = me;

        const auto dest = GEP8_PTR_I64(gVar, k.patchOffset);
        if (fromSegment == SegmentKind::Constant)
        {
            const auto src = GEP8_PTR_I64(pp.constantSeg, k.fromOffset);
            builder.CreateStore(src, dest);
        }
        else if (fromSegment == SegmentKind::Bss)
        {
            const auto src = GEP8_PTR_I64(pp.bssSeg, k.fromOffset);
            builder.CreateStore(src, dest);
        }
        else if (fromSegment == SegmentKind::Data)
        {
            const auto src = GEP8_PTR_I64(pp.mutableSeg, k.fromOffset);
            builder.CreateStore(src, dest);
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }

    for (auto& k : dataSegment->initFuncPtr)
    {
        const auto dest = GEP8_PTR_I64(gVar, k.first);
        auto       func = modu.getOrInsertFunction(k.second.cstr(), fctType);
        const auto src  = builder.CreateCast(llvm::Instruction::CastOps::PtrToInt, func.getCallee(), I64_TY());
        builder.CreateStore(src, dest);
    }

    builder.CreateRetVoid();
    return true;
}
