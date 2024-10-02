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

#pragma optimize("", off)

bool ByteCodeSanity::loop()
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
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::computeIp(ip, ra);
                const auto typeInfo     = reinterpret_cast<TypeInfo*>(ip->c.pointer);
                const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                const auto returnType   = typeInfoFunc->concreteReturnType();
                if (returnType->isNullable() && !returnType->isClosure())
                {
#if 0
                        uint32_t ipIdx = static_cast<uint32_t>(ip - context.bc->out);
                        if (!STATE()->forceParamsU.contains(ipIdx))
                        {
                            const auto state = newState(context, ip, ip);
                            state->forceParamsU.push_back(ipIdx);
                            ra->kind        = SanityValueKind::Constant;
                            ra->reg.pointer = nullptr;
                        }
#endif
                }
                break;
            }

            case ByteCodeOp::CopyRT2toRARB:
            {
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                ra->setUnknown();
                rb->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                const auto typeInfo     = reinterpret_cast<TypeInfo*>(ip->c.pointer);
                const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                const auto returnType   = typeInfoFunc->concreteReturnType();
                if (returnType->isNullable())
                {
#if 0                        
                         ra->kind        = SanityValueKind::Constant;
                         ra->reg.pointer = nullptr;
                         rb->kind        = SanityValueKind::Constant;
                         rb->reg.pointer = nullptr;
#endif
                }
                break;
            }

            case ByteCodeOp::GetParam64:
            {
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::computeIp(ip, ra);

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

            // Fake 1 value
            case ByteCodeOp::InternalGetTlsPtr:
            case ByteCodeOp::IntrinsicGetContext:
            case ByteCodeOp::IntrinsicGetProcessInfos:
            case ByteCodeOp::InternalHasErr:
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
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::CopyRAtoRT:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                if (ra->isStackAddr())
                    invalidateCurStateStack();
                break;

            case ByteCodeOp::IntrinsicCVaStart:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNullArguments({ip->a.u32}, "@cvastart"));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, 8));
                    setStackValue(addr, 8, SanityValueKind::Unknown);
                }
                break;

            case ByteCodeOp::IntrinsicCVaArg:
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                rb->setUnknown();
                SanityValue::computeIp(ip, nullptr, rb);
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
                SWAG_CHECK(checkNotNullArguments({ip->b.u32, ip->a.u32}, "@itfTableOf"));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                rc->setUnknown();
                SanityValue::computeIp(ip, nullptr, nullptr, rc);
                break;

            case ByteCodeOp::IntrinsicIs:
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                rc->setUnknown();
                SanityValue::computeIp(ip, nullptr, nullptr, rc);
                break;

            case ByteCodeOp::IntrinsicTypeCmp:
            case ByteCodeOp::IntrinsicStringCmp:
            case ByteCodeOp::IntrinsicAs:
                SWAG_CHECK(getRegister(rd, ip->d.u32));
                rd->setUnknown();
                SanityValue::computeIp(ip, nullptr, nullptr, nullptr, rd);
                break;

            // Fake 2 values
            case ByteCodeOp::GetParam64SI:
            case ByteCodeOp::IntrinsicGvtd:
            case ByteCodeOp::IntrinsicGetErr:
            case ByteCodeOp::IntrinsicModules:
            case ByteCodeOp::DeRefStringSlice:
            case ByteCodeOp::IntrinsicArguments:
            case ByteCodeOp::IntrinsicCompiler:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                ra->setUnknown();
                rb->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;

            case ByteCodeOp::Jump:
                ip->dynFlags.add(BCID_SAN_PASS);
                ip += ip->b.s32 + 1;
                continue;

            case ByteCodeOp::JumpIfFalse:
                JUMP1(!va.reg.b);
                if (jmpAddState)
                {
                    jmpAddState->regs[ip->a.u32].setConstant(0LL);
                    SanityValue::computeIp(ip, &jmpAddState->regs[ip->a.u32]);
                }
                else
                {
                    STATE()->regs[ip->a.u32].setConstant(1LL);
                    SanityValue::computeIp(ip, &STATE()->regs[ip->a.u32]);
                }
                break;
            case ByteCodeOp::JumpIfTrue:
                JUMP1(va.reg.b);
                if (jmpAddState)
                {
                    jmpAddState->regs[ip->a.u32].setConstant(1LL);
                    SanityValue::computeIp(ip, &jmpAddState->regs[ip->a.u32]);
                }
                else
                {
                    STATE()->regs[ip->a.u32].setConstant(0LL);
                    SanityValue::computeIp(ip, &STATE()->regs[ip->a.u32]);
                }
                break;

            case ByteCodeOp::JumpIfEqual64:
                JUMP2(va.reg.u64 == vc.reg.u64);
                if (jmpAddState && vc.isConstant())
                {
                    jmpAddState->regs[ip->a.u32].setConstant(vc.reg.u64);
                    SanityValue::computeIp(ip, &jmpAddState->regs[ip->a.u32]);
                }
                break;

            case ByteCodeOp::JumpIfZero8:
                JUMP1(va.reg.u8 == 0);
                if (jmpAddState)
                {
                    jmpAddState->regs[ip->a.u32].setConstant(0LL);
                    SanityValue::computeIp(ip, &jmpAddState->regs[ip->a.u32]);
                }
                break;
            case ByteCodeOp::JumpIfZero16:
                JUMP1(va.reg.u16 == 0);
                if (jmpAddState)
                {
                    jmpAddState->regs[ip->a.u32].setConstant(0LL);
                    SanityValue::computeIp(ip, &jmpAddState->regs[ip->a.u32]);
                }
                break;
            case ByteCodeOp::JumpIfZero32:
                JUMP1(va.reg.u32 == 0);
                if (jmpAddState)
                {
                    jmpAddState->regs[ip->a.u32].setConstant(0LL);
                    SanityValue::computeIp(ip, &jmpAddState->regs[ip->a.u32]);
                }
                break;
            case ByteCodeOp::JumpIfZero64:
                JUMP1(va.reg.u64 == 0);
                if (jmpAddState)
                {
                    jmpAddState->regs[ip->a.u32].setConstant(0LL);
                    SanityValue::computeIp(ip, &jmpAddState->regs[ip->a.u32]);
                }
                break;

            case ByteCodeOp::JumpIfLowerEqS64:
                JUMP2(va.reg.s64 <= vc.reg.s64);
                break;
            case ByteCodeOp::JumpIfNotZero8:
                JUMP1(va.reg.u8 != 0);
                break;
            case ByteCodeOp::JumpIfNotZero16:
                SWAG_CHECK(getImmediateA(va));
                JUMP1(va.reg.u16 != 0);
                break;
            case ByteCodeOp::JumpIfNotZero32:
                SWAG_CHECK(getImmediateA(va));
                JUMP1(va.reg.u32 != 0);
                break;
            case ByteCodeOp::JumpIfNotZero64:
                SWAG_CHECK(getImmediateA(va));
                JUMP1(va.reg.u64 != 0);
                break;

            case ByteCodeOp::CopyRBtoRA64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                *ra = *rb;
                ra->ips.push_back(ip);
                break;

            case ByteCodeOp::CompareOp3Way8:
                SWAG_CHECK(getImmediateA(va));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                SanityValue::computeIp(ip, &va, &vb, rc);
                if (rc->isConstant())
                {
                    auto sub    = va.reg.u8 - vb.reg.u8;
                    rc->reg.s32 = (sub > 0) - (sub < 0);
                }
                break;
            case ByteCodeOp::CompareOp3Way16:
                SWAG_CHECK(getImmediateA(va));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                SanityValue::computeIp(ip, &va, &vb, rc);
                if (rc->isConstant())
                {
                    auto sub    = va.reg.u16 - vb.reg.u16;
                    rc->reg.s32 = (sub > 0) - (sub < 0);
                }
                break;
            case ByteCodeOp::CompareOp3Way32:
                SWAG_CHECK(getImmediateA(va));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                SanityValue::computeIp(ip, &va, &vb, rc);
                if (rc->isConstant())
                {
                    auto sub    = va.reg.u32 - vb.reg.u32;
                    rc->reg.s32 = (sub > 0) - (sub < 0);
                }
                break;
            case ByteCodeOp::CompareOp3Way64:
                SWAG_CHECK(getImmediateA(va));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                SanityValue::computeIp(ip, &va, &vb, rc);
                if (rc->isConstant())
                {
                    auto sub    = va.reg.u64 - vb.reg.u64;
                    rc->reg.s32 = (sub > 0) - (sub < 0);
                }
                break;
            case ByteCodeOp::CompareOp3WayF32:
                SWAG_CHECK(getImmediateA(va));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                SanityValue::computeIp(ip, &va, &vb, rc);
                if (rc->isConstant())
                {
                    auto sub    = va.reg.f32 - vb.reg.f32;
                    rc->reg.s32 = (sub > 0) - (sub < 0);
                }
                break;
            case ByteCodeOp::CompareOp3WayF64:
                SWAG_CHECK(getImmediateA(va));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                rc->kind = va.isConstant() && vb.isConstant() ? SanityValueKind::Constant : SanityValueKind::Unknown;
                SanityValue::computeIp(ip, &va, &vb, rc);
                if (rc->isConstant())
                {
                    auto sub    = va.reg.f64 - vb.reg.f64;
                    rc->reg.s32 = (sub > 0) - (sub < 0);
                }
                break;

            case ByteCodeOp::ZeroToTrue:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 == 0;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::GreaterEqZeroToTrue:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 >= 0;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::GreaterZeroToTrue:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 > 0;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::LowerEqZeroToTrue:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 <= 0;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::LowerZeroToTrue:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.b = ra->reg.s32 < 0;
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::IncrementRA64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.u64++;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::DecrementRA64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.u64--;
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::MakeStackPointer:
                SWAG_CHECK(checkStackOffset(nullptr, ip->b.u32));
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->kind    = SanityValueKind::StackAddr;
                ra->reg.u64 = ip->b.u32;
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::SetZeroStackX:
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->a.u32, ip->b.u32));
                memset(addr, 0, ip->b.u32);
                setStackValue(addr, ip->b.u32, SanityValueKind::Constant);
                break;
            case ByteCodeOp::SetZeroStack8:
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->a.u32, 1));
                *addr = 0;
                setStackValue(addr, 1, SanityValueKind::Constant);
                break;
            case ByteCodeOp::SetZeroStack16:
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->a.u32, 2));
                *reinterpret_cast<uint16_t*>(addr) = 0;
                setStackValue(addr, 2, SanityValueKind::Constant);
                break;
            case ByteCodeOp::SetZeroStack32:
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->a.u32, 4));
                *reinterpret_cast<uint32_t*>(addr) = 0;
                setStackValue(addr, 4, SanityValueKind::Constant);
                break;
            case ByteCodeOp::SetZeroStack64:
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->a.u32, 8));
                *reinterpret_cast<uint64_t*>(addr) = 0;
                setStackValue(addr, 8, SanityValueKind::Constant);
                break;

            case ByteCodeOp::SetAtStackPointer64:
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->a.u32, 8));
                SWAG_CHECK(getImmediateB(vb));
                *reinterpret_cast<uint64_t*>(addr) = vb.reg.u64;
                setStackValue(addr, 8, vb.kind);
                break;

            case ByteCodeOp::SetZeroAtPointerXRB:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_ASSERT(rb->reg.u64 * ip->c.u64 <= UINT32_MAX);
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64)));
                    memset(addr, 0, rb->reg.u64 * ip->c.u64);
                    setStackValue(addr, static_cast<uint32_t>(rb->reg.u64 * ip->c.u64), SanityValueKind::Constant);
                }
                break;

            case ByteCodeOp::SetZeroAtPointerX:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_ASSERT(ip->b.u64 <= UINT32_MAX);
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->c.u32, static_cast<uint32_t>(ip->b.u64)));
                    memset(addr, 0, ip->b.u64);
                    setStackValue(addr, static_cast<uint32_t>(ip->b.u64), SanityValueKind::Constant);
                }
                break;
            case ByteCodeOp::SetZeroAtPointer8:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->b.u32, 1));
                    *addr = 0;
                    setStackValue(addr, 1, SanityValueKind::Constant);
                }
                break;
            case ByteCodeOp::SetZeroAtPointer16:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->b.u32, 2));
                    *reinterpret_cast<uint16_t*>(addr) = 0;
                    setStackValue(addr, 2, SanityValueKind::Constant);
                }
                break;
            case ByteCodeOp::SetZeroAtPointer32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->b.u32, 4));
                    *reinterpret_cast<uint32_t*>(addr) = 0;
                    setStackValue(addr, 4, SanityValueKind::Constant);
                }
                break;
            case ByteCodeOp::SetZeroAtPointer64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->b.u32, 8));
                    *reinterpret_cast<uint64_t*>(addr) = 0;
                    setStackValue(addr, 8, SanityValueKind::Constant);
                }
                break;

            case ByteCodeOp::SetAtPointer8:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->c.u32, 1));
                    SWAG_CHECK(getImmediateB(vb));
                    *addr = vb.reg.u8;
                    setStackValue(addr, 1, vb.kind);
                }
                break;
            case ByteCodeOp::SetAtPointer16:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->c.u32, 2));
                    SWAG_CHECK(getImmediateB(vb));
                    *reinterpret_cast<uint16_t*>(addr) = vb.reg.u16;
                    setStackValue(addr, 2, vb.kind);
                }
                break;
            case ByteCodeOp::SetAtPointer32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->c.u32, 4));
                    SWAG_CHECK(getImmediateB(vb));
                    *reinterpret_cast<uint32_t*>(addr) = vb.reg.u32;
                    setStackValue(addr, 4, vb.kind);
                }
                break;
            case ByteCodeOp::SetAtPointer64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                if (ra->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u64 + ip->c.u32, 8));
                    SWAG_CHECK(getImmediateB(vb));
                    *reinterpret_cast<uint64_t*>(addr) = vb.reg.u64;
                    setStackValue(addr, 8, vb.kind);
                }
                break;

            case ByteCodeOp::SetImmediate32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->setConstant(ip->b.u32);
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::SetImmediate64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->kind = SanityValueKind::Constant;
                ra->setConstant(ip->b.u64);
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::ClearRA:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->setConstant(0LL);
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::Add64byVB64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.s64 += ip->b.s64;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::Mul64byVB64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.s64 *= ip->b.s64;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::Div64byVB64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->reg.s64 /= ip->b.s64;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::MulAddVC64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isUnknown())
                    ra->setUnknown();
                else
                    ra->reg.u64 = ra->reg.u64 * (rb->reg.u64 + ip->c.u32);
                SanityValue::computeIp(ip, ra, rb);
                break;

            case ByteCodeOp::GetFromStack8:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->b.u32, 1));
                SWAG_CHECK(getStackValue(ra, addr, 1));
                SWAG_CHECK(checkStackInitialized(addr, 1, ra));
                ra->reg.u64 = *addr;
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::GetFromStack16:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->b.u32, 2));
                SWAG_CHECK(getStackValue(ra, addr, 2));
                SWAG_CHECK(checkStackInitialized(addr, 2, ra));
                ra->reg.u64 = *reinterpret_cast<uint16_t*>(addr);
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::GetFromStack32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->b.u32, 4));
                SWAG_CHECK(getStackValue(ra, addr, 4));
                SWAG_CHECK(checkStackInitialized(addr, 4, ra));
                ra->reg.u64 = *reinterpret_cast<uint32_t*>(addr);
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::GetFromStack64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getStackAddress(addr, nullptr, ip->b.u32, 8));
                SWAG_CHECK(getStackValue(ra, addr, 8));
                SWAG_CHECK(checkStackInitialized(addr, 8, ra));
                ra->reg.u64 = *reinterpret_cast<uint64_t*>(addr);
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::IncPointer64:
            {
                SWAG_CHECK(getRegister(ra, ip->a.u32));

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
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (ra->isConstant() && vb.isConstant())
                    rc->setConstant(ra->reg.u64 + vb.reg.s64);
                else if (ra->isStackAddr() && vb.isConstant())
                    rc->setStackAddr(ra->reg.u64 + vb.reg.s64);
                else
                    rc->setUnknown();
                SanityValue::computeIp(ip, ra, &vb, rc);
            }
            break;
            case ByteCodeOp::DecPointer64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (ra->isConstant() && vb.isConstant())
                    rc->setConstant(ra->reg.u64 - vb.reg.s64);
                else if (ra->isStackAddr() && vb.isConstant())
                    rc->setStackAddr(ra->reg.u64 - vb.reg.s64);
                else
                    rc->setUnknown();
                SanityValue::computeIp(ip, ra, &vb, rc);
                break;
            case ByteCodeOp::IncMulPointer64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (ra->isConstant() && vb.isConstant())
                    rc->setConstant(ra->reg.u64 + vb.reg.s64 * ip->d.u64);
                else if (ra->isStackAddr() && vb.isConstant())
                    rc->setStackAddr(ra->reg.u64 + vb.reg.s64 * ip->d.u64);
                else
                    rc->setUnknown();
                SanityValue::computeIp(ip, ra, &vb, rc);
                break;

            case ByteCodeOp::CopyRAtoRR:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNullReturn(ip->a.u32));
                if (!ip->hasFlag(BCI_IMM_A))
                {
                    if (ra->isStackAddr())
                        return checkEscapeFrame(ra);
                }
                break;

            case ByteCodeOp::CopyRARBtoRR2:
                if (!ip->hasFlag(BCI_IMM_A))
                {
                    SWAG_CHECK(getRegister(ra, ip->a.u32));
#if 0
                    SWAG_CHECK(checkNotNullReturn(ip->a.u32));
#endif
                    if (ra->isStackAddr())
                    {
                        // Legit in #run block, as we will make a copy
                        if (context.bc->node && context.bc->node->hasAstFlag(AST_IN_RUN_BLOCK))
                            break;
                        return checkEscapeFrame(ra);
                    }
                }
                break;

            case ByteCodeOp::DeRef8:
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(rb));
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                if (rb->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, rb, rb->reg.u64 + ip->c.s64, 1));
                    SWAG_CHECK(checkStackInitialized(addr, 1));
                    SWAG_CHECK(getStackValue(ra, addr, 1));
                    ra->reg.u64 = *addr;
                }
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::DeRef16:
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(rb));
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                if (rb->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, rb, rb->reg.u64 + ip->c.s64, 2));
                    SWAG_CHECK(checkStackInitialized(addr, 2));
                    SWAG_CHECK(getStackValue(ra, addr, 2));
                    ra->reg.u64 = *reinterpret_cast<uint16_t*>(addr);
                }
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::DeRef32:
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(rb));
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                if (rb->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, rb, rb->reg.u64 + ip->c.s64, 4));
                    SWAG_CHECK(checkStackInitialized(addr, 4));
                    SWAG_CHECK(getStackValue(ra, addr, 4));
                    ra->reg.u64 = *reinterpret_cast<uint32_t*>(addr);
                }
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::DeRef64:
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNull(rb));
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                if (rb->isStackAddr())
                {
                    SWAG_CHECK(getStackAddress(addr, rb, rb->reg.u64 + ip->c.s64, 8));
                    SWAG_CHECK(checkStackInitialized(addr, 8));
                    SWAG_CHECK(getStackValue(ra, addr, 8));
                    ra->reg.u64 = *reinterpret_cast<uint64_t*>(addr);
                }
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;

            case ByteCodeOp::NegBool:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(rb->reg.b ? 0LL : 1LL);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::NegS32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(-vb.reg.s32);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::NegS64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(-rb->reg.s64);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::NegF32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(-rb->reg.f32);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::NegF64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (vb.isConstant())
                    ra->setConstant(-rb->reg.f64);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;

            case ByteCodeOp::CastBool8:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(vb.reg.u8 ? 1LL : 0);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastBool16:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(vb.reg.u16 ? 1LL : 0);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastBool32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(vb.reg.u32 ? 1LL : 0);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastBool64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(vb.reg.u64 ? 1LL : 0);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8S16:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int16_t>(vb.reg.s8));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8S32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int32_t>(vb.reg.s8));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS16S32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int32_t>(vb.reg.s16));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastF32S32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int32_t>(vb.reg.f32));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8S64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int64_t>(vb.reg.s8));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS16S64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int64_t>(vb.reg.s16));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS32S64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int64_t>(vb.reg.s32));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastF64S64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<int64_t>(vb.reg.f64));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.s8));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS16F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.s16));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS32F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.s32));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS64F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.s64));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU8F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.u8));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU16F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.u16));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU32F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.u32));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU64F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.u64));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;

            case ByteCodeOp::CastS8F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.s8));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS16F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.s16));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS32F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.s32));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastS64F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.s64));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU8F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.u8));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU16F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.u16));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU32F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.u32));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastU64F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.u64));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;
            case ByteCodeOp::CastF32F64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<double>(vb.reg.f32));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
                break;

            case ByteCodeOp::CastF64F32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                if (vb.isConstant())
                    ra->setConstant(static_cast<float>(vb.reg.f64));
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
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

            case ByteCodeOp::ClearMaskU32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                if (ra->isConstant())
                    ra->setConstant(ra->reg.u32 & ip->b.u32);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra);
                break;
            case ByteCodeOp::ClearMaskU64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                if (ra->isConstant())
                    ra->setConstant(ra->reg.u64 & ip->b.u64);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::InvertU8:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(~rb->reg.u8);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::InvertU16:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(~rb->reg.u16);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::InvertU32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(~rb->reg.u32);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;
            case ByteCodeOp::InvertU64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                if (rb->isConstant())
                    ra->setConstant(~rb->reg.u64);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, rb);
                break;

            case ByteCodeOp::IntrinsicStrCmp:
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                SWAG_CHECK(checkNotNullArguments({ip->c.u32, ip->b.u32}, "@strcmp"));
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::IntrinsicStrLen:
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(checkNotNullArguments({ip->b.u32}, "@strlen"));
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::computeIp(ip, ra);
                break;

            case ByteCodeOp::IntrinsicMemCmp:
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(getRegister(rc, ip->c.u32));
                SWAG_CHECK(checkNotNullArguments({ip->c.u32, ip->b.u32}, "@memcmp"));
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                ra->setUnknown();
                SanityValue::computeIp(ip, ra);
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
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(getImmediateC(vc));
                SWAG_CHECK(checkNotNullArguments({ip->b.u32, ip->a.u32}, "@memcpy"));
                if (ra->isStackAddr() && rb->isStackAddr() && vc.isConstant())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, vc.reg.u32));
                    SWAG_CHECK(getStackAddress(addr2, rb, rb->reg.u32, vc.reg.u32));
                    SWAG_CHECK(checkStackInitialized(addr2, vc.reg.u32, rb));
                    SWAG_CHECK(getStackValue(&va, addr2, vc.reg.u32));
                    setStackValue(addr, vc.reg.u32, va.kind);
                    std::copy_n(addr2, vc.reg.u32, addr);
                }
                else
                    invalidateCurStateStack();
                break;

            case ByteCodeOp::IntrinsicMemMove:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getRegister(rb, ip->b.u32));
                SWAG_CHECK(getImmediateC(vc));
                SWAG_CHECK(checkNotNullArguments({ip->b.u32, ip->a.u32}, "@memmove"));
                if (ra->isStackAddr() && rb->isStackAddr() && vc.isConstant())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, vc.reg.u32));
                    SWAG_CHECK(getStackAddress(addr2, rb, rb->reg.u32, vc.reg.u32));
                    SWAG_CHECK(checkStackInitialized(addr2, vc.reg.u32, rb));
                    SWAG_CHECK(getStackValue(&va, addr2, vc.reg.u32));
                    setStackValue(addr, vc.reg.u32, va.kind);
                    memmove(addr, addr2, vc.reg.u32);
                }
                else
                    invalidateCurStateStack();
                break;
            case ByteCodeOp::IntrinsicMemSet:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getImmediateC(vc));
                SWAG_CHECK(checkNotNullArguments({ip->a.u32}, "@memset"));
                if (ra->isStackAddr() && vb.isConstant() && vc.isConstant())
                {
                    SWAG_CHECK(getStackAddress(addr, ra, ra->reg.u32, vc.reg.u32));
                    setStackValue(addr, vc.reg.u32, SanityValueKind::Constant);
                    memset(addr, vb.reg.u8, vc.reg.u32);
                }
                else
                    invalidateCurStateStack();
                break;

            case ByteCodeOp::LambdaCall:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(checkNotNull(ra));
                SWAG_CHECK(checkNotNullArguments(pushParams, ""));
                invalidateCurStateStack();
                pushParams.clear();
                break;

            case ByteCodeOp::LocalCall:
            case ByteCodeOp::ForeignCall:
                SWAG_CHECK(checkNotNullArguments(pushParams, ""));
                invalidateCurStateStack();
                pushParams.clear();
                break;

            case ByteCodeOp::IntrinsicMulAddF32:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getImmediateC(vc));
                SWAG_CHECK(getImmediateD(vd));
                if (vb.isConstant() && vc.isConstant() && vd.isConstant())
                    ra->setConstant(vb.reg.f32 * vc.reg.f32 + vd.reg.f32);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb, &vc, &vd);
                break;
            case ByteCodeOp::IntrinsicMulAddF64:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getImmediateC(vc));
                SWAG_CHECK(getImmediateD(vd));
                if (vb.isConstant() && vc.isConstant() && vd.isConstant())
                    ra->setConstant(vb.reg.f64 * vc.reg.f64 + vd.reg.f64);
                else
                    ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb, &vc, &vd);
                break;

            case ByteCodeOp::IntrinsicS8x1:
            case ByteCodeOp::IntrinsicS16x1:
            case ByteCodeOp::IntrinsicS32x1:
            case ByteCodeOp::IntrinsicS64x1:
            case ByteCodeOp::IntrinsicF32x1:
            case ByteCodeOp::IntrinsicF64x1:
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb);
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
                SWAG_CHECK(getRegister(ra, ip->a.u32));
                SWAG_CHECK(getImmediateB(vb));
                SWAG_CHECK(getImmediateC(vc));
                ra->setUnknown();
                SanityValue::computeIp(ip, ra, &vb, &vc);
                if (vb.isConstant() && vc.isConstant())
                {
                    ra->kind = SanityValueKind::Constant;
                    SWAG_CHECK(ByteCodeRun::executeMathIntrinsic(&context, ip, ra->reg, vb.reg, vc.reg, {}));
                }
                break;

            case ByteCodeOp::Nop:
                break;

            default:
                Report::internalError(context.bc->sourceFile->module, form("unknown instruction [[%s]] during sanity check", g_ByteCodeOpDesc[static_cast<int>(ip->op)].name));
                return false;
        }

        ip->dynFlags.add(BCID_SAN_PASS);
        ip++;
    }

    return true;
}
