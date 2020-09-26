#define MK_ALIGN16(__s) \
    if (__s % 16)       \
        __s += 16 - (__s % 16);

#define MK_BINOP8_CAB(__opInd, __op)                                                 \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                                      \
    {                                                                                \
        BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);     \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), RAX, RDI);                 \
    }                                                                                \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))                    \
    {                                                                                \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u8, RAX);                    \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), RAX, RDI);                 \
    }                                                                                \
    else                                                                             \
    {                                                                                \
        if (ip->flags & BCI_IMM_A)                                                   \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u8, RAX);                \
        else                                                                         \
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_B)                                                   \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u8, RCX);                \
        else                                                                         \
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RCX, RDI); \
        BackendX64Inst::__op(pp, RAX, RCX);                                          \
    }

#define MK_BINOP16_CAB(__opInd, __op)                                                 \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                                       \
    {                                                                                 \
        BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);     \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                                 \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))                     \
    {                                                                                 \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u16, RAX);                    \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
        if (ip->flags & BCI_IMM_A)                                                    \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u16, RAX);                \
        else                                                                          \
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_B)                                                    \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u16, RCX);                \
        else                                                                          \
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RCX, RDI); \
        BackendX64Inst::__op(pp, RAX, RCX);                                           \
    }

#define MK_BINOP32_CAB(__opInd, __op)                                                 \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                                       \
    {                                                                                 \
        BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);     \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                                 \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))                     \
    {                                                                                 \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u32, RAX);                    \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
        if (ip->flags & BCI_IMM_A)                                                    \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u32, RAX);                \
        else                                                                          \
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_B)                                                    \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, RCX);                \
        else                                                                          \
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RCX, RDI); \
        BackendX64Inst::__op(pp, RAX, RCX);                                           \
    }

#define MK_BINOP64_CAB(__opInd, __op)                                                 \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                                       \
    {                                                                                 \
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);     \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                                 \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))                     \
    {                                                                                 \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RAX);                    \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), RAX, RDI);                  \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
        if (ip->flags & BCI_IMM_A)                                                    \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RAX);                \
        else                                                                          \
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI); \
        if (ip->flags & BCI_IMM_B)                                                    \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RCX);                \
        else                                                                          \
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RCX, RDI); \
        BackendX64Inst::__op(pp, RAX, RCX);                                           \
    }

#define MK_BINOPF32_CAB(__opInd, __op)                                                  \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                                         \
    {                                                                                   \
        BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);     \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), XMM0, RDI);                   \
    }                                                                                   \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))                       \
    {                                                                                   \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u32, RAX);                      \
        BackendX64Inst::emit_CopyF32(pp, RAX, XMM0);                                    \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), XMM0, RDI);                   \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        if (ip->flags & BCI_IMM_A)                                                      \
        {                                                                               \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u32, RAX);                  \
            BackendX64Inst::emit_CopyF32(pp, RAX, XMM0);                                \
        }                                                                               \
        else                                                                            \
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI); \
        if (ip->flags & BCI_IMM_B)                                                      \
        {                                                                               \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, RAX);                  \
            BackendX64Inst::emit_CopyF32(pp, RAX, XMM1);                                \
        }                                                                               \
        else                                                                            \
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI); \
        BackendX64Inst::__op(pp, XMM0, XMM1);                                           \
    }

#define MK_BINOPF64_CAB(__opInd, __op)                                                  \
    if (!(ip->flags & (BCI_IMM_A | BCI_IMM_B)))                                         \
    {                                                                                   \
        BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);     \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), XMM0, RDI);                   \
    }                                                                                   \
    else if ((ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))                       \
    {                                                                                   \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RAX);                      \
        BackendX64Inst::emit_CopyF64(pp, RAX, XMM0);                                    \
        BackendX64Inst::__opInd(pp, regOffset(ip->b.u32), XMM0, RDI);                   \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        if (ip->flags & BCI_IMM_A)                                                      \
        {                                                                               \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RAX);                  \
            BackendX64Inst::emit_CopyF64(pp, RAX, XMM0);                                \
        }                                                                               \
        else                                                                            \
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI); \
        if (ip->flags & BCI_IMM_B)                                                      \
        {                                                                               \
            BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX);                  \
            BackendX64Inst::emit_CopyF64(pp, RAX, XMM1);                                \
        }                                                                               \
        else                                                                            \
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI); \
        BackendX64Inst::__op(pp, XMM0, XMM1);                                           \
    }

#define MK_IMMB_8(__reg)                                            \
    if (ip->flags & BCI_IMM_B)                                      \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u8, __reg); \
    else                                                            \
        BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_16(__reg)                                            \
    if (ip->flags & BCI_IMM_B)                                       \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u16, __reg); \
    else                                                             \
        BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_32(__reg)                                            \
    if (ip->flags & BCI_IMM_B)                                       \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, __reg); \
    else                                                             \
        BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_64(__reg)                                            \
    if (ip->flags & BCI_IMM_B)                                       \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, __reg); \
    else                                                             \
        BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_F32(__reg)                                         \
    if (ip->flags & BCI_IMM_B)                                     \
    {                                                              \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u32, RAX); \
        BackendX64Inst::emit_CopyF32(pp, RAX, __reg);              \
    }                                                              \
    else                                                           \
        BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMB_F64(__reg)                                         \
    if (ip->flags & BCI_IMM_B)                                     \
    {                                                              \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX); \
        BackendX64Inst::emit_CopyF64(pp, RAX, __reg);              \
    }                                                              \
    else                                                           \
        BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), __reg, RDI);

#define MK_IMMC_32(__reg)                                            \
    if (ip->flags & BCI_IMM_C)                                       \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->c.u32, __reg); \
    else                                                             \
        BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_F32(__reg)                                         \
    if (ip->flags & BCI_IMM_C)                                     \
    {                                                              \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->c.u32, RAX); \
        BackendX64Inst::emit_CopyF32(pp, RAX, __reg);              \
    }                                                              \
    else                                                           \
        BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->c.u32), __reg, RDI);

#define MK_IMMC_F64(__reg)                                         \
    if (ip->flags & BCI_IMM_C)                                     \
    {                                                              \
        BackendX64Inst::emit_Load64_Immediate(pp, ip->c.u64, RAX); \
        BackendX64Inst::emit_CopyF64(pp, RAX, __reg);              \
    }                                                              \
    else                                                           \
        BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->c.u32), __reg, RDI);

#define MK_BINOPEQ8_CAB(__op)                                                 \
    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_8(RAX);                                                           \
    BackendX64Inst::emit_Op8_Indirect(pp, 0, RAX, RCX, __op);

#define MK_BINOPEQ16_CAB(__op)                                                \
    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_16(RAX);                                                          \
    BackendX64Inst::emit_Op16_Indirect(pp, 0, RAX, RCX, __op);

#define MK_BINOPEQ32_CAB(__op)                                                \
    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_32(RAX);                                                          \
    BackendX64Inst::emit_Op32_Indirect(pp, 0, RAX, RCX, __op);

#define MK_BINOPEQ64_CAB(__op)                                                \
    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_64(RAX);                                                          \
    BackendX64Inst::emit_Op64_Indirect(pp, 0, RAX, RCX, __op);

#define MK_BINOPEQF32_CAB(__op)                                               \
    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_F32(XMM1);                                                        \
    BackendX64Inst::emit_OpF32_Indirect(pp, XMM1, RCX, __op);

#define MK_BINOPEQF64_CAB(__op)                                               \
    BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI); \
    MK_IMMB_F64(XMM1);                                                        \
    BackendX64Inst::emit_OpF64_Indirect(pp, XMM1, RCX, __op);
