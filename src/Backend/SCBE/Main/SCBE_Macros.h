#pragma once
#define MK_IMMA(__reg, __numBits)                                               \
    do                                                                          \
    {                                                                           \
        if (ip->hasFlag(BCI_IMM_A))                                             \
            pp.emitLoadNImmediate(__reg, ip->a.u64, __numBits);                 \
        else                                                                    \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), __reg, RDI, __numBits); \
    } while (0)

#define MK_IMMB(__reg, __numBits)                                               \
    do                                                                          \
    {                                                                           \
        if (ip->hasFlag(BCI_IMM_B))                                             \
            pp.emitLoadNImmediate(__reg, ip->b.u64, __numBits);                 \
        else                                                                    \
            pp.emitLoadNIndirect(REG_OFFSET(ip->b.u32), __reg, RDI, __numBits); \
    } while (0)

#define MK_IMMC(__reg, __numBits)                                               \
    do                                                                          \
    {                                                                           \
        if (ip->hasFlag(BCI_IMM_C))                                             \
            pp.emitLoadNImmediate(__reg, ip->c.u64, __numBits);                 \
        else                                                                    \
            pp.emitLoadNIndirect(REG_OFFSET(ip->c.u32), __reg, RDI, __numBits); \
    } while (0)

#define MK_IMMD(__reg, __numBits)                                               \
    do                                                                          \
    {                                                                           \
        if (ip->hasFlag(BCI_IMM_D))                                             \
            pp.emitLoadNImmediate(__reg, ip->d.u64, __numBits);                 \
        else                                                                    \
            pp.emitLoadNIndirect(REG_OFFSET(ip->d.u32), __reg, RDI, __numBits); \
    } while (0)

//////////////////////////////////

#define MK_IMMB_S8_TO_S32(__reg)                                         \
    do                                                                   \
    {                                                                    \
        if (ip->hasFlag(BCI_IMM_B))                                      \
            pp.emitLoad32Immediate(__reg, (int32_t) ip->b.s8);           \
        else                                                             \
            pp.emitLoadS8S32Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_U8_TO_U32(__reg)                                         \
    do                                                                   \
    {                                                                    \
        if (ip->hasFlag(BCI_IMM_B))                                      \
            pp.emitLoad32Immediate(__reg, ip->b.u8);                     \
        else                                                             \
            pp.emitLoadU8U32Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_U8_TO_U16(__reg)                                         \
    do                                                                   \
    {                                                                    \
        if (ip->hasFlag(BCI_IMM_B))                                      \
            pp.emitLoad16Immediate(__reg, ip->b.u8);                     \
        else                                                             \
            pp.emitLoadU8U32Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMC_S8_TO_S32(__reg)                                         \
    do                                                                   \
    {                                                                    \
        if (ip->hasFlag(BCI_IMM_C))                                      \
            pp.emitLoad32Immediate(__reg, (int32_t) ip->c.s8);           \
        else                                                             \
            pp.emitLoadS8S32Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
    } while (0)

#define MK_IMMC_U8_TO_U32(__reg)                                         \
    do                                                                   \
    {                                                                    \
        if (ip->hasFlag(BCI_IMM_C))                                      \
            pp.emitLoad32Immediate(__reg, ip->c.u8);                     \
        else                                                             \
            pp.emitLoadU8U32Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
    } while (0)

//////////////////////////////////

#define MK_BINOP_CAB(__numBits)                                                                                              \
    do                                                                                                                       \
    {                                                                                                                        \
        const auto r0 = SCBE_CPU::isInt(__numBits) ? RAX : XMM0;                                                             \
        if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))                                                                             \
        {                                                                                                                    \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), r0, RDI, __numBits);                                                 \
            pp.emitCmpNIndirect(REG_OFFSET(ip->b.u32), r0, RDI, __numBits);                                                  \
        }                                                                                                                    \
        else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))                                                          \
        {                                                                                                                    \
            pp.emitLoadNImmediate(r0, ip->a.u64, __numBits);                                                                 \
            pp.emitCmpNIndirect(REG_OFFSET(ip->b.u32), r0, RDI, __numBits);                                                  \
        }                                                                                                                    \
        else if (SCBE_CPU::isInt(__numBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF) \
        {                                                                                                                    \
            pp.emitCmpNIndirectDst(REG_OFFSET(ip->a.u32), ip->b.u32, RDI, __numBits);                                        \
        }                                                                                                                    \
        else                                                                                                                 \
        {                                                                                                                    \
            const auto r1 = SCBE_CPU::isInt(__numBits) ? RCX : XMM1;                                                         \
            MK_IMMA(r0, __numBits);                                                                                          \
            MK_IMMB(r1, __numBits);                                                                                          \
            pp.emitCmpN(r0, r1, __numBits);                                                                                  \
        }                                                                                                                    \
    } while (0)

//////////////////////////////////

#define MK_BINOP_EQ_CAB(__op, __numBits)                                    \
    do                                                                      \
    {                                                                       \
        const auto r0 = SCBE_CPU::isInt(__numBits) ? RAX : RCX;             \
        const auto r1 = SCBE_CPU::isInt(__numBits) ? XMM1 : RCX;            \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), r0, RDI, CPUBits::B64); \
        MK_IMMB(r1, __numBits);                                             \
        pp.emitOpNIndirect(0, r1, r0, __op, __numBits);                     \
    } while (0)

#define MK_BINOP_EQ_SCAB(__op, __numBits)                                                    \
    do                                                                                       \
    {                                                                                        \
        if (SCBE_CPU::isInt(__numBits) && ip->hasFlag(BCI_IMM_B))                            \
            pp.emitOpNIndirectDst(offsetStack + ip->a.u32, ip->b.u64, RDI, __op, __numBits); \
        else                                                                                 \
        {                                                                                    \
            const auto r1 = SCBE_CPU::isInt(__numBits) ? RAX : XMM1;                         \
            MK_IMMB(r1, __numBits);                                                          \
            pp.emitOpNIndirect(offsetStack + ip->a.u32, r1, RDI, __op, __numBits);           \
        }                                                                                    \
    } while (0)

#define MK_BINOP_EQ_SSCAB(__op, __numBits)                                     \
    do                                                                         \
    {                                                                          \
        const auto r1 = SCBE_CPU::isInt(__numBits) ? RAX : XMM1;               \
        pp.emitLoadNIndirect(offsetStack + ip->b.u32, r1, RDI, __numBits);     \
        pp.emitOpNIndirect(offsetStack + ip->a.u32, r1, RDI, __op, __numBits); \
    } while (0)

#define MK_BINOP_EQ_CAB_OFF(__op, __numBits)                                \
    do                                                                      \
    {                                                                       \
        const auto r0 = SCBE_CPU::isInt(__numBits) ? RAX : RCX;             \
        const auto r1 = SCBE_CPU::isInt(__numBits) ? RCX : XMM1;            \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), r0, RDI, CPUBits::B64); \
        MK_IMMB(r1, __numBits);                                             \
        pp.emitOpNIndirect(ip->c.u32, r1, r0, __op, __numBits);             \
    } while (0)

#define MK_BINOP_EQ_LOCK_CAB(__op, __numBits)                                \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB(RAX, __numBits);                                             \
        pp.emitOpNIndirect(0, RAX, RCX, __op, __numBits, true);              \
    } while (0)

//////////////////////////////////

#define MK_JMPCMP(__op, __numBits)                                                                                           \
    do                                                                                                                       \
    {                                                                                                                        \
        const auto r0 = SCBE_CPU::isInt(__numBits) ? RAX : XMM0;                                                             \
        const auto r1 = SCBE_CPU::isInt(__numBits) ? RCX : XMM1;                                                             \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                                                              \
        {                                                                                                                    \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), r0, RDI, __numBits);                                                 \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), r0, RDI, __numBits);                                                  \
        }                                                                                                                    \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C) && SCBE_CPU::isInt(__numBits) && ip->c.u64 <= 0x7fffffff) \
        {                                                                                                                    \
            pp.emitCmpNIndirectDst(REG_OFFSET(ip->a.u32), ip->c.u32, RDI, __numBits);                                        \
        }                                                                                                                    \
        else                                                                                                                 \
        {                                                                                                                    \
            MK_IMMA(r0, __numBits);                                                                                          \
            MK_IMMC(r1, __numBits);                                                                                          \
            pp.emitCmpN(r0, r1, __numBits);                                                                                  \
        }                                                                                                                    \
        pp.emitJump(__op, i, ip->b.s32);                                                                                     \
    } while (0)

#define MK_JMPCMP_ADDR(__op, __offset, __memReg, __numBits)                   \
    do                                                                        \
    {                                                                         \
        SWAG_ASSERT(SCBE_CPU::isInt(__numBits));                              \
        if (!ip->hasFlag(BCI_IMM_C))                                          \
        {                                                                     \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, __numBits);         \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, __numBits);  \
        }                                                                     \
        else if (ip->c.u64 <= 0x7fffffff)                                     \
        {                                                                     \
            pp.emitCmpNIndirectDst(__offset, ip->c.u32, __memReg, __numBits); \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, __numBits);         \
            MK_IMMC(RCX, __numBits);                                          \
            pp.emitCmpN(RAX, RCX, __numBits);                                 \
        }                                                                     \
        pp.emitJump(__op, i, ip->b.s32);                                      \
    } while (0)

#define MK_JMPCMP2(__op1, __op2, __numBits)                                    \
    do                                                                         \
    {                                                                          \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                \
        {                                                                      \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, __numBits); \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), XMM0, RDI, __numBits);  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            MK_IMMA(XMM0, __numBits);                                          \
            MK_IMMC(XMM1, __numBits);                                          \
            pp.emitCmpN(XMM0, XMM1, __numBits);                                \
        }                                                                      \
        pp.emitJump(__op1, i, ip->b.s32);                                      \
        pp.emitJump(__op2, i, ip->b.s32);                                      \
    } while (0)

#define MK_JMPCMP3(__op1, __op2, __numBits)                                    \
    do                                                                         \
    {                                                                          \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                \
        {                                                                      \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, __numBits); \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), XMM0, RDI, __numBits);  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            MK_IMMA(XMM0, __numBits);                                          \
            MK_IMMC(XMM1, __numBits);                                          \
            pp.emitCmpN(XMM0, XMM1, __numBits);                                \
        }                                                                      \
        pp.emitJump(__op1, i, 0);                                              \
        pp.emitJump(__op2, i, ip->b.s32);                                      \
    } while (0)
