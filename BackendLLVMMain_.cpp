#include "pch.h"
#include "BackendLLVM.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "Context.h"

bool BackendLLVM::emitMain(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& builder = *pp.builder;
    auto& modu    = *pp.module;

    // Prototype
    vector<llvm::Type*> params;
    params.push_back(llvm::Type::getInt32Ty(context));
    params.push_back(llvm::Type::getInt8PtrTy(context)->getPointerTo());
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), params, false);
    llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", modu);

    // Start of block
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", F);
    builder.SetInsertPoint(BB);

    // Main context
    SWAG_ASSERT(g_defaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) undoByteCodeLambda(((void**) g_defaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    auto allocFct = modu.getOrInsertFunction(bcAlloc->callName().c_str(), pp.allocatorTy);
    builder.CreateStore(allocFct.getCallee(), pp.defaultAllocTable);

    // Arguments

    // Call to global init of this module, and dependencies
    auto funcTypeVoid = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    auto funcInit     = modu.getOrInsertFunction(format("%s_globalInit", module->nameDown.c_str()).c_str(), funcTypeVoid);
    builder.CreateCall(funcInit);

    // Call to test functions
    if (buildParameters.compileType == BackendCompileType::Test)
    {
        if (!module->byteCodeTestFunc.empty())
        {
            for (auto bc : module->byteCodeTestFunc)
            {
                auto node = bc->node;
                if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                    continue;

                auto fcc = modu.getOrInsertFunction(bc->callName().c_str(), funcTypeVoid);
                builder.CreateCall(fcc);
            }
        }
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        auto fncMain = modu.getOrInsertFunction(module->byteCodeMainFunc->callName().c_str(), funcTypeVoid);
        builder.CreateCall(fncMain);
    }

    // Call to global drop of this module
    auto funcDrop = modu.getOrInsertFunction(format("%s_globalDrop", module->nameDown.c_str()).c_str(), funcTypeVoid);
    builder.CreateCall(funcDrop);

    // Call to global drop of all dependencies

    uint32_t value  = 0;
    auto     retVal = llvm::ConstantInt::get(context, llvm::APInt(32, value));
    builder.CreateRet(retVal);

    return true;
}

bool BackendLLVM::emitGlobalInit(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& context = *perThread[ct][precompileIndex].context;
    auto& builder = *perThread[ct][precompileIndex].builder;
    auto  modu    = perThread[ct][precompileIndex].module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, format("%s_globalInit", module->nameDown.c_str()).c_str(), modu);
    fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    // Initialize data segments
    builder.CreateCall(modu->getFunction("initDataSeg"));
    builder.CreateCall(modu->getFunction("initConstantSeg"));

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        auto func = modu->getOrInsertFunction(bc->callName().c_str(), fctType);
        builder.CreateCall(func);
    }

    builder.CreateRetVoid();
    return true;
}

bool BackendLLVM::emitGlobalDrop(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& context = *perThread[ct][precompileIndex].context;
    auto& builder = *perThread[ct][precompileIndex].builder;
    auto  modu    = perThread[ct][precompileIndex].module;

    auto            fctType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* fct     = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, format("%s_globalDrop", module->nameDown.c_str()).c_str(), modu);
    fct->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", fct);
    builder.SetInsertPoint(BB);

    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        auto func = modu->getOrInsertFunction(bc->callName().c_str(), fctType);
        builder.CreateCall(func);
    }

    builder.CreateRetVoid();
    return true;
}