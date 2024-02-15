#pragma once
#define MK_IMMA_8(__reg)                                          \
    do                                                            \
    {                                                             \
        if (ip->hasFlag(BCI_IMM_A))                                \
            pp.emit_Load8_Immediate(__reg, ip->a.u8);             \
        else                                                      \
            pp.emit_Load8_Indirect(REG_OFFSET(ip->a.u32), __reg); \
    } while (0)

#define MK_IMMA_16(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_A))                                 \
            pp.emit_Load16_Immediate(__reg, ip->a.u16);            \
        else                                                       \
            pp.emit_Load16_Indirect(REG_OFFSET(ip->a.u32), __reg); \
    } while (0)

#define MK_IMMA_32(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_A))                                 \
            pp.emit_Load32_Immediate(__reg, ip->a.u32);            \
        else                                                       \
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), __reg); \
    } while (0)

#define MK_IMMA_64(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_A))                                 \
            pp.emit_Load64_Immediate(__reg, ip->a.u64);            \
        else                                                       \
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), __reg); \
    } while (0)

#define MK_IMMB_64(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_B))                                 \
            pp.emit_Load64_Immediate(__reg, ip->b.u64);            \
        else                                                       \
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), __reg); \
    } while (0)

#define MK_IMMA_F32(__reg)                                          \
    do                                                              \
    {                                                               \
        if (ip->hasFlag(BCI_IMM_A))                                  \
        {                                                           \
            pp.emit_Load32_Immediate(RAX, ip->a.u32);               \
            pp.emit_CopyF32(__reg, RAX);                            \
        }                                                           \
        else                                                        \
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->a.u32), __reg); \
    } while (0)

#define MK_IMMA_F64(__reg)                                          \
    do                                                              \
    {                                                               \
        if (ip->hasFlag(BCI_IMM_A))                                  \
        {                                                           \
            pp.emit_Load64_Immediate(RAX, ip->a.u32);               \
            pp.emit_CopyF64(__reg, RAX);                            \
        }                                                           \
        else                                                        \
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->a.u32), __reg); \
    } while (0)

#define MK_IMMB_8(__reg)                                          \
    do                                                            \
    {                                                             \
        if (ip->hasFlag(BCI_IMM_B))                                \
            pp.emit_Load8_Immediate(__reg, ip->b.u8);             \
        else                                                      \
            pp.emit_Load8_Indirect(REG_OFFSET(ip->b.u32), __reg); \
    } while (0)

#define MK_IMMB_S8_TO_S32(__reg)                                           \
    do                                                                     \
    {                                                                      \
        if (ip->hasFlag(BCI_IMM_B))                                         \
            pp.emit_Load32_Immediate(__reg, (int32_t) ip->b.s8);           \
        else                                                               \
            pp.emit_LoadS8S32_Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_U8_TO_U32(__reg)                                           \
    do                                                                     \
    {                                                                      \
        if (ip->hasFlag(BCI_IMM_B))                                         \
            pp.emit_Load32_Immediate(__reg, ip->b.u8);                     \
        else                                                               \
            pp.emit_LoadU8U32_Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_U8_TO_U16(__reg)                                           \
    do                                                                     \
    {                                                                      \
        if (ip->hasFlag(BCI_IMM_B))                                         \
            pp.emit_Load16_Immediate(__reg, ip->b.u8);                     \
        else                                                               \
            pp.emit_LoadU8U32_Indirect(REG_OFFSET(ip->b.u32), __reg, RDI); \
    } while (0)

#define MK_IMMB_16(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_B))                                 \
            pp.emit_Load16_Immediate(__reg, ip->b.u16);            \
        else                                                       \
            pp.emit_Load16_Indirect(REG_OFFSET(ip->b.u32), __reg); \
    } while (0)

#define MK_IMMB_32(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_B))                                 \
            pp.emit_Load32_Immediate(__reg, ip->b.u32);            \
        else                                                       \
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), __reg); \
    } while (0)

#define MK_IMMB_64(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_B))                                 \
            pp.emit_Load64_Immediate(__reg, ip->b.u64);            \
        else                                                       \
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), __reg); \
    } while (0)

#define MK_IMMB_F32(__reg)                                          \
    do                                                              \
    {                                                               \
        if (ip->hasFlag(BCI_IMM_B))                                  \
        {                                                           \
            pp.emit_Load32_Immediate(RAX, ip->b.u32);               \
            pp.emit_CopyF32(__reg, RAX);                            \
        }                                                           \
        else                                                        \
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->b.u32), __reg); \
    } while (0)

#define MK_IMMB_F64(__reg)                                          \
    do                                                              \
    {                                                               \
        if (ip->hasFlag(BCI_IMM_B))                                  \
        {                                                           \
            pp.emit_Load64_Immediate(RAX, ip->b.u64);               \
            pp.emit_CopyF64(__reg, RAX);                            \
        }                                                           \
        else                                                        \
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->b.u32), __reg); \
    } while (0)

#define MK_IMMC_8(__reg)                                          \
    do                                                            \
    {                                                             \
        if (ip->hasFlag(BCI_IMM_C))                                \
            pp.emit_Load8_Immediate(__reg, ip->c.u8);             \
        else                                                      \
            pp.emit_Load8_Indirect(REG_OFFSET(ip->c.u32), __reg); \
    } while (0)

#define MK_IMMC_S8_TO_S32(__reg)                                           \
    do                                                                     \
    {                                                                      \
        if (ip->hasFlag(BCI_IMM_C))                                         \
            pp.emit_Load32_Immediate(__reg, (int32_t) ip->c.s8);           \
        else                                                               \
            pp.emit_LoadS8S32_Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
    } while (0)

#define MK_IMMC_U8_TO_U32(__reg)                                           \
    do                                                                     \
    {                                                                      \
        if (ip->hasFlag(BCI_IMM_C))                                         \
            pp.emit_Load32_Immediate(__reg, ip->c.u8);                     \
        else                                                               \
            pp.emit_LoadU8U32_Indirect(REG_OFFSET(ip->c.u32), __reg, RDI); \
    } while (0)

#define MK_IMMC_16(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_C))                                 \
            pp.emit_Load16_Immediate(__reg, ip->c.u16);            \
        else                                                       \
            pp.emit_Load16_Indirect(REG_OFFSET(ip->c.u32), __reg); \
    } while (0)

#define MK_IMMC_32(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_C))                                 \
            pp.emit_Load32_Immediate(__reg, ip->c.u32);            \
        else                                                       \
            pp.emit_Load32_Indirect(REG_OFFSET(ip->c.u32), __reg); \
    } while (0)

#define MK_IMMC_64(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_C))                                 \
            pp.emit_Load64_Immediate(__reg, ip->c.u64);            \
        else                                                       \
            pp.emit_Load64_Indirect(REG_OFFSET(ip->c.u32), __reg); \
    } while (0)

#define MK_IMMC_F32(__reg)                                          \
    do                                                              \
    {                                                               \
        if (ip->hasFlag(BCI_IMM_C))                                  \
        {                                                           \
            pp.emit_Load32_Immediate(RAX, ip->c.u32);               \
            pp.emit_CopyF32(__reg, RAX);                            \
        }                                                           \
        else                                                        \
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->c.u32), __reg); \
    } while (0)

#define MK_IMMC_F64(__reg)                                          \
    do                                                              \
    {                                                               \
        if (ip->hasFlag(BCI_IMM_C))                                  \
        {                                                           \
            pp.emit_Load64_Immediate(RAX, ip->c.u64);               \
            pp.emit_CopyF64(__reg, RAX);                            \
        }                                                           \
        else                                                        \
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->c.u32), __reg); \
    } while (0)

#define MK_IMMD_64(__reg)                                          \
    do                                                             \
    {                                                              \
        if (ip->hasFlag(BCI_IMM_D))                                 \
            pp.emit_Load64_Immediate(__reg, ip->d.u64);            \
        else                                                       \
            pp.emit_Load64_Indirect(REG_OFFSET(ip->d.u32), __reg); \
    } while (0)

#define MK_IMMD_F32(__reg)                                          \
    do                                                              \
    {                                                               \
        if (ip->hasFlag(BCI_IMM_D))                                  \
        {                                                           \
            pp.emit_Load32_Immediate(RAX, ip->d.u32);               \
            pp.emit_CopyF32(__reg, RAX);                            \
        }                                                           \
        else                                                        \
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->d.u32), __reg); \
    } while (0)

#define MK_IMMD_F64(__reg)                                          \
    do                                                              \
    {                                                               \
        if (ip->hasFlag(BCI_IMM_D))                                  \
        {                                                           \
            pp.emit_Load64_Immediate(RAX, ip->d.u64);               \
            pp.emit_CopyF64(__reg, RAX);                            \
        }                                                           \
        else                                                        \
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->d.u32), __reg); \
    } while (0)

//////////////////////////////////

#define MK_BINOP8_CAB(__opIndDst, __opInd, __op)                         \
    do                                                                   \
    {                                                                    \
        if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                      \
        {                                                                \
            pp.emit_Load8_Indirect(REG_OFFSET(ip->a.u32), RAX);          \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B8);    \
        }                                                                \
        else if ((ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_B)))    \
        {                                                                \
            pp.emit_Load8_Immediate(RAX, ip->a.u8);                      \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B8);    \
        }                                                                \
        else if (!(ip->hasFlag(BCI_IMM_A)) && (ip->hasFlag(BCI_IMM_B)))    \
        {                                                                \
            pp.__opIndDst(REG_OFFSET(ip->a.u32), ip->b.u8, CPUBits::B8); \
        }                                                                \
        else                                                             \
        {                                                                \
            MK_IMMA_8(RAX);                                              \
            MK_IMMB_8(RCX);                                              \
            pp.__op(RAX, RCX, CPUBits::B8);                              \
        }                                                                \
    } while (0)

#define MK_BINOP16_CAB(__opIndDst, __opInd, __op)                          \
    do                                                                     \
    {                                                                      \
        if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                        \
        {                                                                  \
            pp.emit_Load16_Indirect(REG_OFFSET(ip->a.u32), RAX);           \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B16);     \
        }                                                                  \
        else if ((ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_B)))      \
        {                                                                  \
            pp.emit_Load16_Immediate(RAX, ip->a.u16);                      \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B16);     \
        }                                                                  \
        else if (!(ip->hasFlag(BCI_IMM_A)) && (ip->hasFlag(BCI_IMM_B)))      \
        {                                                                  \
            pp.__opIndDst(REG_OFFSET(ip->a.u32), ip->b.u16, CPUBits::B16); \
        }                                                                  \
        else                                                               \
        {                                                                  \
            MK_IMMA_16(RAX);                                               \
            MK_IMMB_16(RCX);                                               \
            pp.__op(RAX, RCX, CPUBits::B16);                               \
        }                                                                  \
    } while (0)

#define MK_BINOP32_CAB(__opIndDst, __opInd, __op)                          \
    do                                                                     \
    {                                                                      \
        if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                        \
        {                                                                  \
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);           \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B32);     \
        }                                                                  \
        else if ((ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_B)))      \
        {                                                                  \
            pp.emit_Load32_Immediate(RAX, ip->a.u32);                      \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B32);     \
        }                                                                  \
        else if (!(ip->hasFlag(BCI_IMM_A)) && (ip->hasFlag(BCI_IMM_B)))      \
        {                                                                  \
            pp.__opIndDst(REG_OFFSET(ip->a.u32), ip->b.u32, CPUBits::B32); \
        }                                                                  \
        else                                                               \
        {                                                                  \
            MK_IMMA_32(RAX);                                               \
            MK_IMMB_32(RCX);                                               \
            pp.__op(RAX, RCX, CPUBits::B32);                               \
        }                                                                  \
    } while (0)

#define MK_BINOP64_CAB(__opIndDst, __opInd, __op)                                                \
    do                                                                                           \
    {                                                                                            \
        if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                                              \
        {                                                                                        \
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);                                 \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B64);                           \
        }                                                                                        \
        else if ((ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_B)))                            \
        {                                                                                        \
            pp.emit_Load64_Immediate(RAX, ip->a.u64);                                            \
            pp.__opInd(REG_OFFSET(ip->b.u32), RAX, RDI, CPUBits::B64);                           \
        }                                                                                        \
        else if (!(ip->hasFlag(BCI_IMM_A)) && (ip->hasFlag(BCI_IMM_B)) && ip->b.u64 <= 0x7FFFFFFF) \
        {                                                                                        \
            pp.__opIndDst(REG_OFFSET(ip->a.u32), ip->b.u32, CPUBits::B64);                       \
        }                                                                                        \
        else                                                                                     \
        {                                                                                        \
            MK_IMMA_64(RAX);                                                                     \
            MK_IMMB_64(RCX);                                                                     \
            pp.__op(RAX, RCX, CPUBits::B64);                                                     \
        }                                                                                        \
    } while (0)

#define MK_BINOPF32_CAB(__opInd, __op)                                \
    do                                                                \
    {                                                                 \
        if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                   \
        {                                                             \
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);    \
            pp.__opInd(REG_OFFSET(ip->b.u32), XMM0, RDI);             \
        }                                                             \
        else if ((ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_B))) \
        {                                                             \
            pp.emit_Load32_Immediate(RAX, ip->a.u32);                 \
            pp.emit_CopyF32(XMM0, RAX);                               \
            pp.__opInd(REG_OFFSET(ip->b.u32), XMM0, RDI);             \
        }                                                             \
        else                                                          \
        {                                                             \
            MK_IMMA_F32(XMM0);                                        \
            MK_IMMB_F32(XMM1);                                        \
            pp.__op(XMM0, XMM1);                                      \
        }                                                             \
    } while (0)

#define MK_BINOPF64_CAB(__opInd, __op)                                \
    do                                                                \
    {                                                                 \
        if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                   \
        {                                                             \
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);    \
            pp.__opInd(REG_OFFSET(ip->b.u32), XMM0, RDI);             \
        }                                                             \
        else if ((ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_B))) \
        {                                                             \
            pp.emit_Load64_Immediate(RAX, ip->a.u64);                 \
            pp.emit_CopyF64(XMM0, RAX);                               \
            pp.__opInd(REG_OFFSET(ip->b.u32), XMM0, RDI);             \
        }                                                             \
        else                                                          \
        {                                                             \
            MK_IMMA_F64(XMM0);                                        \
            MK_IMMB_F64(XMM1);                                        \
            pp.__op(XMM0, XMM1);                                      \
        }                                                             \
    } while (0)

//////////////////////////////////

#define MK_BINOPEQ8_CAB(__op)                                 \
    do                                                        \
    {                                                         \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);  \
        MK_IMMB_8(RCX);                                       \
        pp.emit_OpN_Indirect(0, RCX, RAX, __op, CPUBits::B8); \
    } while (0)

#define MK_BINOPEQ8_SCAB(__op)                                                                   \
    do                                                                                           \
    {                                                                                            \
        if (ip->hasFlag(BCI_IMM_B))                                                               \
            pp.emit_OpN_IndirectDst(offsetStack + ip->a.u32, ip->b.u64, RDI, __op, CPUBits::B8); \
        else                                                                                     \
        {                                                                                        \
            pp.emit_Load8_Indirect(REG_OFFSET(ip->b.u32), RAX);                                  \
            pp.emit_OpN_Indirect(offsetStack + ip->a.u32, RAX, RDI, __op, CPUBits::B8);          \
        }                                                                                        \
    } while (0)

#define MK_BINOPEQ8_SSCAB(__op)                                                     \
    do                                                                              \
    {                                                                               \
        pp.emit_Load8_Indirect(offsetStack + ip->b.u32, RAX, RDI);                  \
        pp.emit_OpN_Indirect(offsetStack + ip->a.u32, RAX, RDI, __op, CPUBits::B8); \
    } while (0)

//////////////////////////////////

#define MK_BINOPEQ16_CAB(__op)                                 \
    do                                                         \
    {                                                          \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);   \
        MK_IMMB_16(RCX);                                       \
        pp.emit_OpN_Indirect(0, RCX, RAX, __op, CPUBits::B16); \
    } while (0)

#define MK_BINOPEQ16_SCAB(__op)                                                                   \
    do                                                                                            \
    {                                                                                             \
        if (ip->hasFlag(BCI_IMM_B))                                                                \
            pp.emit_OpN_IndirectDst(offsetStack + ip->a.u32, ip->b.u64, RDI, __op, CPUBits::B16); \
        else                                                                                      \
        {                                                                                         \
            pp.emit_Load16_Indirect(REG_OFFSET(ip->b.u32), RAX);                                  \
            pp.emit_OpN_Indirect(offsetStack + ip->a.u32, RAX, RDI, __op, CPUBits::B16);          \
        }                                                                                         \
    } while (0)

#define MK_BINOPEQ16_SSCAB(__op)                                                     \
    do                                                                               \
    {                                                                                \
        pp.emit_Load16_Indirect(offsetStack + ip->b.u32, RAX, RDI);                  \
        pp.emit_OpN_Indirect(offsetStack + ip->a.u32, RAX, RDI, __op, CPUBits::B16); \
    } while (0)

//////////////////////////////////

#define MK_BINOPEQ32_CAB(__op)                                 \
    do                                                         \
    {                                                          \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);   \
        MK_IMMB_32(RCX);                                       \
        pp.emit_OpN_Indirect(0, RCX, RAX, __op, CPUBits::B32); \
    } while (0)

#define MK_BINOPEQ32_SCAB(__op)                                                                   \
    do                                                                                            \
    {                                                                                             \
        if (ip->hasFlag(BCI_IMM_B))                                                                \
            pp.emit_OpN_IndirectDst(offsetStack + ip->a.u32, ip->b.u64, RDI, __op, CPUBits::B32); \
        else                                                                                      \
        {                                                                                         \
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RAX);                                  \
            pp.emit_OpN_Indirect(offsetStack + ip->a.u32, RAX, RDI, __op, CPUBits::B32);          \
        }                                                                                         \
    } while (0)

#define MK_BINOPEQ32_SSCAB(__op)                                                     \
    do                                                                               \
    {                                                                                \
        pp.emit_Load32_Indirect(offsetStack + ip->b.u32, RAX, RDI);                  \
        pp.emit_OpN_Indirect(offsetStack + ip->a.u32, RAX, RDI, __op, CPUBits::B32); \
    } while (0)

//////////////////////////////////

#define MK_BINOPEQ64_CAB(__op)                                 \
    do                                                         \
    {                                                          \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);   \
        MK_IMMB_64(RCX);                                       \
        pp.emit_OpN_Indirect(0, RCX, RAX, __op, CPUBits::B64); \
    } while (0)

#define MK_BINOPEQ64_SCAB(__op)                                                                   \
    do                                                                                            \
    {                                                                                             \
        if (ip->hasFlag(BCI_IMM_B))                                                                \
            pp.emit_OpN_IndirectDst(offsetStack + ip->a.u32, ip->b.u64, RDI, __op, CPUBits::B64); \
        else                                                                                      \
        {                                                                                         \
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);                                  \
            pp.emit_OpN_Indirect(offsetStack + ip->a.u32, RAX, RDI, __op, CPUBits::B64);          \
        }                                                                                         \
    } while (0)

#define MK_BINOPEQ64_SSCAB(__op)                                                     \
    do                                                                               \
    {                                                                                \
        pp.emit_Load64_Indirect(offsetStack + ip->b.u32, RAX, RDI);                  \
        pp.emit_OpN_Indirect(offsetStack + ip->a.u32, RAX, RDI, __op, CPUBits::B64); \
    } while (0)

//////////////////////////////////

#define MK_BINOPEQF32_CAB(__op)                              \
    do                                                       \
    {                                                        \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX); \
        MK_IMMB_F32(XMM1);                                   \
        pp.emit_OpF32_Indirect(XMM1, RCX, __op);             \
    } while (0)

#define MK_BINOPEQF32_SCAB(__op)                                         \
    do                                                                   \
    {                                                                    \
        pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI); \
        MK_IMMB_F32(XMM1);                                               \
        pp.emit_OpF32_Indirect(XMM1, RCX, __op);                         \
    } while (0)

#define MK_BINOPEQF32_SSCAB(__op)                                        \
    do                                                                   \
    {                                                                    \
        pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI); \
        pp.emit_LoadF32_Indirect(offsetStack + ip->b.u32, XMM1, RDI);    \
        pp.emit_OpF32_Indirect(XMM1, RCX, __op);                         \
    } while (0)

#define MK_BINOPEQF64_CAB(__op)                              \
    do                                                       \
    {                                                        \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX); \
        MK_IMMB_F64(XMM1);                                   \
        pp.emit_OpF64_Indirect(XMM1, RCX, __op);             \
    } while (0)

#define MK_BINOPEQF64_SCAB(__op)                                         \
    do                                                                   \
    {                                                                    \
        pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI); \
        MK_IMMB_F64(XMM1);                                               \
        pp.emit_OpF64_Indirect(XMM1, RCX, __op);                         \
    } while (0)

#define MK_BINOPEQF64_SSCAB(__op)                                        \
    do                                                                   \
    {                                                                    \
        pp.emit_LoadAddress_Indirect(offsetStack + ip->a.u32, RCX, RDI); \
        pp.emit_LoadF64_Indirect(offsetStack + ip->b.u32, XMM1, RDI);    \
        pp.emit_OpF64_Indirect(XMM1, RCX, __op);                         \
    } while (0)

//////////////////////////////////

#define MK_BINOPEQ8_LOCK_CAB(__op)                                  \
    do                                                              \
    {                                                               \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);        \
        MK_IMMB_8(RAX);                                             \
        pp.emit_OpN_Indirect(0, RAX, RCX, __op, CPUBits::B8, true); \
    } while (0)

#define MK_BINOPEQ16_LOCK_CAB(__op)                                  \
    do                                                               \
    {                                                                \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);         \
        MK_IMMB_16(RAX);                                             \
        pp.emit_OpN_Indirect(0, RAX, RCX, __op, CPUBits::B16, true); \
    } while (0)

#define MK_BINOPEQ32_LOCK_CAB(__op)                                  \
    do                                                               \
    {                                                                \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);         \
        MK_IMMB_32(RAX);                                             \
        pp.emit_OpN_Indirect(0, RAX, RCX, __op, CPUBits::B32, true); \
    } while (0)

#define MK_BINOPEQ64_LOCK_CAB(__op)                                  \
    do                                                               \
    {                                                                \
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);         \
        MK_IMMB_64(RAX);                                             \
        pp.emit_OpN_Indirect(0, RAX, RCX, __op, CPUBits::B64, true); \
    } while (0)

//////////////////////////////////

#define MK_JMPCMP_8(__op)                                                           \
    do                                                                              \
    {                                                                               \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))                   \
        {                                                                           \
            pp.emit_Load8_Indirect(REG_OFFSET(ip->a.u32), RAX);                     \
            pp.emit_CmpN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B8);    \
        }                                                                           \
        else if (!(ip->hasFlag(BCI_IMM_A)) && (ip->hasFlag(BCI_IMM_C)))               \
        {                                                                           \
            pp.emit_CmpN_IndirectDst(REG_OFFSET(ip->a.u32), ip->c.u8, CPUBits::B8); \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            MK_IMMA_8(RAX);                                                         \
            MK_IMMC_8(RCX);                                                         \
            pp.emit_CmpN(RAX, RCX, CPUBits::B8);                                    \
        }                                                                           \
        pp.emit_Jump(__op, i, ip->b.s32);                                           \
    } while (0)

#define MK_JMPCMP_16(__op)                                                            \
    do                                                                                \
    {                                                                                 \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))                     \
        {                                                                             \
            pp.emit_Load16_Indirect(REG_OFFSET(ip->a.u32), RAX);                      \
            pp.emit_CmpN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B16);     \
        }                                                                             \
        else if (!(ip->hasFlag(BCI_IMM_A)) && (ip->hasFlag(BCI_IMM_C)))                 \
        {                                                                             \
            pp.emit_CmpN_IndirectDst(REG_OFFSET(ip->a.u32), ip->c.u16, CPUBits::B16); \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            MK_IMMA_16(RAX);                                                          \
            MK_IMMC_16(RCX);                                                          \
            pp.emit_CmpN(RAX, RCX, CPUBits::B16);                                     \
        }                                                                             \
        pp.emit_Jump(__op, i, ip->b.s32);                                             \
    } while (0)

#define MK_JMPCMP_32(__op)                                                            \
    do                                                                                \
    {                                                                                 \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))                     \
        {                                                                             \
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);                      \
            pp.emit_CmpN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B32);     \
        }                                                                             \
        else if (!(ip->hasFlag(BCI_IMM_A)) && (ip->hasFlag(BCI_IMM_C)))                 \
        {                                                                             \
            pp.emit_CmpN_IndirectDst(REG_OFFSET(ip->a.u32), ip->c.u32, CPUBits::B32); \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            MK_IMMA_32(RAX);                                                          \
            MK_IMMC_32(RCX);                                                          \
            pp.emit_CmpN(RAX, RCX, CPUBits::B32);                                     \
        }                                                                             \
        pp.emit_Jump(__op, i, ip->b.s32);                                             \
    } while (0)

#define MK_JMPCMP_64(__op)                                                                       \
    do                                                                                           \
    {                                                                                            \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))                                \
        {                                                                                        \
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);                                 \
            pp.emit_CmpN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, CPUBits::B64);                \
        }                                                                                        \
        else if (!(ip->hasFlag(BCI_IMM_A)) && (ip->hasFlag(BCI_IMM_C)) && ip->c.u64 <= 0x7fffffff) \
        {                                                                                        \
            pp.emit_CmpN_IndirectDst(REG_OFFSET(ip->a.u32), ip->c.u32, CPUBits::B64);            \
        }                                                                                        \
        else                                                                                     \
        {                                                                                        \
            MK_IMMA_64(RAX);                                                                     \
            MK_IMMC_64(RCX);                                                                     \
            pp.emit_CmpN(RAX, RCX, CPUBits::B64);                                                \
        }                                                                                        \
        pp.emit_Jump(__op, i, ip->b.s32);                                                        \
    } while (0)

#define MK_JMPCMP_F32(__op)                                            \
    do                                                                 \
    {                                                                  \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))      \
        {                                                              \
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);     \
            pp.emit_CmpF32_Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI); \
        }                                                              \
        else                                                           \
        {                                                              \
            MK_IMMA_F32(XMM0);                                         \
            MK_IMMC_F32(XMM1);                                         \
            pp.emit_CmpF32(XMM0, XMM1);                                \
        }                                                              \
        pp.emit_Jump(__op, i, ip->b.s32);                              \
    } while (0)

#define MK_JMPCMP_F64(__op)                                            \
    do                                                                 \
    {                                                                  \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))      \
        {                                                              \
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);     \
            pp.emit_CmpF64_Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI); \
        }                                                              \
        else                                                           \
        {                                                              \
            MK_IMMA_F64(XMM0);                                         \
            MK_IMMC_F64(XMM1);                                         \
            pp.emit_CmpF64(XMM0, XMM1);                                \
        }                                                              \
        pp.emit_Jump(__op, i, ip->b.s32);                              \
    } while (0)

#define MK_JMPCMP2_F32(__op1, __op2)                                   \
    do                                                                 \
    {                                                                  \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))      \
        {                                                              \
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);     \
            pp.emit_CmpF32_Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI); \
        }                                                              \
        else                                                           \
        {                                                              \
            MK_IMMA_F32(XMM0);                                         \
            MK_IMMC_F32(XMM1);                                         \
            pp.emit_CmpF32(XMM0, XMM1);                                \
        }                                                              \
        pp.emit_Jump(__op1, i, ip->b.s32);                             \
        pp.emit_Jump(__op2, i, ip->b.s32);                             \
    } while (0)

#define MK_JMPCMP2_F64(__op1, __op2)                                   \
    do                                                                 \
    {                                                                  \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))      \
        {                                                              \
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);     \
            pp.emit_CmpF64_Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI); \
        }                                                              \
        else                                                           \
        {                                                              \
            MK_IMMA_F64(XMM0);                                         \
            MK_IMMC_F64(XMM1);                                         \
            pp.emit_CmpF64(XMM0, XMM1);                                \
        }                                                              \
        pp.emit_Jump(__op1, i, ip->b.s32);                             \
        pp.emit_Jump(__op2, i, ip->b.s32);                             \
    } while (0)

#define MK_JMPCMP3_F32(__op1, __op2)                                   \
    do                                                                 \
    {                                                                  \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))      \
        {                                                              \
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);     \
            pp.emit_CmpF32_Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI); \
        }                                                              \
        else                                                           \
        {                                                              \
            MK_IMMA_F32(XMM0);                                         \
            MK_IMMC_F32(XMM1);                                         \
            pp.emit_CmpF32(XMM0, XMM1);                                \
        }                                                              \
        pp.emit_Jump(__op1, i, 0);                                     \
        pp.emit_Jump(__op2, i, ip->b.s32);                             \
    } while (0)

#define MK_JMPCMP3_F64(__op1, __op2)                                   \
    do                                                                 \
    {                                                                  \
        if (!(ip->hasFlag(BCI_IMM_A)) && !(ip->hasFlag(BCI_IMM_C)))      \
        {                                                              \
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);     \
            pp.emit_CmpF64_Indirect(REG_OFFSET(ip->c.u32), XMM0, RDI); \
        }                                                              \
        else                                                           \
        {                                                              \
            MK_IMMA_F64(XMM0);                                         \
            MK_IMMC_F64(XMM1);                                         \
            pp.emit_CmpF64(XMM0, XMM1);                                \
        }                                                              \
        pp.emit_Jump(__op1, i, 0);                                     \
        pp.emit_Jump(__op2, i, ip->b.s32);                             \
    } while (0)
