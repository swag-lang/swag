#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"

struct BuildParameters;
struct ByteCode;
struct ByteCodeInstruction;
struct DataSegment;
struct Job;
struct LLVMDebug;
struct Module;
struct TypeInfo;
union Register;
enum class SegmentKind;
enum class SafetyMsg;

struct LLVM_Encoder : BackendEncoder
{
    void init(const BuildParameters& buildParameters);

    llvm::IRBuilder<>* builder     = nullptr;
    llvm::LLVMContext* llvmContext = nullptr;
    llvm::Module*      llvmModule  = nullptr;
    llvm::Function*    llvmFunc    = nullptr;

    llvm::AllocaInst* allocR      = nullptr;
    llvm::AllocaInst* allocRR     = nullptr;
    llvm::AllocaInst* allocResult = nullptr;
    llvm::AllocaInst* allocVa     = nullptr;
    llvm::AllocaInst* allocStack  = nullptr;
    llvm::AllocaInst* allocT      = nullptr;

    llvm::GlobalVariable* bssSeg              = nullptr;
    llvm::GlobalVariable* mutableSeg          = nullptr;
    llvm::GlobalVariable* constantSeg         = nullptr;
    llvm::GlobalVariable* tlsSeg              = nullptr;
    llvm::GlobalVariable* mainContext         = nullptr;
    llvm::GlobalVariable* defaultAllocTable   = nullptr;
    llvm::GlobalVariable* processInfos        = nullptr;
    llvm::GlobalVariable* symTlsThreadLocalId = nullptr;

    llvm::Type*         errorTy        = nullptr;
    llvm::Type*         scratchTy      = nullptr;
    llvm::Type*         interfaceTy    = nullptr;
    llvm::Type*         contextTy      = nullptr;
    llvm::Type*         sliceTy        = nullptr;
    llvm::Type*         processInfosTy = nullptr;
    llvm::FunctionType* allocatorTy    = nullptr;
    llvm::FunctionType* bytecodeRunTy  = nullptr;
    llvm::FunctionType* makeCallbackTy = nullptr;

    llvm::FunctionCallee fnAcosF32;
    llvm::FunctionCallee fnAcosF64;
    llvm::FunctionCallee fnAsinF32;
    llvm::FunctionCallee fnAsinF64;
    llvm::FunctionCallee fnTanF32;
    llvm::FunctionCallee fnTanF64;
    llvm::FunctionCallee fnAtanF32;
    llvm::FunctionCallee fnAtanF64;
    llvm::FunctionCallee fnSinhF32;
    llvm::FunctionCallee fnSinhF64;
    llvm::FunctionCallee fnCoshF32;
    llvm::FunctionCallee fnCoshF64;
    llvm::FunctionCallee fnTanhF32;
    llvm::FunctionCallee fnTanhF64;
    llvm::FunctionCallee fnPowF32;
    llvm::FunctionCallee fnPowF64;
    llvm::FunctionCallee fnAtan2F32;
    llvm::FunctionCallee fnAtan2F64;
    llvm::FunctionCallee fnMemCmp;
    llvm::FunctionCallee fnStrlen;
    llvm::FunctionCallee fnStrcmp;
    llvm::FunctionCallee fnMalloc;
    llvm::FunctionCallee fnReAlloc;
    llvm::FunctionCallee fnFree;

    Map<int64_t, llvm::BasicBlock*>             labels;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternal;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternalClosure;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeign;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeignClosure;

    // Debug infos
    LLVMDebug* dbg = nullptr;
};
