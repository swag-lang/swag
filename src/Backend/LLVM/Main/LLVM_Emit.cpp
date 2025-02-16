#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/LLVM/Main/LLVM.h"
#include "Backend/LLVM/Main/LLVM_Macros.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/SourceFile.h"

void LLVM::emitShiftRightArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        const auto r0 = MK_IMMA_IX(numBits);
        const auto r1 = builder.getIntN(numBits, min(ip->b.u32, numBits - 1));
        const auto r2 = builder.CreateAShr(r0, r1);
        const auto r3 = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        builder.CreateStore(r2, r3);
    }
    else
    {
        const auto r0      = MK_IMMA_IX(numBits);
        const auto r1      = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));
        const auto r2      = builder.CreateICmpULT(r1, builder.getInt32(numBits));
        const auto ifTrue  = r1;
        const auto ifFalse = builder.getInt32(numBits - 1);
        const auto r3      = builder.CreateSelect(r2, ifTrue, ifFalse);
        const auto r4      = builder.CreateIntCast(r3, IX_TY(numBits), false);
        const auto r5      = builder.CreateAShr(r0, r4);
        const auto r6      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        builder.CreateStore(r5, r6);
    }
}

void LLVM::emitShiftRightEqArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        const auto r0 = builder.getIntN(numBits, min(ip->b.u32, numBits - 1));
        const auto r1 = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto r2 = builder.CreateLoad(IX_TY(numBits), r1);
        const auto r3 = builder.CreateAShr(r2, r0);
        builder.CreateStore(r3, r1);
    }
    else
    {
        const auto r0      = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));
        const auto r1      = builder.CreateICmpULT(r0, builder.getInt32(numBits));
        const auto ifTrue  = r0;
        const auto ifFalse = builder.getInt32(numBits - 1);
        const auto r2      = builder.CreateSelect(r1, ifTrue, ifFalse);
        const auto r3      = builder.CreateIntCast(r2, IX_TY(numBits), false);
        const auto r4      = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto r5      = builder.CreateLoad(IX_TY(numBits), r4);
        const auto r6      = builder.CreateAShr(r5, r3);
        builder.CreateStore(r6, r4);
    }
}

void LLVM::emitShiftLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits, bool left)
{
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= numBits)
    {
        const auto r0 = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        const auto r1 = llvm::ConstantInt::get(IX_TY(numBits), 0);
        builder.CreateStore(r1, r0);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        const auto r0 = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        const auto r1 = MK_IMMA_IX(numBits);
        const auto r2 = builder.getIntN(numBits, ip->b.u8);
        const auto r3 = left ? builder.CreateShl(r1, r2) : builder.CreateLShr(r1, r2);
        builder.CreateStore(r3, r0);
    }
    else
    {
        const auto r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        const auto r1      = MK_IMMA_IX(numBits);
        const auto r2      = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));
        const auto r3      = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        const auto r4      = builder.CreateIntCast(r2, IX_TY(numBits), false);
        const auto ifTrue  = left ? builder.CreateShl(r1, r4) : builder.CreateLShr(r1, r4);
        const auto ifFalse = llvm::ConstantInt::get(IX_TY(numBits), 0);
        const auto r5      = builder.CreateSelect(r3, ifTrue, ifFalse);
        builder.CreateStore(r5, r0);
    }
}

void LLVM::emitShiftEqLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits, bool left)
{
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= numBits)
    {
        const auto r0 = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto r1 = llvm::ConstantInt::get(IX_TY(numBits), 0);
        builder.CreateStore(r1, r0);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        const auto r0 = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto r1 = builder.getIntN(numBits, ip->b.u8);
        const auto r2 = builder.CreateLoad(IX_TY(numBits), r0);
        const auto r3 = left ? builder.CreateShl(r2, r1) : builder.CreateLShr(r2, r1);
        builder.CreateStore(r3, r0);
    }
    else
    {
        const auto r0      = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto r1      = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u8));
        const auto r2      = builder.CreateICmpULT(r1, builder.getInt32(numBits));
        const auto r3      = builder.CreateLoad(IX_TY(numBits), r0);
        const auto r4      = builder.CreateIntCast(r1, IX_TY(numBits), false);
        const auto ifTrue  = left ? builder.CreateShl(r3, r4) : builder.CreateLShr(r3, r4);
        const auto ifFalse = llvm::ConstantInt::get(IX_TY(numBits), 0);
        const auto r5      = builder.CreateSelect(r2, ifTrue, ifFalse);
        builder.CreateStore(r5, r0);
    }
}

void LLVM::emitInternalPanic(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const AstNode* node, const char* message)
{
    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;
    auto&      pp              = encoder<LLVMEncoder>(ct, precompileIndex);
    auto&      context         = *pp.llvmContext;
    auto&      builder         = *pp.builder;

    // Filename
    const auto r1 = builder.CreateGlobalString(node->token.sourceFile->path.cstr());
    const auto r2 = builder.CreateInBoundsGEP(I8_TY(), r1, {pp.cstAi32});

    // Line & column
    const auto r3 = builder.getInt32(node->token.startLocation.line);
    const auto r4 = builder.getInt32(node->token.startLocation.column);

    // Message
    llvm::Value* r5;
    if (message)
    {
        r5 = builder.CreateGlobalString(message);
        r5 = builder.CreateInBoundsGEP(I8_TY(), r5, {pp.cstAi32});
    }
    else
        r5 = builder.CreateIntToPtr(pp.cstAi64, PTR_I8_TY());

    emitCall(buildParameters, g_LangSpec->name_priv_panic, allocR, allocT, {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX}, {r2, r3, r4, r5});
}

void LLVM::storeTypedValueToRegister(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Value* value, uint32_t reg, llvm::AllocaInst* allocR) const
{
    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = encoder<LLVMEncoder>(ct, precompileIndex);
    auto&       builder         = *pp.builder;

    SWAG_ASSERT(value);
    const auto r1 = value;

    if (value->getType()->isPointerTy())
        builder.CreateStore(builder.CreatePtrToInt(r1, I64_TY()), GEP64(allocR, reg));
    else if (value->getType()->isIntegerTy(8))
        builder.CreateStore(r1, GEP64_PTR_I8(allocR, reg));
    else if (value->getType()->isIntegerTy(16))
        builder.CreateStore(r1, GEP64_PTR_I16(allocR, reg));
    else if (value->getType()->isIntegerTy(32))
        builder.CreateStore(r1, GEP64_PTR_I32(allocR, reg));
    else if (value->getType()->isFloatTy())
        builder.CreateStore(r1, GEP64_PTR_F32(allocR, reg));
    else if (value->getType()->isDoubleTy())
        builder.CreateStore(r1, GEP64_PTR_F64(allocR, reg));
    else
        builder.CreateStore(r1, GEP64(allocR, reg));
}

void LLVM::emitRT2ToRegisters(llvm::LLVMContext& context,
                              llvm::IRBuilder<>& builder,
                              uint32_t           reg0,
                              uint32_t           reg1,
                              llvm::AllocaInst*  allocR,
                              llvm::AllocaInst*  allocRR)
{
    const auto r0 = GEP64(allocR, reg0);
    const auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocRR, 0));
    builder.CreateStore(r1, r0);
    const auto r2 = GEP64(allocR, reg1);
    const auto r3 = builder.CreateLoad(I64_TY(), GEP64(allocRR, 1));
    builder.CreateStore(r3, r2);
}

void LLVM::emitBinOpOverflow(const BuildParameters&                 buildParameters,
                             llvm::Function*                        func,
                             llvm::AllocaInst*                      allocR,
                             llvm::AllocaInst*                      allocT,
                             const ByteCodeInstruction*             ip,
                             llvm::Value*                           r0,
                             llvm::Value*                           r1,
                             llvm::Value*                           r2,
                             llvm::Intrinsic::IndependentIntrinsics op,
                             SafetyMsg                              msg)
{
    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = encoder<LLVMEncoder>(ct, precompileIndex);
    auto&       context         = *pp.llvmContext;
    auto&       builder         = *pp.builder;

    const auto r3       = builder.CreateBinaryIntrinsic(op, r1, r2);
    const auto r4       = builder.CreateExtractValue(r3, {0});
    const auto r5       = builder.CreateExtractValue(r3, {1});
    const auto blockOk  = llvm::BasicBlock::Create(context, "", func);
    const auto blockErr = llvm::BasicBlock::Create(context, "", func);
    const auto r6       = builder.CreateIsNull(r5);

    builder.CreateCondBr(r6, blockOk, blockErr);
    builder.SetInsertPoint(blockErr);
    emitInternalPanic(buildParameters, allocR, allocT, ip->node, ByteCodeGen::safetyMsg(msg, BackendEncoder::getOpType(ip->op)));
    builder.CreateBr(blockOk);
    builder.SetInsertPoint(blockOk);
    builder.CreateStore(r4, r0);
}

void LLVM::emitBinOpEqOverflow(const BuildParameters&                 buildParameters,
                               llvm::Function*                        func,
                               llvm::AllocaInst*                      allocR,
                               llvm::AllocaInst*                      allocT,
                               const ByteCodeInstruction*             ip,
                               llvm::Value*                           r0,
                               llvm::Value*                           r1,
                               llvm::Value*                           r2,
                               llvm::Intrinsic::IndependentIntrinsics op,
                               SafetyMsg                              msg)
{
    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = encoder<LLVMEncoder>(ct, precompileIndex);
    auto&       context         = *pp.llvmContext;
    auto&       builder         = *pp.builder;
    const auto  numBits         = BackendEncoder::getNumBits(ip->op);

    const auto r3       = builder.CreateBinaryIntrinsic(op, builder.CreateLoad(IX_TY(numBits), r1), r2);
    const auto r4       = builder.CreateExtractValue(r3, {0});
    const auto r5       = builder.CreateExtractValue(r3, {1});
    const auto blockOk  = llvm::BasicBlock::Create(context, "", func);
    const auto blockErr = llvm::BasicBlock::Create(context, "", func);
    const auto r6       = builder.CreateIsNull(r5);

    builder.CreateCondBr(r6, blockOk, blockErr);
    builder.SetInsertPoint(blockErr);
    emitInternalPanic(buildParameters, allocR, allocT, ip->node, ByteCodeGen::safetyMsg(msg, BackendEncoder::getOpType(ip->op)));
    builder.CreateBr(blockOk);
    builder.SetInsertPoint(blockOk);
    builder.CreateStore(r4, r1);
}
