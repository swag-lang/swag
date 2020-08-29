#define MK_BINOP_CAB_S32(__op)                                  \
    concat.addStringFormat("r[%u].b=", ip->c.u32);              \
    if (ip->flags & BCI_IMM_A)                                  \
        concat.addStringFormat("%d%s", ip->a.s32, __op);        \
    else                                                        \
        concat.addStringFormat("r[%u].s32%s", ip->a.u32, __op); \
    if (ip->flags & BCI_IMM_B)                                  \
        concat.addStringFormat("%d;", ip->b.s32);               \
    else                                                        \
        concat.addStringFormat("r[%u].s32;", ip->b.u32);

#define MK_BINOP_CAB_U8(__op)                                  \
    concat.addStringFormat("r[%u].b=", ip->c.u32);             \
    if (ip->flags & BCI_IMM_A)                                 \
        concat.addStringFormat("%u%s", ip->a.u8, __op);        \
    else                                                       \
        concat.addStringFormat("r[%u].u8%s", ip->a.u32, __op); \
    if (ip->flags & BCI_IMM_B)                                 \
        concat.addStringFormat("%u;", ip->b.u8);               \
    else                                                       \
        concat.addStringFormat("r[%u].u8;", ip->b.u32);

#define MK_BINOP_CAB_U16(__op)                                  \
    concat.addStringFormat("r[%u].b=", ip->c.u32);              \
    if (ip->flags & BCI_IMM_A)                                  \
        concat.addStringFormat("%u%s", ip->a.u16, __op);        \
    else                                                        \
        concat.addStringFormat("r[%u].u16%s", ip->a.u32, __op); \
    if (ip->flags & BCI_IMM_B)                                  \
        concat.addStringFormat("%u;", ip->b.u16);               \
    else                                                        \
        concat.addStringFormat("r[%u].u16;", ip->b.u32);

#define MK_BINOP_CAB_U32(__op)                                  \
    concat.addStringFormat("r[%u].b=", ip->c.u32);              \
    if (ip->flags & BCI_IMM_A)                                  \
        concat.addStringFormat("%u%s", ip->a.u32, __op);        \
    else                                                        \
        concat.addStringFormat("r[%u].u32%s", ip->a.u32, __op); \
    if (ip->flags & BCI_IMM_B)                                  \
        concat.addStringFormat("%u;", ip->b.u32);               \
    else                                                        \
        concat.addStringFormat("r[%u].u32;", ip->b.u32);

#define MK_BINOP_CAB_U64(__op)                                  \
    concat.addStringFormat("r[%u].b=", ip->c.u32);              \
    if (ip->flags & BCI_IMM_A)                                  \
        concat.addStringFormat("0x%I64x%s", ip->a.u64, __op);   \
    else                                                        \
        concat.addStringFormat("r[%u].u64%s", ip->a.u32, __op); \
    if (ip->flags & BCI_IMM_B)                                  \
        concat.addStringFormat("0x%I64x;", ip->b.u64);          \
    else                                                        \
        concat.addStringFormat("r[%u].u64;", ip->b.u32);
