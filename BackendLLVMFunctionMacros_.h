#define MK_BINOP8_CAB()                                                 \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));            \
    llvm::Value *r1, *r2;                                               \
    if (ip->flags & BCI_IMM_A)                                          \
        r1 = builder.getInt8(ip->a.u8);                                 \
    else                                                                \
        r1 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->a.u32))); \
    if (ip->flags & BCI_IMM_B)                                          \
        r2 = builder.getInt8(ip->b.u8);                                 \
    else                                                                \
        r2 = builder.CreateLoad(TO_PTR_I8(GEP_I32(allocR, ip->b.u32)));

#define MK_BINOP16_CAB()                                                 \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));             \
    llvm::Value *r1, *r2;                                                \
    if (ip->flags & BCI_IMM_A)                                           \
        r1 = builder.getInt16(ip->a.u16);                                \
    else                                                                 \
        r1 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->a.u32))); \
    if (ip->flags & BCI_IMM_B)                                           \
        r2 = builder.getInt16(ip->b.u16);                                \
    else                                                                 \
        r2 = builder.CreateLoad(TO_PTR_I16(GEP_I32(allocR, ip->b.u32)));

#define MK_BINOP32_CAB()                                                 \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));             \
    llvm::Value *r1, *r2;                                                \
    if (ip->flags & BCI_IMM_A)                                           \
        r1 = builder.getInt32(ip->a.u32);                                \
    else                                                                 \
        r1 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->a.u32))); \
    if (ip->flags & BCI_IMM_B)                                           \
        r2 = builder.getInt32(ip->b.u32);                                \
    else                                                                 \
        r2 = builder.CreateLoad(TO_PTR_I32(GEP_I32(allocR, ip->b.u32)));

#define MK_BINOP64_CAB()                                                 \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));             \
    llvm::Value *r1, *r2;                                                \
    if (ip->flags & BCI_IMM_A)                                           \
        r1 = builder.getInt64(ip->a.u64);                                \
    else                                                                 \
        r1 = builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->a.u32))); \
    if (ip->flags & BCI_IMM_B)                                           \
        r2 = builder.getInt64(ip->b.u64);                                \
    else                                                                 \
        r2 = builder.CreateLoad(TO_PTR_I64(GEP_I32(allocR, ip->b.u32)));

#define MK_BINOPF32_CAB()                                                       \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));                    \
    llvm::Value *r1, *r2;                                                       \
    if (ip->flags & BCI_IMM_A)                                                  \
        r1 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), ip->a.f32); \
    else                                                                        \
        r1 = builder.CreateLoad(TO_PTR_F32(GEP_I32(allocR, ip->a.u32)));        \
    if (ip->flags & BCI_IMM_B)                                                  \
        r2 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), ip->b.f32); \
    else                                                                        \
        r2 = builder.CreateLoad(TO_PTR_F32(GEP_I32(allocR, ip->b.u32)));

#define MK_BINOPF64_CAB()                                                        \
    auto         r0 = TO_PTR_I8(GEP_I32(allocR, ip->c.u32));                     \
    llvm::Value *r1, *r2;                                                        \
    if (ip->flags & BCI_IMM_A)                                                   \
        r1 = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), ip->a.f64); \
    else                                                                         \
        r1 = builder.CreateLoad(TO_PTR_F64(GEP_I32(allocR, ip->a.u32)));         \
    if (ip->flags & BCI_IMM_B)                                                   \
        r2 = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), ip->b.f64); \
    else                                                                         \
        r2 = builder.CreateLoad(TO_PTR_F64(GEP_I32(allocR, ip->b.u32)));
