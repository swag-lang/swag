#include "pch.h"
#include "BackendLLVM.h"

bool BackendLLVM::emitMain(int precompileIndex)
{
    auto& context = *llvmContext[precompileIndex];
    auto& builder = *llvmBuilder[precompileIndex];

    std::vector<llvm::Type*> params;
    params.push_back(llvm::Type::getInt32Ty(context));
    params.push_back(llvm::Type::getInt32PtrTy(context));
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), params, false);
    llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", llvmModule[precompileIndex]);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", F);
    builder.SetInsertPoint(BB);

    uint32_t value  = 0;
    auto     retVal = llvm::ConstantInt::get(context, llvm::APInt(32, value));
    builder.CreateRet(retVal);

    return true;
}
