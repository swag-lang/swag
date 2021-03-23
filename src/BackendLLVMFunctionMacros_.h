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

#define MK_BINOPEQ16_CAB()                                    \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_I16(r0)); \
    llvm::Value* r2 = MK_IMMB_16();

#define MK_BINOPEQ32_CAB()                                    \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_I32(r0)); \
    llvm::Value* r2 = MK_IMMB_32();

#define MK_BINOPEQ64_CAB()                                \
    llvm::Value* r1;                                      \
    if (ip->flags & BCI_STACKPTR_A)                       \
    {                                                     \
        r1 = TO_PTR_I64(GEP_I32(allocStack, ip->a.u32));  \
    }                                                     \
    else                                                  \
    {                                                     \
        auto r0 = GEP_I32(allocR, ip->a.u32);             \
        r1      = builder.CreateLoad(TO_PTR_PTR_I64(r0)); \
    }                                                     \
    llvm::Value* r2 = MK_IMMB_64();

#define MK_BINOPEQF32_CAB()                                   \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_F32(r0)); \
    llvm::Value* r2 = MK_IMMB_F32();

#define MK_BINOPEQF64_CAB()                                   \
    auto         r0 = GEP_I32(allocR, ip->a.u32);             \
    auto         r1 = builder.CreateLoad(TO_PTR_PTR_F64(r0)); \
    llvm::Value* r2 = MK_IMMB_F64();