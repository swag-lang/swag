#pragma once
#define MK_IMMA_8(__reg)                                             \
    do                                                               \
    {                                                                \
        if (ip->hasFlag(BCI_IMM_A))                                  \
            pp.emitLoad8Immediate(__reg, ip->a.u8);                  \
        else                                                         \
            pp.emitLoad8Indirect(REG_OFFSET(ip->a.u32), __reg, RDI); \
    } while (0)

#define MK_IMMA_16(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_A))                                   \
            pp.emitLoad16Immediate(__reg, ip->a.u16);                 \
        else                                                          \
            pp.emitLoad16Indirect(REG_OFFSET(ip->a.u32), __reg, RDI); \
    } while (0)

#define MK_IMMA_32(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_A))                                   \
            pp.emitLoad32Immediate(__reg, ip->a.u32);                 \
        else                                                          \
            pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), __reg, RDI); \
    } while (0)

#define MK_IMMA_64(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_A))                                   \
            pp.emitLoad64Immediate(__reg, ip->a.u64);                 \
        else                                                          \
            pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), __reg, RDI); \
    } while (0)

#define MK_IMMA_F32(__reg)                                                         \
    do                                                                             \
    {                                                                              \
        if (ip->hasFlag(BCI_IMM_A))                                                \
        {                                                                          \
            pp.emitLoad32Immediate(RAX, ip->a.u32);                                \
            pp.emitCopyF32(__reg, RAX);                                            \
        }                                                                          \
        else                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), __reg, RDI, CPUBits::F32); \
    } while (0)

#define MK_IMMA_F64(__reg)                                                         \
    do                                                                             \
    {                                                                              \
        if (ip->hasFlag(BCI_IMM_A))                                                \
        {                                                                          \
            pp.emitLoad64Immediate(RAX, ip->a.u32);                                \
            pp.emitCopyF64(__reg, RAX);                                            \
        }                                                                          \
        else                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), __reg, RDI, CPUBits::F64); \
    } while (0)

#define MK_IMMB_8(__reg)                                             \
    do                                                               \
    {                                                                \
        if (ip->hasFlag(BCI_IMM_B))                                  \
            pp.emitLoad8Immediate(__reg, ip->b.u8);                  \
        else                                                         \
            pp.emitLoad8Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

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

#define MK_IMMB_16(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_B))                                   \
            pp.emitLoad16Immediate(__reg, ip->b.u16);                 \
        else                                                          \
            pp.emitLoad16Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_32(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_B))                                   \
            pp.emitLoad32Immediate(__reg, ip->b.u32);                 \
        else                                                          \
            pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_64(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_B))                                   \
            pp.emitLoad64Immediate(__reg, ip->b.u64);                 \
        else                                                          \
            pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_F32(__reg)                                                         \
    do                                                                             \
    {                                                                              \
        if (ip->hasFlag(BCI_IMM_B))                                                \
        {                                                                          \
            pp.emitLoad32Immediate(RAX, ip->b.u32);                                \
            pp.emitCopyF32(__reg, RAX);                                            \
        }                                                                          \
        else                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->b.u32), __reg, RDI, CPUBits::F32); \
    } while (0)

#define MK_IMMB_F64(__reg)                                                         \
    do                                                                             \
    {                                                                              \
        if (ip->hasFlag(BCI_IMM_B))                                                \
        {                                                                          \
            pp.emitLoad64Immediate(RAX, ip->b.u64);                                \
            pp.emitCopyF64(__reg, RAX);                                            \
        }                                                                          \
        else                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->b.u32), __reg, RDI, CPUBits::F64); \
    } while (0)

#define MK_IMMC_8(__reg)                                             \
    do                                                               \
    {                                                                \
        if (ip->hasFlag(BCI_IMM_C))                                  \
            pp.emitLoad8Immediate(__reg, ip->c.u8);                  \
        else                                                         \
            pp.emitLoad8Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
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

#define MK_IMMC_16(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_C))                                   \
            pp.emitLoad16Immediate(__reg, ip->c.u16);                 \
        else                                                          \
            pp.emitLoad16Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
    } while (0)

#define MK_IMMC_32(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_C))                                   \
            pp.emitLoad32Immediate(__reg, ip->c.u32);                 \
        else                                                          \
            pp.emitLoad32Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
    } while (0)

#define MK_IMMC_64(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_C))                                   \
            pp.emitLoad64Immediate(__reg, ip->c.u64);                 \
        else                                                          \
            pp.emitLoad64Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
    } while (0)

#define MK_IMMC_F32(__reg)                                                         \
    do                                                                             \
    {                                                                              \
        if (ip->hasFlag(BCI_IMM_C))                                                \
        {                                                                          \
            pp.emitLoad32Immediate(RAX, ip->c.u32);                                \
            pp.emitCopyF32(__reg, RAX);                                            \
        }                                                                          \
        else                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->c.u32), __reg, RDI, CPUBits::F32); \
    } while (0)

#define MK_IMMC_F64(__reg)                                                         \
    do                                                                             \
    {                                                                              \
        if (ip->hasFlag(BCI_IMM_C))                                                \
        {                                                                          \
            pp.emitLoad64Immediate(RAX, ip->c.u64);                                \
            pp.emitCopyF64(__reg, RAX);                                            \
        }                                                                          \
        else                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->c.u32), __reg, RDI, CPUBits::F64); \
    } while (0)

#define MK_IMMD_64(__reg)                                             \
    do                                                                \
    {                                                                 \
        if (ip->hasFlag(BCI_IMM_D))                                   \
            pp.emitLoad64Immediate(__reg, ip->d.u64);                 \
        else                                                          \
            pp.emitLoad64Indirect(REG_OFFSET(ip->d.u32), __reg, RDI); \
    } while (0)

#define MK_IMMD_F32(__reg)                                                         \
    do                                                                             \
    {                                                                              \
        if (ip->hasFlag(BCI_IMM_D))                                                \
        {                                                                          \
            pp.emitLoad32Immediate(RAX, ip->d.u32);                                \
            pp.emitCopyF32(__reg, RAX);                                            \
        }                                                                          \
        else                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->d.u32), __reg, RDI, CPUBits::F32); \
    } while (0)

#define MK_IMMD_F64(__reg)                                                         \
    do                                                                             \
    {                                                                              \
        if (ip->hasFlag(BCI_IMM_D))                                                \
        {                                                                          \
            pp.emitLoad64Immediate(RAX, ip->d.u64);                                \
            pp.emitCopyF64(__reg, RAX);                                            \
        }                                                                          \
        else                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->d.u32), __reg, RDI, CPUBits::F64); \
    } while (0)

//////////////////////////////////

#define MK_BINOP8_CAB(__opIndDst, __opInd, __op)                                \
    do                                                                          \
    {                                                                           \
        if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))                                \
        {                                                                       \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B8); \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B8);           \
        }                                                                       \
        else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))             \
        {                                                                       \
            pp.emitLoadNImmediate(RAX, ip->a.u64, CPUBits::B8);                 \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B8);           \
        }                                                                       \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))             \
        {                                                                       \
            pp.__opIndDst(REG_OFFSET(ip->a.u32), ip->b.u8, RDI, CPUBits::B8);   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            MK_IMMA_8(RAX);                                                     \
            MK_IMMB_8(RCX);                                                     \
            pp.__op(RAX, RCX, CPUBits::B8);                                     \
        }                                                                       \
    } while (0)

#define MK_BINOP16_CAB(__opIndDst, __opInd, __op)                                \
    do                                                                           \
    {                                                                            \
        if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))                                 \
        {                                                                        \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B16); \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B16);           \
        }                                                                        \
        else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))              \
        {                                                                        \
            pp.emitLoadNImmediate(RAX, ip->a.u64, CPUBits::B16);                 \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B16);           \
        }                                                                        \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))              \
        {                                                                        \
            pp.__opIndDst(REG_OFFSET(ip->a.u32), ip->b.u16, RDI, CPUBits::B16);  \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            MK_IMMA_16(RAX);                                                     \
            MK_IMMB_16(RCX);                                                     \
            pp.__op(RAX, RCX, CPUBits::B16);                                     \
        }                                                                        \
    } while (0)

#define MK_BINOP32_CAB(__opIndDst, __opInd, __op)                                \
    do                                                                           \
    {                                                                            \
        if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))                                 \
        {                                                                        \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B32); \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B32);           \
        }                                                                        \
        else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))              \
        {                                                                        \
            pp.emitLoadNImmediate(RAX, ip->a.u64, CPUBits::B32);                 \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B32);           \
        }                                                                        \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))              \
        {                                                                        \
            pp.__opIndDst(REG_OFFSET(ip->a.u32), ip->b.u32, RDI, CPUBits::B32);  \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            MK_IMMA_32(RAX);                                                     \
            MK_IMMB_32(RCX);                                                     \
            pp.__op(RAX, RCX, CPUBits::B32);                                     \
        }                                                                        \
    } while (0)

#define MK_BINOP64_CAB(__opIndDst, __opInd, __op)                                              \
    do                                                                                         \
    {                                                                                          \
        if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))                                               \
        {                                                                                      \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64);               \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B64);                         \
        }                                                                                      \
        else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))                            \
        {                                                                                      \
            pp.emitLoadNImmediate(RAX, ip->a.u64, CPUBits::B64);                               \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B64);                         \
        }                                                                                      \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF) \
        {                                                                                      \
            pp.__opIndDst(REG_OFFSET(ip->a.u32), ip->b.u32, RDI, CPUBits::B64);                \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            MK_IMMA_64(RAX);                                                                   \
            MK_IMMB_64(RCX);                                                                   \
            pp.__op(RAX, RCX, CPUBits::B64);                                                   \
        }                                                                                      \
    } while (0)

#define MK_BINOPF32_CAB(__opInd, __op)                                            \
    do                                                                            \
    {                                                                             \
        if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))                                  \
        {                                                                         \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, CPUBits::F32); \
            pp.__opInd(REG_OFFSET(ip->b.u32), XMM0, RDI);                         \
        }                                                                         \
        else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))               \
        {                                                                         \
            pp.emitLoadNImmediate(RAX, ip->a.u32, CPUBits::B32);                  \
            pp.emitCopyF32(XMM0, RAX);                                            \
            pp.__opInd(REG_OFFSET(ip->b.u32), XMM0, RDI);                         \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            MK_IMMA_F32(XMM0);                                                    \
            MK_IMMB_F32(XMM1);                                                    \
            pp.__op(XMM0, XMM1);                                                  \
        }                                                                         \
    } while (0)

#define MK_BINOPF64_CAB(__opInd, __op)                                            \
    do                                                                            \
    {                                                                             \
        if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))                                  \
        {                                                                         \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, CPUBits::F64); \
            pp.__opInd(REG_OFFSET(ip->b.u32), XMM0, RDI);                         \
        }                                                                         \
        else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))               \
        {                                                                         \
            pp.emitLoadNImmediate(RAX, ip->a.u64, CPUBits::B64);                  \
            pp.emitCopyF64(XMM0, RAX);                                            \
            pp.__opInd(REG_OFFSET(ip->b.u32), XMM0, RDI);                         \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            MK_IMMA_F64(XMM0);                                                    \
            MK_IMMB_F64(XMM1);                                                    \
            pp.__op(XMM0, XMM1);                                                  \
        }                                                                         \
    } while (0)

//////////////////////////////////

#define MK_BINOP_EQ8_CAB(__op)                                               \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64); \
        MK_IMMB_8(RCX);                                                      \
        pp.emitOpNIndirect(0, RCX, RAX, __op, CPUBits::B8);                  \
    } while (0)

#define MK_BINOP_EQ8_CAB_OFF(__op)                                           \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64); \
        MK_IMMB_8(RCX);                                                      \
        pp.emitOpNIndirect(ip->c.u32, RCX, RAX, __op, CPUBits::B8);          \
    } while (0)

#define MK_BINOP_EQN_SCAB(__op, __numBits)                                                   \
    do                                                                                       \
    {                                                                                        \
        if (ip->hasFlag(BCI_IMM_B))                                                          \
            pp.emitOpNIndirectDst(offsetStack + ip->a.u32, ip->b.u64, RDI, __op, __numBits); \
        else                                                                                 \
        {                                                                                    \
            pp.emitLoadNIndirect(REG_OFFSET(ip->b.u32), RAX, RDI, __numBits);                \
            pp.emitOpNIndirect(offsetStack + ip->a.u32, RAX, RDI, __op, __numBits);          \
        }                                                                                    \
    } while (0)

#define MK_BINOP_EQN_SSCAB(__op, __numBits)                                     \
    do                                                                          \
    {                                                                           \
        pp.emitLoadNIndirect(offsetStack + ip->b.u32, RAX, RDI, __numBits);     \
        pp.emitOpNIndirect(offsetStack + ip->a.u32, RAX, RDI, __op, __numBits); \
    } while (0)

//////////////////////////////////

#define MK_BINOP_EQ16_CAB(__op)                                              \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64); \
        MK_IMMB_16(RCX);                                                     \
        pp.emitOpNIndirect(0, RCX, RAX, __op, CPUBits::B16);                 \
    } while (0)

#define MK_BINOP_EQ16_CAB_OFF(__op)                                          \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64); \
        MK_IMMB_16(RCX);                                                     \
        pp.emitOpNIndirect(ip->c.u32, RCX, RAX, __op, CPUBits::B16);         \
    } while (0)

//////////////////////////////////

#define MK_BINOP_EQ32_CAB(__op)                                              \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64); \
        MK_IMMB_32(RCX);                                                     \
        pp.emitOpNIndirect(0, RCX, RAX, __op, CPUBits::B32);                 \
    } while (0)

#define MK_BINOP_EQ32_CAB_OFF(__op)                                          \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64); \
        MK_IMMB_32(RCX);                                                     \
        pp.emitOpNIndirect(ip->c.u32, RCX, RAX, __op, CPUBits::B32);         \
    } while (0)

//////////////////////////////////

#define MK_BINOP_EQ64_CAB(__op)                                              \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64); \
        MK_IMMB_64(RCX);                                                     \
        pp.emitOpNIndirect(0, RCX, RAX, __op, CPUBits::B64);                 \
    } while (0)

#define MK_BINOP_EQ64_CAB_OFF(__op)                                          \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64); \
        MK_IMMB_64(RCX);                                                     \
        pp.emitOpNIndirect(ip->c.u32, RCX, RAX, __op, CPUBits::B64);         \
    } while (0)

//////////////////////////////////

#define MK_BINOP_EQF32_CAB(__op)                                             \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB_F32(XMM1);                                                   \
        pp.emitOpF32Indirect(0, XMM1, RCX, __op);                            \
    } while (0)
#define MK_BINOP_EQF32_CAB_OFF(__op)                                         \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB_F32(XMM1);                                                   \
        pp.emitOpF32Indirect(ip->c.u32, XMM1, RCX, __op);                    \
    } while (0)

#define MK_BINOP_EQF32_SCAB(__op)                                      \
    do                                                                 \
    {                                                                  \
        pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI); \
        MK_IMMB_F32(XMM1);                                             \
        pp.emitOpF32Indirect(0, XMM1, RCX, __op);                      \
    } while (0)

#define MK_BINOP_EQF32_SSCAB(__op)                                              \
    do                                                                          \
    {                                                                           \
        pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);          \
        pp.emitLoadNIndirect(offsetStack + ip->b.u32, XMM1, RDI, CPUBits::F32); \
        pp.emitOpF32Indirect(0, XMM1, RCX, __op);                               \
    } while (0)

#define MK_BINOP_EQF64_CAB(__op)                                             \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB_F64(XMM1);                                                   \
        pp.emitOpF64Indirect(0, XMM1, RCX, __op);                            \
    } while (0)
#define MK_BINOP_EQF64_CAB_OFF(__op)                                         \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB_F64(XMM1);                                                   \
        pp.emitOpF64Indirect(ip->c.u32, XMM1, RCX, __op);                    \
    } while (0)

#define MK_BINOP_EQF64_SCAB(__op)                                      \
    do                                                                 \
    {                                                                  \
        pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI); \
        MK_IMMB_F64(XMM1);                                             \
        pp.emitOpF64Indirect(0, XMM1, RCX, __op);                      \
    } while (0)

#define MK_BINOP_EQF64_SSCAB(__op)                                              \
    do                                                                          \
    {                                                                           \
        pp.emitLoadAddressIndirect(offsetStack + ip->a.u32, RCX, RDI);          \
        pp.emitLoadNIndirect(offsetStack + ip->b.u32, XMM1, RDI, CPUBits::F64); \
        pp.emitOpF64Indirect(0, XMM1, RCX, __op);                               \
    } while (0)

//////////////////////////////////

#define MK_BINOP_EQ8_LOCK_CAB(__op)                                          \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB_8(RAX);                                                      \
        pp.emitOpNIndirect(0, RAX, RCX, __op, CPUBits::B8, true);            \
    } while (0)

#define MK_BINOP_EQ16_LOCK_CAB(__op)                                         \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB_16(RAX);                                                     \
        pp.emitOpNIndirect(0, RAX, RCX, __op, CPUBits::B16, true);           \
    } while (0)

#define MK_BINOP_EQ32_LOCK_CAB(__op)                                         \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB_32(RAX);                                                     \
        pp.emitOpNIndirect(0, RAX, RCX, __op, CPUBits::B32, true);           \
    } while (0)

#define MK_BINOP_EQ64_LOCK_CAB(__op)                                         \
    do                                                                       \
    {                                                                        \
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RCX, RDI, CPUBits::B64); \
        MK_IMMB_64(RAX);                                                     \
        pp.emitOpNIndirect(0, RAX, RCX, __op, CPUBits::B64, true);           \
    } while (0)

//////////////////////////////////

#define MK_JMPCMP_8(__op)                                                              \
    do                                                                                 \
    {                                                                                  \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                        \
        {                                                                              \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B8);        \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B8);         \
        }                                                                              \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))                    \
        {                                                                              \
            pp.emitCmpNIndirectDst(REG_OFFSET(ip->a.u32), ip->c.u8, RDI, CPUBits::B8); \
        }                                                                              \
        else                                                                           \
        {                                                                              \
            MK_IMMA_8(RAX);                                                            \
            MK_IMMC_8(RCX);                                                            \
            pp.emitCmpN(RAX, RCX, CPUBits::B8);                                        \
        }                                                                              \
        pp.emitJump(__op, i, ip->b.s32);                                               \
    } while (0)

#define MK_JMPCMP_16(__op)                                                               \
    do                                                                                   \
    {                                                                                    \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                          \
        {                                                                                \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B16);         \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B16);          \
        }                                                                                \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))                      \
        {                                                                                \
            pp.emitCmpNIndirectDst(REG_OFFSET(ip->a.u32), ip->c.u16, RDI, CPUBits::B16); \
        }                                                                                \
        else                                                                             \
        {                                                                                \
            MK_IMMA_16(RAX);                                                             \
            MK_IMMC_16(RCX);                                                             \
            pp.emitCmpN(RAX, RCX, CPUBits::B16);                                         \
        }                                                                                \
        pp.emitJump(__op, i, ip->b.s32);                                                 \
    } while (0)

#define MK_JMPCMP_32(__op)                                                               \
    do                                                                                   \
    {                                                                                    \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                          \
        {                                                                                \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B32);         \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B32);          \
        }                                                                                \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))                      \
        {                                                                                \
            pp.emitCmpNIndirectDst(REG_OFFSET(ip->a.u32), ip->c.u32, RDI, CPUBits::B32); \
        }                                                                                \
        else                                                                             \
        {                                                                                \
            MK_IMMA_32(RAX);                                                             \
            MK_IMMC_32(RCX);                                                             \
            pp.emitCmpN(RAX, RCX, CPUBits::B32);                                         \
        }                                                                                \
        pp.emitJump(__op, i, ip->b.s32);                                                 \
    } while (0)

#define MK_JMPCMP_64(__op)                                                                     \
    do                                                                                         \
    {                                                                                          \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                                \
        {                                                                                      \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, CPUBits::B64);               \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B64);                \
        }                                                                                      \
        else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C) && ip->c.u64 <= 0x7fffffff) \
        {                                                                                      \
            pp.emitCmpNIndirectDst(REG_OFFSET(ip->a.u32), ip->c.u32, RDI, CPUBits::B64);       \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            MK_IMMA_64(RAX);                                                                   \
            MK_IMMC_64(RCX);                                                                   \
            pp.emitCmpN(RAX, RCX, CPUBits::B64);                                               \
        }                                                                                      \
        pp.emitJump(__op, i, ip->b.s32);                                                       \
    } while (0)

#define MK_JMPCMP_ADDR_8(__op, __offset, __memReg)                              \
    do                                                                          \
    {                                                                           \
        if (!ip->hasFlag(BCI_IMM_C))                                            \
        {                                                                       \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, CPUBits::B8);         \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B8);  \
        }                                                                       \
        else if (ip->c.u64 <= 0x7f)                                             \
        {                                                                       \
            pp.emitCmpNIndirectDst(__offset, ip->c.u32, __memReg, CPUBits::B8); \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, CPUBits::B8);         \
            MK_IMMC_8(RCX);                                                     \
            pp.emitCmpN(RAX, RCX, CPUBits::B8);                                 \
        }                                                                       \
        pp.emitJump(__op, i, ip->b.s32);                                        \
    } while (0)

#define MK_JMPCMP_ADDR_16(__op, __offset, __memReg)                              \
    do                                                                           \
    {                                                                            \
        if (!ip->hasFlag(BCI_IMM_C))                                             \
        {                                                                        \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, CPUBits::B16);         \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B16);  \
        }                                                                        \
        else if (ip->c.u64 <= 0x7fff)                                            \
        {                                                                        \
            pp.emitCmpNIndirectDst(__offset, ip->c.u32, __memReg, CPUBits::B16); \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, CPUBits::B16);         \
            MK_IMMC_16(RCX);                                                     \
            pp.emitCmpN(RAX, RCX, CPUBits::B16);                                 \
        }                                                                        \
        pp.emitJump(__op, i, ip->b.s32);                                         \
    } while (0)

#define MK_JMPCMP_ADDR_32(__op, __offset, __memReg)                              \
    do                                                                           \
    {                                                                            \
        if (!ip->hasFlag(BCI_IMM_C))                                             \
        {                                                                        \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, CPUBits::B32);         \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B32);  \
        }                                                                        \
        else if (ip->c.u64 <= 0x7fffffff)                                        \
        {                                                                        \
            pp.emitCmpNIndirectDst(__offset, ip->c.u32, __memReg, CPUBits::B32); \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, CPUBits::B32);         \
            MK_IMMC_32(RCX);                                                     \
            pp.emitCmpN(RAX, RCX, CPUBits::B32);                                 \
        }                                                                        \
        pp.emitJump(__op, i, ip->b.s32);                                         \
    } while (0)

#define MK_JMPCMP_ADDR_64(__op, __offset, __memReg)                              \
    do                                                                           \
    {                                                                            \
        if (!ip->hasFlag(BCI_IMM_C))                                             \
        {                                                                        \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, CPUBits::B64);         \
            pp.emitCmpNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B64);  \
        }                                                                        \
        else if (ip->c.u64 <= 0x7fffffff)                                        \
        {                                                                        \
            pp.emitCmpNIndirectDst(__offset, ip->c.u32, __memReg, CPUBits::B64); \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            pp.emitLoadNIndirect(__offset, RAX, __memReg, CPUBits::B64);         \
            MK_IMMC_64(RCX);                                                     \
            pp.emitCmpN(RAX, RCX, CPUBits::B64);                                 \
        }                                                                        \
        pp.emitJump(__op, i, ip->b.s32);                                         \
    } while (0)

#define MK_JMPCMP_F32(__op)                                                       \
    do                                                                            \
    {                                                                             \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                   \
        {                                                                         \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, CPUBits::F32); \
            pp.emitCmpF32Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI);              \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            MK_IMMA_F32(XMM0);                                                    \
            MK_IMMC_F32(XMM1);                                                    \
            pp.emitCmpF32(XMM0, XMM1);                                            \
        }                                                                         \
        pp.emitJump(__op, i, ip->b.s32);                                          \
    } while (0)

#define MK_JMPCMP_F64(__op)                                                       \
    do                                                                            \
    {                                                                             \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                   \
        {                                                                         \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, CPUBits::F64); \
            pp.emitCmpF64Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI);              \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            MK_IMMA_F64(XMM0);                                                    \
            MK_IMMC_F64(XMM1);                                                    \
            pp.emitCmpF64(XMM0, XMM1);                                            \
        }                                                                         \
        pp.emitJump(__op, i, ip->b.s32);                                          \
    } while (0)

#define MK_JMPCMP2_F32(__op1, __op2)                                              \
    do                                                                            \
    {                                                                             \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                   \
        {                                                                         \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, CPUBits::F32); \
            pp.emitCmpF32Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI);              \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            MK_IMMA_F32(XMM0);                                                    \
            MK_IMMC_F32(XMM1);                                                    \
            pp.emitCmpF32(XMM0, XMM1);                                            \
        }                                                                         \
        pp.emitJump(__op1, i, ip->b.s32);                                         \
        pp.emitJump(__op2, i, ip->b.s32);                                         \
    } while (0)

#define MK_JMPCMP2_F64(__op1, __op2)                                              \
    do                                                                            \
    {                                                                             \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                   \
        {                                                                         \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, CPUBits::F64); \
            pp.emitCmpF64Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI);              \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            MK_IMMA_F64(XMM0);                                                    \
            MK_IMMC_F64(XMM1);                                                    \
            pp.emitCmpF64(XMM0, XMM1);                                            \
        }                                                                         \
        pp.emitJump(__op1, i, ip->b.s32);                                         \
        pp.emitJump(__op2, i, ip->b.s32);                                         \
    } while (0)

#define MK_JMPCMP3_F32(__op1, __op2)                                              \
    do                                                                            \
    {                                                                             \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                   \
        {                                                                         \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, CPUBits::F32); \
            pp.emitCmpF32Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI);              \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            MK_IMMA_F32(XMM0);                                                    \
            MK_IMMC_F32(XMM1);                                                    \
            pp.emitCmpF32(XMM0, XMM1);                                            \
        }                                                                         \
        pp.emitJump(__op1, i, 0);                                                 \
        pp.emitJump(__op2, i, ip->b.s32);                                         \
    } while (0)

#define MK_JMPCMP3_F64(__op1, __op2)                                              \
    do                                                                            \
    {                                                                             \
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))                   \
        {                                                                         \
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), XMM0, RDI, CPUBits::F64); \
            pp.emitCmpF64Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI);              \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            MK_IMMA_F64(XMM0);                                                    \
            MK_IMMC_F64(XMM1);                                                    \
            pp.emitCmpF64(XMM0, XMM1);                                            \
        }                                                                         \
        pp.emitJump(__op1, i, 0);                                                 \
        pp.emitJump(__op2, i, ip->b.s32);                                         \
    } while (0)
