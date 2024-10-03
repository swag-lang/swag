#define STATE() context.states[context.state]

#define MEMCPY(__cast, __sizeof)                                                \
    SWAG_CHECK(getRegister(ra, ip->a.u32));                                     \
    SWAG_CHECK(getRegister(rb, ip->b.u32));                                     \
    if (ra->isStackAddr() && rb->isStackAddr())                                 \
    {                                                                           \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, __sizeof, ra));  \
        SWAG_CHECK(STATE()->getStackAddress(addr2, rb->reg.u32, __sizeof, rb)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr2, __sizeof, rb));        \
        SWAG_CHECK(STATE()->getStackKind(&vb, addr2, __sizeof));                \
        STATE()->setStackKind(addr, __sizeof, vb.kind);                         \
        *(__cast*) addr = *(__cast*) addr2;                                     \
    }                                                                           \
    else                                                                        \
        STATE()->invalidateStack();

#define BINOP_EQ(__cast, __op, __reg)                                                      \
    SWAG_CHECK(getRegister(ra, ip->a.u32));                                                \
    SWAG_CHECK(checkNotNull(ra));                                                          \
    if (ra->isStackAddr())                                                                 \
    {                                                                                      \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
        SWAG_CHECK(STATE()->getStackKind(&va, addr, sizeof(vb.reg.__reg)));                \
        SWAG_CHECK(getImmediateB(vb));                                                     \
        if (va.isUnknown() || vb.isUnknown())                                              \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);   \
        else                                                                               \
        {                                                                                  \
            *(__cast*) addr __op vb.reg.__reg;                                             \
            STATE()->updateStackKind(addr, sizeof(vb.reg.__reg));                          \
        }                                                                                  \
    }

#define BINOP_EQ_OVF(__cast, __op, __reg, __ovf, __msg, __type)                                                     \
    SWAG_CHECK(getRegister(ra, ip->a.u32));                                                                         \
    SWAG_CHECK(checkNotNull(ra));                                                                                   \
    if (ra->isStackAddr())                                                                                          \
    {                                                                                                               \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra));                          \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));                                 \
        SWAG_CHECK(STATE()->getStackKind(&va, addr, sizeof(vb.reg.__reg)));                                         \
        SWAG_CHECK(getImmediateB(vb));                                                                              \
        if (va.isUnknown() || vb.isUnknown())                                                                       \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);                            \
        else                                                                                                        \
        {                                                                                                           \
            SWAG_CHECK(checkOverflow(!__ovf(ip, ip->node, *(__cast*) addr, (__cast) vb.reg.__reg), __msg, __type)); \
            *(__cast*) addr __op vb.reg.__reg;                                                                      \
            STATE()->updateStackKind(addr, sizeof(vb.reg.__reg));                                                   \
        }                                                                                                           \
    }

#define ATOM_EQ(__cast, __op, __reg)                                                       \
    SWAG_CHECK(getRegister(ra, ip->a.u32));                                                \
    SWAG_CHECK(checkNotNull(ra));                                                          \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                \
    rc->setUnknown();                                                                      \
    if (ra->isStackAddr())                                                                 \
    {                                                                                      \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
        SWAG_CHECK(STATE()->getStackKind(rc, addr, sizeof(vb.reg.__reg)));                 \
        SWAG_CHECK(getRegister(rb, ip->b.u32));                                            \
        if (rc->isUnknown() || rb->isUnknown())                                            \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);   \
        else                                                                               \
        {                                                                                  \
            rc->reg.__reg = *(__cast*) addr;                                               \
            *(__cast*) addr __op rb->reg.__reg;                                            \
            STATE()->updateStackKind(addr, sizeof(vb.reg.__reg));                          \
        }                                                                                  \
    }

#define ATOM_EQ_XCHG(__cast, __reg)                                                        \
    SWAG_CHECK(getRegister(ra, ip->a.u32));                                                \
    SWAG_CHECK(checkNotNull(ra));                                                          \
    SWAG_CHECK(getRegister(rd, ip->d.u32));                                                \
    rd->setUnknown();                                                                      \
    if (ra->isStackAddr())                                                                 \
    {                                                                                      \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
        SWAG_CHECK(STATE()->getStackKind(rd, addr, sizeof(vb.reg.__reg)));                 \
        SWAG_CHECK(getRegister(rb, ip->b.u32));                                            \
        SWAG_CHECK(getRegister(rc, ip->c.u32));                                            \
        if (rd->isUnknown() || rb->isUnknown() || rc->isUnknown())                         \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);   \
        else                                                                               \
        {                                                                                  \
            rd->reg.__reg = *(__cast*) addr;                                               \
            if (rd->reg.__reg == rb->reg.__reg)                                            \
                *(__cast*) addr = rc->reg.__reg;                                           \
            STATE()->updateStackKind(addr, sizeof(vb.reg.__reg));                          \
        }                                                                                  \
    }

#define BINOP_EQ_DIV(__cast, __op, __reg)             \
    SWAG_CHECK(getRegister(ra, ip->a.u32));           \
    SWAG_CHECK(getImmediateB(vb));                    \
    SWAG_CHECK(checkDivZero(&vb, vb.reg.__reg == 0)); \
    if (vb.isConstant() && vb.reg.__reg == 0)         \
    {                                                 \
        SWAG_CHECK(getRegister(rc, ip->c.u32));       \
        rc->setUnknown();                             \
        break;                                        \
    }                                                 \
    BINOP_EQ(__cast, __op, __reg);

#define BINOP_EQ_SHIFT(__cast, __reg, __func, __isSigned)                                  \
    SWAG_CHECK(getRegister(ra, ip->a.u32));                                                \
    SWAG_CHECK(checkNotNull(ra));                                                          \
    if (ra->isStackAddr())                                                                 \
    {                                                                                      \
        SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, sizeof(vb.reg.__reg), ra)); \
        SWAG_CHECK(STATE()->checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
        SWAG_CHECK(STATE()->getStackKind(&va, addr, sizeof(vb.reg.__reg)));                \
        SWAG_CHECK(getImmediateB(vb));                                                     \
        if (va.isUnknown() || vb.isUnknown())                                              \
            STATE()->setStackKind(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);   \
        else                                                                               \
        {                                                                                  \
            Register r;                                                                    \
            r.__reg = *(__cast*) addr;                                                     \
            __func(&r, r, vb.reg, sizeof(vb.reg.__reg) * 8, __isSigned);                   \
            *(__cast*) addr = r.__reg;                                                     \
            STATE()->updateStackKind(addr, sizeof(vb.reg.__reg));                          \
        }                                                                                  \
    }

#define BINOP_SHIFT(__cast, __reg, __func, __isSigned)                                                    \
    SWAG_CHECK(getImmediateA(va));                                                                        \
    SWAG_CHECK(getImmediateB(vb));                                                                        \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                               \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->computeIp(ip, &va, &vb, rc);                                                                      \
    if (rc->isConstant())                                                                                 \
    {                                                                                                     \
        __func(&rc->reg, va.reg, vb.reg, sizeof(va.reg.__reg) * 8, __isSigned);                           \
    }

#define BINOP(__op, __reg)                                                                                \
    SWAG_CHECK(getImmediateA(va));                                                                        \
    SWAG_CHECK(getImmediateB(vb));                                                                        \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                               \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->computeIp(ip, &va, &vb, rc);                                                                      \
    if (rc->isConstant())                                                                                 \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;

#define BINOP_OVF(__op, __reg, __ovf, __msg, __type)                                                      \
    SWAG_CHECK(getImmediateA(va));                                                                        \
    SWAG_CHECK(getImmediateB(vb));                                                                        \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                               \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->computeIp(ip, &va, &vb, rc);                                                                      \
    if (rc->isConstant())                                                                                 \
    {                                                                                                     \
        SWAG_CHECK(checkOverflow(!__ovf(ip, ip->node, va.reg.__reg, vb.reg.__reg), __msg, __type));       \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;                                                   \
    }

#define BINOP_DIV(__op, __reg)                        \
    SWAG_CHECK(getRegister(ra, ip->a.u32));           \
    SWAG_CHECK(getImmediateB(vb));                    \
    SWAG_CHECK(checkDivZero(&vb, vb.reg.__reg == 0)); \
    if (vb.isConstant() && vb.reg.__reg == 0)         \
    {                                                 \
        SWAG_CHECK(getRegister(rc, ip->c.u32));       \
        rc->setUnknown();                             \
        rc->computeIp(ip, ra, &vb);                   \
        break;                                        \
    }                                                 \
    BINOP(__op, __reg);

#define CMP_OP(__op, __reg)                                                                               \
    SWAG_CHECK(getImmediateA(va));                                                                        \
    SWAG_CHECK(getImmediateB(vb));                                                                        \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                               \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->computeIp(ip, &va, &vb, rc);                                                                      \
    if (rc->isConstant())                                                                                 \
        rc->reg.b = va.reg.__reg __op vb.reg.__reg;
