#define MK_ALIGN16(__s) \
    if (__s % 16)       \
        __s += 16 - (__s % 16);

#define MK_BINOP8_CAB(__opInd, __op)                                \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                     \
    {                                                               \
        pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX, RDI);     \
        pp.__opInd(regOffset(ip->b.u32), RAX, RDI);                 \
    }                                                               \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))   \
    {                                                               \
        pp.emit_Load8_Immediate(ip->a.u8, RAX);                     \
        pp.__opInd(regOffset(ip->b.u32), RAX, RDI);                 \
    }                                                               \
    else                                                            \
    {                                                               \
        if (ip->flags & BCI_IMM_A)                                  \
            pp.emit_Load8_Immediate(ip->a.u8, RAX);                 \
        else                                                        \
            pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_B)                                  \
            pp.emit_Load8_Immediate(ip->b.u8, RCX);                 \
        else                                                        \
            pp.emit_Load8_Indirect(regOffset(ip->b.u32), RCX, RDI); \
        pp.__op(RAX, RCX);                                          \
    }

#define MK_BINOP16_CAB(__opInd, __op)                                \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                      \
    {                                                                \
        pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX, RDI);     \
        pp.__opInd(regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))    \
    {                                                                \
        pp.emit_Load16_Immediate(ip->a.u16, RAX);                    \
        pp.__opInd(regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                \
    else                                                             \
    {                                                                \
        if (ip->flags & BCI_IMM_A)                                   \
            pp.emit_Load16_Immediate(ip->a.u16, RAX);                \
        else                                                         \
            pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_B)                                   \
            pp.emit_Load16_Immediate(ip->b.u16, RCX);                \
        else                                                         \
            pp.emit_Load16_Indirect(regOffset(ip->b.u32), RCX, RDI); \
        pp.__op(RAX, RCX);                                           \
    }

#define MK_BINOP32_CAB(__opIndDst, __opInd, __op)                    \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                      \
    {                                                                \
        pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX, RDI);     \
        pp.__opInd(regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))    \
    {                                                                \
        pp.emit_Load32_Immediate(ip->a.u32, RAX);                    \
        pp.__opInd(regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                \
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))    \
    {                                                                \
        pp.__opIndDst(regOffset(ip->a.u32), ip->b.u32);              \
    }                                                                \
    else                                                             \
    {                                                                \
        if (ip->flags & BCI_IMM_A)                                   \
            pp.emit_Load32_Immediate(ip->a.u32, RAX);                \
        else                                                         \
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_B)                                   \
            pp.emit_Load32_Immediate(ip->b.u32, RCX);                \
        else                                                         \
            pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX, RDI); \
        pp.__op(RAX, RCX);                                           \
    }

#define MK_BINOP64_CAB(__opIndDst, __opInd, __op)                                            \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                                              \
    {                                                                                        \
        pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX, RDI);                             \
        pp.__opInd(regOffset(ip->b.u32), RAX, RDI);                                          \
    }                                                                                        \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))                            \
    {                                                                                        \
        pp.emit_Load64_Immediate(ip->a.u64, RAX);                                            \
        pp.__opInd(regOffset(ip->b.u32), RAX, RDI);                                          \
    }                                                                                        \
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF) \
    {                                                                                        \
        pp.__opIndDst(regOffset(ip->a.u32), ip->b.u32);                                      \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
        if (ip->flags & BCI_IMM_A)                                                           \
            pp.emit_Load64_Immediate(ip->a.u64, RAX);                                        \
        else                                                                                 \
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX, RDI);                         \
        if (ip->flags & BCI_IMM_B)                                                           \
            pp.emit_Load64_Immediate(ip->b.u64, RCX);                                        \
        else                                                                                 \
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RCX, RDI);                         \
        pp.__op(RAX, RCX);                                                                   \
    }

#define MK_BINOPF32_CAB(__opInd, __op)                                 \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                        \
    {                                                                  \
        pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0, RDI);     \
        pp.__opInd(regOffset(ip->b.u32), XMM0, RDI);                   \
    }                                                                  \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))      \
    {                                                                  \
        pp.emit_Load32_Immediate(ip->a.u32, RAX);                      \
        pp.emit_CopyF32(RAX, XMM0);                                    \
        pp.__opInd(regOffset(ip->b.u32), XMM0, RDI);                   \
    }                                                                  \
    else                                                               \
    {                                                                  \
        if (ip->flags & BCI_IMM_A)                                     \
        {                                                              \
            pp.emit_Load32_Immediate(ip->a.u32, RAX);                  \
            pp.emit_CopyF32(RAX, XMM0);                                \
        }                                                              \
        else                                                           \
            pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0, RDI); \
        if (ip->flags & BCI_IMM_B)                                     \
        {                                                              \
            pp.emit_Load32_Immediate(ip->b.u32, RAX);                  \
            pp.emit_CopyF32(RAX, XMM1);                                \
        }                                                              \
        else                                                           \
            pp.emit_LoadF32_Indirect(regOffset(ip->b.u32), XMM1, RDI); \
        pp.__op(XMM0, XMM1);                                           \
    }

#define MK_BINOPF64_CAB(__opInd, __op)                                 \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                        \
    {                                                                  \
        pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0, RDI);     \
        pp.__opInd(regOffset(ip->b.u32), XMM0, RDI);                   \
    }                                                                  \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))      \
    {                                                                  \
        pp.emit_Load64_Immediate(ip->a.u64, RAX);                      \
        pp.emit_CopyF64(RAX, XMM0);                                    \
        pp.__opInd(regOffset(ip->b.u32), XMM0, RDI);                   \
    }                                                                  \
    else                                                               \
    {                                                                  \
        if (ip->flags & BCI_IMM_A)                                     \
        {                                                              \
            pp.emit_Load64_Immediate(ip->a.u64, RAX);                  \
            pp.emit_CopyF64(RAX, XMM0);                                \
        }                                                              \
        else                                                           \
            pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0, RDI); \
        if (ip->flags & BCI_IMM_B)                                     \
        {                                                              \
            pp.emit_Load64_Immediate(ip->b.u64, RAX);                  \
            pp.emit_CopyF64(RAX, XMM1);                                \
        }                                                              \
        else                                                           \
            pp.emit_LoadF64_Indirect(regOffset(ip->b.u32), XMM1, RDI); \
        pp.__op(XMM0, XMM1);                                           \
    }

#define MK_IMMB_8(__reg)                          \
    if (ip->flags & BCI_IMM_B)                    \
        pp.emit_Load8_Immediate(ip->b.u8, __reg); \
    else                                          \
        pp.emit_Load8_Indirect(regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_S8_TO_S32(__reg)                  \
    if (ip->flags & BCI_IMM_B)                    \
    {                                             \
        pp.emit_Load8_Immediate(ip->b.u8, __reg); \
        pp.emit_SignedExtend_8_To_32(__reg);      \
    }                                             \
    else                                          \
        pp.emit_LoadS8S32_Indirect(regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_U8_TO_U32(__reg)                   \
    if (ip->flags & BCI_IMM_B)                     \
        pp.emit_Load32_Immediate(ip->b.u8, __reg); \
    else                                           \
        pp.emit_LoadU8U32_Indirect(regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_16(__reg)                           \
    if (ip->flags & BCI_IMM_B)                      \
        pp.emit_Load16_Immediate(ip->b.u16, __reg); \
    else                                            \
        pp.emit_Load16_Indirect(regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_32(__reg)                           \
    if (ip->flags & BCI_IMM_B)                      \
        pp.emit_Load32_Immediate(ip->b.u32, __reg); \
    else                                            \
        pp.emit_Load32_Indirect(regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_64(__reg)                           \
    if (ip->flags & BCI_IMM_B)                      \
        pp.emit_Load64_Immediate(ip->b.u64, __reg); \
    else                                            \
        pp.emit_Load64_Indirect(regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_F32(__reg)                        \
    if (ip->flags & BCI_IMM_B)                    \
    {                                             \
        pp.emit_Load32_Immediate(ip->b.u32, RAX); \
        pp.emit_CopyF32(RAX, __reg);              \
    }                                             \
    else                                          \
        pp.emit_LoadF32_Indirect(regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_F64(__reg)                        \
    if (ip->flags & BCI_IMM_B)                    \
    {                                             \
        pp.emit_Load64_Immediate(ip->b.u64, RAX); \
        pp.emit_CopyF64(RAX, __reg);              \
    }                                             \
    else                                          \
        pp.emit_LoadF64_Indirect(regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMC_8(__reg)                          \
    if (ip->flags & BCI_IMM_C)                    \
        pp.emit_Load8_Immediate(ip->c.u8, __reg); \
    else                                          \
        pp.emit_Load8_Indirect(regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_S8_TO_S32(__reg)                  \
    if (ip->flags & BCI_IMM_C)                    \
    {                                             \
        pp.emit_Load8_Immediate(ip->c.u8, __reg); \
        pp.emit_SignedExtend_8_To_32(__reg);      \
    }                                             \
    else                                          \
        pp.emit_LoadS8S32_Indirect(regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_U8_TO_U32(__reg)                   \
    if (ip->flags & BCI_IMM_C)                     \
        pp.emit_Load32_Immediate(ip->c.u8, __reg); \
    else                                           \
        pp.emit_LoadU8U32_Indirect(regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_16(__reg)                           \
    if (ip->flags & BCI_IMM_C)                      \
        pp.emit_Load16_Immediate(ip->c.u16, __reg); \
    else                                            \
        pp.emit_Load16_Indirect(regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_32(__reg)                           \
    if (ip->flags & BCI_IMM_C)                      \
        pp.emit_Load32_Immediate(ip->c.u32, __reg); \
    else                                            \
        pp.emit_Load32_Indirect(regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_64(__reg)                           \
    if (ip->flags & BCI_IMM_C)                      \
        pp.emit_Load64_Immediate(ip->c.u64, __reg); \
    else                                            \
        pp.emit_Load64_Indirect(regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_F32(__reg)                        \
    if (ip->flags & BCI_IMM_C)                    \
    {                                             \
        pp.emit_Load32_Immediate(ip->c.u32, RAX); \
        pp.emit_CopyF32(RAX, __reg);              \
    }                                             \
    else                                          \
        pp.emit_LoadF32_Indirect(regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_F64(__reg)                        \
    if (ip->flags & BCI_IMM_C)                    \
    {                                             \
        pp.emit_Load64_Immediate(ip->c.u64, RAX); \
        pp.emit_CopyF64(RAX, __reg);              \
    }                                             \
    else                                          \
        pp.emit_LoadF64_Indirect(regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMD_64(__reg)                           \
    if (ip->flags & BCI_IMM_D)                      \
        pp.emit_Load64_Immediate(ip->d.u64, __reg); \
    else                                            \
        pp.emit_Load64_Indirect(regOffset(ip->d.u32), __reg, RDI);

#define MK_BINOPEQ8_CAB(__op)                                \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7F)          \
    {                                                        \
        pp.concat.addU8(0x80);                               \
        pp.concat.addU8((uint8_t) __op);                     \
        pp.concat.addU8(ip->b.u8);                           \
    }                                                        \
    else                                                     \
    {                                                        \
        MK_IMMB_8(RAX);                                      \
        pp.emit_Op8_Indirect(0, RAX, RCX, __op);             \
    }

#define MK_BINOPEQ16_CAB(__op)                               \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFF)        \
    {                                                        \
        pp.concat.addU8(0x66);                               \
        if (ip->b.u64 <= 0x7F)                               \
            pp.concat.addU8(0x83);                           \
        else                                                 \
            pp.concat.addU8(0x81);                           \
        pp.concat.addU8((uint8_t) __op);                     \
        if (ip->b.u64 <= 0x7F)                               \
            pp.concat.addU8(ip->b.u8);                       \
        else                                                 \
            pp.concat.addU16(ip->b.u16);                     \
    }                                                        \
    else                                                     \
    {                                                        \
        MK_IMMB_16(RAX);                                     \
        pp.emit_Op16_Indirect(0, RAX, RCX, __op);            \
    }

#define MK_BINOPEQ32_CAB(__op)                               \
                                                             \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)    \
    {                                                        \
        if (ip->b.u64 <= 0x7F)                               \
            pp.concat.addU8(0x83);                           \
        else                                                 \
            pp.concat.addU8(0x81);                           \
        pp.concat.addU8((uint8_t) __op);                     \
        if (ip->b.u64 <= 0x7F)                               \
            pp.concat.addU8(ip->b.u8);                       \
        else                                                 \
            pp.concat.addU32(ip->b.u32);                     \
    }                                                        \
    else                                                     \
    {                                                        \
        MK_IMMB_32(RAX);                                     \
        pp.emit_Op32_Indirect(0, RAX, RCX, __op);            \
    }

#define MK_BINOPEQ64_CAB(__op)                               \
                                                             \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    if (ip->flags & BCI_IMM_B && ip->b.u64 <= 0x7FFFFFFF)    \
    {                                                        \
        pp.concat.addU8(0x48);                               \
        if (ip->b.u64 <= 0x7F)                               \
            pp.concat.addU8(0x83);                           \
        else                                                 \
            pp.concat.addU8(0x81);                           \
        pp.concat.addU8((uint8_t) __op);                     \
        if (ip->b.u64 <= 0x7F)                               \
            pp.concat.addU8(ip->b.u8);                       \
        else                                                 \
            pp.concat.addU32(ip->b.u32);                     \
    }                                                        \
    else                                                     \
    {                                                        \
        MK_IMMB_64(RAX);                                     \
        pp.emit_Op64_IndirectDst(0, RAX, RCX, __op);         \
    }

#define MK_BINOPEQF32_CAB(__op)                              \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_F32(XMM1);                                       \
    pp.emit_OpF32_Indirect(XMM1, RCX, __op);

#define MK_BINOPEQF64_CAB(__op)                              \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_F64(XMM1);                                       \
    pp.emit_OpF64_Indirect(XMM1, RCX, __op);

#define MK_BINOPEQ8_LOCK_CAB(__op)                           \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_8(RAX);                                          \
    pp.emit_Op8_Indirect(0, RAX, RCX, __op, true);

#define MK_BINOPEQ16_LOCK_CAB(__op)                          \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_16(RAX);                                         \
    pp.emit_Op16_Indirect(0, RAX, RCX, __op, true);

#define MK_BINOPEQ32_LOCK_CAB(__op)                          \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_32(RAX);                                         \
    pp.emit_Op32_Indirect(0, RAX, RCX, __op, true);

#define MK_BINOPEQ64_LOCK_CAB(__op)                          \
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_64(RAX);                                         \
    pp.emit_Op64_IndirectDst(0, RAX, RCX, __op, true);

#define MK_JMPCMP_8(__op)                                           \
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_C))       \
    {                                                               \
        pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX, RDI);     \
        pp.emit_Cmp8_Indirect(regOffset(ip->c.u32), RAX, RDI);      \
    }                                                               \
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_C))   \
    {                                                               \
        pp.emit_Cmp8_IndirectDst(regOffset(ip->a.u32), ip->c.u8);   \
    }                                                               \
    else                                                            \
    {                                                               \
        if (ip->flags & BCI_IMM_A)                                  \
            pp.emit_Load8_Immediate(ip->a.u8, RAX);                 \
        else                                                        \
            pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_C)                                  \
            pp.emit_Load8_Immediate(ip->c.u8, RCX);                 \
        else                                                        \
            pp.emit_Load8_Indirect(regOffset(ip->c.u32), RCX, RDI); \
        pp.emit_Cmp8(RAX, RCX);                                     \
    }                                                               \
    pp.emit_Jump(__op, i, ip->b.s32);

#define MK_JMPCMP_16(__op)                                           \
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_C))        \
    {                                                                \
        pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX, RDI);     \
        pp.emit_Cmp16_Indirect(regOffset(ip->c.u32), RAX, RDI);      \
    }                                                                \
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_C))    \
    {                                                                \
        pp.emit_Cmp16_IndirectDst(regOffset(ip->a.u32), ip->c.u16);  \
    }                                                                \
    else                                                             \
    {                                                                \
        if (ip->flags & BCI_IMM_A)                                   \
            pp.emit_Load16_Immediate(ip->a.u16, RAX);                \
        else                                                         \
            pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_C)                                   \
            pp.emit_Load16_Immediate(ip->c.u16, RCX);                \
        else                                                         \
            pp.emit_Load16_Indirect(regOffset(ip->c.u32), RCX, RDI); \
        pp.emit_Cmp16(RAX, RCX);                                     \
    }                                                                \
    pp.emit_Jump(__op, i, ip->b.s32);

#define MK_JMPCMP_32(__op)                                           \
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_C))        \
    {                                                                \
        pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX, RDI);     \
        pp.emit_Cmp32_Indirect(regOffset(ip->c.u32), RAX, RDI);      \
    }                                                                \
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_C))    \
    {                                                                \
        pp.emit_Cmp32_IndirectDst(regOffset(ip->a.u32), ip->c.u32);  \
    }                                                                \
    else                                                             \
    {                                                                \
        if (ip->flags & BCI_IMM_A)                                   \
            pp.emit_Load32_Immediate(ip->a.u32, RAX);                \
        else                                                         \
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_C)                                   \
            pp.emit_Load32_Immediate(ip->c.u32, RCX);                \
        else                                                         \
            pp.emit_Load32_Indirect(regOffset(ip->c.u32), RCX, RDI); \
        pp.emit_Cmp32(RAX, RCX);                                     \
    }                                                                \
    pp.emit_Jump(__op, i, ip->b.s32);

#define MK_JMPCMP_64(__op)                                                                   \
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_C))                                \
    {                                                                                        \
        pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX, RDI);                             \
        pp.emit_Cmp64_Indirect(regOffset(ip->c.u32), RAX, RDI);                              \
    }                                                                                        \
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_C) && ip->c.u64 <= 0x7fffffff) \
    {                                                                                        \
        pp.emit_Cmp64_IndirectDst(regOffset(ip->a.u32), ip->c.u32);                          \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
        if (ip->flags & BCI_IMM_A)                                                           \
            pp.emit_Load64_Immediate(ip->a.u64, RAX);                                        \
        else                                                                                 \
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX, RDI);                         \
        if (ip->flags & BCI_IMM_C)                                                           \
            pp.emit_Load64_Immediate(ip->c.u64, RCX);                                        \
        else                                                                                 \
            pp.emit_Load64_Indirect(regOffset(ip->c.u32), RCX, RDI);                         \
        pp.emit_Cmp64(RAX, RCX);                                                             \
    }                                                                                        \
    pp.emit_Jump(__op, i, ip->b.s32);

#define MK_JMPCMP_F32(__op)                                            \
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_C))          \
    {                                                                  \
        pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0, RDI);     \
        pp.emit_CmpF32_Indirect(regOffset(ip->c.u32), XMM0, RDI);      \
    }                                                                  \
    else                                                               \
    {                                                                  \
        if (ip->flags & BCI_IMM_A)                                     \
        {                                                              \
            pp.emit_Load32_Immediate(ip->a.u32, RAX);                  \
            pp.emit_CopyF32(RAX, XMM0);                                \
        }                                                              \
        else                                                           \
            pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0, RDI); \
        if (ip->flags & BCI_IMM_C)                                     \
        {                                                              \
            pp.emit_Load32_Immediate(ip->c.u32, RAX);                  \
            pp.emit_CopyF32(RAX, XMM1);                                \
        }                                                              \
        else                                                           \
            pp.emit_LoadF32_Indirect(regOffset(ip->c.u32), XMM1, RDI); \
        pp.emit_CmpF32(XMM0, XMM1);                                    \
    }                                                                  \
    pp.emit_Jump(__op, i, ip->b.s32);

#define MK_JMPCMP_F64(__op)                                            \
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_C))          \
    {                                                                  \
        pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0, RDI);     \
        pp.emit_CmpF64_Indirect(regOffset(ip->c.u32), XMM0, RDI);      \
    }                                                                  \
    else                                                               \
    {                                                                  \
        if (ip->flags & BCI_IMM_A)                                     \
        {                                                              \
            pp.emit_Load64_Immediate(ip->a.u64, RAX);                  \
            pp.emit_CopyF64(RAX, XMM0);                                \
        }                                                              \
        else                                                           \
            pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0, RDI); \
        if (ip->flags & BCI_IMM_C)                                     \
        {                                                              \
            pp.emit_Load64_Immediate(ip->c.u64, RAX);                  \
            pp.emit_CopyF64(RAX, XMM1);                                \
        }                                                              \
        else                                                           \
            pp.emit_LoadF64_Indirect(regOffset(ip->c.u32), XMM1, RDI); \
        pp.emit_CmpF64(XMM0, XMM1);                                    \
    }                                                                  \
    pp.emit_Jump(__op, i, ip->b.s32);