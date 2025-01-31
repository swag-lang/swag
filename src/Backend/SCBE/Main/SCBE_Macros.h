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
