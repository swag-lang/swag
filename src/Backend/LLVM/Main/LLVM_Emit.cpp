#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/LLVM/Main/LLVM.h"
#include "Backend/LLVM/Main/LLVM_Macros.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/SourceFile.h"

void LLVM::emitShiftRightArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        llvm::Value* r1 = getImmediateConstantA(context, builder, allocR, ip, numBits);
        const auto   r2 = builder.getIntN(numBits, min(ip->b.u32, numBits - 1));
        const auto   v0 = builder.CreateAShr(r1, r2);
        const auto   r0 = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        builder.CreateStore(v0, r0);
    }
    else
    {
        llvm::Value* r1      = getImmediateConstantA(context, builder, allocR, ip, numBits);
        const auto   v0      = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));
        const auto   cond    = builder.CreateICmpULT(v0, builder.getInt32(numBits));
        const auto   iftrue  = v0;
        const auto   iffalse = builder.getInt32(numBits - 1);
        auto         r2      = builder.CreateSelect(cond, iftrue, iffalse);
        r2                   = builder.CreateIntCast(r2, IX_TY(numBits), false);
        const auto v1        = builder.CreateAShr(r1, r2);
        const auto r0        = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        builder.CreateStore(v1, r0);
    }
}

void LLVM::emitShiftRightEqArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        const auto r2 = builder.getIntN(numBits, min(ip->b.u32, numBits - 1));
        const auto r0 = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto v1 = builder.CreateLoad(IX_TY(numBits), r0);
        const auto v0 = builder.CreateAShr(v1, r2);
        builder.CreateStore(v0, r0);
    }
    else
    {
        const auto v0      = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));
        const auto cond    = builder.CreateICmpULT(v0, builder.getInt32(numBits));
        const auto iftrue  = v0;
        const auto iffalse = builder.getInt32(numBits - 1);
        auto       r2      = builder.CreateSelect(cond, iftrue, iffalse);
        r2                 = builder.CreateIntCast(r2, IX_TY(numBits), false);
        const auto r0      = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto v1      = builder.CreateLoad(IX_TY(numBits), r0);
        const auto v2      = builder.CreateAShr(v1, r2);
        builder.CreateStore(v2, r0);
    }
}

void LLVM::emitShiftLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits, bool left)
{
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= numBits)
    {
        const auto r0 = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        const auto v0 = llvm::ConstantInt::get(IX_TY(numBits), 0);
        builder.CreateStore(v0, r0);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        const auto   r0 = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        llvm::Value* r1 = getImmediateConstantA(context, builder, allocR, ip, numBits);
        const auto   r2 = builder.getIntN(numBits, ip->b.u8);
        const auto   v0 = left ? builder.CreateShl(r1, r2) : builder.CreateLShr(r1, r2);
        builder.CreateStore(v0, r0);
    }
    else
    {
        const auto   r0      = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
        llvm::Value* r1      = getImmediateConstantA(context, builder, allocR, ip, numBits);
        const auto   r2      = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));
        const auto   cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        const auto   l1      = builder.CreateIntCast(r2, IX_TY(numBits), false);
        const auto   iftrue  = left ? builder.CreateShl(r1, l1) : builder.CreateLShr(r1, l1);
        const auto   iffalse = llvm::ConstantInt::get(IX_TY(numBits), 0);
        const auto   v0      = builder.CreateSelect(cond, iftrue, iffalse);
        builder.CreateStore(v0, r0);
    }
}

void LLVM::emitShiftEqLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits, bool left)
{
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= numBits)
    {
        const auto r0 = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto v0 = llvm::ConstantInt::get(IX_TY(numBits), 0);
        builder.CreateStore(v0, r0);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        const auto r0 = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto r2 = builder.getIntN(numBits, ip->b.u8);
        const auto v1 = builder.CreateLoad(IX_TY(numBits), r0);
        const auto v0 = left ? builder.CreateShl(v1, r2) : builder.CreateLShr(v1, r2);
        builder.CreateStore(v0, r0);
    }
    else
    {
        const auto r0      = builder.CreateLoad(PTR_IX_TY(numBits), GEP64(allocR, ip->a.u32));
        const auto r2      = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u8));
        const auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        const auto l0      = builder.CreateLoad(IX_TY(numBits), r0);
        const auto l1      = builder.CreateIntCast(r2, IX_TY(numBits), false);
        const auto iftrue  = left ? builder.CreateShl(l0, l1) : builder.CreateLShr(l0, l1);
        const auto iffalse = llvm::ConstantInt::get(IX_TY(numBits), 0);
        const auto v0      = builder.CreateSelect(cond, iftrue, iffalse);
        builder.CreateStore(v0, r0);
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
    llvm::Value* r1 = builder.CreateGlobalString(node->token.sourceFile->path.cstr());
    r1              = builder.CreateInBoundsGEP(I8_TY(), r1, {pp.cstAi32});

    // Line & column
    auto r2 = builder.getInt32(node->token.startLocation.line);
    auto r3 = builder.getInt32(node->token.startLocation.column);

    // Message
    llvm::Value* r4;
    if (message)
    {
        r4 = builder.CreateGlobalString(message);
        r4 = builder.CreateInBoundsGEP(I8_TY(), r4, {pp.cstAi32});
    }
    else
        r4 = builder.CreateIntToPtr(pp.cstAi64, PTR_I8_TY());

    emitCall(buildParameters, g_LangSpec->name_priv_panic, allocR, allocT, {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX}, {r1, r2, r3, r4});
}

llvm::Value* LLVM::getImmediateConstantA(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits)
{
    if (ip->hasFlag(BCI_IMM_A))
    {
        switch (numBits)
        {
            case 8:
                return builder.getInt8(ip->a.u8);
            case 16:
                return builder.getInt16(ip->a.u16);
            case 32:
                return builder.getInt32(ip->a.u32);
            case 64:
                return builder.getInt64(ip->a.u64);
            default:
                break;
        }
    }

    const auto r0 = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
    return builder.CreateLoad(IX_TY(numBits), r0);
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

void LLVM::storeRT2ToRegisters(llvm::LLVMContext&     context,
                               const BuildParameters& buildParameters,
                               uint32_t               reg0,
                               uint32_t               reg1,
                               llvm::AllocaInst*      allocR,
                               llvm::AllocaInst*      allocRR) const
{
    const auto  ct              = buildParameters.compileType;
    const auto  precompileIndex = buildParameters.precompileIndex;
    const auto& pp              = encoder<LLVMEncoder>(ct, precompileIndex);
    auto&       builder         = *pp.builder;

    auto r0 = GEP64(allocR, reg0);
    auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocRR, 0));
    builder.CreateStore(r1, r0);
    r0 = GEP64(allocR, reg1);
    r1 = builder.CreateLoad(I64_TY(), GEP64(allocRR, 1));
    builder.CreateStore(r1, r0);
}
