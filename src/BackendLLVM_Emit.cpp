#include "pch.h"
#include "BackendLLVM.h"
#include "BackendLLVM_Macros.h"
#include "ByteCode.h"
#include "LanguageSpec.h"

void BackendLLVM::emitShiftRightArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits)
{
    auto         iType = llvm::Type::getIntNTy(context, numBits);
    llvm::Value* r1    = getImmediateConstantA(context, builder, allocR, ip, numBits);

    llvm::Value* r2;
    if (ip->flags & BCI_IMM_B)
        r2 = builder.getInt32(ip->b.u32 & (numBits - 1));
    else
        r2 = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));

    auto c2    = builder.CreateIntCast(r2, iType, false);
    auto shift = llvm::ConstantInt::get(iType, numBits - 1);

    // Smart shift, imm mode overflow
    if ((ip->flags & BCI_IMM_B) && ip->b.u32 >= numBits)
        c2 = shift;

    // Smart shift, dyn mode
    else if (!(ip->flags & BCI_IMM_B))
    {
        auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        auto iftrue  = c2;
        auto iffalse = shift;
        c2           = builder.CreateSelect(cond, iftrue, iffalse);
    }

    // Small shift, dyn mode, we mask the operand
    else if (!(ip->flags & BCI_IMM_B))
        c2 = builder.CreateAnd(c2, llvm::ConstantInt::get(iType, numBits - 1));

    auto v0 = builder.CreateAShr(r1, c2);
    auto r0 = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
    builder.CreateStore(v0, r0);
}

void BackendLLVM::emitShiftLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits, bool left)
{
    auto iType = llvm::Type::getIntNTy(context, numBits);
    auto r0    = GEP64_PTR_IX(allocR, ip->c.u32, numBits);
    auto zero  = llvm::ConstantInt::get(iType, 0);

    // Smart shift, imm mode overflow
    if ((ip->flags & BCI_IMM_B) && ip->b.u32 >= numBits)
        builder.CreateStore(zero, r0);
    else
    {
        llvm::Value* r1 = getImmediateConstantA(context, builder, allocR, ip, numBits);

        llvm::Value* r2;
        if (ip->flags & BCI_IMM_B)
            r2 = builder.getInt32(ip->b.u32 & (numBits - 1));
        else
            r2 = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));
        auto c2 = builder.CreateIntCast(r2, iType, false);

        // Smart shift, dyn mode
        if (!(ip->flags & BCI_IMM_B))
        {
            auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
            auto iftrue  = left ? builder.CreateShl(r1, c2) : builder.CreateLShr(r1, c2);
            auto iffalse = zero;
            auto v0      = builder.CreateSelect(cond, iftrue, iffalse);
            builder.CreateStore(v0, r0);
        }
        else
        {
            // Small shift, dyn mode, we mask the operand
            if (!(ip->flags & BCI_IMM_B))
                c2 = builder.CreateAnd(c2, llvm::ConstantInt::get(iType, numBits - 1));
            auto v0 = left ? builder.CreateShl(r1, c2) : builder.CreateLShr(r1, c2);
            builder.CreateStore(v0, r0);
        }
    }
}

void BackendLLVM::emitShiftRightEqArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits)
{
    auto iType = llvm::Type::getIntNTy(context, numBits);
    auto r0    = GEP64(allocR, ip->a.u32);

    llvm::Value* r2;
    if (ip->flags & BCI_IMM_B)
        r2 = builder.getInt32(ip->b.u32 & (numBits - 1));
    else
        r2 = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));

    auto c2    = builder.CreateIntCast(r2, iType, false);
    auto shift = llvm::ConstantInt::get(iType, numBits - 1);

    // Smart shift, imm mode overflow
    if ((ip->flags & BCI_IMM_B) && ip->b.u32 >= numBits)
        c2 = shift;

    // Smart shift, dyn mode
    else if (!(ip->flags & BCI_IMM_B))
    {
        auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        auto iftrue  = c2;
        auto iffalse = shift;
        c2           = builder.CreateSelect(cond, iftrue, iffalse);
    }

    // Small shift, dyn mode, we mask the operand
    else if (!(ip->flags & BCI_IMM_B))
        c2 = builder.CreateAnd(c2, llvm::ConstantInt::get(iType, numBits - 1));

    auto r1 = builder.CreateLoad(PTR_IX_TY(numBits), r0);
    auto v0 = builder.CreateAShr(builder.CreateLoad(IX_TY(numBits), r1), c2);
    builder.CreateStore(v0, r1);
}

void BackendLLVM::emitShiftEqLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits, bool left)
{
    auto iType = llvm::Type::getIntNTy(context, numBits);
    auto r0    = GEP64(allocR, ip->a.u32);
    auto r1    = builder.CreateLoad(PTR_IX_TY(numBits), r0);

    llvm::Value* r2;
    if (ip->flags & BCI_IMM_B)
        r2 = builder.getInt32(ip->b.u32 & (numBits - 1));
    else
        r2 = builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32));

    auto         c2   = builder.CreateIntCast(r2, iType, false);
    auto         zero = llvm::ConstantInt::get(iType, 0);
    auto         v1   = builder.CreateLoad(IX_TY(numBits), r1);
    llvm::Value* v0;

    // Smart shift, imm mode overflow
    if ((ip->flags & BCI_IMM_B) && ip->b.u32 >= numBits)
        v0 = zero;

    // Smart shift, dyn mode
    else if (!(ip->flags & BCI_IMM_B))
    {
        auto cond    = builder.CreateICmpULT(r2, builder.getInt32(numBits));
        auto iftrue  = left ? builder.CreateShl(v1, c2) : builder.CreateLShr(v1, c2);
        auto iffalse = zero;
        v0           = builder.CreateSelect(cond, iftrue, iffalse);
    }

    // Imm mode
    else
    {
        // Small shift, dyn mode, we mask the operand
        if (!(ip->flags & BCI_IMM_B))
            c2 = builder.CreateAnd(c2, llvm::ConstantInt::get(iType, numBits - 1));
        v0 = left ? builder.CreateShl(v1, c2) : builder.CreateLShr(v1, c2);
    }

    builder.CreateStore(v0, r1);
}

void BackendLLVM::emitInternalPanic(const BuildParameters& buildParameters, Module* moduleToGen, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, AstNode* node, const char* message)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;
    auto& builder         = *pp.builder;

    // Filename
    llvm::Value* r1 = builder.CreateGlobalString(node->sourceFile->path.string().c_str());
    r1              = builder.CreateInBoundsGEP(I8_TY(), r1, {pp.cst0_i32});

    // Line & column
    auto r2 = builder.getInt32(node->token.startLocation.line);
    auto r3 = builder.getInt32(node->token.startLocation.column);

    // Message
    llvm::Value* r4;
    if (message)
    {
        r4 = builder.CreateGlobalString(message);
        r4 = builder.CreateInBoundsGEP(I8_TY(), r4, {pp.cst0_i32});
    }
    else
        r4 = builder.CreateIntToPtr(pp.cst0_i64, PTR_I8_TY());

    emitCall(buildParameters, moduleToGen, g_LangSpec->name__panic, allocR, allocT, {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX}, {r1, r2, r3, r4});
}

llvm::Value* BackendLLVM::getImmediateConstantA(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits)
{
    if (ip->flags & BCI_IMM_A)
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
        }
    }

    auto r0 = GEP64_PTR_IX(allocR, ip->a.u32, numBits);
    return builder.CreateLoad(IX_TY(numBits), r0);
}

void BackendLLVM::storeTypedValueToRegister(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Value* value, uint32_t reg, llvm::AllocaInst* allocR)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    SWAG_ASSERT(value);
    auto r1 = value;

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

void BackendLLVM::storeRT2ToRegisters(llvm::LLVMContext& context, const BuildParameters& buildParameters, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    auto r0 = GEP64(allocR, reg0);
    auto r1 = builder.CreateLoad(I64_TY(), GEP64(allocRR, 0));
    builder.CreateStore(r1, r0);
    r0 = GEP64(allocR, reg1);
    r1 = builder.CreateLoad(I64_TY(), GEP64(allocRR, 1));
    builder.CreateStore(r1, r0);
}
