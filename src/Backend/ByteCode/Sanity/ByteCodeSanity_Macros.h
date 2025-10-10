#define STATE() context.states[context.state]

#define BINOP_EQ(__cast, __op, __reg)                                                      \
    SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));                                       \
    SWAG_CHECK(checkNotNull(ra));                                                          \
    if (ra->isStackAddr())                                                                 \
    {                                                                                      \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
        SWAG_CHECK(STATE()->getStackKind(&va, addr, sizeof(vb.reg.__reg)));                \
        SWAG_CHECK(STATE()->getImmediateB(vb));                                            \
        if (va.isUnknown() || vb.isUnknown())                                              \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);   \
        else                                                                               \
            *(__cast*) addr __op vb.reg.__reg;                                             \
        STATE()->setStackIps(addr, sizeof(vb.reg.__reg), false);                           \
    }

#define BINOP_EQ_OVF(__cast, __op, __reg, __ovf, __msg, __type)                                              \
    SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));                                                         \
    SWAG_CHECK(checkNotNull(ra));                                                                            \
    if (ra->isStackAddr())                                                                                   \
    {                                                                                                        \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra));                   \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));                          \
        SWAG_CHECK(STATE()->getStackKind(&va, addr, sizeof(vb.reg.__reg)));                                  \
        SWAG_CHECK(STATE()->getImmediateB(vb));                                                              \
        if (va.isUnknown() || vb.isUnknown())                                                                \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);                     \
        else                                                                                                 \
        {                                                                                                    \
            const auto ovf = __ovf(ip, ip->node, *(__cast*) addr, (__cast) vb.reg.__reg);                    \
            SWAG_CHECK(checkOverflow(!ovf, __msg, __type, (const void*) addr, (const void*) &vb.reg.__reg)); \
            *(__cast*) addr __op vb.reg.__reg;                                                               \
        }                                                                                                    \
        STATE()->setStackIps(addr, sizeof(vb.reg.__reg), false);                                             \
    }

#define ATOM_EQ(__cast, __op, __reg)                                                       \
    SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));                                       \
    SWAG_CHECK(checkNotNull(ra));                                                          \
    SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));                                       \
    rc->setUnknown();                                                                      \
    if (ra->isStackAddr())                                                                 \
    {                                                                                      \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
        SWAG_CHECK(STATE()->getStackKind(rc, addr, sizeof(vb.reg.__reg)));                 \
        SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));                                   \
        if (rc->isUnknown() || rb->isUnknown())                                            \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);   \
        else                                                                               \
        {                                                                                  \
            rc->reg.__reg = *(__cast*) addr;                                               \
            *(__cast*) addr __op rb->reg.__reg;                                            \
        }                                                                                  \
        STATE()->setStackIps(addr, sizeof(vb.reg.__reg), false);                           \
    }

#define ATOM_EQ_XCHG(__cast, __reg)                                                        \
    SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));                                       \
    SWAG_CHECK(checkNotNull(ra));                                                          \
    SWAG_CHECK(STATE()->getRegister(rd, ip->d.u32));                                       \
    rd->setUnknown();                                                                      \
    if (ra->isStackAddr())                                                                 \
    {                                                                                      \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
        SWAG_CHECK(STATE()->getStackKind(rd, addr, sizeof(vb.reg.__reg)));                 \
        SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));                                   \
        SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));                                   \
        if (rd->isUnknown() || rb->isUnknown() || rc->isUnknown())                         \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);   \
        else                                                                               \
        {                                                                                  \
            rd->reg.__reg = *(__cast*) addr;                                               \
            if (rd->reg.__reg == rb->reg.__reg)                                            \
                *(__cast*) addr = rc->reg.__reg;                                           \
        }                                                                                  \
        STATE()->setStackIps(addr, sizeof(vb.reg.__reg), false);                           \
    }

#define BINOP_EQ_DIV(__cast, __op, __reg)                \
    SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));     \
    SWAG_CHECK(STATE()->getImmediateB(vb));              \
    SWAG_CHECK(checkDivZero(&vb, vb.reg.__reg == 0));    \
    if (vb.isConstant() && vb.reg.__reg == 0)            \
    {                                                    \
        SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32)); \
        rc->setUnknown();                                \
        break;                                           \
    }                                                    \
    BINOP_EQ(__cast, __op, __reg);

#define BINOP_EQ_SHIFT(__cast, __reg, __func, __isSigned)                                  \
    SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));                                       \
    SWAG_CHECK(checkNotNull(ra));                                                          \
    if (ra->isStackAddr())                                                                 \
    {                                                                                      \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
        SWAG_CHECK(STATE()->getStackKind(&va, addr, sizeof(vb.reg.__reg)));                \
        SWAG_CHECK(STATE()->getImmediateB(vb));                                            \
        if (va.isUnknown() || vb.isUnknown())                                              \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);   \
        else                                                                               \
        {                                                                                  \
            Register r;                                                                    \
            r.__reg = *(__cast*) addr;                                                     \
            __func(&r, r, vb.reg, sizeof(vb.reg.__reg) * 8, __isSigned);                   \
            *(__cast*) addr = r.__reg;                                                     \
        }                                                                                  \
        STATE()->setStackIps(addr, sizeof(vb.reg.__reg), false);                           \
    }

#define BINOP_SHIFT(__cast, __reg, __func, __isSigned)                                                    \
    SWAG_CHECK(STATE()->getImmediateA(va));                                                               \
    SWAG_CHECK(STATE()->getImmediateB(vb));                                                               \
    SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));                                                      \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->setIps(ip, &va, &vb, rc);                                                                         \
    if (rc->isConstant())                                                                                 \
    {                                                                                                     \
        __func(&rc->reg, va.reg, vb.reg, sizeof(va.reg.__reg) * 8, __isSigned);                           \
    }

#define BINOP(__op, __reg)                                                                                \
    SWAG_CHECK(STATE()->getImmediateA(va));                                                               \
    SWAG_CHECK(STATE()->getImmediateB(vb));                                                               \
    SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));                                                      \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->setIps(ip, &va, &vb, rc);                                                                         \
    if (rc->isConstant())                                                                                 \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;

#define BINOP_OVF(__op, __reg, __ovf, __msg, __type)                                                              \
    SWAG_CHECK(STATE()->getImmediateA(va));                                                                       \
    SWAG_CHECK(STATE()->getImmediateB(vb));                                                                       \
    SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));                                                              \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;         \
    rc->setIps(ip, &va, &vb, rc);                                                                                 \
    if (rc->isConstant())                                                                                         \
    {                                                                                                             \
        const auto ovf = __ovf(ip, ip->node, va.reg.__reg, vb.reg.__reg);                                         \
        SWAG_CHECK(checkOverflow(!ovf, __msg, __type, (const void*) &va.reg.__reg, (const void*) &vb.reg.__reg)); \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;                                                           \
    }

#define BINOP_DIV(__op, __reg)                           \
    SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));     \
    SWAG_CHECK(STATE()->getImmediateB(vb));              \
    SWAG_CHECK(checkDivZero(&vb, vb.reg.__reg == 0));    \
    if (vb.isConstant() && vb.reg.__reg == 0)            \
    {                                                    \
        SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32)); \
        rc->setUnknown();                                \
        rc->setIps(ip, ra, &vb);                         \
        break;                                           \
    }                                                    \
    BINOP(__op, __reg);

#define CMP_OP(__op, __reg)                          \
    SWAG_CHECK(STATE()->getImmediateA(va));          \
    SWAG_CHECK(STATE()->getImmediateB(vb));          \
    SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32)); \
    if (va.isConstant() && vb.isConstant())          \
        rc->kind = SanityValueKind::Constant;        \
    else if (va.isStackAddr() && vb.isStackAddr())   \
        rc->kind = SanityValueKind::Constant;        \
    else                                             \
        rc->kind = SanityValueKind::Unknown;         \
    rc->setIps(ip, &va, &vb, rc);                    \
    if (rc->isConstant())                            \
        rc->reg.b = va.reg.__reg __op vb.reg.__reg;  \
    else if (rc->isStackAddr())                      \
        rc->reg.b = va.reg.u64 __op vb.reg.u64;
