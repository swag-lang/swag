#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "Job.h"
#include "Module.h"
#include "Workspace.h"
#include "OS.h"
#include "ByteCode.h"
#include "DataSegment.h"

BackendFunctionBodyJob* BackendLLVM::newFunctionJob()
{
    return g_Pool_backendLLVMFunctionBodyJob.alloc();
}

bool BackendLLVM::emitFunctionBody(Module* moduleToGen, ByteCode* bc, int precompileIndex)
{
    auto& context = *llvmContext[precompileIndex];
    auto& builder = *llvmBuilder[precompileIndex];

    //std::vector<llvm::Type*> params;
    llvm::FunctionType*      FT = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function*          F  = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, bc->callName().c_str(), llvmModule[precompileIndex]);
    //F->setDLLStorageClass(llvm::GlobalValue::DLLExportStorageClass);

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(context, "entry", F);
    builder.SetInsertPoint(BB);
    builder.CreateRetVoid();

    return true;
}