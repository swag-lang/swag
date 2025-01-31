#pragma once
#define MK_IMMA(__reg, __numBits)                                              \
    do                                                                         \
    {                                                                          \
        if (ip->hasFlag(BCI_IMM_A))                                            \
            pp.emitLoadImmediate(__reg, ip->a.u64, __numBits);                 \
        else                                                                   \
            pp.emitLoadIndirect(REG_OFFSET(ip->a.u32), __reg, RDI, __numBits); \
    } while (0)

#define MK_IMMB(__reg, __numBits)                                              \
    do                                                                         \
    {                                                                          \
        if (ip->hasFlag(BCI_IMM_B))                                            \
            pp.emitLoadImmediate(__reg, ip->b.u64, __numBits);                 \
        else                                                                   \
            pp.emitLoadIndirect(REG_OFFSET(ip->b.u32), __reg, RDI, __numBits); \
    } while (0)

#define MK_IMMC(__reg, __numBits)                                              \
    do                                                                         \
    {                                                                          \
        if (ip->hasFlag(BCI_IMM_C))                                            \
            pp.emitLoadImmediate(__reg, ip->c.u64, __numBits);                 \
        else                                                                   \
            pp.emitLoadIndirect(REG_OFFSET(ip->c.u32), __reg, RDI, __numBits); \
    } while (0)

#define MK_IMMD(__reg, __numBits)                                              \
    do                                                                         \
    {                                                                          \
        if (ip->hasFlag(BCI_IMM_D))                                            \
            pp.emitLoadImmediate(__reg, ip->d.u64, __numBits);                 \
        else                                                                   \
            pp.emitLoadIndirect(REG_OFFSET(ip->d.u32), __reg, RDI, __numBits); \
    } while (0)

//////////////////////////////////

#define MK_IMMB_S8_TO_S32(__reg)                                           \
    do                                                                     \
    {                                                                      \
        if (ip->hasFlag(BCI_IMM_B))                                        \
            pp.emitLoadImmediate(__reg, (int32_t) ip->b.s8, CPUBits::B32); \
        else                                                               \
            pp.emitLoadS8S32Indirect(REG_OFFSET(ip->b.u32), __reg, RDI);   \
    } while (0)

#define MK_IMMB_U8_TO_U32(__reg)                                         \
    do                                                                   \
    {                                                                    \
        if (ip->hasFlag(BCI_IMM_B))                                      \
            pp.emitLoadImmediate(__reg, ip->b.u8, CPUBits::B32);         \
        else                                                             \
            pp.emitLoadU8U32Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_U8_TO_U16(__reg)                                         \
    do                                                                   \
    {                                                                    \
        if (ip->hasFlag(BCI_IMM_B))                                      \
            pp.emitLoadImmediate(__reg, ip->b.u8, CPUBits::B16);         \
        else                                                             \
            pp.emitLoadU8U32Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMC_S8_TO_S32(__reg)                                           \
    do                                                                     \
    {                                                                      \
        if (ip->hasFlag(BCI_IMM_C))                                        \
            pp.emitLoadImmediate(__reg, (int32_t) ip->c.s8, CPUBits::B32); \
        else                                                               \
            pp.emitLoadS8S32Indirect(REG_OFFSET(ip->c.u32), __reg, RDI);   \
    } while (0)

#define MK_IMMC_U8_TO_U32(__reg)                                         \
    do                                                                   \
    {                                                                    \
        if (ip->hasFlag(BCI_IMM_C))                                      \
            pp.emitLoadImmediate(__reg, ip->c.u8, CPUBits::B32);         \
        else                                                             \
            pp.emitLoadU8U32Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
    } while (0)

//////////////////////////////////

#define MK_BINOP_EQ(__offset, __op, __numBits)                             \
    do                                                                     \
    {                                                                      \
        const auto r0 = SCBE_CPU::isInt(__numBits) ? RAX : RCX;            \
        const auto r1 = SCBE_CPU::isInt(__numBits) ? XMM1 : RCX;           \
        pp.emitLoadIndirect(REG_OFFSET(ip->a.u32), r0, RDI, CPUBits::B64); \
        MK_IMMB(r1, __numBits);                                            \
        pp.emitOpIndirect(__offset, r1, r0, __op, __numBits);              \
    } while (0)

#define MK_BINOP_EQ_S(__op, __numBits)                                                      \
    do                                                                                      \
    {                                                                                       \
        if (SCBE_CPU::isInt(__numBits) && ip->hasFlag(BCI_IMM_B))                           \
            pp.emitOpIndirectDst(offsetStack + ip->a.u32, ip->b.u64, RDI, __op, __numBits); \
        else                                                                                \
        {                                                                                   \
            const auto r1 = SCBE_CPU::isInt(__numBits) ? RAX : XMM1;                        \
            MK_IMMB(r1, __numBits);                                                         \
            pp.emitOpIndirect(offsetStack + ip->a.u32, r1, RDI, __op, __numBits);           \
        }                                                                                   \
    } while (0)

#define MK_BINOP_EQ_SS(__op, __numBits)                                       \
    do                                                                        \
    {                                                                         \
        const auto r1 = SCBE_CPU::isInt(__numBits) ? RAX : XMM1;              \
        pp.emitLoadIndirect(offsetStack + ip->b.u32, r1, RDI, __numBits);     \
        pp.emitOpIndirect(offsetStack + ip->a.u32, r1, RDI, __op, __numBits); \
    } while (0)

#define MK_BINOP_EQ_LOCK(__op, __numBits)                                   \
    do                                                                      \
    {                                                                       \
        pp.emitLoadIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB(RAX, __numBits);                                            \
        pp.emitOpIndirect(0, RAX, RCX, __op, __numBits, true);              \
    } while (0)

//////////////////////////////////

