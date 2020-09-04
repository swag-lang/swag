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