#pragma once
#define MK_IMMB_U8_TO_U32(__reg)                                                                           \
    do                                                                                                     \
    {                                                                                                      \
        if (ip->hasFlag(BCI_IMM_B))                                                                        \
            pp.emitLoadImmediate(__reg, ip->b.u8, CPUBits::B32);                                           \
        else                                                                                               \
            pp.emitLoadIndirect(CPUSignedType::U8, CPUSignedType::U32, __reg, RDI, REG_OFFSET(ip->b.u32)); \
    } while (0)

#define MK_IMMB_U8_TO_U16(__reg)                                                                           \
    do                                                                                                     \
    {                                                                                                      \
        if (ip->hasFlag(BCI_IMM_B))                                                                        \
            pp.emitLoadImmediate(__reg, ip->b.u8, CPUBits::B16);                                           \
        else                                                                                               \
            pp.emitLoadIndirect(CPUSignedType::U8, CPUSignedType::U32, __reg, RDI, REG_OFFSET(ip->b.u32)); \
    } while (0)

#define MK_IMMC_U8_TO_U32(__reg)                                                                           \
    do                                                                                                     \
    {                                                                                                      \
        if (ip->hasFlag(BCI_IMM_C))                                                                        \
            pp.emitLoadImmediate(__reg, ip->c.u8, CPUBits::B32);                                           \
        else                                                                                               \
            pp.emitLoadIndirect(CPUSignedType::U8, CPUSignedType::U32, __reg, RDI, REG_OFFSET(ip->c.u32)); \
    } while (0)
