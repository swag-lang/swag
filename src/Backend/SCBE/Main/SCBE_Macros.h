#pragma once
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
