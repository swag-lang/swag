#include "pch.h"
#include "BackendLLVM.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCode.h"

bool BackendLLVM::emitMain(const BuildParameters& buildParameters, int precompileIndex)
{
    auto& context = *llvmContext[precompileIndex];
    auto& builder = *llvmBuilder[precompileIndex];
    auto  modu    = llvmModule[precompileIndex];

    // Prototype
    vector<llvm::Type*> params;
    params.push_back(llvm::Type::getInt32Ty(context));
    params.push_back(llvm::Type::getInt32PtrTy(context));
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), params, false);
    llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", modu);

    // Start of block
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", F);
    builder.SetInsertPoint(BB);

    // Generate call to test functions
    auto fccType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    if (buildParameters.flags & BUILDPARAM_FOR_TEST)
    {
        if (!module->byteCodeTestFunc.empty())
        {
            for (auto bc : module->byteCodeTestFunc)
            {
                auto node = bc->node;
                if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                    continue;

                auto fcc = modu->getOrInsertFunction(bc->callName().c_str(), fccType);
                builder.CreateCall(fcc);
            }
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        auto fcc = modu->getOrInsertFunction(module->byteCodeMainFunc->callName().c_str(), fccType);
        builder.CreateCall(fcc);
    }

    // Call to global drop of this module
    auto fcc = modu->getOrInsertFunction(format("%s_globalDrop", module->nameDown.c_str()).c_str(), fccType);
    builder.CreateCall(fcc);

    uint32_t value  = 0;
    auto     retVal = llvm::ConstantInt::get(context, llvm::APInt(32, value));
    builder.CreateRet(retVal);

    return true;
}

bool BackendLLVM::emitGlobalDrop(const BuildParameters& buildParameters, int precompileIndex)
{
    auto& context = *llvmContext[precompileIndex];
    auto& builder = *llvmBuilder[precompileIndex];
    auto  modu    = llvmModule[precompileIndex];

    auto              fccType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function*   F       = llvm::Function::Create(fccType, llvm::Function::ExternalLinkage, format("%s_globalDrop", module->nameDown.c_str()).c_str(), modu);
    llvm::BasicBlock* BB      = llvm::BasicBlock::Create(context, "entry", F);
    builder.SetInsertPoint(BB);

    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        auto fcc = modu->getOrInsertFunction(bc->callName().c_str(), fccType);
        builder.CreateCall(fcc);
    }

    builder.CreateRetVoid();
    return true;
}