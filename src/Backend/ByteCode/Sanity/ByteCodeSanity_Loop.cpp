#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanity.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanity_Macros.h"
#include "Report/Diagnostic.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool ByteCodeSanity::backTrace(ByteCodeSanityState* state, uint32_t reg)
{
    SanityValue*                             ra = nullptr;
    SanityValue*                             rb = nullptr;
    SanityValue*                             rc = nullptr;
    SanityValue                              va, vb;
    uint8_t*                                 addr = nullptr;
    Vector<std::pair<uint32_t, SanityValue>> map;

    bool done = false;
    for (uint32_t i = state->ips.size() - 2; i != UINT_MAX && !done; i--)
    {
        const auto ip = state->ips[i];
        if (!ip->hasWriteRefToReg(reg))
            break;

        // Store the last value of a given register, to restore it at the end, once the backtrace is done
        bool here = false;
        for (const auto& key : map | std::views::keys)
        {
            if (key == reg)
            {
                here = true;
                break;
            }
        }

        if (!here)
        {
            SWAG_CHECK(state->getRegister(ra, reg));
            map.push_back({reg, *ra});
        }

        switch (ip->op)
        {
            case ByteCodeOp::GetFromStack8:
                SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                SWAG_CHECK(state->getStackAddress(addr, ip->b.u32, 1, nullptr));
                *addr = ra->reg.u8;
                state->setStackKind(addr, 1, ra->kind, ra->flags);
                break;

            case ByteCodeOp::GetFromStack16:
                SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                SWAG_CHECK(state->getStackAddress(addr, ip->b.u32, 2, nullptr));
                *reinterpret_cast<uint16_t*>(addr) = ra->reg.u16;
                state->setStackKind(addr, 2, ra->kind, ra->flags);
                break;

            case ByteCodeOp::GetFromStack32:
                SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                SWAG_CHECK(state->getStackAddress(addr, ip->b.u32, 4, nullptr));
                *reinterpret_cast<uint32_t*>(addr) = ra->reg.u32;
                state->setStackKind(addr, 4, ra->kind, ra->flags);
                break;

            case ByteCodeOp::GetFromStack64:
                SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                SWAG_CHECK(state->getStackAddress(addr, ip->b.u32, 8, nullptr));
                *reinterpret_cast<uint64_t*>(addr) = ra->reg.u64;
                state->setStackKind(addr, 8, ra->kind, ra->flags);
                break;

            case ByteCodeOp::CastBool8:
            case ByteCodeOp::CastBool16:
            case ByteCodeOp::CastBool32:
            case ByteCodeOp::CastBool64:
                SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                SWAG_CHECK(state->getRegister(rb, ip->b.u32));
                if (ra->isZero())
                    rb->setConstant(0LL);
                else
                    rb->setUnknown(SANITY_VALUE_FLAG_NOT_ZERO);
                reg = ip->b.u32;
                break;

            case ByteCodeOp::CopyRBtoRA64:
                SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                SWAG_CHECK(state->getRegister(rb, ip->b.u32));
                *rb = *ra;
                reg = ip->b.u32;
                break;

            case ByteCodeOp::NegBool:
                SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                SWAG_CHECK(state->getRegister(rb, ip->b.u32));
                if (ra->isZero())
                    rb->setConstant(1LL);
                else if (ra->isNotZero())
                    rb->setConstant(0LL);
                else
                    rb->setUnknown();
                reg = ip->b.u32;
                break;

            case ByteCodeOp::DeRef8:
            case ByteCodeOp::DeRef16:
            case ByteCodeOp::DeRef32:
            case ByteCodeOp::DeRef64:
                SWAG_CHECK(state->getRegister(rb, ip->b.u32));
                rb->setUnknown(SANITY_VALUE_FLAG_NOT_ZERO);
                reg = ip->b.u32;
                break;

            case ByteCodeOp::IncPointer64:
                SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                ra->setUnknown(SANITY_VALUE_FLAG_NOT_ZERO);
                reg = ip->a.u32;
                break;

            case ByteCodeOp::CompareOpEqual64:
                SWAG_CHECK(state->getImmediateA(va, ip));
                SWAG_CHECK(state->getImmediateB(vb, ip));
                SWAG_CHECK(state->getRegister(rc, ip->c.u32));
                if (rc->isZero() && va.isNotZero() && vb.isTrueUnknown())
                {
                    SWAG_CHECK(state->getRegister(rb, ip->b.u32));
                    rb->setUnknown(SANITY_VALUE_FLAG_NOT_ZERO);
                    reg = ip->b.u32;
                }
                else if (rc->isZero() && va.isTrueUnknown() && vb.isNotZero())
                {
                    SWAG_CHECK(state->getRegister(ra, ip->a.u32));
                    ra->setUnknown(SANITY_VALUE_FLAG_NOT_ZERO);
                    reg = ip->a.u32;
                }
                else
                    done = true;
                break;

            default:
                done = true;
                break;
        }
    }

    for (const auto& it : map)
    {
        SWAG_CHECK(state->getRegister(ra, it.first));
        *ra = it.second;
    }

    return true;
}

bool ByteCodeSanity::loop()
{
    SanityValue*           ra    = nullptr;
    SanityValue*           rb    = nullptr;
    SanityValue*           rc    = nullptr;
    SanityValue*           rd    = nullptr;
    uint8_t*               addr  = nullptr;
    uint8_t*               addr2 = nullptr;
    SanityValue            va, vb, vc, vd;
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
        STATE()->ips.push_back(ip);

        switch (ip->op)
        {
            case ByteCodeOp::Ret:
            case ByteCodeOp::InternalPanic:
            case ByteCodeOp::IntrinsicPanic:
            case ByteCodeOp::Unreachable:
            case ByteCodeOp::InternalUnreachable:
                return true;

            case ByteCodeOp::Nop:
                break;

                /////////////////////////////////////////

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
            case ByteCodeOp::IntrinsicBcBreakpoint:
            case ByteCodeOp::ClearRR8:
            case ByteCodeOp::ClearRR16:
            case ByteCodeOp::ClearRR32:
            case ByteCodeOp::ClearRR64:
            case ByteCodeOp::ClearRRX:
            case ByteCodeOp::InternalGetTlsPtr:
            case ByteCodeOp::IntrinsicGetContext:
            case ByteCodeOp::IntrinsicGetProcessInfos:
            case ByteCodeOp::InternalHasErr:
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
            case ByteCodeOp::SaveRRtoRA:
            case ByteCodeOp::CopyRRtoRA:
            case ByteCodeOp::CopySPVaargs:
            case ByteCodeOp::MakeLambda:
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
            case ByteCodeOp::IntrinsicCVaArg:
            case ByteCodeOp::IntrinsicIs:
            case ByteCodeOp::IntrinsicTypeCmp:
            case ByteCodeOp::IntrinsicStringCmp:
            case ByteCodeOp::IntrinsicAs:
            case ByteCodeOp::GetParam64SI:
            case ByteCodeOp::IntrinsicGvtd:
            case ByteCodeOp::IntrinsicGetErr:
            case ByteCodeOp::IntrinsicModules:
            case ByteCodeOp::DeRefStringSlice:
            case ByteCodeOp::IntrinsicArguments:
            case ByteCodeOp::IntrinsicCompiler:
                if (ip->hasWriteRegInA())
                {
                    SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                    ra->setUnknown();
                }
                if (ip->hasWriteRegInB())
                {
                    SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                    rb->setUnknown();
                }
                if (ip->hasWriteRegInC())
                {
                    SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                    rc->setUnknown();
                }
                if (ip->hasWriteRegInD())
                {
                    SWAG_CHECK(STATE()->getRegister(rd, ip->d.u32));
                    rd->setUnknown();
                }
                SanityValue::setIps(ip, ra, rb, rc, rd);
                break;

            case ByteCodeOp::MakeBssSegPointer:
            case ByteCodeOp::MakeMutableSegPointer:
            case ByteCodeOp::MakeCompilerSegPointer:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setUnknown(SANITY_VALUE_FLAG_NOT_ZERO);
                SanityValue::setIps(ip, ra);
                break;

                /////////////////////////////////////////

            case ByteCodeOp::CopyRAtoRT:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                if (ra->isStackAddr())
                    STATE()->invalidateStack();
                break;

            case ByteCodeOp::CopyRAtoRR:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNullReturn(ip->a.u32));
                if (!ip->hasFlag(BCI_IMM_A))
                {
                    if (ra->isStackAddr())
                        return checkEscapeFrame(ra);
                }
                break;

            case ByteCodeOp::CopyRARBtoRR2:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNullReturn(ip->a.u32));
                if (ra->isStackAddr())
                {
                    // Legit in #run block, as we will make a copy
                    if (context.bc->node && context.bc->node->hasAstFlag(AST_IN_RUN_BLOCK))
                        break;
                    return checkEscapeFrame(ra);
                }
                break;

            case ByteCodeOp::CopyRTtoRA:
            {
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::setIps(ip, ra);
                const auto typeInfo     = reinterpret_cast<TypeInfo*>(ip->c.pointer);
                const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                const auto returnType   = typeInfoFunc->concreteReturnType();
                if (returnType->isNullable() && !returnType->isClosure() && !ip->node->hasOwnerTryCatchAssume())
                {
                    uint32_t ipIdx = static_cast<uint32_t>(ip - context.bc->out);
                    if (!STATE()->forceParamsU.contains(ipIdx))
                    {
                        const auto state = newState(ip, ip);
                        state->forceParamsU.push_back(ipIdx);
                        ra->setConstant(0LL);
                    }
                }
                break;
            }

            case ByteCodeOp::CopyRT2toRARB:
            {
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                ra->setUnknown();
                rb->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                const auto typeInfo     = reinterpret_cast<TypeInfo*>(ip->c.pointer);
                const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                const auto returnType   = typeInfoFunc->concreteReturnType();
                if (returnType->isNullable() && !ip->node->hasOwnerTryCatchAssume())
                {
                    uint32_t ipIdx = static_cast<uint32_t>(ip - context.bc->out);
                    if (!STATE()->forceParamsU.contains(ipIdx))
                    {
                        const auto state = newState(ip, ip);
                        state->forceParamsU.push_back(ipIdx);
                        ra->setConstant(0LL);
                        rb->setConstant(0LL);
                    }
                }
                break;
            }

            case ByteCodeOp::GetParam64:
            {
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::setIps(ip, ra);

                const auto overload = ip->node->resolvedSymbolOverload();
                const auto idx      = context.bc->typeInfoFunc->registerIdxToParamIdx(ip->b.mergeU64U32.high);

                if (ip->node && overload && !STATE()->forceParamsU.contains(idx))
                {
                    const auto typeOver = TypeManager::concreteType(overload->typeInfo);
                    if (overload->typeInfo->isNullable() && !typeOver->isClosure())
                    {
                        ra->setConstant(0LL);
                        if (!STATE()->forceParams0.contains(idx))
                        {
                            const auto state = newState(ip, ip);
                            state->forceParamsU.push_back(idx);
                            STATE()->forceParams0.push_back(idx);
                        }
                    }
                }
            }
            break;

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

            case ByteCodeOp::LambdaCall:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                SWAG_CHECK(checkNotNullArguments(pushParams, ""));
                STATE()->invalidateStack();
                pushParams.clear();
                break;

            case ByteCodeOp::LocalCall:
            case ByteCodeOp::ForeignCall:
                SWAG_CHECK(checkNotNullArguments(pushParams, ""));
                STATE()->invalidateStack();
                pushParams.clear();
                break;

                /////////////////////////////////////////

            case ByteCodeOp::Jump:
                ip->dynFlags.add(BCID_SAN_PASS);
                ip += ip->b.s32 + 1;
                continue;

            case ByteCodeOp::JumpIfFalse:
            case ByteCodeOp::JumpIfZero8:
            case ByteCodeOp::JumpIfZero16:
            case ByteCodeOp::JumpIfZero32:
            case ByteCodeOp::JumpIfZero64:
                SWAG_CHECK(STATE()->getImmediateA(va));
                if (va.isZero())
                {
                    ip->dynFlags.add(BCID_SAN_PASS);
                    ip += ip->b.s32 + 1;
                    continue;
                }
                if (!va.isNotZero() && !context.statesHere.contains(ip + ip->b.s32 + 1))
                {
                    context.statesHere.insert(ip + ip->b.s32 + 1);
                    const auto state = newState(ip, ip + ip->b.s32 + 1);
                    state->regs[ip->a.u32].setConstant(0LL);
                    SWAG_CHECK(backTrace(state, ip->a.u32));
                    SanityValue::setIps(ip, &state->regs[ip->a.u32]);
                }

                STATE()->regs[ip->a.u32].setConstant(1LL);
                SanityValue::setIps(ip, &STATE()->regs[ip->a.u32]);
                SWAG_CHECK(backTrace(STATE(), ip->a.u32));
                break;

            case ByteCodeOp::JumpIfTrue:
            case ByteCodeOp::JumpIfNotZero8:
            case ByteCodeOp::JumpIfNotZero16:
            case ByteCodeOp::JumpIfNotZero32:
            case ByteCodeOp::JumpIfNotZero64:
                SWAG_CHECK(STATE()->getImmediateA(va));
                if (va.isNotZero())
                {
                    STATE()->regs[ip->a.u32].setConstant(1LL);
                    backTrace(STATE(), ip->a.u32);
                    ip->dynFlags.add(BCID_SAN_PASS);
                    ip += ip->b.s32 + 1;
                    continue;
                }
                if (!va.isZero() && !context.statesHere.contains(ip + ip->b.s32 + 1))
                {
                    context.statesHere.insert(ip + ip->b.s32 + 1);
                    const auto state = newState(ip, ip + ip->b.s32 + 1);
                    state->regs[ip->a.u32].setConstant(1LL);
                    backTrace(state, ip->a.u32);
                    SanityValue::setIps(ip, &state->regs[ip->a.u32]);
                }

                STATE()->regs[ip->a.u32].setConstant(0LL);
                SanityValue::setIps(ip, &STATE()->regs[ip->a.u32]);
                backTrace(STATE(), ip->a.u32);
                break;

            case ByteCodeOp::JumpIfNotEqual64:
                SWAG_CHECK(STATE()->getImmediateA(va));
                SWAG_CHECK(STATE()->getImmediateC(vc));

                if (va.isConstant() && vc.isConstant() && va.reg.u64 == vc.reg.u64)
                {
                    ip->dynFlags.add(BCID_SAN_PASS);
                    ip = ip + 1;
                    continue;
                }

                if ((va.isConstant() && vc.isConstant() && va.reg.u64 != vc.reg.u64) ||
                    (va.isNotZero() && vc.isZero()) ||
                    (va.isZero() && vc.isNotZero()))
                {
                    ip->dynFlags.add(BCID_SAN_PASS);
                    ip += ip->b.s32 + 1;
                    continue;
                }

                if (!context.statesHere.contains(ip + ip->b.s32 + 1))
                {
                    context.statesHere.insert(ip + ip->b.s32 + 1);
                    newState(ip, ip + ip->b.s32 + 1);
                }
                break;

            case ByteCodeOp::JumpIfEqual64:
                SWAG_CHECK(STATE()->getImmediateA(va));
                SWAG_CHECK(STATE()->getImmediateC(vc));

                if (va.isConstant() && vc.isConstant() && va.reg.u64 == vc.reg.u64)
                {
                    ip->dynFlags.add(BCID_SAN_PASS);
                    ip += ip->b.s32 + 1;
                    continue;
                }

                if ((va.isConstant() && vc.isConstant() && va.reg.u64 != vc.reg.u64) ||
                    (va.isNotZero() && vc.isZero()) ||
                    (va.isZero() && vc.isNotZero()))
                {
                    ip->dynFlags.add(BCID_SAN_PASS);
                    ip = ip + 1;
                    continue;
                }

                if (!context.statesHere.contains(ip + ip->b.s32 + 1))
                {
                    context.statesHere.insert(ip + ip->b.s32 + 1);
                    newState(ip, ip + ip->b.s32 + 1);
                }
                break;

                /////////////////////////////////////////

            case ByteCodeOp::CopyRBtoRA64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                *ra = *rb;
                ra->ips.push_back(ip);
                break;

            case ByteCodeOp::CompareOp3Way8:
                SWAG_CHECK(STATE()->getImmediateA(va));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                if (va.isConstant() && vb.isConstant())
                {
                    auto sub = va.reg.u8 - vb.reg.u8;
                    rc->setConstant((sub > 0) - (sub < 0));
                }
                else
                    rc->setUnknown();
                SanityValue::setIps(ip, &va, &vb, rc);
                break;
            case ByteCodeOp::CompareOp3Way16:
                SWAG_CHECK(STATE()->getImmediateA(va));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                if (va.isConstant() && vb.isConstant())
                {
                    auto sub = va.reg.u16 - vb.reg.u16;
                    rc->setConstant((sub > 0) - (sub < 0));
                }
                else
                    rc->setUnknown();
                SanityValue::setIps(ip, &va, &vb, rc);
                break;
            case ByteCodeOp::CompareOp3Way32:
                SWAG_CHECK(STATE()->getImmediateA(va));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                if (va.isConstant() && vb.isConstant())
                {
                    auto sub = va.reg.u32 - vb.reg.u32;
                    rc->setConstant((sub > 0) - (sub < 0));
                }
                else
                    rc->setUnknown();
                SanityValue::setIps(ip, &va, &vb, rc);
                break;
            case ByteCodeOp::CompareOp3Way64:
                SWAG_CHECK(STATE()->getImmediateA(va));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                if (va.isConstant() && vb.isConstant())
                {
                    auto sub = va.reg.u64 - vb.reg.u64;
                    rc->setConstant((sub > 0) - (sub < 0));
                }
                else
                    rc->setUnknown();
                SanityValue::setIps(ip, &va, &vb, rc);
                break;
            case ByteCodeOp::CompareOp3WayF32:
                SWAG_CHECK(STATE()->getImmediateA(va));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                if (va.isConstant() && vb.isConstant())
                {
                    auto sub = va.reg.f32 - vb.reg.f32;
                    rc->setConstant((sub > 0) - (sub < 0));
                }
                else
                    rc->setUnknown();
                SanityValue::setIps(ip, &va, &vb, rc);
                break;
            case ByteCodeOp::CompareOp3WayF64:
                SWAG_CHECK(STATE()->getImmediateA(va));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                if (va.isConstant() && vb.isConstant())
                {
                    auto sub = va.reg.f64 - vb.reg.f64;
                    rc->setConstant((sub > 0) - (sub < 0));
                }
                else
                    rc->setUnknown();
                SanityValue::setIps(ip, &va, &vb, rc);
                break;

            case ByteCodeOp::ZeroToTrue:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 == 0;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::GreaterEqZeroToTrue:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 >= 0;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 > 0;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 <= 0;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::LowerZeroToTrue:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 < 0;
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::MakeConstantSegPointer:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setConstantAddr(ip->b.u32);
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::MakeStackPointer:
                SWAG_CHECK(STATE()->checkStackOffset(ip->b.u32, 0, nullptr));
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setStackAddr(ip->b.u32);
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::SetZeroStackX:
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->a.u32, ip->b.u32, nullptr));
                memset(addr, 0, ip->b.u32);
                STATE()->setStackKind(addr, ip->b.u32, SanityValueKind::Constant);
                STATE()->setStackIps(addr, ip->b.u32, true);
                break;
            case ByteCodeOp::SetZeroStack8:
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->a.u32, 1, nullptr));
                *addr = 0;
                STATE()->setStackKind(addr, 1, SanityValueKind::Constant);
                STATE()->setStackIps(addr, 1, true);
                break;
            case ByteCodeOp::SetZeroStack16:
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->a.u32, 2, nullptr));
                *reinterpret_cast<uint16_t*>(addr) = 0;
                STATE()->setStackKind(addr, 2, SanityValueKind::Constant);
                STATE()->setStackIps(addr, 2, true);
                break;
            case ByteCodeOp::SetZeroStack32:
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->a.u32, 4, nullptr));
                *reinterpret_cast<uint32_t*>(addr) = 0;
                STATE()->setStackKind(addr, 4, SanityValueKind::Constant);
                STATE()->setStackIps(addr, 4, true);
                break;
            case ByteCodeOp::SetZeroStack64:
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->a.u32, 8, nullptr));
                *reinterpret_cast<uint64_t*>(addr) = 0;
                STATE()->setStackKind(addr, 8, SanityValueKind::Constant);
                STATE()->setStackIps(addr, 8, true);
                break;

            case ByteCodeOp::SetAtStackPointer64:
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->a.u32, 8, nullptr));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                *reinterpret_cast<uint64_t*>(addr) = vb.reg.u64;
                STATE()->setStackKind(addr, 8, vb.kind);
                STATE()->setStackIps(addr, 8, true);
                break;

            case ByteCodeOp::SetZeroAtPointerXRB:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_ASSERT(rb->reg.u64 * ip->c.u64 <= UINT32_MAX);
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64), ra));
                    memset(addr, 0, rb->reg.u64 * ip->c.u64);
                    STATE()->setStackKind(addr, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64), SanityValueKind::Constant);
                    STATE()->setStackIps(addr, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64), true);
                }
                break;

            case ByteCodeOp::SetZeroAtPointerX:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_ASSERT(ip->b.u64 <= UINT32_MAX);
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->c.u32, static_cast<uint32_t>(ip->b.u64), ra));
                    memset(addr, 0, ip->b.u64);
                    STATE()->setStackKind(addr, static_cast<uint32_t>(ip->b.u64), SanityValueKind::Constant);
                    STATE()->setStackIps(addr, static_cast<uint32_t>(ip->b.u64), true);
                }
                break;
            case ByteCodeOp::SetZeroAtPointer8:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->b.u32, 1, ra));
                    *addr = 0;
                    STATE()->setStackKind(addr, 1, SanityValueKind::Constant);
                    STATE()->setStackIps(addr, 1, true);
                }
                break;
            case ByteCodeOp::SetZeroAtPointer16:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->b.u32, 2, ra));
                    *reinterpret_cast<uint16_t*>(addr) = 0;
                    STATE()->setStackKind(addr, 2, SanityValueKind::Constant);
                    STATE()->setStackIps(addr, 2, true);
                }
                break;
            case ByteCodeOp::SetZeroAtPointer32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->b.u32, 4, ra));
                    *reinterpret_cast<uint32_t*>(addr) = 0;
                    STATE()->setStackKind(addr, 4, SanityValueKind::Constant);
                    STATE()->setStackIps(addr, 4, true);
                }
                break;
            case ByteCodeOp::SetZeroAtPointer64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->b.u32, 8, ra));
                    *reinterpret_cast<uint64_t*>(addr) = 0;
                    STATE()->setStackKind(addr, 8, SanityValueKind::Constant);
                    STATE()->setStackIps(addr, 8, true);
                }
                break;

            case ByteCodeOp::SetAtPointer8:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->c.u32, 1, ra));
                    SWAG_CHECK(STATE()->getImmediateB(vb));
                    *addr = vb.reg.u8;
                    STATE()->setStackKind(addr, 1, vb.kind);
                    STATE()->setStackIps(addr, 1, true);
                }
                break;
            case ByteCodeOp::SetAtPointer16:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->c.u32, 2, ra));
                    SWAG_CHECK(STATE()->getImmediateB(vb));
                    *reinterpret_cast<uint16_t*>(addr) = vb.reg.u16;
                    STATE()->setStackKind(addr, 2, vb.kind);
                    STATE()->setStackIps(addr, 2, true);
                }
                break;
            case ByteCodeOp::SetAtPointer32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->c.u32, 4, ra));
                    SWAG_CHECK(STATE()->getImmediateB(vb));
                    *reinterpret_cast<uint32_t*>(addr) = vb.reg.u32;
                    STATE()->setStackKind(addr, 4, vb.kind);
                    STATE()->setStackIps(addr, 4, true);
                }
                break;
            case ByteCodeOp::SetAtPointer64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u64 + ip->c.u32, 8, ra));
                    SWAG_CHECK(STATE()->getImmediateB(vb));
                    *reinterpret_cast<uint64_t*>(addr) = vb.reg.u64;
                    STATE()->setStackKind(addr, 8, vb.kind);
                    STATE()->setStackIps(addr, 8, true);
                }
                break;

            case ByteCodeOp::SetImmediate32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setConstant(ip->b.u32);
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::SetImmediate64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->kind = SanityValueKind::Constant;
                ra->setConstant(ip->b.u64);
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::ClearRA:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setConstant(0LL);
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::IncrementRA64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.u64++;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::DecrementRA64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.u64--;
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::Add64byVB64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.s64 += ip->b.s64;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::Mul64byVB64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.s64 *= ip->b.s64;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::Div64byVB64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->reg.s64 /= ip->b.s64;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::MulAddVC64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isUnknown())
                    ra->setUnknown();
                else
                    ra->reg.u64 = ra->reg.u64 * (rb->reg.u64 + ip->c.u32);
                SanityValue::setIps(ip, ra, rb);
                break;

            case ByteCodeOp::GetFromStack8:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->b.u32, 1, nullptr));
                SWAG_CHECK(STATE()->getStackKind(ra, addr, 1));
                SWAG_CHECK(STATE()->checkStackInitialized(addr, 1, ra));
                ra->reg.u64 = *addr;
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::GetFromStack16:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->b.u32, 2, nullptr));
                SWAG_CHECK(STATE()->getStackKind(ra, addr, 2));
                SWAG_CHECK(STATE()->checkStackInitialized(addr, 2, ra));
                ra->reg.u64 = *reinterpret_cast<uint16_t*>(addr);
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::GetFromStack32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->b.u32, 4, nullptr));
                SWAG_CHECK(STATE()->getStackKind(ra, addr, 4));
                SWAG_CHECK(STATE()->checkStackInitialized(addr, 4, ra));
                ra->reg.u64 = *reinterpret_cast<uint32_t*>(addr);
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::GetFromStack64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getStackAddress(addr, ip->b.u32, 8, nullptr));
                SWAG_CHECK(STATE()->getStackKind(ra, addr, 8));
                SWAG_CHECK(STATE()->checkStackInitialized(addr, 8, ra));
                ra->reg.u64 = *reinterpret_cast<uint64_t*>(addr);
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::IncPointer64:
            {
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));

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

                SWAG_CHECK(checkNotNull(ra));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (ra->isConstant() && vb.isConstant())
                    rc->setConstant(ra->reg.u64 + vb.reg.s64);
                else if (ra->isStackAddr() && vb.isConstant())
                    rc->setStackAddr(ra->reg.u64 + vb.reg.s64);
                else if (ra->isConstantAddr() && vb.isConstant())
                    rc->setConstantAddr(ra->reg.u64 + vb.reg.s64);
                else
                    rc->setUnknown(ra->flags);
                SanityValue::setIps(ip, ra, &vb, rc);
            }
            break;
            case ByteCodeOp::DecPointer64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (ra->isConstant() && vb.isConstant())
                    rc->setConstant(ra->reg.u64 - vb.reg.s64);
                else if (ra->isStackAddr() && vb.isConstant())
                    rc->setStackAddr(ra->reg.u64 - vb.reg.s64);
                else if (ra->isConstantAddr() && vb.isConstant())
                    rc->setConstantAddr(ra->reg.u64 - vb.reg.s64);
                else
                    rc->setUnknown();
                SanityValue::setIps(ip, ra, &vb, rc);
                break;
            case ByteCodeOp::IncMulPointer64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (ra->isConstant() && vb.isConstant())
                    rc->setConstant(ra->reg.u64 + vb.reg.s64 * ip->d.u64);
                else if (ra->isStackAddr() && vb.isConstant())
                    rc->setStackAddr(ra->reg.u64 + vb.reg.s64 * ip->d.u64);
                else if (ra->isConstantAddr() && vb.isConstant())
                    rc->setConstantAddr(ra->reg.u64 + vb.reg.s64 * ip->d.u64);
                else
                    rc->setUnknown(ra->flags);
                SanityValue::setIps(ip, ra, &vb, rc);
                break;

            case ByteCodeOp::DeRef8:
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(rb));
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                if (rb->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, rb->reg.u64 + ip->c.s64, 1, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr, 1));
                    SWAG_CHECK(STATE()->getStackKind(ra, addr, 1));
                    ra->reg.u64 = *addr;
                }
                else if (rb->isConstantAddr())
                {
                    addr = context.bc->sourceFile->module->constantSegment.address(rb->reg.u32);
                    ra->setConstant(*addr);
                }
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::DeRef16:
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(rb));
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                if (rb->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, rb->reg.u64 + ip->c.s64, 2, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr, 2));
                    SWAG_CHECK(STATE()->getStackKind(ra, addr, 2));
                    ra->reg.u64 = *reinterpret_cast<uint16_t*>(addr);
                }
                else if (rb->isConstantAddr())
                {
                    addr = context.bc->sourceFile->module->constantSegment.address(rb->reg.u32);
                    ra->setConstant(*reinterpret_cast<uint16_t*>(addr));
                }
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::DeRef32:
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(rb));
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                if (rb->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, rb->reg.u64 + ip->c.s64, 4, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr, 4));
                    SWAG_CHECK(STATE()->getStackKind(ra, addr, 4));
                    ra->reg.u64 = *reinterpret_cast<uint32_t*>(addr);
                }
                else if (rb->isConstantAddr())
                {
                    addr = context.bc->sourceFile->module->constantSegment.address(rb->reg.u32);
                    ra->setConstant(*reinterpret_cast<uint32_t*>(addr));
                }
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::DeRef64:
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(rb));
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                if (rb->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, rb->reg.u64 + ip->c.s64, 8, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr, 8));
                    SWAG_CHECK(STATE()->getStackKind(ra, addr, 8));
                    ra->reg.u64 = *reinterpret_cast<uint64_t*>(addr);
                }
                else if (rb->isConstantAddr())
                {
                    addr = context.bc->sourceFile->module->constantSegment.address(rb->reg.u32);
                    ra->setConstant(*reinterpret_cast<uint64_t*>(addr));
                }
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                break;

            case ByteCodeOp::NegBool:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(rb->reg.b ? 0LL : 1LL);
                else if (rb->isNotZero() && !rb->isConstant())
                    ra->setConstant(0LL);
                else
                    ra->setUnknown(rb->flags);
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::NegS32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(-vb.reg.s32);
                else
                    ra->setUnknown(rb->flags);
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::NegS64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(-rb->reg.s64);
                else
                    ra->setUnknown(rb->flags);
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::NegF32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(-rb->reg.f32);
                else
                    ra->setUnknown(rb->flags);
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::NegF64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (vb.isConstant())
                    ra->setConstant(-rb->reg.f64);
                else
                    ra->setUnknown(rb->flags);
                SanityValue::setIps(ip, ra, rb);
                break;

            case ByteCodeOp::CastBool8:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(vb.reg.u8 ? 1LL : 0);
                else if (va.isUnknown() && vb.flags.has(SANITY_VALUE_FLAG_NOT_ZERO))
                    ra->setConstant(1LL);
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastBool16:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(vb.reg.u16 ? 1LL : 0);
                else if (va.isUnknown() && vb.flags.has(SANITY_VALUE_FLAG_NOT_ZERO))
                    ra->setConstant(1LL);
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastBool32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(vb.reg.u32 ? 1LL : 0);
                else if (va.isUnknown() && vb.flags.has(SANITY_VALUE_FLAG_NOT_ZERO))
                    ra->setConstant(1LL);
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastBool64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(vb.reg.u64 ? 1LL : 0);
                else if (va.isUnknown() && vb.flags.has(SANITY_VALUE_FLAG_NOT_ZERO))
                    ra->setConstant(1LL);
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8S16:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int16_t>(vb.reg.s8));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8S32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int32_t>(vb.reg.s8));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS16S32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int32_t>(vb.reg.s16));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastF32S32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int32_t>(vb.reg.f32));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8S64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int64_t>(vb.reg.s8));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS16S64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int64_t>(vb.reg.s16));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS32S64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int64_t>(vb.reg.s32));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastF64S64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int64_t>(vb.reg.f64));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.s8));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS16F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.s16));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS32F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.s32));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS64F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.s64));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU8F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.u8));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU16F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.u16));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU32F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.u32));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU64F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.u64));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.s8));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS16F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.s16));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS32F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.s32));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS64F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.s64));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU8F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.u8));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU16F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.u16));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU32F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.u32));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU64F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.u64));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;
            case ByteCodeOp::CastF32F64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.f32));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;

            case ByteCodeOp::CastF64F32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.f64));
                else
                    ra->setUnknown(vb.flags);
                SanityValue::setIps(ip, ra, &vb);
                break;

            case ByteCodeOp::ClearMaskU32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                if (ra->isConstant())
                    ra->setConstant(ra->reg.u32 & ip->b.u32);
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra);
                break;
            case ByteCodeOp::ClearMaskU64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                if (ra->isConstant())
                    ra->setConstant(ra->reg.u64 & ip->b.u64);
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::InvertU8:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(~rb->reg.u8);
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::InvertU16:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(~rb->reg.u16);
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::InvertU32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(~rb->reg.u32);
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                break;
            case ByteCodeOp::InvertU64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(~rb->reg.u64);
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, rb);
                break;

                /////////////////////////////////////////

            case ByteCodeOp::IntrinsicItfTableOf:
                SWAG_CHECK(checkNotNullArguments({ip->b.u32, ip->a.u32}, "@itfTableOf"));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                rc->setUnknown();
                SanityValue::setIps(ip, nullptr, nullptr, rc);
                break;

            case ByteCodeOp::IntrinsicCVaStart:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNullArguments({ip->a.u32}, "@cvastart"));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, 8, ra));
                    STATE()->setStackKind(addr, 8, SanityValueKind::Unknown);
                }
                break;

            case ByteCodeOp::IntrinsicStrCmp:
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                SWAG_CHECK(checkNotNullArguments({ip->c.u32, ip->b.u32}, "@strcmp"));
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::IntrinsicStrLen:
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNullArguments({ip->b.u32}, "@strlen"));
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::IntrinsicMemCmp:
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(STATE()->getRegister(rc, ip->c.u32));
                SWAG_CHECK(checkNotNullArguments({ip->c.u32, ip->b.u32}, "@memcmp"));
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::setIps(ip, ra);
                break;

            case ByteCodeOp::MemCpy8:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (ra->isStackAddr() && rb->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, 1, ra));
                    SWAG_CHECK(STATE()->getStackAddress(addr2, rb->reg.u32, 1, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr2, 1, rb));
                    SWAG_CHECK(STATE()->getStackKind(&va, addr2, 1));
                    STATE()->setStackKind(addr, 1, va.kind, va.flags);
                    STATE()->updateStackIps(addr, 1, &va);
                    *addr = *addr2;
                }
                else
                    STATE()->invalidateStack();
                break;

            case ByteCodeOp::MemCpy16:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (ra->isStackAddr() && rb->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, 2, ra));
                    SWAG_CHECK(STATE()->getStackAddress(addr2, rb->reg.u32, 2, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr2, 2, rb));
                    SWAG_CHECK(STATE()->getStackKind(&va, addr2, 2));
                    STATE()->setStackKind(addr, 2, va.kind, va.flags);
                    STATE()->updateStackIps(addr, 2, &va);
                    *reinterpret_cast<uint16_t*>(addr) = *reinterpret_cast<uint16_t*>(addr2);
                }
                else
                    STATE()->invalidateStack();
                break;

            case ByteCodeOp::MemCpy32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (ra->isStackAddr() && rb->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, 4, ra));
                    SWAG_CHECK(STATE()->getStackAddress(addr2, rb->reg.u32, 4, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr2, 4, rb));
                    SWAG_CHECK(STATE()->getStackKind(&va, addr2, 4));
                    STATE()->setStackKind(addr, 4, va.kind, va.flags);
                    STATE()->updateStackIps(addr, 4, &va);
                    *reinterpret_cast<uint32_t*>(addr) = *reinterpret_cast<uint32_t*>(addr2);
                }
                else
                    STATE()->invalidateStack();
                break;

            case ByteCodeOp::MemCpy64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                if (ra->isStackAddr() && rb->isStackAddr())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, 8, ra));
                    SWAG_CHECK(STATE()->getStackAddress(addr2, rb->reg.u32, 8, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr2, 8, rb));
                    SWAG_CHECK(STATE()->getStackKind(&va, addr2, 8));
                    STATE()->setStackKind(addr, 8, va.kind, va.flags);
                    STATE()->updateStackIps(addr, 8, &va);
                    *reinterpret_cast<uint64_t*>(addr) = *reinterpret_cast<uint64_t*>(addr2);
                }
                else
                    STATE()->invalidateStack();
                break;

            case ByteCodeOp::IntrinsicMemCpy:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(STATE()->getImmediateC(vc));
                SWAG_CHECK(checkNotNullArguments({ip->b.u32, ip->a.u32}, "@memcpy"));
                if (ra->isStackAddr() && rb->isStackAddr() && vc.isConstant())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, vc.reg.u32, ra));
                    SWAG_CHECK(STATE()->getStackAddress(addr2, rb->reg.u32, vc.reg.u32, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr2, vc.reg.u32, rb));
                    SWAG_CHECK(STATE()->getStackKind(&va, addr2, vc.reg.u32));
                    STATE()->setStackKind(addr, vc.reg.u32, va.kind, va.flags);
                    STATE()->updateStackIps(addr, vc.reg.u32, &va);
                    std::copy_n(addr2, vc.reg.u32, addr);
                }
                else
                    STATE()->invalidateStack();
                break;

            case ByteCodeOp::IntrinsicMemMove:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getRegister(rb, ip->b.u32));
                SWAG_CHECK(STATE()->getImmediateC(vc));
                SWAG_CHECK(checkNotNullArguments({ip->b.u32, ip->a.u32}, "@memmove"));
                if (ra->isStackAddr() && rb->isStackAddr() && vc.isConstant())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, vc.reg.u32, ra));
                    SWAG_CHECK(STATE()->getStackAddress(addr2, rb->reg.u32, vc.reg.u32, rb));
                    SWAG_CHECK(STATE()->checkStackInitialized(addr2, vc.reg.u32, rb));
                    SWAG_CHECK(STATE()->getStackKind(&va, addr2, vc.reg.u32));
                    STATE()->setStackKind(addr, vc.reg.u32, va.kind);
                    STATE()->updateStackIps(addr, vc.reg.u32, &va);
                    memmove(addr, addr2, vc.reg.u32);
                }
                else
                    STATE()->invalidateStack();
                break;
            case ByteCodeOp::IntrinsicMemSet:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getImmediateC(vc));
                SWAG_CHECK(checkNotNullArguments({ip->a.u32}, "@memset"));
                if (ra->isStackAddr() && vb.isConstant() && vc.isConstant())
                {
                    SWAG_CHECK(STATE()->getStackAddress(addr, ra->reg.u32, vc.reg.u32, ra));
                    STATE()->setStackKind(addr, vc.reg.u32, SanityValueKind::Constant);
                    STATE()->setStackIps(addr, vc.reg.u32, true);
                    memset(addr, vb.reg.u8, vc.reg.u32);
                }
                else
                    STATE()->invalidateStack();
                break;

            case ByteCodeOp::IntrinsicMulAddF32:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getImmediateC(vc));
                SWAG_CHECK(STATE()->getImmediateD(vd));
                if (vb.isConstant() && vc.isConstant() && vd.isConstant())
                    ra->setConstant(vb.reg.f32 * vc.reg.f32 + vd.reg.f32);
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, &vb, &vc, &vd);
                break;
            case ByteCodeOp::IntrinsicMulAddF64:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getImmediateC(vc));
                SWAG_CHECK(STATE()->getImmediateD(vd));
                if (vb.isConstant() && vc.isConstant() && vd.isConstant())
                    ra->setConstant(vb.reg.f64 * vc.reg.f64 + vd.reg.f64);
                else
                    ra->setUnknown();
                SanityValue::setIps(ip, ra, &vb, &vc, &vd);
                break;

            case ByteCodeOp::IntrinsicS8x1:
            case ByteCodeOp::IntrinsicS16x1:
            case ByteCodeOp::IntrinsicS32x1:
            case ByteCodeOp::IntrinsicS64x1:
            case ByteCodeOp::IntrinsicF32x1:
            case ByteCodeOp::IntrinsicF64x1:
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                ra->setUnknown();
                SanityValue::setIps(ip, ra, &vb);
                if (vb.isConstant())
                {
                    ra->kind = SanityValueKind::Constant;
                    SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(&context, ip, ra->reg, vb.reg, {}, {}));
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
                SWAG_CHECK(STATE()->getRegister(ra, ip->a.u32));
                SWAG_CHECK(STATE()->getImmediateB(vb));
                SWAG_CHECK(STATE()->getImmediateC(vc));
                ra->setUnknown();
                SanityValue::setIps(ip, ra, &vb, &vc);
                if (vb.isConstant() && vc.isConstant())
                {
                    ra->kind = SanityValueKind::Constant;
                    SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(&context, ip, ra->reg, vb.reg, vc.reg, {}));
                }
                break;

                /////////////////////////////////////////

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
            case ByteCodeOp::AffectOpPlusEqF32_Safe:
                BINOP_EQ(float, +=, f32);
                break;
            case ByteCodeOp::AffectOpPlusEqF64_Safe:
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
            case ByteCodeOp::AffectOpMinusEqF32_Safe:
                BINOP_EQ(float, -=, f32);
                break;
            case ByteCodeOp::AffectOpMinusEqF64_Safe:
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
            case ByteCodeOp::AffectOpMulEqF32_Safe:
                BINOP_EQ(float, *=, f32);
                break;
            case ByteCodeOp::AffectOpMulEqF64_Safe:
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

            case ByteCodeOp::AffectOpAndEq8:
                BINOP_EQ(uint8_t, &=, u8);
                break;
            case ByteCodeOp::AffectOpAndEq16:
                BINOP_EQ(uint16_t, &=, u16);
                break;
            case ByteCodeOp::AffectOpAndEq32:
                BINOP_EQ(uint32_t, &=, u32);
                break;
            case ByteCodeOp::AffectOpAndEq64:
                BINOP_EQ(uint64_t, &=, u64);
                break;

            case ByteCodeOp::AffectOpOrEq8:
                BINOP_EQ(uint8_t, |=, u8);
                break;
            case ByteCodeOp::AffectOpOrEq16:
                BINOP_EQ(uint16_t, |=, u16);
                break;
            case ByteCodeOp::AffectOpOrEq32:
                BINOP_EQ(uint32_t, |=, u32);
                break;
            case ByteCodeOp::AffectOpOrEq64:
                BINOP_EQ(uint64_t, |=, u64);
                break;

            case ByteCodeOp::AffectOpXorEq8:
                BINOP_EQ(uint8_t, ^=, u8);
                break;
            case ByteCodeOp::AffectOpXorEq16:
                BINOP_EQ(uint16_t, ^=, u16);
                break;
            case ByteCodeOp::AffectOpXorEq32:
                BINOP_EQ(uint32_t, ^=, u32);
                break;
            case ByteCodeOp::AffectOpXorEq64:
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

            case ByteCodeOp::BinOpXor8:
                BINOP(^, u8);
                break;
            case ByteCodeOp::BinOpXor16:
                BINOP(^, u16);
                break;
            case ByteCodeOp::BinOpXor32:
                BINOP(^, u32);
                break;
            case ByteCodeOp::BinOpXor64:
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
            case ByteCodeOp::BinOpPlusF32_Safe:
                BINOP(+, f32);
                break;
            case ByteCodeOp::BinOpPlusF64_Safe:
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
            case ByteCodeOp::BinOpMinusF32_Safe:
                BINOP(-, f32);
                break;
            case ByteCodeOp::BinOpMinusF64_Safe:
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
            case ByteCodeOp::BinOpMulF32_Safe:
                BINOP(*, f32);
                break;
            case ByteCodeOp::BinOpMulF64_Safe:
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

            default:
                Report::internalError(context.bc->sourceFile->module, form("unknown instruction [[%s]] during sanity check", ByteCode::opName(ip->op)));
                return false;
        }

        ip->dynFlags.add(BCID_SAN_PASS);
        ip++;
    }

    return true;
}
