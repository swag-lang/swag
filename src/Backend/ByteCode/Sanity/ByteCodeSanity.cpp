#include "pch.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanity.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#pragma optimize("", off)

#define STATE() context->states[context->state]

#define MEMCPY(__cast, __sizeof)                                                          \
    SWAG_CHECK(getRegister(context, ra, ip->a.u32));                                      \
    SWAG_CHECK(getRegister(context, rb, ip->b.u32));                                      \
    if (ra->kind == SanityValueKind::StackAddr && rb->kind == SanityValueKind::StackAddr) \
    {                                                                                     \
        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, __sizeof));            \
        SWAG_CHECK(getStackAddress(addr2, context, rb, rb->reg.u32, __sizeof));           \
        SWAG_CHECK(checkStackInitialized(context, addr2, __sizeof, rb));                  \
        SWAG_CHECK(getStackValue(context, &vb, addr2, __sizeof));                         \
        setStackValue(context, addr, __sizeof, vb.kind);                                  \
        *(__cast*) addr = *(__cast*) addr2;                                               \
        break;                                                                            \
    }                                                                                     \
    invalidateCurStateStack(context);

#define BINOP_EQ(__cast, __op, __reg)                                                          \
    do                                                                                         \
    {                                                                                          \
        SWAG_CHECK(getRegister(context, ra, ip->a.u32));                                       \
        SWAG_CHECK(checkNotNull(context, ra));                                                 \
        if (ra->kind == SanityValueKind::StackAddr)                                            \
        {                                                                                      \
            SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, sizeof(vb.reg.__reg))); \
            SWAG_CHECK(checkStackInitialized(context, addr, sizeof(vb.reg.__reg), ra));        \
            SWAG_CHECK(getStackValue(context, &va, addr, sizeof(vb.reg.__reg)));               \
            SWAG_CHECK(getImmediateB(context, vb));                                            \
            if (va.kind == SanityValueKind::Unknown || vb.kind == SanityValueKind::Unknown)    \
                setStackValue(context, addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);  \
            else                                                                               \
            {                                                                                  \
                *(__cast*) addr __op vb.reg.__reg;                                             \
                updateStackValue(context, addr, sizeof(vb.reg.__reg));                         \
            }                                                                                  \
        }                                                                                      \
    } while (0);

#define BINOP_EQ_OVF(__cast, __op, __reg, __ovf, __msg, __type)                                                                  \
    do                                                                                                                           \
    {                                                                                                                            \
        SWAG_CHECK(getRegister(context, ra, ip->a.u32));                                                                         \
        SWAG_CHECK(checkNotNull(context, ra));                                                                                   \
        if (ra->kind == SanityValueKind::StackAddr)                                                                              \
        {                                                                                                                        \
            SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, sizeof(vb.reg.__reg)));                                   \
            SWAG_CHECK(checkStackInitialized(context, addr, sizeof(vb.reg.__reg), ra));                                          \
            SWAG_CHECK(getStackValue(context, &va, addr, sizeof(vb.reg.__reg)));                                                 \
            SWAG_CHECK(getImmediateB(context, vb));                                                                              \
            if (va.kind == SanityValueKind::Unknown || vb.kind == SanityValueKind::Unknown)                                      \
                setStackValue(context, addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);                                    \
            else                                                                                                                 \
            {                                                                                                                    \
                SWAG_CHECK(checkOverflow(context, !__ovf(ip, ip->node, *(__cast*) addr, (__cast) vb.reg.__reg), __msg, __type)); \
                *(__cast*) addr __op vb.reg.__reg;                                                                               \
                updateStackValue(context, addr, sizeof(vb.reg.__reg));                                                           \
            }                                                                                                                    \
        }                                                                                                                        \
    } while (0);

#define ATOM_EQ(__cast, __op, __reg)                                                           \
    do                                                                                         \
    {                                                                                          \
        SWAG_CHECK(getRegister(context, ra, ip->a.u32));                                       \
        SWAG_CHECK(checkNotNull(context, ra));                                                 \
        SWAG_CHECK(getRegister(context, rc, ip->c.u32));                                       \
        rc->kind = SanityValueKind::Unknown;                                                   \
        if (ra->kind == SanityValueKind::StackAddr)                                            \
        {                                                                                      \
            SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, sizeof(vb.reg.__reg))); \
            SWAG_CHECK(checkStackInitialized(context, addr, sizeof(vb.reg.__reg), ra));        \
            SWAG_CHECK(getStackValue(context, rc, addr, sizeof(vb.reg.__reg)));                \
            SWAG_CHECK(getRegister(context, rb, ip->b.u32));                                   \
            if (rc->kind == SanityValueKind::Unknown || rb->kind == SanityValueKind::Unknown)  \
                setStackValue(context, addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);  \
            else                                                                               \
            {                                                                                  \
                rc->reg.__reg = *(__cast*) addr;                                               \
                *(__cast*) addr __op rb->reg.__reg;                                            \
                updateStackValue(context, addr, sizeof(vb.reg.__reg));                         \
            }                                                                                  \
        }                                                                                      \
    } while (0);

#define ATOM_EQ_XCHG(__cast, __reg)                                                                                                   \
    do                                                                                                                                \
    {                                                                                                                                 \
        SWAG_CHECK(getRegister(context, ra, ip->a.u32));                                                                              \
        SWAG_CHECK(checkNotNull(context, ra));                                                                                        \
        SWAG_CHECK(getRegister(context, rd, ip->d.u32));                                                                              \
        rd->kind = SanityValueKind::Unknown;                                                                                          \
        if (ra->kind == SanityValueKind::StackAddr)                                                                                   \
        {                                                                                                                             \
            SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, sizeof(vb.reg.__reg)));                                        \
            SWAG_CHECK(checkStackInitialized(context, addr, sizeof(vb.reg.__reg), ra));                                               \
            SWAG_CHECK(getStackValue(context, rd, addr, sizeof(vb.reg.__reg)));                                                       \
            SWAG_CHECK(getRegister(context, rb, ip->b.u32));                                                                          \
            SWAG_CHECK(getRegister(context, rc, ip->c.u32));                                                                          \
            if (rd->kind == SanityValueKind::Unknown || rb->kind == SanityValueKind::Unknown || rc->kind == SanityValueKind::Unknown) \
                setStackValue(context, addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);                                         \
            else                                                                                                                      \
            {                                                                                                                         \
                rd->reg.__reg = *(__cast*) addr;                                                                                      \
                if (rd->reg.__reg == rb->reg.__reg)                                                                                   \
                    *(__cast*) addr = rc->reg.__reg;                                                                                  \
                updateStackValue(context, addr, sizeof(vb.reg.__reg));                                                                \
            }                                                                                                                         \
        }                                                                                                                             \
    } while (0);

#define BINOP_EQ_DIV(__cast, __op, __reg)                      \
    SWAG_CHECK(getRegister(context, ra, ip->a.u32));           \
    SWAG_CHECK(getImmediateB(context, vb));                    \
    SWAG_CHECK(checkDivZero(context, &vb, vb.reg.__reg == 0)); \
    if (vb.isConstant() && vb.reg.__reg == 0)                  \
    {                                                          \
        SWAG_CHECK(getRegister(context, rc, ip->c.u32));       \
        rc->kind = SanityValueKind::Unknown;                   \
        break;                                                 \
    }                                                          \
    BINOP_EQ(__cast, __op, __reg);

#define BINOP_EQ_SHIFT(__cast, __reg, __func, __isSigned)                                      \
    do                                                                                         \
    {                                                                                          \
        SWAG_CHECK(getRegister(context, ra, ip->a.u32));                                       \
        SWAG_CHECK(checkNotNull(context, ra));                                                 \
        if (ra->kind == SanityValueKind::StackAddr)                                            \
        {                                                                                      \
            SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, sizeof(vb.reg.__reg))); \
            SWAG_CHECK(checkStackInitialized(context, addr, sizeof(vb.reg.__reg), ra));        \
            SWAG_CHECK(getStackValue(context, &va, addr, sizeof(vb.reg.__reg)));               \
            SWAG_CHECK(getImmediateB(context, vb));                                            \
            if (va.kind == SanityValueKind::Unknown || vb.kind == SanityValueKind::Unknown)    \
                setStackValue(context, addr, sizeof(vb.reg.__reg), SanityValueKind::Unknown);  \
            else                                                                               \
            {                                                                                  \
                Register r;                                                                    \
                r.__reg = *(__cast*) addr;                                                     \
                __func(&r, r, vb.reg, sizeof(vb.reg.__reg) * 8, __isSigned);                   \
                *(__cast*) addr = r.__reg;                                                     \
                updateStackValue(context, addr, sizeof(vb.reg.__reg));                         \
            }                                                                                  \
        }                                                                                      \
    } while (0);

#define BINOP_SHIFT(__cast, __reg, __func, __isSigned)                                                    \
    SWAG_CHECK(getImmediateA(context, va));                                                               \
    SWAG_CHECK(getImmediateB(context, vb));                                                               \
    SWAG_CHECK(getRegister(context, rc, ip->c.u32));                                                      \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->update(ip);                                                                                       \
    if (rc->isConstant())                                                                                 \
    {                                                                                                     \
        __func(&rc->reg, va.reg, vb.reg, sizeof(va.reg.__reg) * 8, __isSigned);                           \
    }

#define BINOP(__op, __reg)                                                                                \
    SWAG_CHECK(getImmediateA(context, va));                                                               \
    SWAG_CHECK(getImmediateB(context, vb));                                                               \
    SWAG_CHECK(getRegister(context, rc, ip->c.u32));                                                      \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->update(ip);                                                                                       \
    if (rc->isConstant())                                                                                 \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;

#define BINOP_OVF(__op, __reg, __ovf, __msg, __type)                                                         \
    SWAG_CHECK(getImmediateA(context, va));                                                                  \
    SWAG_CHECK(getImmediateB(context, vb));                                                                  \
    SWAG_CHECK(getRegister(context, rc, ip->c.u32));                                                         \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;    \
    rc->update(ip);                                                                                          \
    if (rc->isConstant())                                                                                    \
    {                                                                                                        \
        SWAG_CHECK(checkOverflow(context, !__ovf(ip, ip->node, va.reg.__reg, vb.reg.__reg), __msg, __type)); \
        rc->reg.__reg = va.reg.__reg __op vb.reg.__reg;                                                      \
    }

#define BINOP_DIV(__op, __reg)                                 \
    SWAG_CHECK(getRegister(context, ra, ip->a.u32));           \
    SWAG_CHECK(getImmediateB(context, vb));                    \
    SWAG_CHECK(checkDivZero(context, &vb, vb.reg.__reg == 0)); \
    if (vb.isConstant() && vb.reg.__reg == 0)                  \
    {                                                          \
        SWAG_CHECK(getRegister(context, rc, ip->c.u32));       \
        rc->kind = SanityValueKind::Unknown;                   \
        rc->update(ip);                                        \
        break;                                                 \
    }                                                          \
    BINOP(__op, __reg);

#define CMP_OP(__op, __reg)                                                                               \
    SWAG_CHECK(getImmediateA(context, va));                                                               \
    SWAG_CHECK(getImmediateB(context, vb));                                                               \
    SWAG_CHECK(getRegister(context, rc, ip->c.u32));                                                      \
    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown; \
    rc->update(ip);                                                                                       \
    if (rc->isConstant())                                                                                 \
        rc->reg.b = va.reg.__reg __op vb.reg.__reg;

#define JUMPT(__isCst, __expr1)                              \
    jmpAddState = nullptr;                                   \
    if (__isCst)                                             \
    {                                                        \
        ip->dynFlags.add(BCID_SAN_PASS);                     \
        if (__expr1)                                         \
            ip += ip->b.s32 + 1;                             \
        else                                                 \
            ip = ip + 1;                                     \
        continue;                                            \
    }                                                        \
    jmpAddState = newState(context, ip, ip + ip->b.s32 + 1); \
    if (!jmpAddState)                                        \
        return true;

#define JUMP1(__expr)                       \
    SWAG_CHECK(getImmediateA(context, va)); \
    JUMPT(va.isConstant(), __expr);

#define JUMP2(__expr)                       \
    SWAG_CHECK(getImmediateA(context, va)); \
    SWAG_CHECK(getImmediateC(context, vc)); \
    JUMPT(va.isConstant() && vc.isConstant(), __expr);

namespace
{
    SanityState* newState(SanityContext* context, ByteCodeInstruction* fromIp, ByteCodeInstruction* startIp)
    {
        if (context->statesHere.contains(startIp))
            return nullptr;
        context->statesHere.insert(startIp);

        const auto state = new SanityState;
        *state           = *STATE();

        context->states.push_back(state);
        state->fromIp = fromIp;
        state->ip     = startIp;
        state->parent = context->state;
        return state;
    }

    bool getRegister(SanityContext* context, SanityValue*& result, uint32_t reg)
    {
        SWAG_ASSERT(reg < STATE()->regs.size());
        result = &STATE()->regs[reg];
        return true;
    }

    bool getStackValue(SanityContext* context, SanityValue* result, void* addr, uint32_t sizeOf)
    {
        const auto offset = static_cast<uint8_t*>(addr) - STATE()->stack.data();
        SWAG_ASSERT(offset + sizeOf <= STATE()->stackValue.size());
        auto addrValue = STATE()->stackValue.data() + offset;

        *result = *addrValue;
        addrValue++;

        for (uint32_t i = 1; i < sizeOf; i++)
        {
            if (addrValue->kind != result->kind)
            {
                result->kind = SanityValueKind::Unknown;
                return true;
            }

            addrValue++;
        }

        return true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////

    Diagnostic* raiseError(SanityContext* context, const Utf8& msg, const SanityValue* locValue = nullptr, AstNode* locNode = nullptr)
    {
        if (!context->bc->sourceFile->module->mustEmitSafety(STATE()->ip->node, SAFETY_SANITY))
            return nullptr;

        const auto ip = STATE()->ip;
        if (!ip->node)
            return nullptr;
        if (!locNode)
            locNode = ip->node;

        const auto err = new Diagnostic{locNode, locNode->token, msg};
        if (!locValue)
            return err;

        VectorNative<SymbolOverload*>      doneOverload;
        VectorNative<AstNode*>             doneNodes;
        VectorNative<ByteCodeInstruction*> ips;
        VectorNative<AstNode*>             ipNodes;

        for (const auto it : locValue->ips)
        {
            ips.push_back(it);
            ipNodes.push_back(it->node);
        }

        if (locNode)
        {
            ips.push_back(nullptr);
            ipNodes.push_back(locNode);
            doneNodes.push_back(locNode);
        }

        const auto isHere = [&](const AstNode* node) {
            for (const auto it : doneNodes)
            {
                if (it->token.startLocation == node->token.startLocation && it->token.endLocation == node->token.endLocation)
                    return true;
            }
            return false;
        };

        for (uint32_t i = ipNodes.size() - 1; i != UINT32_MAX; i--)
        {
            const auto ipNode = ipNodes[i];
            if (!ipNode)
                continue;

            const auto overload = ipNode->resolvedSymbolOverload();

            if (!isHere(ipNode) && (!overload || ipNode != overload->node))
            {
                err->setForceFromContext(true);
                if (ipNode->hasComputedValue() && ipNode->typeInfo->isPointerNull())
                    err->addNote(ipNode, ipNode->token, "from null value");
                else if (ipNode->hasComputedValue())
                    err->addNote(ipNode, ipNode->token, "from compile-time value");
                else if (ipNode->is(AstNodeKind::Return) && ipNode->hasOwnerInline())
                    err->addNote(ipNode->ownerInline(), form("from return value of inlined function [[%s]]", ipNode->ownerInline()->func->tokenName.cstr()));
                else
                    err->addNote(ipNode, ipNode->token, "from");
                err->setForceFromContext(false);
                doneNodes.push_back(ipNode);
            }

            // Show the symbol declaration the last time it's present in an ip node
            if (overload)
            {
                if (!doneOverload.contains(overload) && !isHere(overload->node))
                {
                    bool here = false;
                    for (uint32_t j = 0; j < i; j++)
                    {
                        if (ipNodes[j] && ipNodes[j]->resolvedSymbolOverload() == overload)
                        {
                            here = true;
                            break;
                        }
                    }

                    if (!here)
                    {
                        err->setForceFromContext(true);
                        doneOverload.push_back(overload);

                        Utf8 what = form("declaration of %s [[%s]]", Naming::kindName(overload).cstr(), overload->symbol->name.cstr());
                        err->addNote(overload->node, overload->node->getTokenName(), what);

                        doneNodes.push_back(overload->node);
                        err->setForceFromContext(false);
                    }
                }
            }
        }

        return err;
    }

    bool checkOverflow(SanityContext* context, bool isValid, const char* msgKind, TypeInfo* type)
    {
        if (isValid)
            return true;
        const auto err = raiseError(context, formErr(San0010, msgKind, type->getDisplayNameC()));
        if (err)
            return context->report(*err);
        return true;
    }

    bool checkDivZero(SanityContext* context, const SanityValue* value, bool isZero)
    {
        if (!value->isConstant() || !isZero)
            return true;
        const auto err = raiseError(context, toErr(San0002), value);
        if (err)
            return context->report(*err);
        return true;
    }

    bool checkEscapeFrame(SanityContext* context, const SanityValue* value)
    {
        SWAG_ASSERT(value->reg.u32 < UINT32_MAX);
        const auto err = raiseError(context, toErr(San0004), value);
        if (err)
            return context->report(*err);
        return true;
    }

    bool checkStackOffset(SanityContext* context, const SanityValue* value, uint64_t stackOffset, uint32_t sizeOf = 0)
    {
        if (stackOffset + sizeOf <= static_cast<size_t>(STATE()->stack.size()))
            return true;
        const auto err = raiseError(context, formErr(San0007, stackOffset + sizeOf, STATE()->stack.size()), value);
        if (err)
            return context->report(*err);
        return true;
    }

    bool checkNotNull(SanityContext* context, const SanityValue* value)
    {
        if (!value->isConstant() || value->reg.u64)
            return true;
        const auto err = raiseError(context, toErr(San0006), value);
        if (err)
            return context->report(*err);
        return true;
    }

    bool checkNotNullReturn(SanityContext* context, uint32_t reg)
    {
        if (!context->bc->sourceFile->module->mustEmitSafety(STATE()->ip->node, SAFETY_SANITY | SAFETY_NULL))
            return true;

        const auto ip = STATE()->ip;
        if (ip->flags.has(BCI_CANT_OVERFLOW))
            return true;

        SanityValue* ra = nullptr;
        SWAG_CHECK(getRegister(context, ra, reg));

        if (ra->kind == SanityValueKind::Constant && !ra->reg.u64)
        {
            const auto returnType = context->bc->typeInfoFunc->concreteReturnType();
            if (!returnType->isNullable() && returnType->couldBeNull())
            {
                const auto err = raiseError(context, toErr(San0003), ra);
                if (err)
                    return context->report(*err);
            }
        }

        return true;
    }

    bool checkNotNullArguments(SanityContext* context, VectorNative<uint32_t> pushParams, const Utf8& intrinsic)
    {
        if (!context->bc->sourceFile->module->mustEmitSafety(STATE()->ip->node, SAFETY_SANITY | SAFETY_NULL))
            return true;

        const auto        ip       = STATE()->ip;
        TypeInfoFuncAttr* typeFunc = nullptr;

        if (intrinsic.empty())
        {
            if (!ip->node || !ip->node->is(AstNodeKind::Identifier))
                return true;
            const auto ident = castAst<AstIdentifier>(ip->node, AstNodeKind::Identifier);
            if (!ident->callParameters)
                return true;
            typeFunc = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);
        }

        VectorNative<uint32_t> done;
        bool                   doneObjInterface = false;
        for (uint32_t i = pushParams.size() - 1; i != UINT32_MAX; i--)
        {
            uint32_t idx;
            if (typeFunc)
                idx = typeFunc->registerIdxToParamIdx(pushParams.size() - i - 1);
            else
                idx = pushParams.size() - i - 1;

            if (typeFunc && typeFunc->isClosure() && idx == 0)
                continue;
            const auto typeParam = typeFunc ? typeFunc->parameters[idx]->typeInfo : nullptr;

            // For an interface, we test the second register, which is the pointer to the 'itable'.
            // It is legit to have a null object with a table.
            if (typeParam && typeParam->isInterface())
            {
                if (!doneObjInterface)
                {
                    doneObjInterface = true;
                    continue;
                }
            }

            if (done.contains(idx))
                continue;
            done.push_back(idx);

            if (!typeParam || (typeParam->couldBeNull() && !typeParam->isNullable()))
            {
                SanityValue* ra = nullptr;
                SWAG_CHECK(getRegister(context, ra, pushParams[i]));

                if (ra->isConstant() && !ra->reg.u64)
                {
                    Utf8 msg;
                    if (ip->op == ByteCodeOp::LambdaCall)
                        msg = formErr(San0001, "lambda");
                    else if (typeFunc)
                        msg = formErr(San0001, typeFunc->declNode->token.cstr());
                    else
                        msg = formErr(San0001, intrinsic.cstr());

                    AstNode* locNode = nullptr;
                    if (ip->node && ip->node->childCount() && ip->node->lastChild()->is(AstNodeKind::FuncCallParams))
                        locNode = ip->node->lastChild()->children[idx];
                    else if (ip->node && ip->node->childCount() == 1)
                        locNode = ip->node->firstChild();

                    const auto err = raiseError(context, msg, ra, locNode);
                    if (!err)
                        continue;

                    if (typeFunc && typeFunc->declNode && typeFunc->declNode->is(AstNodeKind::FuncDecl))
                    {
                        const auto funcDecl = castAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);
                        SWAG_ASSERT(funcDecl->parameters);

                        const auto childParam = funcDecl->parameters->children[idx];
                        if (childParam->isGeneratedSelf())
                        {
                            msg = formNte(Nte0224, "function", funcDecl->token.cstr(), "an implicit UFCS first argument");
                            err->addNote(funcDecl, funcDecl->getTokenName(), msg);
                        }
                        else
                        {
                            msg = formNte(Nte0224, "function", funcDecl->token.cstr(), Naming::aNiceArgumentRank(idx + 1).cstr());
                            err->addNote(childParam, msg);
                        }
                    }
                    else if (!intrinsic.empty())
                    {
                        err->addNote(formNte(Nte0224, "intrinsic", intrinsic.cstr(), Naming::aNiceArgumentRank(idx + 1).cstr()));
                    }

                    return context->report(*err);
                }
            }
        }

        return true;
    }

    bool checkStackInitialized(SanityContext* context, void* addr, uint32_t sizeOf, const SanityValue* locValue = nullptr)
    {
        SanityValue memValue;
        SWAG_CHECK(getStackValue(context, &memValue, addr, sizeOf));
        if (memValue.kind != SanityValueKind::Invalid)
            return true;

        const auto err = raiseError(context, toErr(San0008), locValue);
        if (err)
            return context->report(*err);
        return true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////

    bool getImmediateA(SanityContext* context, SanityValue& result)
    {
        const auto ip = STATE()->ip;
        if (ip->hasFlag(BCI_IMM_A))
        {
            result.kind = SanityValueKind::Constant;
            result.reg  = ip->b;
            return true;
        }

        SanityValue* ra = nullptr;
        SWAG_CHECK(getRegister(context, ra, ip->a.u32));
        result = *ra;
        return true;
    }

    bool getImmediateB(SanityContext* context, SanityValue& result)
    {
        const auto ip = STATE()->ip;
        if (ip->hasFlag(BCI_IMM_B))
        {
            result.kind = SanityValueKind::Constant;
            result.reg  = ip->b;
            return true;
        }

        SanityValue* rb = nullptr;
        SWAG_CHECK(getRegister(context, rb, ip->b.u32));
        result = *rb;
        return true;
    }

    bool getImmediateC(SanityContext* context, SanityValue& result)
    {
        const auto ip = STATE()->ip;
        if (ip->hasFlag(BCI_IMM_C))
        {
            result.kind = SanityValueKind::Constant;
            result.reg  = ip->c;
            return true;
        }

        SanityValue* rc = nullptr;
        SWAG_CHECK(getRegister(context, rc, ip->c.u32));
        result = *rc;
        return true;
    }

    bool getImmediateD(SanityContext* context, SanityValue& result)
    {
        const auto ip = STATE()->ip;
        if (ip->hasFlag(BCI_IMM_D))
        {
            result.kind = SanityValueKind::Constant;
            result.reg  = ip->d;
            return true;
        }

        SanityValue* rd = nullptr;
        SWAG_CHECK(getRegister(context, rd, ip->d.u32));
        result = *rd;
        return true;
    }

    bool getStackAddress(uint8_t*& result, SanityContext* context, SanityValue* value, uint64_t stackOffset, uint32_t sizeOf = 0)
    {
        SWAG_CHECK(checkStackOffset(context, value, stackOffset, sizeOf));
        result = STATE()->stack.data() + stackOffset;
        return true;
    }

    void setStackValue(SanityContext* context, void* addr, uint32_t sizeOf, SanityValueKind kind)
    {
        const auto offset = static_cast<uint32_t>(static_cast<uint8_t*>(addr) - STATE()->stack.data());
        SWAG_ASSERT(offset + sizeOf <= STATE()->stackValue.size());
        for (uint32_t i = offset; i < offset + sizeOf; i++)
        {
            auto& val = STATE()->stackValue[i];
            val.kind  = kind;
            val.ips.clear();
            if (kind == SanityValueKind::Constant)
                val.ips.push_back(STATE()->ip);
        }
    }

    void updateStackValue(SanityContext* context, void* addr, uint32_t sizeOf)
    {
        const auto offset = static_cast<uint32_t>(static_cast<uint8_t*>(addr) - STATE()->stack.data());
        SWAG_ASSERT(offset + sizeOf <= STATE()->stackValue.size());
        for (uint32_t i = offset; i < offset + sizeOf; i++)
        {
            auto& val = STATE()->stackValue[i];
            val.ips.push_back(STATE()->ip);
        }
    }

    void invalidateCurStateStack(SanityContext* context)
    {
        setStackValue(context, STATE()->stack.data(), STATE()->stack.size(), SanityValueKind::Unknown);
    }

    bool sanityLoop(SanityContext* context)
    {
        SanityValue*           ra    = nullptr;
        SanityValue*           rb    = nullptr;
        SanityValue*           rc    = nullptr;
        SanityValue*           rd    = nullptr;
        uint8_t*               addr  = nullptr;
        uint8_t*               addr2 = nullptr;
        SanityValue            va, vb, vc, vd;
        SanityState*           jmpAddState = nullptr;
        VectorNative<uint32_t> pushParams;

        auto ip = STATE()->ip;
        while (ip->op != ByteCodeOp::End)
        {
            if (ip->dynFlags.has(BCID_SAN_PASS))
                return true;

            if (ip->hasFlag(BCI_SAFETY) || !ip->node)
            {
                ip->dynFlags.add(BCID_SAN_PASS);
                ip++;
                continue;
            }

            STATE()->ip = ip;
            switch (ip->op)
            {
                case ByteCodeOp::Ret:
                case ByteCodeOp::InternalPanic:
                case ByteCodeOp::IntrinsicPanic:
                    return true;

                case ByteCodeOp::PushRAParamCond:
                    pushParams.push_back(ip->b.u32);
                    break;

                case ByteCodeOp::PushRAParam:
                    pushParams.push_back(ip->a.u32);
                    break;

                case ByteCodeOp::PushRAParam2:
                    pushParams.push_back(ip->a.u32);
                    pushParams.push_back(ip->b.u32);
                    break;

                case ByteCodeOp::PushRAParam3:
                    pushParams.push_back(ip->a.u32);
                    pushParams.push_back(ip->b.u32);
                    pushParams.push_back(ip->c.u32);
                    break;

                case ByteCodeOp::PushRAParam4:
                    pushParams.push_back(ip->a.u32);
                    pushParams.push_back(ip->b.u32);
                    pushParams.push_back(ip->c.u32);
                    pushParams.push_back(ip->d.u32);
                    break;

                case ByteCodeOp::DecSPBP:
                case ByteCodeOp::IncSPPostCall:
                case ByteCodeOp::IncSPPostCallCond:
                case ByteCodeOp::PushRVParam:
                case ByteCodeOp::InternalInitStackTrace:
                case ByteCodeOp::InternalStackTrace:
                case ByteCodeOp::InternalStackTraceConst:
                case ByteCodeOp::PushRR:
                case ByteCodeOp::PopRR:
                case ByteCodeOp::InternalSetErr:
                case ByteCodeOp::InternalPushErr:
                case ByteCodeOp::InternalPopErr:
                case ByteCodeOp::InternalCatchErr:
                case ByteCodeOp::InternalFailedAssume:
                case ByteCodeOp::IntrinsicCompilerError:
                case ByteCodeOp::IntrinsicCompilerWarning:
                case ByteCodeOp::IntrinsicSetContext:
                case ByteCodeOp::IntrinsicCVaEnd:
                case ByteCodeOp::IntrinsicFree:
                case ByteCodeOp::DebugNop:
                case ByteCodeOp::Unreachable:
                case ByteCodeOp::InternalUnreachable:
                case ByteCodeOp::IntrinsicBcBreakpoint:
                case ByteCodeOp::ClearRR8:
                case ByteCodeOp::ClearRR16:
                case ByteCodeOp::ClearRR32:
                case ByteCodeOp::ClearRR64:
                case ByteCodeOp::ClearRRX:
                    break;

                case ByteCodeOp::CopyRTtoRA:
                {
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->set(ip);
                    const auto typeInfo     = reinterpret_cast<TypeInfo*>(ip->c.pointer);
                    const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                    const auto returnType   = typeInfoFunc->concreteReturnType();
                    if (returnType->isNullable() && !returnType->isClosure())
                    {
                        // ra->kind        = SanityValueKind::ForceNull;
                        // ra->reg.pointer = nullptr;
                    }
                    break;
                }

                case ByteCodeOp::CopyRT2toRARB:
                {
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind = SanityValueKind::Unknown;
                    rb->kind = SanityValueKind::Unknown;
                    ra->set(ip);
                    rb->set(ip);
                    const auto typeInfo     = reinterpret_cast<TypeInfo*>(ip->c.pointer);
                    const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                    const auto returnType   = typeInfoFunc->concreteReturnType();
                    if (returnType->isNullable())
                    {
                        // ra->kind        = SanityValueKind::ForceNull;
                        // ra->reg.pointer = nullptr;
                        // rb->kind        = SanityValueKind::ForceNull;
                        // rb->reg.pointer = nullptr;
                    }
                    break;
                }

                case ByteCodeOp::GetParam64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->set(ip);

                    if (ip->node && ip->node->resolvedSymbolOverload())
                    {
                        const auto overload = ip->node->resolvedSymbolOverload();
                        const auto typeOver = TypeManager::concreteType(overload->typeInfo);
                        if (overload->typeInfo->isNullable() && !typeOver->isClosure())
                        {
                            ra->kind        = SanityValueKind::ForceNull;
                            ra->reg.pointer = nullptr;
                        }
                    }

                    break;

                // Fake 1 value
                case ByteCodeOp::InternalGetTlsPtr:
                case ByteCodeOp::IntrinsicGetContext:
                case ByteCodeOp::IntrinsicGetProcessInfos:
                case ByteCodeOp::InternalHasErr:
                case ByteCodeOp::GetParam64SI:
                case ByteCodeOp::MakeBssSegPointer:
                case ByteCodeOp::MakeConstantSegPointer:
                case ByteCodeOp::MakeMutableSegPointer:
                case ByteCodeOp::MakeCompilerSegPointer:
                case ByteCodeOp::GetFromMutableSeg8:
                case ByteCodeOp::GetFromMutableSeg16:
                case ByteCodeOp::GetFromMutableSeg32:
                case ByteCodeOp::GetFromMutableSeg64:
                case ByteCodeOp::GetFromBssSeg8:
                case ByteCodeOp::GetFromBssSeg16:
                case ByteCodeOp::GetFromBssSeg32:
                case ByteCodeOp::GetFromBssSeg64:
                case ByteCodeOp::GetFromCompilerSeg8:
                case ByteCodeOp::GetFromCompilerSeg16:
                case ByteCodeOp::GetFromCompilerSeg32:
                case ByteCodeOp::GetFromCompilerSeg64:
                case ByteCodeOp::CopySP:
                case ByteCodeOp::SaveRRtoRA:
                case ByteCodeOp::CopyRRtoRA:
                case ByteCodeOp::CopySPVaargs:
                case ByteCodeOp::MakeLambda:
                case ByteCodeOp::CopyRBAddrToRA:
                case ByteCodeOp::IntrinsicMakeCallback:
                case ByteCodeOp::CloneString:
                case ByteCodeOp::IntrinsicIsConstExprSI:
                case ByteCodeOp::IntrinsicLocationSI:
                case ByteCodeOp::IntrinsicAlloc:
                case ByteCodeOp::IntrinsicRealloc:
                case ByteCodeOp::IntrinsicSysAlloc:
                case ByteCodeOp::IntrinsicDbgAlloc:
                case ByteCodeOp::IntrinsicRtFlags:
                case ByteCodeOp::IntrinsicIsByteCode:
                case ByteCodeOp::JumpIfError:
                case ByteCodeOp::JumpIfNoError:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    break;

                case ByteCodeOp::CopyRAtoRT:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    if (ra->kind == SanityValueKind::StackAddr)
                        invalidateCurStateStack(context);
                    break;

                case ByteCodeOp::IntrinsicCVaStart:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNullArguments(context, {ip->a.u32}, "@cvastart"));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, 8));
                        setStackValue(context, addr, 8, SanityValueKind::Unknown);
                    }
                    break;

                case ByteCodeOp::IntrinsicCVaArg:
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    rb->kind = SanityValueKind::Unknown;
                    rb->update(ip);
                    break;

                case ByteCodeOp::IntrinsicAtomicAddS8:
                    ATOM_EQ(int8_t, +=, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicAddS16:
                    ATOM_EQ(int16_t, +=, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicAddS32:
                    ATOM_EQ(int32_t, +=, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicAddS64:
                    ATOM_EQ(int64_t, +=, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicAndS8:
                    ATOM_EQ(int8_t, &=, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicAndS16:
                    ATOM_EQ(int16_t, &=, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicAndS32:
                    ATOM_EQ(int32_t, &=, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicAndS64:
                    ATOM_EQ(int64_t, &=, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicOrS8:
                    ATOM_EQ(int8_t, |=, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicOrS16:
                    ATOM_EQ(int16_t, |=, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicOrS32:
                    ATOM_EQ(int32_t, |=, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicOrS64:
                    ATOM_EQ(int64_t, |=, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicXorS8:
                    ATOM_EQ(int8_t, ^=, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicXorS16:
                    ATOM_EQ(int16_t, ^=, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicXorS32:
                    ATOM_EQ(int32_t, ^=, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicXorS64:
                    ATOM_EQ(int64_t, ^=, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicXchgS8:
                    ATOM_EQ(int8_t, =, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicXchgS16:
                    ATOM_EQ(int16_t, =, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicXchgS32:
                    ATOM_EQ(int32_t, =, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicXchgS64:
                    ATOM_EQ(int64_t, =, s64);
                    break;

                case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
                    ATOM_EQ_XCHG(int8_t, s8);
                    break;
                case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
                    ATOM_EQ_XCHG(int16_t, s16);
                    break;
                case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
                    ATOM_EQ_XCHG(int32_t, s32);
                    break;
                case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
                    ATOM_EQ_XCHG(int64_t, s64);
                    break;

                case ByteCodeOp::IntrinsicItfTableOf:
                    SWAG_CHECK(checkNotNullArguments(context, {ip->b.u32, ip->a.u32}, "@itfTableOf"));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    rc->kind = SanityValueKind::Unknown;
                    rc->set(ip);
                    break;

                case ByteCodeOp::IntrinsicIs:
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    rc->kind = SanityValueKind::Unknown;
                    rc->set(ip);
                    break;

                case ByteCodeOp::IntrinsicTypeCmp:
                case ByteCodeOp::IntrinsicStringCmp:
                case ByteCodeOp::IntrinsicAs:
                    SWAG_CHECK(getRegister(context, rd, ip->d.u32));
                    rd->kind = SanityValueKind::Unknown;
                    rd->set(ip);
                    break;

                // Fake 2 values
                case ByteCodeOp::IntrinsicGvtd:
                case ByteCodeOp::IntrinsicGetErr:
                case ByteCodeOp::IntrinsicModules:
                case ByteCodeOp::DeRefStringSlice:
                case ByteCodeOp::IntrinsicArguments:
                case ByteCodeOp::IntrinsicCompiler:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind = SanityValueKind::Unknown;
                    rb->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    rb->update(ip);
                    break;

                case ByteCodeOp::Jump:
                    ip->dynFlags.add(BCID_SAN_PASS);
                    ip += ip->b.s32 + 1;
                    continue;

                case ByteCodeOp::JumpIfFalse:
                    JUMP1(!va.reg.b);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = SanityValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                        jmpAddState->regs[ip->a.u32].update(ip);
                    }
                    break;
                case ByteCodeOp::JumpIfTrue:
                    JUMP1(va.reg.b);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = SanityValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 1;
                        jmpAddState->regs[ip->a.u32].update(ip);
                    }
                    break;

                case ByteCodeOp::JumpIfEqual64:
                    JUMP2(va.reg.u64 == vc.reg.u64);
                    if (jmpAddState && vc.isConstant())
                    {
                        jmpAddState->regs[ip->a.u32].kind    = vc.kind;
                        jmpAddState->regs[ip->a.u32].reg.u64 = vc.reg.u64;
                        jmpAddState->regs[ip->a.u32].update(ip);
                    }
                    break;

                case ByteCodeOp::JumpIfZero8:
                    JUMP1(va.reg.u8 == 0);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = SanityValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                        jmpAddState->regs[ip->a.u32].update(ip);
                    }
                    break;
                case ByteCodeOp::JumpIfZero16:
                    JUMP1(va.reg.u16 == 0);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = SanityValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                        jmpAddState->regs[ip->a.u32].update(ip);
                    }
                    break;
                case ByteCodeOp::JumpIfZero32:
                    JUMP1(va.reg.u32 == 0);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = SanityValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                        jmpAddState->regs[ip->a.u32].update(ip);
                    }
                    break;
                case ByteCodeOp::JumpIfZero64:
                    JUMP1(va.reg.u64 == 0);
                    if (jmpAddState)
                    {
                        jmpAddState->regs[ip->a.u32].kind    = SanityValueKind::Constant;
                        jmpAddState->regs[ip->a.u32].reg.u64 = 0;
                        jmpAddState->regs[ip->a.u32].update(ip);
                    }
                    break;

                case ByteCodeOp::JumpIfLowerEqS64:
                    JUMP2(va.reg.s64 <= vc.reg.s64);
                    break;
                case ByteCodeOp::JumpIfNotZero8:
                    JUMP1(va.reg.u8 != 0);
                    break;
                case ByteCodeOp::JumpIfNotZero16:
                    SWAG_CHECK(getImmediateA(context, va));
                    JUMP1(va.reg.u16 != 0);
                    break;
                case ByteCodeOp::JumpIfNotZero32:
                    SWAG_CHECK(getImmediateA(context, va));
                    JUMP1(va.reg.u32 != 0);
                    break;
                case ByteCodeOp::JumpIfNotZero64:
                    SWAG_CHECK(getImmediateA(context, va));
                    JUMP1(va.reg.u64 != 0);
                    break;

                case ByteCodeOp::CopyRBtoRA64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    *ra = *rb;
                    ra->update(ip);
                    break;

                case ByteCodeOp::CompareOp3Way8:
                    SWAG_CHECK(getImmediateA(context, va));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                    rc->update(ip);
                    if (rc->isConstant())
                    {
                        auto sub    = va.reg.u8 - vb.reg.u8;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3Way16:
                    SWAG_CHECK(getImmediateA(context, va));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                    rc->update(ip);
                    if (rc->isConstant())
                    {
                        auto sub    = va.reg.u16 - vb.reg.u16;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3Way32:
                    SWAG_CHECK(getImmediateA(context, va));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                    rc->update(ip);
                    if (rc->isConstant())
                    {
                        auto sub    = va.reg.u32 - vb.reg.u32;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3Way64:
                    SWAG_CHECK(getImmediateA(context, va));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                    rc->update(ip);
                    if (rc->isConstant())
                    {
                        auto sub    = va.reg.u64 - vb.reg.u64;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3WayF32:
                    SWAG_CHECK(getImmediateA(context, va));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                    rc->update(ip);
                    if (rc->isConstant())
                    {
                        auto sub    = va.reg.f32 - vb.reg.f32;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;
                case ByteCodeOp::CompareOp3WayF64:
                    SWAG_CHECK(getImmediateA(context, va));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                    rc->update(ip);
                    if (rc->isConstant())
                    {
                        auto sub    = va.reg.f64 - vb.reg.f64;
                        rc->reg.s32 = (sub > 0) - (sub < 0);
                    }
                    break;

                case ByteCodeOp::ZeroToTrue:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.b = ra->reg.s32 == 0;
                    ra->update(ip);
                    break;
                case ByteCodeOp::GreaterEqZeroToTrue:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.b = ra->reg.s32 >= 0;
                    ra->update(ip);
                    break;
                case ByteCodeOp::GreaterZeroToTrue:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.b = ra->reg.s32 > 0;
                    ra->update(ip);
                    break;
                case ByteCodeOp::LowerEqZeroToTrue:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.b = ra->reg.s32 <= 0;
                    ra->update(ip);
                    break;
                case ByteCodeOp::LowerZeroToTrue:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.b = ra->reg.s32 < 0;
                    ra->update(ip);
                    break;

                case ByteCodeOp::IncrementRA64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.u64++;
                    ra->update(ip);
                    break;
                case ByteCodeOp::DecrementRA64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.u64--;
                    ra->update(ip);
                    break;

                case ByteCodeOp::MakeStackPointer:
                    SWAG_CHECK(checkStackOffset(context, nullptr, ip->b.u32));
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind    = SanityValueKind::StackAddr;
                    ra->reg.u64 = ip->b.u32;
                    ra->update(ip);
                    break;

                case ByteCodeOp::SetZeroStackX:
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->a.u32, ip->b.u32));
                    memset(addr, 0, ip->b.u32);
                    setStackValue(context, addr, ip->b.u32, SanityValueKind::Constant);
                    break;
                case ByteCodeOp::SetZeroStack8:
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->a.u32, 1));
                    *addr = 0;
                    setStackValue(context, addr, 1, SanityValueKind::Constant);
                    break;
                case ByteCodeOp::SetZeroStack16:
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->a.u32, 2));
                    *reinterpret_cast<uint16_t*>(addr) = 0;
                    setStackValue(context, addr, 2, SanityValueKind::Constant);
                    break;
                case ByteCodeOp::SetZeroStack32:
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->a.u32, 4));
                    *reinterpret_cast<uint32_t*>(addr) = 0;
                    setStackValue(context, addr, 4, SanityValueKind::Constant);
                    break;
                case ByteCodeOp::SetZeroStack64:
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->a.u32, 8));
                    *reinterpret_cast<uint64_t*>(addr) = 0;
                    setStackValue(context, addr, 8, SanityValueKind::Constant);
                    break;

                case ByteCodeOp::SetAtStackPointer64:
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->a.u32, 8));
                    SWAG_CHECK(getImmediateB(context, vb));
                    *reinterpret_cast<uint64_t*>(addr) = vb.reg.u64;
                    setStackValue(context, addr, 8, vb.kind);
                    break;

                case ByteCodeOp::SetZeroAtPointerXRB:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_ASSERT(rb->reg.u64 * ip->c.u64 <= UINT32_MAX);
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64)));
                        memset(addr, 0, rb->reg.u64 * ip->c.u64);
                        setStackValue(context, addr, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64), SanityValueKind::Constant);
                    }
                    break;

                case ByteCodeOp::SetZeroAtPointerX:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_ASSERT(ip->b.u64 <= UINT32_MAX);
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->c.u32, static_cast<uint32_t>(ip->b.u64)));
                        memset(addr, 0, ip->b.u64);
                        setStackValue(context, addr, static_cast<uint32_t>(ip->b.u64), SanityValueKind::Constant);
                    }
                    break;
                case ByteCodeOp::SetZeroAtPointer8:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->b.u32, 1));
                        *addr = 0;
                        setStackValue(context, addr, 1, SanityValueKind::Constant);
                    }
                    break;
                case ByteCodeOp::SetZeroAtPointer16:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->b.u32, 2));
                        *reinterpret_cast<uint16_t*>(addr) = 0;
                        setStackValue(context, addr, 2, SanityValueKind::Constant);
                    }
                    break;
                case ByteCodeOp::SetZeroAtPointer32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->b.u32, 4));
                        *reinterpret_cast<uint32_t*>(addr) = 0;
                        setStackValue(context, addr, 4, SanityValueKind::Constant);
                    }
                    break;
                case ByteCodeOp::SetZeroAtPointer64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->b.u32, 8));
                        *reinterpret_cast<uint64_t*>(addr) = 0;
                        setStackValue(context, addr, 8, SanityValueKind::Constant);
                    }
                    break;

                case ByteCodeOp::SetAtPointer8:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->c.u32, 1));
                        SWAG_CHECK(getImmediateB(context, vb));
                        *addr = vb.reg.u8;
                        setStackValue(context, addr, 1, vb.kind);
                    }
                    break;
                case ByteCodeOp::SetAtPointer16:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->c.u32, 2));
                        SWAG_CHECK(getImmediateB(context, vb));
                        *reinterpret_cast<uint16_t*>(addr) = vb.reg.u16;
                        setStackValue(context, addr, 2, vb.kind);
                    }
                    break;
                case ByteCodeOp::SetAtPointer32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->c.u32, 4));
                        SWAG_CHECK(getImmediateB(context, vb));
                        *reinterpret_cast<uint32_t*>(addr) = vb.reg.u32;
                        setStackValue(context, addr, 4, vb.kind);
                    }
                    break;
                case ByteCodeOp::SetAtPointer64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u64 + ip->c.u32, 8));
                        SWAG_CHECK(getImmediateB(context, vb));
                        *reinterpret_cast<uint64_t*>(addr) = vb.reg.u64;
                        setStackValue(context, addr, 8, vb.kind);
                    }
                    break;

                case ByteCodeOp::SetImmediate32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind    = SanityValueKind::Constant;
                    ra->reg.u64 = ip->b.u32;
                    ra->set(ip);
                    break;
                case ByteCodeOp::SetImmediate64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind    = SanityValueKind::Constant;
                    ra->reg.u64 = ip->b.u64;
                    ra->set(ip);
                    break;
                case ByteCodeOp::ClearRA:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind    = SanityValueKind::Constant;
                    ra->reg.u64 = 0;
                    ra->set(ip);
                    break;

                case ByteCodeOp::Add64byVB64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.s64 += ip->b.s64;
                    ra->update(ip);
                    break;
                case ByteCodeOp::Mul64byVB64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.s64 *= ip->b.s64;
                    ra->update(ip);
                    break;
                case ByteCodeOp::Div64byVB64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.s64 /= ip->b.s64;
                    ra->update(ip);
                    break;
                case ByteCodeOp::MulAddVC64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    if (rb->kind == SanityValueKind::Unknown)
                        ra->kind = SanityValueKind::Unknown;
                    else
                        ra->reg.u64 = ra->reg.u64 * (rb->reg.u64 + ip->c.u32);
                    ra->update(ip);
                    break;

                case ByteCodeOp::GetFromStack8:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->b.u32, 1));
                    SWAG_CHECK(getStackValue(context, ra, addr, 1));
                    SWAG_CHECK(checkStackInitialized(context, addr, 1, ra));
                    ra->reg.u64 = *addr;
                    ra->update(ip);
                    break;
                case ByteCodeOp::GetFromStack16:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->b.u32, 2));
                    SWAG_CHECK(getStackValue(context, ra, addr, 2));
                    SWAG_CHECK(checkStackInitialized(context, addr, 2, ra));
                    ra->reg.u64 = *reinterpret_cast<uint16_t*>(addr);
                    ra->update(ip);
                    break;
                case ByteCodeOp::GetFromStack32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->b.u32, 4));
                    SWAG_CHECK(getStackValue(context, ra, addr, 4));
                    SWAG_CHECK(checkStackInitialized(context, addr, 4, ra));
                    ra->reg.u64 = *reinterpret_cast<uint32_t*>(addr);
                    ra->update(ip);
                    break;
                case ByteCodeOp::GetFromStack64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getStackAddress(addr, context, nullptr, ip->b.u32, 8));
                    SWAG_CHECK(getStackValue(context, ra, addr, 8));
                    SWAG_CHECK(checkStackInitialized(context, addr, 8, ra));
                    ra->reg.u64 = *reinterpret_cast<uint64_t*>(addr);
                    ra->update(ip);
                    break;

                case ByteCodeOp::IncPointer64:
                {
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));

                    if (ip->node->is(AstNodeKind::Identifier))
                    {
                        const auto id    = castAst<AstIdentifier>(ip->node, AstNodeKind::Identifier);
                        const auto idRef = id->identifierRef();
                        for (auto i = id->childParentIdx() - 1; i != UINT32_MAX; i--)
                        {
                            const auto node = idRef->children[i];
                            if (node->is(AstNodeKind::Identifier) && node->hasSpecFlag(AstIdentifier::SPEC_FLAG_FROM_USING))
                                continue;

                            for (auto it : ra->ips)
                            {
                                if (it->node == ip->node)
                                    it->node = node;
                                else if (it->node->is(AstNodeKind::Identifier) && it->node->hasSpecFlag(AstIdentifier::SPEC_FLAG_FROM_USING))
                                    it->node = node;
                            }

                            ip->node = node;
                            break;
                        }
                    }

                    SWAG_CHECK(checkNotNull(context, ra));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    *rc = *ra;
                    SWAG_CHECK(getImmediateB(context, vb));
                    if (vb.kind == SanityValueKind::Unknown)
                        rc->kind = SanityValueKind::Unknown;
                    else
                        rc->reg.u64 += vb.reg.s64;
                    rc->update(ip);
                }
                break;

                case ByteCodeOp::IncMulPointer64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    *rc = *ra;
                    SWAG_CHECK(getImmediateB(context, vb));
                    if (vb.kind == SanityValueKind::Unknown)
                        rc->kind = SanityValueKind::Unknown;
                    else
                        rc->reg.u64 += vb.reg.s64 * ip->d.u64;
                    rc->update(ip);
                    break;
                case ByteCodeOp::DecPointer64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    *rc = *ra;
                    SWAG_CHECK(getImmediateB(context, vb));
                    if (vb.kind == SanityValueKind::Unknown)
                        rc->kind = SanityValueKind::Unknown;
                    else
                        rc->reg.u64 -= vb.reg.s64;
                    rc->update(ip);
                    break;

                case ByteCodeOp::CopyRAtoRR:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNullReturn(context, ip->a.u32));

                    if (!ip->hasFlag(BCI_IMM_A))
                    {
                        if (ra->kind == SanityValueKind::StackAddr)
                            return checkEscapeFrame(context, ra);
                    }
                    break;

                case ByteCodeOp::CopyRARBtoRR2:
                    if (ip->hasFlag(BCI_IMM_A))
                        break;
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));

                    if (ra->kind == SanityValueKind::StackAddr)
                    {
                        // Legit in #run block, as we will make a copy
                        if (context->bc->node && context->bc->node->hasAstFlag(AST_IN_RUN_BLOCK))
                            break;
                        return checkEscapeFrame(context, ra);
                    }

                    break;

                case ByteCodeOp::DeRef8:
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(checkNotNull(context, rb));
                    if (rb->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                        SWAG_CHECK(getStackAddress(addr, context, rb, rb->reg.u64 + ip->c.s64, 1));
                        SWAG_CHECK(checkStackInitialized(context, addr, 1));
                        SWAG_CHECK(getStackValue(context, ra, addr, 1));
                        ra->reg.u64 = *addr;
                        ra->update(ip);
                        break;
                    }
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    break;
                case ByteCodeOp::DeRef16:
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(checkNotNull(context, rb));
                    if (rb->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                        SWAG_CHECK(getStackAddress(addr, context, rb, rb->reg.u64 + ip->c.s64, 2));
                        SWAG_CHECK(checkStackInitialized(context, addr, 2));
                        SWAG_CHECK(getStackValue(context, ra, addr, 2));
                        ra->reg.u64 = *reinterpret_cast<uint16_t*>(addr);
                        ra->update(ip);
                        break;
                    }
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    break;
                case ByteCodeOp::DeRef32:
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(checkNotNull(context, rb));
                    if (rb->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                        SWAG_CHECK(getStackAddress(addr, context, rb, rb->reg.u64 + ip->c.s64, 4));
                        SWAG_CHECK(checkStackInitialized(context, addr, 4));
                        SWAG_CHECK(getStackValue(context, ra, addr, 4));
                        ra->reg.u64 = *reinterpret_cast<uint32_t*>(addr);
                        ra->update(ip);
                        break;
                    }
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    break;
                case ByteCodeOp::DeRef64:
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(checkNotNull(context, rb));
                    if (rb->kind == SanityValueKind::StackAddr)
                    {
                        SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                        SWAG_CHECK(getStackAddress(addr, context, rb, rb->reg.u64 + ip->c.s64, 8));
                        SWAG_CHECK(checkStackInitialized(context, addr, 8));
                        SWAG_CHECK(getStackValue(context, ra, addr, 8));
                        ra->reg.u64 = *reinterpret_cast<uint64_t*>(addr);
                        ra->update(ip);
                        break;
                    }

                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    break;

                case ByteCodeOp::AffectOpPlusEqS8:
                    BINOP_EQ_OVF(int8_t, +=, s8, addWillOverflow, "+=", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::AffectOpPlusEqS16:
                    BINOP_EQ_OVF(int16_t, +=, s16, addWillOverflow, "+=", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::AffectOpPlusEqS32:
                    BINOP_EQ_OVF(int32_t, +=, s32, addWillOverflow, "+=", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::AffectOpPlusEqS64:
                    BINOP_EQ_OVF(int64_t, +=, s64, addWillOverflow, "+=", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::AffectOpPlusEqU8:
                    BINOP_EQ_OVF(uint8_t, +=, u8, addWillOverflow, "+=", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::AffectOpPlusEqU16:
                    BINOP_EQ_OVF(uint16_t, +=, u16, addWillOverflow, "+=", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::AffectOpPlusEqU32:
                    BINOP_EQ_OVF(uint32_t, +=, u32, addWillOverflow, "+=", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::AffectOpPlusEqU64:
                    BINOP_EQ_OVF(uint64_t, +=, u64, addWillOverflow, "+=", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::AffectOpPlusEqF32:
                    BINOP_EQ(float, +=, f32);
                    break;
                case ByteCodeOp::AffectOpPlusEqF64:
                    BINOP_EQ(double, +=, f64);
                    break;

                case ByteCodeOp::AffectOpMinusEqS8:
                    BINOP_EQ_OVF(int8_t, -=, s8, subWillOverflow, "-=", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::AffectOpMinusEqS16:
                    BINOP_EQ_OVF(int16_t, -=, s16, subWillOverflow, "-=", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::AffectOpMinusEqS32:
                    BINOP_EQ_OVF(int32_t, -=, s32, subWillOverflow, "-=", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::AffectOpMinusEqS64:
                    BINOP_EQ_OVF(int64_t, -=, s64, subWillOverflow, "-=", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::AffectOpMinusEqU8:
                    BINOP_EQ_OVF(uint8_t, -=, u8, subWillOverflow, "-=", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::AffectOpMinusEqU16:
                    BINOP_EQ_OVF(uint16_t, -=, u16, subWillOverflow, "-=", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::AffectOpMinusEqU32:
                    BINOP_EQ_OVF(uint32_t, -=, u32, subWillOverflow, "-=", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::AffectOpMinusEqU64:
                    BINOP_EQ_OVF(uint64_t, -=, u64, subWillOverflow, "-=", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::AffectOpMinusEqF32:
                    BINOP_EQ(float, -=, f32);
                    break;
                case ByteCodeOp::AffectOpMinusEqF64:
                    BINOP_EQ(double, -=, f64);
                    break;

                case ByteCodeOp::AffectOpMulEqS8:
                    BINOP_EQ_OVF(int8_t, *=, s8, mulWillOverflow, "*=", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::AffectOpMulEqS16:
                    BINOP_EQ_OVF(int16_t, *=, s16, mulWillOverflow, "*=", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::AffectOpMulEqS32:
                    BINOP_EQ_OVF(int32_t, *=, s32, mulWillOverflow, "*=", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::AffectOpMulEqS64:
                    BINOP_EQ_OVF(int64_t, *=, s64, mulWillOverflow, "*=", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::AffectOpMulEqU8:
                    BINOP_EQ_OVF(uint8_t, *=, u8, mulWillOverflow, "*=", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::AffectOpMulEqU16:
                    BINOP_EQ_OVF(uint16_t, *=, u16, mulWillOverflow, "*=", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::AffectOpMulEqU32:
                    BINOP_EQ_OVF(uint32_t, *=, u32, mulWillOverflow, "*=", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::AffectOpMulEqU64:
                    BINOP_EQ_OVF(uint64_t, *=, u64, mulWillOverflow, "*=", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::AffectOpMulEqF32:
                    BINOP_EQ(float, *=, f32);
                    break;
                case ByteCodeOp::AffectOpMulEqF64:
                    BINOP_EQ(double, *=, f64);
                    break;

                case ByteCodeOp::AffectOpDivEqS8:
                    BINOP_EQ_DIV(int8_t, /=, s8);
                    break;
                case ByteCodeOp::AffectOpDivEqS16:
                    BINOP_EQ_DIV(int16_t, /=, s16);
                    break;
                case ByteCodeOp::AffectOpDivEqS32:
                    BINOP_EQ_DIV(int32_t, /=, s32);
                    break;
                case ByteCodeOp::AffectOpDivEqS64:
                    BINOP_EQ_DIV(int64_t, /=, s64);
                    break;
                case ByteCodeOp::AffectOpDivEqU8:
                    BINOP_EQ_DIV(uint8_t, /=, u8);
                    break;
                case ByteCodeOp::AffectOpDivEqU16:
                    BINOP_EQ_DIV(uint16_t, /=, u16);
                    break;
                case ByteCodeOp::AffectOpDivEqU32:
                    BINOP_EQ_DIV(uint32_t, /=, u32);
                    break;
                case ByteCodeOp::AffectOpDivEqU64:
                    BINOP_EQ_DIV(uint64_t, /=, u64);
                    break;
                case ByteCodeOp::AffectOpDivEqF32:
                    BINOP_EQ_DIV(float, /=, f32);
                    break;
                case ByteCodeOp::AffectOpDivEqF64:
                    BINOP_EQ_DIV(double, /=, f64);
                    break;

                case ByteCodeOp::AffectOpModuloEqS8:
                    BINOP_EQ_DIV(int8_t, %=, s8);
                    break;
                case ByteCodeOp::AffectOpModuloEqS16:
                    BINOP_EQ_DIV(int16_t, %=, s16);
                    break;
                case ByteCodeOp::AffectOpModuloEqS32:
                    BINOP_EQ_DIV(int32_t, %=, s32);
                    break;
                case ByteCodeOp::AffectOpModuloEqS64:
                    BINOP_EQ_DIV(int64_t, %=, s64);
                    break;
                case ByteCodeOp::AffectOpModuloEqU8:
                    BINOP_EQ_DIV(uint8_t, %=, u8);
                    break;
                case ByteCodeOp::AffectOpModuloEqU16:
                    BINOP_EQ_DIV(uint16_t, %=, u16);
                    break;
                case ByteCodeOp::AffectOpModuloEqU32:
                    BINOP_EQ_DIV(uint32_t, %=, u32);
                    break;
                case ByteCodeOp::AffectOpModuloEqU64:
                    BINOP_EQ_DIV(uint64_t, %=, u64);
                    break;

                case ByteCodeOp::AffectOpAndEqU8:
                    BINOP_EQ(uint8_t, &=, u8);
                    break;
                case ByteCodeOp::AffectOpAndEqU16:
                    BINOP_EQ(uint16_t, &=, u16);
                    break;
                case ByteCodeOp::AffectOpAndEqU32:
                    BINOP_EQ(uint32_t, &=, u32);
                    break;
                case ByteCodeOp::AffectOpAndEqU64:
                    BINOP_EQ(uint64_t, &=, u64);
                    break;

                case ByteCodeOp::AffectOpOrEqU8:
                    BINOP_EQ(uint8_t, |=, u8);
                    break;
                case ByteCodeOp::AffectOpOrEqU16:
                    BINOP_EQ(uint16_t, |=, u16);
                    break;
                case ByteCodeOp::AffectOpOrEqU32:
                    BINOP_EQ(uint32_t, |=, u32);
                    break;
                case ByteCodeOp::AffectOpOrEqU64:
                    BINOP_EQ(uint64_t, |=, u64);
                    break;

                case ByteCodeOp::AffectOpXorEqU8:
                    BINOP_EQ(uint8_t, ^=, u8);
                    break;
                case ByteCodeOp::AffectOpXorEqU16:
                    BINOP_EQ(uint16_t, ^=, u16);
                    break;
                case ByteCodeOp::AffectOpXorEqU32:
                    BINOP_EQ(uint32_t, ^=, u32);
                    break;
                case ByteCodeOp::AffectOpXorEqU64:
                    BINOP_EQ(uint64_t, ^=, u64);
                    break;

                case ByteCodeOp::AffectOpShiftLeftEqS8:
                    BINOP_EQ_SHIFT(uint8_t, u8, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqS16:
                    BINOP_EQ_SHIFT(uint16_t, u16, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqS32:
                    BINOP_EQ_SHIFT(uint32_t, u32, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqS64:
                    BINOP_EQ_SHIFT(uint64_t, u64, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqU8:
                    BINOP_EQ_SHIFT(uint8_t, u8, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqU16:
                    BINOP_EQ_SHIFT(uint16_t, u16, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqU32:
                    BINOP_EQ_SHIFT(uint32_t, u32, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftLeftEqU64:
                    BINOP_EQ_SHIFT(uint64_t, u64, ByteCodeRun::executeLeftShift, false);
                    break;

                case ByteCodeOp::AffectOpShiftRightEqS8:
                    BINOP_EQ_SHIFT(int8_t, s8, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqS16:
                    BINOP_EQ_SHIFT(int16_t, s16, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqS32:
                    BINOP_EQ_SHIFT(int32_t, s32, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqS64:
                    BINOP_EQ_SHIFT(int64_t, s64, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqU8:
                    BINOP_EQ_SHIFT(uint8_t, u8, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqU16:
                    BINOP_EQ_SHIFT(uint16_t, u16, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqU32:
                    BINOP_EQ_SHIFT(uint32_t, u32, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::AffectOpShiftRightEqU64:
                    BINOP_EQ_SHIFT(uint64_t, u64, ByteCodeRun::executeRightShift, false);
                    break;

                case ByteCodeOp::NegBool:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind  = rb->kind;
                    ra->reg.b = rb->reg.b ^ 1;
                    ra->update(ip);
                    break;
                case ByteCodeOp::NegS32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.s32 = -rb->reg.s32;
                    ra->update(ip);
                    break;
                case ByteCodeOp::NegS64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.s64 = -rb->reg.s64;
                    ra->update(ip);
                    break;
                case ByteCodeOp::NegF32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.f32 = -rb->reg.f32;
                    ra->update(ip);
                    break;
                case ByteCodeOp::NegF64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.f64 = -rb->reg.f64;
                    ra->update(ip);
                    break;

                case ByteCodeOp::CastBool8:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind  = vb.kind;
                    ra->reg.b = vb.reg.u8 ? true : false;
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastBool16:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind  = vb.kind;
                    ra->reg.b = vb.reg.u16 ? true : false;
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastBool32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind  = vb.kind;
                    ra->reg.b = vb.reg.u32 ? true : false;
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastBool64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind  = vb.kind;
                    ra->reg.b = vb.reg.u64 ? true : false;
                    ra->update(ip);
                    break;

                case ByteCodeOp::CastS8S16:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.s16 = static_cast<int16_t>(vb.reg.s8);
                    ra->update(ip);
                    break;

                case ByteCodeOp::CastS8S32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.s32 = static_cast<int32_t>(vb.reg.s8);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS16S32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.s32 = static_cast<int32_t>(vb.reg.s16);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastF32S32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.s32 = static_cast<int32_t>(vb.reg.f32);
                    ra->update(ip);
                    break;

                case ByteCodeOp::CastS8S64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.s64 = static_cast<int64_t>(vb.reg.s8);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS16S64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.s64 = static_cast<int64_t>(vb.reg.s16);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS32S64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.s64 = static_cast<int64_t>(vb.reg.s32);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastF64S64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.s64 = static_cast<int64_t>(vb.reg.f64);
                    ra->update(ip);
                    break;

                case ByteCodeOp::CastS8F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.s8);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS16F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.s16);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS32F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.s32);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS64F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.s64);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastU8F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.u8);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastU16F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.u16);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastU32F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.u32);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastU64F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.u64);
                    ra->update(ip);
                    break;

                case ByteCodeOp::CastS8F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.s8);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS16F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.s16);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS32F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.s32);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastS64F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.s64);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastU8F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.u8);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastU16F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.u16);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastU32F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.u32);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastU64F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.u64);
                    ra->update(ip);
                    break;
                case ByteCodeOp::CastF32F64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f64 = static_cast<double>(vb.reg.f32);
                    ra->update(ip);
                    break;

                case ByteCodeOp::CastF64F32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind    = vb.kind;
                    ra->reg.f32 = static_cast<float>(vb.reg.f64);
                    ra->update(ip);
                    break;

                case ByteCodeOp::CompareOpEqual8:
                    CMP_OP(==, u8);
                    break;
                case ByteCodeOp::CompareOpEqual16:
                    CMP_OP(==, u16);
                    break;
                case ByteCodeOp::CompareOpEqual32:
                    CMP_OP(==, u32);
                    break;
                case ByteCodeOp::CompareOpEqual64:
                    CMP_OP(==, u64);
                    break;
                case ByteCodeOp::CompareOpEqualF32:
                    CMP_OP(==, f32);
                    break;
                case ByteCodeOp::CompareOpEqualF64:
                    CMP_OP(==, f64);
                    break;

                case ByteCodeOp::CompareOpNotEqual8:
                    CMP_OP(!=, u8);
                    break;
                case ByteCodeOp::CompareOpNotEqual16:
                    CMP_OP(!=, u16);
                    break;
                case ByteCodeOp::CompareOpNotEqual32:
                    CMP_OP(!=, u32);
                    break;
                case ByteCodeOp::CompareOpNotEqual64:
                    CMP_OP(!=, u64);
                    break;
                case ByteCodeOp::CompareOpNotEqualF32:
                    CMP_OP(!=, f32);
                    break;
                case ByteCodeOp::CompareOpNotEqualF64:
                    CMP_OP(!=, f64);
                    break;

                case ByteCodeOp::CompareOpGreaterEqS8:
                    CMP_OP(>=, s8);
                    break;
                case ByteCodeOp::CompareOpGreaterEqS16:
                    CMP_OP(>=, s16);
                    break;
                case ByteCodeOp::CompareOpGreaterEqS32:
                    CMP_OP(>=, s32);
                    break;
                case ByteCodeOp::CompareOpGreaterEqS64:
                    CMP_OP(>=, s64);
                    break;
                case ByteCodeOp::CompareOpGreaterEqU8:
                    CMP_OP(>=, u8);
                    break;
                case ByteCodeOp::CompareOpGreaterEqU16:
                    CMP_OP(>=, u16);
                    break;
                case ByteCodeOp::CompareOpGreaterEqU32:
                    CMP_OP(>=, u32);
                    break;
                case ByteCodeOp::CompareOpGreaterEqU64:
                    CMP_OP(>=, u64);
                    break;
                case ByteCodeOp::CompareOpGreaterEqF32:
                    CMP_OP(>=, f32);
                    break;
                case ByteCodeOp::CompareOpGreaterEqF64:
                    CMP_OP(>=, f64);
                    break;

                case ByteCodeOp::CompareOpGreaterS8:
                    CMP_OP(>, s8);
                    break;
                case ByteCodeOp::CompareOpGreaterS16:
                    CMP_OP(>, s16);
                    break;
                case ByteCodeOp::CompareOpGreaterS32:
                    CMP_OP(>, s32);
                    break;
                case ByteCodeOp::CompareOpGreaterS64:
                    CMP_OP(>, s64);
                    break;
                case ByteCodeOp::CompareOpGreaterU8:
                    CMP_OP(>, u8);
                    break;
                case ByteCodeOp::CompareOpGreaterU16:
                    CMP_OP(>, u16);
                    break;
                case ByteCodeOp::CompareOpGreaterU32:
                    CMP_OP(>, u32);
                    break;
                case ByteCodeOp::CompareOpGreaterU64:
                    CMP_OP(>, u64);
                    break;
                case ByteCodeOp::CompareOpGreaterF32:
                    CMP_OP(>, f32);
                    break;
                case ByteCodeOp::CompareOpGreaterF64:
                    CMP_OP(>, f64);
                    break;

                case ByteCodeOp::CompareOpLowerEqS8:
                    CMP_OP(<=, s8);
                    break;
                case ByteCodeOp::CompareOpLowerEqS16:
                    CMP_OP(<=, s16);
                    break;
                case ByteCodeOp::CompareOpLowerEqS32:
                    CMP_OP(<=, s32);
                    break;
                case ByteCodeOp::CompareOpLowerEqS64:
                    CMP_OP(<=, s64);
                    break;
                case ByteCodeOp::CompareOpLowerEqU8:
                    CMP_OP(<=, u8);
                    break;
                case ByteCodeOp::CompareOpLowerEqU16:
                    CMP_OP(<=, u16);
                    break;
                case ByteCodeOp::CompareOpLowerEqU32:
                    CMP_OP(<=, u32);
                    break;
                case ByteCodeOp::CompareOpLowerEqU64:
                    CMP_OP(<=, u64);
                    break;
                case ByteCodeOp::CompareOpLowerEqF32:
                    CMP_OP(<=, f32);
                    break;
                case ByteCodeOp::CompareOpLowerEqF64:
                    CMP_OP(<=, f64);
                    break;

                case ByteCodeOp::CompareOpLowerS8:
                    CMP_OP(<, s8);
                    break;
                case ByteCodeOp::CompareOpLowerS16:
                    CMP_OP(<, s16);
                    break;
                case ByteCodeOp::CompareOpLowerS32:
                    CMP_OP(<, s32);
                    break;
                case ByteCodeOp::CompareOpLowerS64:
                    CMP_OP(<, s64);
                    break;
                case ByteCodeOp::CompareOpLowerU8:
                    CMP_OP(<, u8);
                    break;
                case ByteCodeOp::CompareOpLowerU16:
                    CMP_OP(<, u16);
                    break;
                case ByteCodeOp::CompareOpLowerU32:
                    CMP_OP(<, u32);
                    break;
                case ByteCodeOp::CompareOpLowerU64:
                    CMP_OP(<, u64);
                    break;
                case ByteCodeOp::CompareOpLowerF32:
                    CMP_OP(<, f32);
                    break;
                case ByteCodeOp::CompareOpLowerF64:
                    CMP_OP(<, f64);
                    break;

                case ByteCodeOp::ClearMaskU32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.u32 &= ip->b.u32;
                    ra->update(ip);
                    break;
                case ByteCodeOp::ClearMaskU64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->reg.u64 &= ip->b.u64;
                    ra->update(ip);
                    break;

                case ByteCodeOp::BinOpBitmaskAnd8:
                    BINOP(&, u8);
                    break;
                case ByteCodeOp::BinOpBitmaskAnd16:
                    BINOP(&, u16);
                    break;
                case ByteCodeOp::BinOpBitmaskAnd32:
                    BINOP(&, u32);
                    break;
                case ByteCodeOp::BinOpBitmaskAnd64:
                    BINOP(&, u64);
                    break;

                case ByteCodeOp::BinOpBitmaskOr8:
                    BINOP(|, u8);
                    break;
                case ByteCodeOp::BinOpBitmaskOr16:
                    BINOP(|, u16);
                    break;
                case ByteCodeOp::BinOpBitmaskOr32:
                    BINOP(|, u32);
                    break;
                case ByteCodeOp::BinOpBitmaskOr64:
                    BINOP(|, u64);
                    break;

                case ByteCodeOp::BinOpXorU8:
                    BINOP(^, u8);
                    break;
                case ByteCodeOp::BinOpXorU16:
                    BINOP(^, u16);
                    break;
                case ByteCodeOp::BinOpXorU32:
                    BINOP(^, u32);
                    break;
                case ByteCodeOp::BinOpXorU64:
                    BINOP(^, u64);
                    break;

                case ByteCodeOp::BinOpPlusS8:
                    BINOP_OVF(+, s8, addWillOverflow, "+", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::BinOpPlusS16:
                    BINOP_OVF(+, s16, addWillOverflow, "+", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::BinOpPlusS32:
                    BINOP_OVF(+, s32, addWillOverflow, "+", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::BinOpPlusS64:
                    BINOP_OVF(+, s64, addWillOverflow, "+", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::BinOpPlusU8:
                    BINOP_OVF(+, u8, addWillOverflow, "+", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::BinOpPlusU16:
                    BINOP_OVF(+, u16, addWillOverflow, "+", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::BinOpPlusU32:
                    BINOP_OVF(+, u32, addWillOverflow, "+", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::BinOpPlusU64:
                    BINOP_OVF(+, u64, addWillOverflow, "+", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::BinOpPlusF32:
                    BINOP(+, f32);
                    break;
                case ByteCodeOp::BinOpPlusF64:
                    BINOP(+, f64);
                    break;

                case ByteCodeOp::BinOpMinusS8:
                    BINOP_OVF(-, s8, subWillOverflow, "-", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::BinOpMinusS16:
                    BINOP_OVF(-, s16, subWillOverflow, "-", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::BinOpMinusS32:
                    BINOP_OVF(-, s32, subWillOverflow, "-", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::BinOpMinusS64:
                    BINOP_OVF(-, s64, subWillOverflow, "-", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::BinOpMinusU8:
                    BINOP_OVF(-, u8, subWillOverflow, "-", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::BinOpMinusU16:
                    BINOP_OVF(-, u16, subWillOverflow, "-", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::BinOpMinusU32:
                    BINOP_OVF(-, u32, subWillOverflow, "-", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::BinOpMinusU64:
                    BINOP_OVF(-, u64, subWillOverflow, "-", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::BinOpMinusF32:
                    BINOP(-, f32);
                    break;
                case ByteCodeOp::BinOpMinusF64:
                    BINOP(-, f64);
                    break;

                case ByteCodeOp::BinOpMulS8:
                    BINOP_OVF(*, s8, mulWillOverflow, "*", g_TypeMgr->typeInfoS8);
                    break;
                case ByteCodeOp::BinOpMulS16:
                    BINOP_OVF(*, s16, mulWillOverflow, "*", g_TypeMgr->typeInfoS16);
                    break;
                case ByteCodeOp::BinOpMulS32:
                    BINOP_OVF(*, s32, mulWillOverflow, "*", g_TypeMgr->typeInfoS32);
                    break;
                case ByteCodeOp::BinOpMulS64:
                    BINOP_OVF(*, s64, mulWillOverflow, "*", g_TypeMgr->typeInfoS64);
                    break;
                case ByteCodeOp::BinOpMulU8:
                    BINOP_OVF(*, u8, mulWillOverflow, "*", g_TypeMgr->typeInfoU8);
                    break;
                case ByteCodeOp::BinOpMulU16:
                    BINOP_OVF(*, u16, mulWillOverflow, "*", g_TypeMgr->typeInfoU16);
                    break;
                case ByteCodeOp::BinOpMulU32:
                    BINOP_OVF(*, u32, mulWillOverflow, "*", g_TypeMgr->typeInfoU32);
                    break;
                case ByteCodeOp::BinOpMulU64:
                    BINOP_OVF(*, u64, mulWillOverflow, "*", g_TypeMgr->typeInfoU64);
                    break;
                case ByteCodeOp::BinOpMulF32:
                    BINOP(*, f32);
                    break;
                case ByteCodeOp::BinOpMulF64:
                    BINOP(*, f64);
                    break;

                case ByteCodeOp::BinOpDivS8:
                    BINOP_DIV(/, s8);
                    break;
                case ByteCodeOp::BinOpDivS16:
                    BINOP_DIV(/, s16);
                    break;
                case ByteCodeOp::BinOpDivS32:
                    BINOP_DIV(/, s32);
                    break;
                case ByteCodeOp::BinOpDivS64:
                    BINOP_DIV(/, s64);
                    break;
                case ByteCodeOp::BinOpDivU8:
                    BINOP_DIV(/, u8);
                    break;
                case ByteCodeOp::BinOpDivU16:
                    BINOP_DIV(/, u16);
                    break;
                case ByteCodeOp::BinOpDivU32:
                    BINOP_DIV(/, u32);
                    break;
                case ByteCodeOp::BinOpDivU64:
                    BINOP_DIV(/, u64);
                    break;
                case ByteCodeOp::BinOpDivF32:
                    BINOP_DIV(/, f32);
                    break;
                case ByteCodeOp::BinOpDivF64:
                    BINOP_DIV(/, f64);
                    break;

                case ByteCodeOp::BinOpModuloS8:
                    BINOP_DIV(%, s8);
                    break;
                case ByteCodeOp::BinOpModuloS16:
                    BINOP_DIV(%, s16);
                    break;
                case ByteCodeOp::BinOpModuloS32:
                    BINOP_DIV(%, s32);
                    break;
                case ByteCodeOp::BinOpModuloS64:
                    BINOP_DIV(%, s64);
                    break;
                case ByteCodeOp::BinOpModuloU8:
                    BINOP_DIV(%, u8);
                    break;
                case ByteCodeOp::BinOpModuloU16:
                    BINOP_DIV(%, u16);
                    break;
                case ByteCodeOp::BinOpModuloU32:
                    BINOP_DIV(%, u32);
                    break;
                case ByteCodeOp::BinOpModuloU64:
                    BINOP_DIV(%, u64);
                    break;

                case ByteCodeOp::BinOpShiftLeftS8:
                    BINOP_SHIFT(uint8_t, u8, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::BinOpShiftLeftS16:
                    BINOP_SHIFT(uint16_t, u16, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::BinOpShiftLeftS32:
                    BINOP_SHIFT(uint32_t, u32, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::BinOpShiftLeftS64:
                    BINOP_SHIFT(uint64_t, u64, ByteCodeRun::executeLeftShift, true);
                    break;
                case ByteCodeOp::BinOpShiftLeftU8:
                    BINOP_SHIFT(uint8_t, u8, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::BinOpShiftLeftU16:
                    BINOP_SHIFT(uint16_t, u16, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::BinOpShiftLeftU32:
                    BINOP_SHIFT(uint32_t, u32, ByteCodeRun::executeLeftShift, false);
                    break;
                case ByteCodeOp::BinOpShiftLeftU64:
                    BINOP_SHIFT(uint64_t, u64, ByteCodeRun::executeLeftShift, false);
                    break;

                case ByteCodeOp::BinOpShiftRightS8:
                    BINOP_SHIFT(int8_t, s8, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::BinOpShiftRightS16:
                    BINOP_SHIFT(int16_t, s16, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::BinOpShiftRightS32:
                    BINOP_SHIFT(int32_t, s32, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::BinOpShiftRightS64:
                    BINOP_SHIFT(int64_t, s64, ByteCodeRun::executeRightShift, true);
                    break;
                case ByteCodeOp::BinOpShiftRightU8:
                    BINOP_SHIFT(uint8_t, u8, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::BinOpShiftRightU16:
                    BINOP_SHIFT(uint16_t, u16, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::BinOpShiftRightU32:
                    BINOP_SHIFT(uint32_t, u32, ByteCodeRun::executeRightShift, false);
                    break;
                case ByteCodeOp::BinOpShiftRightU64:
                    BINOP_SHIFT(uint64_t, u64, ByteCodeRun::executeRightShift, false);
                    break;

                case ByteCodeOp::InvertU8:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind   = rb->kind;
                    ra->reg.u8 = ~rb->reg.u8;
                    ra->update(ip);
                    break;
                case ByteCodeOp::InvertU16:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.u16 = ~rb->reg.u16;
                    ra->update(ip);
                    break;
                case ByteCodeOp::InvertU32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.u32 = ~rb->reg.u32;
                    ra->update(ip);
                    break;
                case ByteCodeOp::InvertU64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    ra->kind    = rb->kind;
                    ra->reg.u64 = ~rb->reg.u64;
                    ra->update(ip);
                    break;

                case ByteCodeOp::IntrinsicStrCmp:
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    SWAG_CHECK(checkNotNullArguments(context, {ip->c.u32, ip->b.u32}, "@strcmp"));
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    break;

                case ByteCodeOp::IntrinsicStrLen:
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(checkNotNullArguments(context, {ip->b.u32}, "@strlen"));
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    break;

                case ByteCodeOp::IntrinsicMemCmp:
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(getRegister(context, rc, ip->c.u32));
                    SWAG_CHECK(checkNotNullArguments(context, {ip->c.u32, ip->b.u32}, "@memcmp"));
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    break;

                case ByteCodeOp::MemCpy8:
                    MEMCPY(uint8_t, 1);
                    break;
                case ByteCodeOp::MemCpy16:
                    MEMCPY(uint16_t, 2);
                    break;
                case ByteCodeOp::MemCpy32:
                    MEMCPY(uint32_t, 4);
                    break;
                case ByteCodeOp::MemCpy64:
                    MEMCPY(uint64_t, 8);
                    break;
                case ByteCodeOp::IntrinsicMemCpy:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(getImmediateC(context, vc));
                    SWAG_CHECK(checkNotNullArguments(context, {ip->b.u32, ip->a.u32}, "@memcpy"));
                    if (ra->kind == SanityValueKind::StackAddr && rb->kind == SanityValueKind::StackAddr && vc.isConstant())
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, vc.reg.u32));
                        SWAG_CHECK(getStackAddress(addr2, context, rb, rb->reg.u32, vc.reg.u32));
                        SWAG_CHECK(checkStackInitialized(context, addr2, vc.reg.u32, rb));
                        SWAG_CHECK(getStackValue(context, &va, addr2, vc.reg.u32));
                        setStackValue(context, addr, vc.reg.u32, va.kind);
                        std::copy_n(addr2, vc.reg.u32, addr);
                    }
                    invalidateCurStateStack(context);
                    break;

                case ByteCodeOp::IntrinsicMemMove:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getRegister(context, rb, ip->b.u32));
                    SWAG_CHECK(getImmediateC(context, vc));
                    SWAG_CHECK(checkNotNullArguments(context, {ip->b.u32, ip->a.u32}, "@memmove"));
                    if (ra->kind == SanityValueKind::StackAddr && rb->kind == SanityValueKind::StackAddr && vc.isConstant())
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, vc.reg.u32));
                        SWAG_CHECK(getStackAddress(addr2, context, rb, rb->reg.u32, vc.reg.u32));
                        SWAG_CHECK(checkStackInitialized(context, addr2, vc.reg.u32, rb));
                        SWAG_CHECK(getStackValue(context, &va, addr2, vc.reg.u32));
                        setStackValue(context, addr, vc.reg.u32, va.kind);
                        memmove(addr, addr2, vc.reg.u32);
                        break;
                    }
                    invalidateCurStateStack(context);
                    break;
                case ByteCodeOp::IntrinsicMemSet:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getImmediateC(context, vc));
                    SWAG_CHECK(checkNotNullArguments(context, {ip->a.u32}, "@memset"));
                    if (ra->kind == SanityValueKind::StackAddr && vb.isConstant() && vc.isConstant())
                    {
                        SWAG_CHECK(getStackAddress(addr, context, ra, ra->reg.u32, vc.reg.u32));
                        setStackValue(context, addr, vc.reg.u32, SanityValueKind::Constant);
                        memset(addr, vb.reg.u8, vc.reg.u32);
                        break;
                    }
                    invalidateCurStateStack(context);
                    break;

                case ByteCodeOp::LambdaCall:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(checkNotNull(context, ra));
                    [[fallthrough]];

                case ByteCodeOp::LocalCall:
                case ByteCodeOp::ForeignCall:
                    SWAG_CHECK(checkNotNullArguments(context, pushParams, ""));
                    invalidateCurStateStack(context);
                    pushParams.clear();
                    break;

                case ByteCodeOp::IntrinsicMulAddF32:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getImmediateC(context, vc));
                    SWAG_CHECK(getImmediateD(context, vd));
                    ra->kind = vb.isConstant() && vc.isConstant() && vd.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                    if (ra->isConstant())
                        ra->reg.f32 = vb.reg.f32 * vc.reg.f32 + vd.reg.f32;
                    ra->update(ip);
                    break;
                case ByteCodeOp::IntrinsicMulAddF64:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getImmediateC(context, vc));
                    SWAG_CHECK(getImmediateD(context, vd));
                    ra->kind = vb.isConstant() && vc.isConstant() && vd.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                    if (ra->isConstant())
                        ra->reg.f64 = vb.reg.f64 * vc.reg.f64 + vd.reg.f64;
                    ra->update(ip);
                    break;

                case ByteCodeOp::IntrinsicS8x1:
                case ByteCodeOp::IntrinsicS16x1:
                case ByteCodeOp::IntrinsicS32x1:
                case ByteCodeOp::IntrinsicS64x1:
                case ByteCodeOp::IntrinsicF32x1:
                case ByteCodeOp::IntrinsicF64x1:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    if (vb.isConstant())
                    {
                        ra->kind = SanityValueKind::Constant;
                        SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(context, ip, ra->reg, vb.reg, {}, {}));
                    }
                    break;

                case ByteCodeOp::IntrinsicS8x2:
                case ByteCodeOp::IntrinsicS16x2:
                case ByteCodeOp::IntrinsicS32x2:
                case ByteCodeOp::IntrinsicS64x2:
                case ByteCodeOp::IntrinsicU8x2:
                case ByteCodeOp::IntrinsicU16x2:
                case ByteCodeOp::IntrinsicU32x2:
                case ByteCodeOp::IntrinsicU64x2:
                case ByteCodeOp::IntrinsicF32x2:
                case ByteCodeOp::IntrinsicF64x2:
                    SWAG_CHECK(getRegister(context, ra, ip->a.u32));
                    SWAG_CHECK(getImmediateB(context, vb));
                    SWAG_CHECK(getImmediateC(context, vc));
                    ra->kind = SanityValueKind::Unknown;
                    ra->update(ip);
                    if (vb.isConstant() && vc.isConstant())
                    {
                        ra->kind = SanityValueKind::Constant;
                        SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(context, ip, ra->reg, vb.reg, vc.reg, {}));
                    }
                    break;

                case ByteCodeOp::Nop:
                    break;

                default:
                    Report::internalError(context->bc->sourceFile->module, form("unknown instruction [[%s]] during sanity check", g_ByteCodeOpDesc[static_cast<int>(ip->op)].name));
                    return false;
            }

            ip->dynFlags.add(BCID_SAN_PASS);
            ip++;
        }

        return true;
    }
}

bool ByteCodeSanity::process(ByteCode* bc)
{
    if (!bc->node || bc->isCompilerGenerated)
        return true;

    SanityContext context;
    context.bc = bc;

    const auto state    = new SanityState;
    const auto funcDecl = castAst<AstFuncDecl>(context.bc->node, AstNodeKind::FuncDecl);
    state->stack.resize(funcDecl->stackSize);
    state->stackValue.resize(funcDecl->stackSize);
    state->regs.resize(context.bc->maxReservedRegisterRC);
    state->ip = context.bc->out;
    context.states.push_back(state);

    for (uint32_t i = 0; i < context.states.size(); i++)
    {
        context.state = i;
        SWAG_CHECK(sanityLoop(&context));
        if (i == context.states.size() - 1)
            break;
        for (uint32_t j = 0; j < context.bc->numInstructions; j++)
            context.bc->out[j].dynFlags.remove(BCID_SAN_PASS);
    }

    for (const auto s : context.states)
        delete s;

    return true;
}
