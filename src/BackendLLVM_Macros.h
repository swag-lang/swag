#define IX_TY(__n) llvm::Type::getIntNTy(context, __n)
#define I1_TY() llvm::Type::getInt1Ty(context)
#define I8_TY() llvm::Type::getInt8Ty(context)
#define I16_TY() llvm::Type::getInt16Ty(context)
#define I32_TY() llvm::Type::getInt32Ty(context)
#define I64_TY() llvm::Type::getInt64Ty(context)
#define F32_TY() llvm::Type::getFloatTy(context)
#define F64_TY() llvm::Type::getDoubleTy(context)
#define VOID_TY() llvm::Type::getVoidTy(context)

#define PTR_IX_TY(__n) llvm::Type::getIntNPtrTy(context, __n)
#define PTR_I8_TY() llvm::Type::getInt8PtrTy(context)
#define PTR_I16_TY() llvm::Type::getInt16PtrTy(context)
#define PTR_I32_TY() llvm::Type::getInt32PtrTy(context)
#define PTR_I64_TY() llvm::Type::getInt64PtrTy(context)
#define PTR_F32_TY() llvm::Type::getFloatPtrTy(context)
#define PTR_F64_TY() llvm::Type::getDoublePtrTy(context)

#define PTR_PTR_I8_TY() llvm::Type::getInt8PtrTy(context)->getPointerTo()
#define PTR_PTR_I16_TY() llvm::Type::getInt16PtrTy(context)->getPointerTo()
#define PTR_PTR_I32_TY() llvm::Type::getInt32PtrTy(context)->getPointerTo()
#define PTR_PTR_I64_TY() llvm::Type::getInt64PtrTy(context)->getPointerTo()
#define PTR_PTR_F32_TY() llvm::Type::getFloatPtrTy(context)->getPointerTo()
#define PTR_PTR_F64_TY() llvm::Type::getDoublePtrTy(context)->getPointerTo()

#define CST_RA32 builder.getInt32(ip->a.u32)
#define CST_RB32 builder.getInt32(ip->b.u32)
#define CST_RC32 builder.getInt32(ip->c.u32)
#define CST_RD32 builder.getInt32(ip->d.u32)
#define CST_RA64 builder.getInt64(ip->a.u64)
#define CST_RB64 builder.getInt64(ip->b.u64)
#define CST_RC64 builder.getInt64(ip->c.u64)

#define GEP(__type, __data, __offset) (__offset ? builder.CreateInBoundsGEP(__type, __data, builder.getInt64(__offset)) : __data)

#define GEP64(__data, __offset) (__offset ? builder.CreateInBoundsGEP(I64_TY(), __data, builder.getInt64(__offset)) : __data)
#define GEP64_PTR_I8(__data, __offset) builder.CreateInBoundsGEP(I8_TY(), __data, builder.getInt64(__offset * 8))
#define GEP64_PTR_I16(__data, __offset) builder.CreateInBoundsGEP(I16_TY(), __data, builder.getInt64(__offset * 4))
#define GEP64_PTR_I32(__data, __offset) builder.CreateInBoundsGEP(I32_TY(), __data, builder.getInt64(__offset * 2))
#define GEP64_PTR_F32(__data, __offset) builder.CreateInBoundsGEP(F32_TY(), __data, builder.getInt64(__offset * 2))
#define GEP64_PTR_F64(__data, __offset) builder.CreateInBoundsGEP(F64_TY(), __data, builder.getInt64(__offset))

#define GEP64_PTR_PTR_I8(__data, __offset) builder.CreateInBoundsGEP(PTR_I8_TY(), __data, builder.getInt64(__offset))
#define GEP64_PTR_PTR_I16(__data, __offset) builder.CreateInBoundsGEP(PTR_I16_TY(), __data, builder.getInt64(__offset))
#define GEP64_PTR_PTR_I32(__data, __offset) builder.CreateInBoundsGEP(PTR_I32_TY(), __data, builder.getInt64(__offset))
#define GEP64_PTR_PTR_I64(__data, __offset) builder.CreateInBoundsGEP(PTR_I64_TY(), __data, builder.getInt64(__offset))

#define GEP8(__data, __offset) (__offset ? builder.CreateInBoundsGEP(I8_TY(), __data, builder.getInt64(__offset)) : __data)
#define GEP8_PTR_I16(__data, __offset) (__offset & 1) ? TO_PTR_I16(GEP8(__data, __offset)) : builder.CreateInBoundsGEP(I16_TY(), __data, builder.getInt64(__offset / 2))
#define GEP8_PTR_I32(__data, __offset) (__offset & 3) ? TO_PTR_I32(GEP8(__data, __offset)) : builder.CreateInBoundsGEP(I32_TY(), __data, builder.getInt64(__offset / 4))
#define GEP8_PTR_I64(__data, __offset) (__offset & 7) ? TO_PTR_I64(GEP8(__data, __offset)) : builder.CreateInBoundsGEP(I64_TY(), __data, builder.getInt64(__offset / 8))
#define GEP8_PTR_F32(__data, __offset) (__offset & 3) ? TO_PTR_F32(GEP8(__data, __offset)) : builder.CreateInBoundsGEP(F32_TY(), __data, builder.getInt64(__offset / 4))
#define GEP8_PTR_F64(__data, __offset) (__offset & 7) ? TO_PTR_F64(GEP8(__data, __offset)) : builder.CreateInBoundsGEP(F64_TY(), __data, builder.getInt64(__offset / 8))

#define TO_PTR_PTR_I8(__r) builder.CreatePointerCast(__r, PTR_I8_TY()->getPointerTo())
#define TO_PTR_PTR_I16(__r) builder.CreatePointerCast(__r, PTR_I16_TY()->getPointerTo())
#define TO_PTR_PTR_I32(__r) builder.CreatePointerCast(__r, PTR_I32_TY()->getPointerTo())
#define TO_PTR_PTR_I64(__r) builder.CreatePointerCast(__r, PTR_I64_TY()->getPointerTo())
#define TO_PTR_PTR_F32(__r) builder.CreatePointerCast(__r, PTR_F32_TY()->getPointerTo())
#define TO_PTR_PTR_F64(__r) builder.CreatePointerCast(__r, PTR_F64_TY()->getPointerTo())
#define TO_PTR_PTR_PTR_I8(__r) builder.CreatePointerCast(__r, PTR_I8_TY()->getPointerTo()->getPointerTo())
#define TO_PTR_I64(__r) builder.CreatePointerCast(__r, PTR_I64_TY())
#define TO_PTR_I32(__r) builder.CreatePointerCast(__r, PTR_I32_TY())
#define TO_PTR_I16(__r) builder.CreatePointerCast(__r, PTR_I16_TY())
#define TO_PTR_I8(__r) builder.CreatePointerCast(__r, PTR_I8_TY())
#define TO_PTR_F64(__r) builder.CreatePointerCast(__r, PTR_F64_TY())
#define TO_PTR_F32(__r) builder.CreatePointerCast(__r, PTR_F32_TY())
#define TO_PTR_I_N(__r, __n) builder.CreatePointerCast(__r, llvm::Type::getIntNPtrTy(context, __n))
#define TO_PTR_PTR_I_N(__r, __numBits) TO_PTR_I_N(__r, __n)->getPointerTo())
#define TO_BOOL(__r) builder.CreateIntCast(__r, llvm::Type::getInt1Ty(context), false)

#define MK_IMMA_8() (ip->flags & BCI_IMM_A) ? (llvm::Value*) builder.getInt8(ip->a.u8) : (llvm::Value*) builder.CreateLoad(I8_TY(), GEP64(allocR, ip->a.u32))
#define MK_IMMA_16() (ip->flags & BCI_IMM_A) ? (llvm::Value*) builder.getInt16(ip->a.u16) : (llvm::Value*) builder.CreateLoad(I16_TY(), GEP64(allocR, ip->a.u32))
#define MK_IMMA_32() (ip->flags & BCI_IMM_A) ? (llvm::Value*) builder.getInt32(ip->a.u32) : (llvm::Value*) builder.CreateLoad(I32_TY(), GEP64(allocR, ip->a.u32))
#define MK_IMMA_64() (ip->flags & BCI_IMM_A) ? (llvm::Value*) builder.getInt64(ip->a.u64) : (llvm::Value*) builder.CreateLoad(I64_TY(), GEP64(allocR, ip->a.u32))
#define MK_IMMA_F32() (ip->flags & BCI_IMM_A) ? (llvm::Value*) llvm::ConstantFP::get(F32_TY(), ip->a.f32) : (llvm::Value*) builder.CreateLoad(F32_TY(), GEP64(allocR, ip->a.u32))
#define MK_IMMA_F64() (ip->flags & BCI_IMM_A) ? (llvm::Value*) llvm::ConstantFP::get(F64_TY(), ip->a.f64) : (llvm::Value*) builder.CreateLoad(F64_TY(), GEP64(allocR, ip->a.u32))

#define MK_IMMB_8() (ip->flags & BCI_IMM_B) ? (llvm::Value*) builder.getInt8(ip->b.u8) : (llvm::Value*) builder.CreateLoad(I8_TY(), GEP64(allocR, ip->b.u32))
#define MK_IMMB_16() (ip->flags & BCI_IMM_B) ? (llvm::Value*) builder.getInt16(ip->b.u16) : (llvm::Value*) builder.CreateLoad(I16_TY(), GEP64(allocR, ip->b.u32))
#define MK_IMMB_32() (ip->flags & BCI_IMM_B) ? (llvm::Value*) builder.getInt32(ip->b.u32) : (llvm::Value*) builder.CreateLoad(I32_TY(), GEP64(allocR, ip->b.u32))
#define MK_IMMB_64() (ip->flags & BCI_IMM_B) ? (llvm::Value*) builder.getInt64(ip->b.u64) : (llvm::Value*) builder.CreateLoad(I64_TY(), GEP64(allocR, ip->b.u32))
#define MK_IMMB_F32() (ip->flags & BCI_IMM_B) ? (llvm::Value*) llvm::ConstantFP::get(F32_TY(), ip->b.f32) : (llvm::Value*) builder.CreateLoad(F32_TY(), GEP64(allocR, ip->b.u32))
#define MK_IMMB_F64() (ip->flags & BCI_IMM_B) ? (llvm::Value*) llvm::ConstantFP::get(F64_TY(), ip->b.f64) : (llvm::Value*) builder.CreateLoad(F64_TY(), GEP64(allocR, ip->b.u32))

#define MK_IMMC_8() (ip->flags & BCI_IMM_C) ? (llvm::Value*) builder.getInt8(ip->c.u8) : (llvm::Value*) builder.CreateLoad(I8_TY(), GEP64(allocR, ip->c.u32))
#define MK_IMMC_16() (ip->flags & BCI_IMM_C) ? (llvm::Value*) builder.getInt16(ip->c.u16) : (llvm::Value*) builder.CreateLoad(I16_TY(), GEP64(allocR, ip->c.u32))
#define MK_IMMC_32() (ip->flags & BCI_IMM_C) ? (llvm::Value*) builder.getInt32(ip->c.u32) : (llvm::Value*) builder.CreateLoad(I32_TY(), GEP64(allocR, ip->c.u32))
#define MK_IMMC_64() (ip->flags & BCI_IMM_C) ? (llvm::Value*) builder.getInt64(ip->c.u64) : (llvm::Value*) builder.CreateLoad(I64_TY(), GEP64(allocR, ip->c.u32))
#define MK_IMMC_F32() (ip->flags & BCI_IMM_C) ? (llvm::Value*) llvm::ConstantFP::get(F32_TY(), ip->c.f32) : (llvm::Value*) builder.CreateLoad(F32_TY(), GEP64(allocR, ip->c.u32))
#define MK_IMMC_F64() (ip->flags & BCI_IMM_C) ? (llvm::Value*) llvm::ConstantFP::get(F64_TY(), ip->c.f64) : (llvm::Value*) builder.CreateLoad(F64_TY(), GEP64(allocR, ip->c.u32))

#define MK_IMMD_64() (ip->flags & BCI_IMM_D) ? (llvm::Value*) builder.getInt64(ip->d.u64) : (llvm::Value*) builder.CreateLoad(I64_TY(), GEP64(allocR, ip->d.u32))
#define MK_IMMD_F32() (ip->flags & BCI_IMM_D) ? (llvm::Value*) llvm::ConstantFP::get(F32_TY(), ip->d.f32) : (llvm::Value*) builder.CreateLoad(F32_TY(), GEP64(allocR, ip->d.u32))
#define MK_IMMD_F64() (ip->flags & BCI_IMM_D) ? (llvm::Value*) llvm::ConstantFP::get(F64_TY(), ip->d.f64) : (llvm::Value*) builder.CreateLoad(F64_TY(), GEP64(allocR, ip->d.u32))

#define MK_BINOP8_CAB()                                \
    auto         r0 = GEP64_PTR_I8(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_8();                     \
    llvm::Value* r2 = MK_IMMB_8();
#define MK_BINOP16_CAB()                                \
    auto         r0 = GEP64_PTR_I16(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_16();                     \
    llvm::Value* r2 = MK_IMMB_16();
#define MK_BINOP32_CAB()                                \
    auto         r0 = GEP64_PTR_I32(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_32();                     \
    llvm::Value* r2 = MK_IMMB_32();
#define MK_BINOP64_CAB()                        \
    auto         r0 = GEP64(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_64();             \
    llvm::Value* r2 = MK_IMMB_64();
#define MK_BINOPF32_CAB()                               \
    auto         r0 = GEP64_PTR_F32(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_F32();                    \
    llvm::Value* r2 = MK_IMMB_F32();
#define MK_BINOPF64_CAB()                               \
    auto         r0 = GEP64_PTR_F64(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_F64();                    \
    llvm::Value* r2 = MK_IMMB_F64();

#define MK_BINOP16_CAB8()                              \
    auto         r0 = GEP64_PTR_I8(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_16();                    \
    llvm::Value* r2 = MK_IMMB_16();
#define MK_BINOP32_CAB8()                              \
    auto         r0 = GEP64_PTR_I8(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_32();                    \
    llvm::Value* r2 = MK_IMMB_32();
#define MK_BINOP64_CAB8()                              \
    auto         r0 = GEP64_PTR_I8(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_64();                    \
    llvm::Value* r2 = MK_IMMB_64();
#define MK_BINOPF32_CAB8()                             \
    auto         r0 = GEP64_PTR_I8(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_F32();                   \
    llvm::Value* r2 = MK_IMMB_F32();
#define MK_BINOPF64_CAB8()                             \
    auto         r0 = GEP64_PTR_I8(allocR, ip->c.u32); \
    llvm::Value* r1 = MK_IMMA_F64();                   \
    llvm::Value* r2 = MK_IMMB_F64();

#define MK_BINOPEQ8_CAB()                                  \
    auto         r0 = GEP64(allocR, ip->a.u32);            \
    auto         r1 = builder.CreateLoad(PTR_I8_TY(), r0); \
    llvm::Value* r2 = MK_IMMB_8();
#define MK_BINOPEQ8_SCAB()                         \
    auto         r0 = GEP8(allocStack, ip->a.u32); \
    llvm::Value* r1 = MK_IMMB_8();
#define MK_BINOPEQ8_SSCAB()                        \
    auto         r0 = GEP8(allocStack, ip->a.u32); \
    llvm::Value* r1 = builder.CreateInBoundsGEP(I8_TY(), allocStack, CST_RB32);

#define MK_BINOPEQ16_CAB()                                  \
    auto         r0 = GEP64(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(PTR_I16_TY(), r0); \
    llvm::Value* r2 = MK_IMMB_16();
#define MK_BINOPEQ16_SCAB()                                \
    auto         r0 = GEP8_PTR_I16(allocStack, ip->a.u32); \
    llvm::Value* r1 = MK_IMMB_16();
#define MK_BINOPEQ16_SSCAB()                               \
    auto         r0 = GEP8_PTR_I16(allocStack, ip->a.u32); \
    llvm::Value* r1 = GEP8_PTR_I16(allocStack, ip->b.u32);

#define MK_BINOPEQ32_CAB()                                  \
    auto         r0 = GEP64(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(PTR_I32_TY(), r0); \
    llvm::Value* r2 = MK_IMMB_32();
#define MK_BINOPEQ32_SCAB()                                \
    auto         r0 = GEP8_PTR_I32(allocStack, ip->a.u32); \
    llvm::Value* r1 = MK_IMMB_32();
#define MK_BINOPEQ32_SSCAB()                               \
    auto         r0 = GEP8_PTR_I32(allocStack, ip->a.u32); \
    llvm::Value* r1 = GEP8_PTR_I32(allocStack, ip->b.u32);

#define MK_BINOPEQ64_CAB()                                  \
    auto         r0 = GEP64(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(PTR_I64_TY(), r0); \
    llvm::Value* r2 = MK_IMMB_64();
#define MK_BINOPEQ64_SCAB()                                \
    auto         r0 = GEP8_PTR_I64(allocStack, ip->a.u32); \
    llvm::Value* r1 = MK_IMMB_64();
#define MK_BINOPEQ64_SSCAB()                               \
    auto         r0 = GEP8_PTR_I64(allocStack, ip->a.u32); \
    llvm::Value* r1 = GEP8_PTR_I64(allocStack, ip->b.u32);

#define MK_BINOPEQF32_CAB()                                 \
    auto         r0 = GEP64(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(PTR_F32_TY(), r0); \
    llvm::Value* r2 = MK_IMMB_F32();
#define MK_BINOPEQF32_SCAB()                               \
    auto         r0 = GEP8_PTR_F32(allocStack, ip->a.u32); \
    llvm::Value* r1 = MK_IMMB_F32();
#define MK_BINOPEQF32_SSCAB()                      \
    auto r0 = GEP8_PTR_F32(allocStack, ip->a.u32); \
    auto r1 = GEP8_PTR_F32(allocStack, ip->b.u32);

#define MK_BINOPEQF64_CAB()                                 \
    auto         r0 = GEP64(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(PTR_F64_TY(), r0); \
    llvm::Value* r2 = MK_IMMB_F64();
#define MK_BINOPEQF64_SCAB()                               \
    auto         r0 = GEP8_PTR_F64(allocStack, ip->a.u32); \
    llvm::Value* r1 = MK_IMMB_F64();
#define MK_BINOPEQF64_SSCAB()                      \
    auto r0 = GEP8_PTR_F64(allocStack, ip->a.u32); \
    auto r1 = GEP8_PTR_F64(allocStack, ip->b.u32);

#define OPEQ_OVERFLOW(__intr, __inst, __type, __msg)                                                                  \
    if (module->mustEmitSafetyOF(ip->node))                                                                           \
    {                                                                                                                 \
        auto vs = builder.CreateBinaryIntrinsic(llvm::Intrinsic::__intr, builder.CreateLoad(builder.__type, r1), r2); \
        auto v0 = builder.CreateExtractValue(vs, {0});                                                                \
        auto v1 = builder.CreateExtractValue(vs, {1});                                                                \
                                                                                                                      \
        llvm::BasicBlock* blockOk  = llvm::BasicBlock::Create(context, "", func);                                     \
        llvm::BasicBlock* blockErr = llvm::BasicBlock::Create(context, "", func);                                     \
                                                                                                                      \
        auto v2 = builder.CreateIsNull(v1);                                                                           \
        builder.CreateCondBr(v2, blockOk, blockErr);                                                                  \
        builder.SetInsertPoint(blockErr);                                                                             \
        emitInternalPanic(buildParameters, moduleToGen, allocR, allocT, ip->node, __msg);                             \
        builder.CreateBr(blockOk);                                                                                    \
        builder.SetInsertPoint(blockOk);                                                                              \
        builder.CreateStore(v0, r1);                                                                                  \
    }                                                                                                                 \
    else                                                                                                              \
    {                                                                                                                 \
        auto v0 = builder.__inst(builder.CreateLoad(builder.__type, r1), r2);                                         \
        builder.CreateStore(v0, r1);                                                                                  \
    }

#define OP_OVERFLOW(__intr, __inst, __type, __msg)                                        \
    if (module->mustEmitSafetyOF(ip->node))                                               \
    {                                                                                     \
        auto vs = builder.CreateBinaryIntrinsic(llvm::Intrinsic::__intr, r1, r2);         \
        auto v0 = builder.CreateExtractValue(vs, {0});                                    \
        auto v1 = builder.CreateExtractValue(vs, {1});                                    \
                                                                                          \
        llvm::BasicBlock* blockOk  = llvm::BasicBlock::Create(context, "", func);         \
        llvm::BasicBlock* blockErr = llvm::BasicBlock::Create(context, "", func);         \
                                                                                          \
        auto v2 = builder.CreateIsNull(v1);                                               \
        builder.CreateCondBr(v2, blockOk, blockErr);                                      \
        builder.SetInsertPoint(blockErr);                                                 \
        emitInternalPanic(buildParameters, moduleToGen, allocR, allocT, ip->node, __msg); \
        builder.CreateBr(blockOk);                                                        \
        builder.SetInsertPoint(blockOk);                                                  \
        builder.CreateStore(v0, r0);                                                      \
    }                                                                                     \
    else                                                                                  \
    {                                                                                     \
        auto v0 = builder.__inst(r1, r2);                                                 \
        builder.CreateStore(v0, r0);                                                      \
    }
