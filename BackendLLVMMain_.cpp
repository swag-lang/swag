#include "pch.h"
#include "BackendLLVM.h"

bool BackendLLVM::emitMain()
{  
    std::vector<llvm::Type*> params;
    params.push_back(llvm::Type::getInt32Ty(llvmContext));
    params.push_back(llvm::Type::getInt32PtrTy(llvmContext));
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvmContext), params, false);
    llvm::Function*     F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", llvmModule);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(llvmContext, "entry", F);
    llvmBuilder.SetInsertPoint(BB);

    uint32_t value = 0;
    auto retVal = llvm::ConstantInt::get(llvmContext, llvm::APInt(32, value));
    llvmBuilder.CreateRet(retVal);

    return true;
}
