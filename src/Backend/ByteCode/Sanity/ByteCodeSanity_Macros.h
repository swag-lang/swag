#define STATE() context.states[context.state]

#define MEMCPY(__cast, __sizeof)                                       \
    SWAG_CHECK(getRegister(ra, ip->a.u32));                            \
    SWAG_CHECK(getRegister(rb, ip->b.u32));                            \
    if (ra->isStackAddr() && rb->isStackAddr())                        \
    {                                                                  \
        SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, __sizeof));  \
        SWAG_CHECK(getStackAddress(addr2, rb, rb->reg.u32, __sizeof)); \
        SWAG_CHECK(checkStackInitialized(addr2, __sizeof, rb));        \
        SWAG_CHECK(getStackValue(&vb, addr2, __sizeof));               \
        setStackValue(addr, __sizeof, vb.kind);                        \
        *(__cast*) addr = *(__cast*) addr2;                            \
        break;                                                         \
    }                                                                  \
    invalidateCurStateStack();

#define BINOP_EQ(__cast, __op, __reg)                                                 \
    do                                                                                \
    {                                                                                 \
        SWAG_CHECK(getRegister(ra, ip->a.u32));                                       \
        SWAG_CHECK(checkNotNull(ra));                                                 \
        if (ra->isStackAddr())                                                        \
        {                                                                             \
            SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, sizeof(vb.reg.__reg))); \
            SWAG_CHECK(checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
            SWAG_CHECK(getStackValue(&va, addr, sizeof(vb.reg.__reg)));               \
            SWAG_CHECK(getImmediateB(vb));                                            \
            if (va.isUnknown() || vb.isUnknown())                                     \
                setStackValue(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);  \
            else                                                                      \
            {                                                                         \
                *(__cast*) addr __op vb.reg.__reg;                                    \
                updateStackValue(addr, sizeof(vb.reg.__reg));                         \
            }                                                                         \
        }                                                                             \
    } while (0);

#define BINOP_EQ_OVF(__cast, __op, __reg, __ovf, __msg, __type)                                                         \
    do                                                                                                                  \
    {                                                                                                                   \
        SWAG_CHECK(getRegister(ra, ip->a.u32));                                                                         \
        SWAG_CHECK(checkNotNull(ra));                                                                                   \
        if (ra->isStackAddr())                                                                                          \
        {                                                                                                               \
            SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, sizeof(vb.reg.__reg)));                                   \
            SWAG_CHECK(checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));                                          \
            SWAG_CHECK(getStackValue(&va, addr, sizeof(vb.reg.__reg)));                                                 \
            SWAG_CHECK(getImmediateB(vb));                                                                              \
            if (va.isUnknown() || vb.isUnknown())                                                                       \
                setStackValue(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);                                    \
            else                                                                                                        \
            {                                                                                                           \
                SWAG_CHECK(checkOverflow(!__ovf(ip, ip->node, *(__cast*) addr, (__cast) vb.reg.__reg), __msg, __type)); \
                *(__cast*) addr __op vb.reg.__reg;                                                                      \
                updateStackValue(addr, sizeof(vb.reg.__reg));                                                           \
            }                                                                                                           \
        }                                                                                                               \
    } while (0);

#define ATOM_EQ(__cast, __op, __reg)                                                  \
    do                                                                                \
    {                                                                                 \
        SWAG_CHECK(getRegister(ra, ip->a.u32));                                       \
        SWAG_CHECK(checkNotNull(ra));                                                 \
        SWAG_CHECK(getRegister(rc, ip->c.u32));                                       \
        rc->setUnknown();                                                             \
        if (ra->isStackAddr())                                                        \
        {                                                                             \
            SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, sizeof(vb.reg.__reg))); \
            SWAG_CHECK(checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
            SWAG_CHECK(getStackValue(rc, addr, sizeof(vb.reg.__reg)));                \
            SWAG_CHECK(getRegister(rb, ip->b.u32));                                   \
            if (rc->isUnknown() || rb->isUnknown())                                   \
                setStackValue(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);  \
            else                                                                      \
            {                                                                         \
                rc->reg.__reg = *(__cast*) addr;                                      \
                *(__cast*) addr __op rb->reg.__reg;                                   \
                updateStackValue(addr, sizeof(vb.reg.__reg));                         \
            }                                                                         \
        }                                                                             \
    } while (0);

#define ATOM_EQ_XCHG(__cast, __reg)                                                   \
    do                                                                                \
    {                                                                                 \
        SWAG_CHECK(getRegister(ra, ip->a.u32));                                       \
        SWAG_CHECK(checkNotNull(ra));                                                 \
        SWAG_CHECK(getRegister(rd, ip->d.u32));                                       \
        rd->setUnknown();                                                             \
        if (ra->isStackAddr())                                                        \
        {                                                                             \
            SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, sizeof(vb.reg.__reg))); \
            SWAG_CHECK(checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
            SWAG_CHECK(getStackValue(rd, addr, sizeof(vb.reg.__reg)));                \
            SWAG_CHECK(getRegister(rb, ip->b.u32));                                   \
            SWAG_CHECK(getRegister(rc, ip->c.u32));                                   \
            if (rd->isUnknown() || rb->isUnknown() || rc->isUnknown())                \
                setStackValue(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);  \
            else                                                                      \
            {                                                                         \
                rd->reg.__reg = *(__cast*) addr;                                      \
                if (rd->reg.__reg == rb->reg.__reg)                                   \
                    *(__cast*) addr = rc->reg.__reg;                                  \
                updateStackValue(addr, sizeof(vb.reg.__reg));                         \
            }                                                                         \
        }                                                                             \
    } while (0);

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

#define BINOP_EQ_SHIFT(__cast, __reg, __func, __isSigned)                             \
    do                                                                                \
    {                                                                                 \
        SWAG_CHECK(getRegister(ra, ip->a.u32));                                       \
        SWAG_CHECK(checkNotNull(ra));                                                 \
        if (ra->isStackAddr())                                                        \
        {                                                                             \
            SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, sizeof(vb.reg.__reg))); \
            SWAG_CHECK(checkStackInitialized(addr, sizeof(vb.reg.__reg), ra));        \
            SWAG_CHECK(getStackValue(&va, addr, sizeof(vb.reg.__reg)));               \
            SWAG_CHECK(getImmediateB(vb));                                            \
            if (va.isUnknown() || vb.isUnknown())                                     \
                setStackValue(addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);  \
            else                                                                      \
            {                                                                         \
                Register r;                                                           \
                r.__reg = *(__cast*) addr;                                            \
                __func(&r, r, vb.reg, sizeof(vb.reg.__reg) * 8, __isSigned);          \
                *(__cast*) addr = r.__reg;                                            \
                updateStackValue(addr, sizeof(vb.reg.__reg));                         \
            }                                                                         \
        }                                                                             \
    } while (0);

#define BINOP_SHIFT(__cast, __reg, __func, __isSigned)                                                    \
    SWAG_CHECK(getImmediateA(va));                                                                        \
    SWAG_CHECK(getImmediateB(vb));                                                                        \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                               \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->updateIp(ip);                                                                                     \
    if (rc->isConstant())                                                                                 \
    {                                                                                                     \
        __func(&rc->reg, va.reg, vb.reg, sizeof(va.reg.__reg) * 8, __isSigned);                           \
    }

#define BINOP(__op, __reg)                                                                                \
    SWAG_CHECK(getImmediateA(va));                                                                        \
    SWAG_CHECK(getImmediateB(vb));                                                                        \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                               \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->updateIp(ip);                                                                                     \
    if (rc->isConstant())                                                                                 \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;

#define BINOP_OVF(__op, __reg, __ovf, __msg, __type)                                                      \
    SWAG_CHECK(getImmediateA(va));                                                                        \
    SWAG_CHECK(getImmediateB(vb));                                                                        \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                               \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->updateIp(ip);                                                                                     \
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
        rc->updateIp(ip);                             \
        break;                                        \
    }                                                 \
    BINOP(__op, __reg);

#define CMP_OP(__op, __reg)                                                                               \
    SWAG_CHECK(getImmediateA(va));                                                                        \
    SWAG_CHECK(getImmediateB(vb));                                                                        \
    SWAG_CHECK(getRegister(rc, ip->c.u32));                                                               \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->updateIp(ip);                                                                                     \
    if (rc->isConstant())                                                                                 \
        rc->reg.b = va.reg.__reg __op vb.reg.__reg;

#define JUMPT(__isCst, __expr1)                           \
    jmpAddState = nullptr;                                \
    if (__isCst)                                          \
    {                                                     \
        ip->dynFlags.add(BCID_SAN_PASS);                  \
        if (__expr1)                                      \
            ip += ip->b.s32 + 1;                          \
        else                                              \
            ip = ip + 1;                                  \
        continue;                                         \
    }                                                     \
    if (!context.statesHere.contains(ip + ip->b.s32 + 1)) \
    {                                                     \
        context.statesHere.insert(ip + ip->b.s32 + 1);    \
        jmpAddState = newState(ip, ip + ip->b.s32 + 1);   \
    }

#define JUMP1(__expr)              \
    SWAG_CHECK(getImmediateA(va)); \
    JUMPT(va.isConstant(), __expr);

#define JUMP2(__expr)              \
    SWAG_CHECK(getImmediateA(va)); \
    SWAG_CHECK(getImmediateC(vc)); \
    JUMPT(va.isConstant() && vc.isConstant(), __expr);
