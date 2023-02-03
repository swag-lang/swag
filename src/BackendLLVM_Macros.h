#define MK_IMMA_8() (ip->flags & BCI_IMM_A) ? (llvm::Value*) builder.getInt8(ip->a.u8) : (llvm::Value*) builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->a.u32)))
#define MK_IMMA_16() (ip->flags & BCI_IMM_A) ? (llvm::Value*) builder.getInt16(ip->a.u16) : (llvm::Value*) builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->a.u32)))
#define MK_IMMA_32() (ip->flags & BCI_IMM_A) ? (llvm::Value*) builder.getInt32(ip->a.u32) : (llvm::Value*) builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->a.u32)))
#define MK_IMMA_64() (ip->flags & BCI_IMM_A) ? (llvm::Value*) builder.getInt64(ip->a.u64) : (llvm::Value*) builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->a.u32)))
#define MK_IMMA_F32() (ip->flags & BCI_IMM_A) ? (llvm::Value*) llvm::ConstantFP::get(llvm::Type::getFloatTy(context), ip->a.f32) : (llvm::Value*) builder.CreateLoad(TO_PTR_F32(GEP_I32(allocR, ip->a.u32)))
#define MK_IMMA_F64() (ip->flags & BCI_IMM_A) ? (llvm::Value*) llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), ip->a.f64) : (llvm::Value*) builder.CreateLoad(TO_PTR_F64(GEP_I32(allocR, ip->a.u32)))

#define MK_IMMB_8() (ip->flags & BCI_IMM_B) ? (llvm::Value*) builder.getInt8(ip->b.u8) : (llvm::Value*) builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)))
#define MK_IMMB_16() (ip->flags & BCI_IMM_B) ? (llvm::Value*) builder.getInt16(ip->b.u16) : (llvm::Value*) builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)))
#define MK_IMMB_32() (ip->flags & BCI_IMM_B) ? (llvm::Value*) builder.getInt32(ip->b.u32) : (llvm::Value*) builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)))
#define MK_IMMB_64() (ip->flags & BCI_IMM_B) ? (llvm::Value*) builder.getInt64(ip->b.u64) : (llvm::Value*) builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->b.u32)))
#define MK_IMMB_F32() (ip->flags & BCI_IMM_B) ? (llvm::Value*) llvm::ConstantFP::get(llvm::Type::getFloatTy(context), ip->b.f32) : (llvm::Value*) builder.CreateLoad(TO_PTR_F32(GEP_I32(allocR, ip->b.u32)))
#define MK_IMMB_F64() (ip->flags & BCI_IMM_B) ? (llvm::Value*) llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), ip->b.f64) : (llvm::Value*) builder.CreateLoad(TO_PTR_F64(GEP_I32(allocR, ip->b.u32)))

#define MK_IMMC_8() (ip->flags & BCI_IMM_C) ? (llvm::Value*) builder.getInt8(ip->c.u8) : (llvm::Value*) builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->c.u32)))
#define MK_IMMC_16() (ip->flags & BCI_IMM_C) ? (llvm::Value*) builder.getInt16(ip->c.u16) : (llvm::Value*) builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->c.u32)))
#define MK_IMMC_32() (ip->flags & BCI_IMM_C) ? (llvm::Value*) builder.getInt32(ip->c.u32) : (llvm::Value*) builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->c.u32)))
#define MK_IMMC_64() (ip->flags & BCI_IMM_C) ? (llvm::Value*) builder.getInt64(ip->c.u64) : (llvm::Value*) builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->c.u32)))
#define MK_IMMC_F32() (ip->flags & BCI_IMM_C) ? (llvm::Value*) llvm::ConstantFP::get(llvm::Type::getFloatTy(context), ip->c.f32) : (llvm::Value*) builder.CreateLoad(TO_PTR_F32(GEP_I32(allocR, ip->c.u32)))
#define MK_IMMC_F64() (ip->flags & BCI_IMM_C) ? (llvm::Value*) llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), ip->c.f64) : (llvm::Value*) builder.CreateLoad(TO_PTR_F64(GEP_I32(allocR, ip->c.u32)))

#define MK_IMMD_64() (ip->flags & BCI_IMM_D) ? (llvm::Value*) builder.getInt64(ip->d.u64) : (llvm::Value*) builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->d.u32)))

#define MK_BINOP8_CAB()                                      \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32)); \
    llvm::Value* r1 = MK_IMMA_8();                           \
    llvm::Value* r2 = MK_IMMB_8();

#define MK_BINOP16_CAB()                                     \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32)); \
    llvm::Value* r1 = MK_IMMA_16();                          \
    llvm::Value* r2 = MK_IMMB_16();

#define MK_BINOP32_CAB()                                     \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32)); \
    llvm::Value* r1 = MK_IMMA_32();                          \
    llvm::Value* r2 = MK_IMMB_32();

#define MK_BINOP64_CAB()                                     \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32)); \
    llvm::Value* r1 = MK_IMMA_64();                          \
    llvm::Value* r2 = MK_IMMB_64();

#define MK_BINOPF32_CAB()                                    \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32)); \
    llvm::Value* r1 = MK_IMMA_F32();                         \
    llvm::Value* r2 = MK_IMMB_F32();

#define MK_BINOPF64_CAB()                                    \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32)); \
    llvm::Value* r1 = MK_IMMA_F64();                         \
    llvm::Value* r2 = MK_IMMB_F64();

#define MK_BINOPEQ8_CAB()                                    \
    auto         r0 = GEP_I32(allocR, ip->a.u32);            \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_I8(r0)); \
    llvm::Value* r2 = MK_IMMB_8();
#define MK_BINOPEQ8_SCAB()                                                        \
    auto         r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = MK_IMMB_8();
#define MK_BINOPEQ8_SSCAB()                                                       \
    auto         r0 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = TO_PTR_I8(builder.CreateInBoundsGEP(allocStack, CST_RB32));

#define MK_BINOPEQ16_CAB()                                    \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0)); \
    llvm::Value* r2 = MK_IMMB_16();
#define MK_BINOPEQ16_SCAB()                                                        \
    auto         r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = MK_IMMB_16();
#define MK_BINOPEQ16_SSCAB()                                                       \
    auto         r0 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = TO_PTR_I16(builder.CreateInBoundsGEP(allocStack, CST_RB32));

#define MK_BINOPEQ32_CAB()                                    \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0)); \
    llvm::Value* r2 = MK_IMMB_32();
#define MK_BINOPEQ32_SCAB()                                                        \
    auto         r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = MK_IMMB_32();
#define MK_BINOPEQ32_SSCAB()                                                       \
    auto         r0 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = TO_PTR_I32(builder.CreateInBoundsGEP(allocStack, CST_RB32));

#define MK_BINOPEQ64_CAB()                                    \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_I64(r0)); \
    llvm::Value* r2 = MK_IMMB_64();
#define MK_BINOPEQ64_SCAB()                                                        \
    auto         r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = MK_IMMB_64();
#define MK_BINOPEQ64_SSCAB()                                                       \
    auto         r0 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = TO_PTR_I64(builder.CreateInBoundsGEP(allocStack, CST_RB32));

#define MK_BINOPEQF32_CAB()                                   \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_F32(r0)); \
    llvm::Value* r2 = MK_IMMB_F32();
#define MK_BINOPEQF32_SCAB()                                                       \
    auto         r0 = TO_PTR_F32(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = MK_IMMB_F32();
#define MK_BINOPEQF32_SSCAB()                                              \
    auto r0 = TO_PTR_F32(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    auto r1 = TO_PTR_F32(builder.CreateInBoundsGEP(allocStack, CST_RB32));

#define MK_BINOPEQF64_CAB()                                   \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_F64(r0)); \
    llvm::Value* r2 = MK_IMMB_F64();
#define MK_BINOPEQF64_SCAB()                                                       \
    auto         r0 = TO_PTR_F64(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    llvm::Value* r1 = MK_IMMB_F64();
#define MK_BINOPEQF64_SSCAB()                                              \
    auto r0 = TO_PTR_F64(builder.CreateInBoundsGEP(allocStack, CST_RA32)); \
    auto r1 = TO_PTR_F64(builder.CreateInBoundsGEP(allocStack, CST_RB32));

#define OPEQ_OVERFLOW(__intr, __inst, __type, __msg)                                                                                \
    if (module->mustEmitSafetyOF(ip->node))                                                                                         \
    {                                                                                                                               \
        auto vs = builder.CreateIntrinsic(llvm::Intrinsic::__intr, {builder.__type, builder.__type}, {builder.CreateLoad(r1), r2}); \
        auto v0 = builder.CreateExtractValue(vs, {0});                                                                              \
        auto v1 = builder.CreateExtractValue(vs, {1});                                                                              \
                                                                                                                                    \
        llvm::BasicBlock* blockOk  = llvm::BasicBlock::Create(context, "", func);                                                   \
        llvm::BasicBlock* blockErr = llvm::BasicBlock::Create(context, "", func);                                                   \
                                                                                                                                    \
        auto v2 = builder.CreateIsNull(v1);                                                                                         \
        builder.CreateCondBr(v2, blockOk, blockErr);                                                                                \
        builder.SetInsertPoint(blockErr);                                                                                           \
        emitInternalPanic(buildParameters, moduleToGen, allocR, allocT, ip->node, __msg);                                           \
        builder.CreateBr(blockOk);                                                                                                  \
        builder.SetInsertPoint(blockOk);                                                                                            \
        builder.CreateStore(v0, r1);                                                                                                \
    }                                                                                                                               \
    else                                                                                                                            \
    {                                                                                                                               \
        auto v0 = builder.__inst(builder.CreateLoad(r1), r2);                                                                       \
        builder.CreateStore(v0, r1);                                                                                                \
    }

#define OP_OVERFLOW(__intr, __inst, __cast, __type, __msg)                                                      \
    if (module->mustEmitSafetyOF(ip->node))                                                                     \
    {                                                                                                           \
        auto vs = builder.CreateIntrinsic(llvm::Intrinsic::__intr, {builder.__type, builder.__type}, {r1, r2}); \
        auto v0 = builder.CreateExtractValue(vs, {0});                                                          \
        auto v1 = builder.CreateExtractValue(vs, {1});                                                          \
                                                                                                                \
        llvm::BasicBlock* blockOk  = llvm::BasicBlock::Create(context, "", func);                               \
        llvm::BasicBlock* blockErr = llvm::BasicBlock::Create(context, "", func);                               \
                                                                                                                \
        auto v2 = builder.CreateIsNull(v1);                                                                     \
        builder.CreateCondBr(v2, blockOk, blockErr);                                                            \
        builder.SetInsertPoint(blockErr);                                                                       \
        emitInternalPanic(buildParameters, moduleToGen, allocR, allocT, ip->node, __msg);                       \
        builder.CreateBr(blockOk);                                                                              \
        builder.SetInsertPoint(blockOk);                                                                        \
        builder.CreateStore(v0, __cast(r0));                                                                    \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
        auto v0 = builder.__inst(r1, r2);                                                                       \
        builder.CreateStore(v0, __cast(r0));                                                                    \
    }

inline llvm::Value* toPtrNative(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Value* v, NativeTypeKind k)
{
    switch (k)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        return TO_PTR_I8(v);
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        return TO_PTR_I16(v);
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        return TO_PTR_I32(v);
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        return TO_PTR_I64(v);
    case NativeTypeKind::F32:
        return TO_PTR_F32(v);
    case NativeTypeKind::F64:
        return TO_PTR_F64(v);
    }

    SWAG_ASSERT(false);
    return nullptr;
}

inline llvm::Value* toPtrPtrNative(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Value* v, NativeTypeKind k)
{
    switch (k)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
    case NativeTypeKind::Void:
        return TO_PTR_PTR_I8(v);
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        return TO_PTR_PTR_I16(v);
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        return TO_PTR_PTR_I32(v);
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        return TO_PTR_PTR_I64(v);
    case NativeTypeKind::F32:
        return TO_PTR_PTR_F32(v);
    case NativeTypeKind::F64:
        return TO_PTR_PTR_F64(v);
    }

    SWAG_ASSERT(false);
    return nullptr;
}

#define TO_PTR_NATIVE(__value, __kind) toPtrNative(context, builder, __value, __kind)
#define TO_PTR_PTR_NATIVE(__value, __kind) toPtrPtrNative(context, builder, __value, __kind)
