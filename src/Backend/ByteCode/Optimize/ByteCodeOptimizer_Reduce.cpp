#include "pch.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"

void ByteCodeOptimizer::reduceAffectOp(ByteCodeOptContext*, ByteCodeInstruction* ip)
{
    if (ip->op != ByteCodeOp::IncPointer64 || ip[1].a.u32 != ip->a.u32)
        return;
    if (!ip->flags.has(BCI_IMM_B) || ip[0].a.u32 != ip[0].c.u32)
        return;

    switch (ip[1].op)
    {
        case ByteCodeOp::AffectOpMulEqS8_Safe:
        case ByteCodeOp::AffectOpMulEqS16_Safe:
        case ByteCodeOp::AffectOpMulEqS32_Safe:
        case ByteCodeOp::AffectOpMulEqS64_Safe:
        case ByteCodeOp::AffectOpMulEqU8_Safe:
        case ByteCodeOp::AffectOpMulEqU16_Safe:
        case ByteCodeOp::AffectOpMulEqU32_Safe:
        case ByteCodeOp::AffectOpMulEqU64_Safe:
        case ByteCodeOp::AffectOpMulEqF32_Safe:
        case ByteCodeOp::AffectOpMulEqF64_Safe:

        case ByteCodeOp::AffectOpMinusEqS8_Safe:
        case ByteCodeOp::AffectOpMinusEqS16_Safe:
        case ByteCodeOp::AffectOpMinusEqS32_Safe:
        case ByteCodeOp::AffectOpMinusEqS64_Safe:
        case ByteCodeOp::AffectOpMinusEqU8_Safe:
        case ByteCodeOp::AffectOpMinusEqU16_Safe:
        case ByteCodeOp::AffectOpMinusEqU32_Safe:
        case ByteCodeOp::AffectOpMinusEqU64_Safe:
        case ByteCodeOp::AffectOpMinusEqF32_Safe:
        case ByteCodeOp::AffectOpMinusEqF64_Safe:

        case ByteCodeOp::AffectOpPlusEqS8_Safe:
        case ByteCodeOp::AffectOpPlusEqS16_Safe:
        case ByteCodeOp::AffectOpPlusEqS32_Safe:
        case ByteCodeOp::AffectOpPlusEqS64_Safe:
        case ByteCodeOp::AffectOpPlusEqU8_Safe:
        case ByteCodeOp::AffectOpPlusEqU16_Safe:
        case ByteCodeOp::AffectOpPlusEqU32_Safe:
        case ByteCodeOp::AffectOpPlusEqU64_Safe:
        case ByteCodeOp::AffectOpPlusEqF32_Safe:
        case ByteCodeOp::AffectOpPlusEqF64_Safe:

            ip[1].c.u32 += ip[0].b.u32;
            ByteCode::swapInstructions(ip, ip + 1);
            break;
    }
}

void ByteCodeOptimizer::reduceMath(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->hasFlag(BCI_IMM_A))
        return;
    if (!ip->hasFlag(BCI_IMM_B))
        return;

    switch (ip->op)
    {
        case ByteCodeOp::BinOpDivS8:
            if (ip->b.u8 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u8))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftRightS8);
                ip->b.u32 = static_cast<uint32_t>(log2(ip->b.u8));
                ip->addFlag(BCI_CAN_OVERFLOW);
                break;
            }
            break;
        case ByteCodeOp::BinOpDivS16:
            if (ip->b.u16 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u16))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftRightS16);
                ip->b.u32 = static_cast<uint32_t>(log2(ip->b.u16));
                ip->addFlag(BCI_CAN_OVERFLOW);
                break;
            }
            break;
        case ByteCodeOp::BinOpDivS32:
            if (ip->b.u32 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u32))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftRightS32);
                ip->b.u32 = static_cast<uint32_t>(log2(ip->b.u32));
                ip->addFlag(BCI_CAN_OVERFLOW);
                break;
            }
            break;
        case ByteCodeOp::BinOpDivS64:
            if (ip->b.u64 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u64))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftRightS64);
                ip->b.u64 = static_cast<uint64_t>(log2(ip->b.u64));
                ip->addFlag(BCI_CAN_OVERFLOW);
                break;
            }
            break;
        case ByteCodeOp::BinOpDivU8:
            if (ip->b.u8 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u8))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftRightU8);
                ip->b.u32 = static_cast<uint32_t>(log2(ip->b.u8));
                ip->addFlag(BCI_CAN_OVERFLOW);
                break;
            }
            break;
        case ByteCodeOp::BinOpDivU16:
            if (ip->b.u16 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u16))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftRightU16);
                ip->b.u32 = static_cast<uint32_t>(log2(ip->b.u16));
                ip->addFlag(BCI_CAN_OVERFLOW);
                break;
            }
            break;
        case ByteCodeOp::BinOpDivU32:
            if (ip->b.u32 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u32))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftRightU32);
                ip->b.u32 = static_cast<uint32_t>(log2(ip->b.u32));
                ip->addFlag(BCI_CAN_OVERFLOW);
                break;
            }
            break;
        case ByteCodeOp::BinOpDivU64:
            if (ip->b.u64 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u64))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftRightU64);
                ip->b.u64 = static_cast<uint64_t>(log2(ip->b.u64));
                ip->addFlag(BCI_CAN_OVERFLOW);
                break;
            }
            break;

        case ByteCodeOp::BinOpModuloU32:
            if (Math::isPowerOfTwo(ip->b.u32))
            {
                SET_OP(ip, ByteCodeOp::BinOpBitmaskAnd32);
                ip->b.u32 -= 1;
                break;
            }
            break;

        case ByteCodeOp::BinOpModuloU64:
            if (Math::isPowerOfTwo(ip->b.u32))
            {
                SET_OP(ip, ByteCodeOp::BinOpBitmaskAnd64);
                ip->b.u64 -= 1;
                break;
            }
            break;

        case ByteCodeOp::BinOpMulU32_Safe:
            if (ip->b.u32 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u32))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftLeftU32);
                ip->b.u32 = static_cast<uint32_t>(log2(ip->b.u32));
                break;
            }
            break;

        case ByteCodeOp::BinOpMulU64_Safe:
            if (ip->b.u64 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u32))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftLeftU64);
                ip->b.u64 = static_cast<uint32_t>(log2(ip->b.u64));
                break;
            }
            break;

        case ByteCodeOp::BinOpMulS32_Safe:
            if (ip->b.u32 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u32))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftLeftS32);
                ip->b.u32 = static_cast<uint32_t>(log2(ip->b.u32));
                break;
            }
            break;

        case ByteCodeOp::BinOpMulS64_Safe:
            if (ip->b.u64 == 1)
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                ip->b.u32 = ip->a.u32;
                ip->a.u32 = ip->c.u32;
                ip->removeFlag(BCI_IMM_B);
                break;
            }

            if (Math::isPowerOfTwo(ip->b.u32))
            {
                SET_OP(ip, ByteCodeOp::BinOpShiftLeftS64);
                ip->b.u64 = static_cast<uint32_t>(log2(ip->b.u64));
                break;
            }
            break;
    }
}

void ByteCodeOptimizer::reduceFactor(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip[0].op)
    {
        case ByteCodeOp::CopyRBtoRA64:
            if (ip[1].op == ByteCodeOp::Mul64byVB64 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip[0].a.u32 == ip[1].a.u32)
            {
                setNop(context, ip);
                SET_OP(ip + 1, ByteCodeOp::BinOpMulU64_Safe);
                ip[1].a.u32 = ip[0].b.u32;
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
                ip[2].op == ByteCodeOp::CopyRBtoRA64)
            {
                if (ip[0].a.u32 == ip[2].b.u32 && ip[1].a.u32 != ip[0].a.u32)
                {
                    ip[2].b.u32 = ip[0].b.u32;
                    context->setDirtyPass();
                    break;
                }
            }

            if (ip[1].op == ByteCodeOp::DecrementRA64 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].op    = ByteCodeOp::BinOpMinusU64_Safe;
                ip[0].c.u32 = ip[0].a.u32;
                ip[0].a.u32 = ip[0].b.u32;
                ip[0].b.u32 = 1;
                ip[0].addFlag(BCI_IMM_B | BCI_CANT_OVERFLOW);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::IncrementRA64 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].op    = ByteCodeOp::BinOpPlusU64_Safe;
                ip[0].c.u32 = ip[0].a.u32;
                ip[0].a.u32 = ip[0].b.u32;
                ip[0].b.u32 = 1;
                ip[0].addFlag(BCI_IMM_B | BCI_CANT_OVERFLOW);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::Add64byVB64 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].op    = ByteCodeOp::BinOpPlusS64_Safe;
                ip[0].c.u32 = ip[0].a.u32;
                ip[0].a.u32 = ip[0].b.u32;
                ip[0].b.u64 = ip[1].b.u64;
                ip[0].addFlag(BCI_IMM_B | BCI_CANT_OVERFLOW);
                setNop(context, ip + 1);
                break;
            }

            break;
    }
}

void ByteCodeOptimizer::reduceErr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip[0].op)
    {
        case ByteCodeOp::InternalHasErr:
            // Has err followed by ret
            if (ByteCode::isRet(ip + 1))
            {
                setNop(context, ip);
                break;
            }

            // GetErr/Jump just after
            if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                ip[0].hasFlag(BCI_TRY_CATCH) &&
                ip[1].hasFlag(BCI_TRY_CATCH) &&
                ip[1].b.s32 == 0)
            {
                setNop(context, ip);
                setNop(context, ip + 1);
                break;
            }

            // Useless InternalHasErr
            if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
                ip[2].op == ByteCodeOp::MakeConstantSegPointer &&
                ip[3].op == ByteCodeOp::InternalStackTrace &&
                ip[4].op == ByteCodeOp::InternalHasErr &&
                ip[5].op == ByteCodeOp::JumpIfZero32 &&
                !ip[4].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 4);
                setNop(context, ip + 5);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
                ip[2].op == ByteCodeOp::InternalStackTraceConst &&
                ip[3].op == ByteCodeOp::InternalHasErr &&
                ip[4].op == ByteCodeOp::JumpIfZero32 &&
                !ip[3].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 3);
                setNop(context, ip + 4);
                break;
            }

            // Useless InternalHasErr
            if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
                ip[2].op == ByteCodeOp::InternalHasErr &&
                ip[3].op == ByteCodeOp::JumpIfZero32 &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 2);
                setNop(context, ip + 3);
                break;
            }

            // GetErr/Jump on another GetErr/Jump, make a shortcut
            if (ip[1].op == ByteCodeOp::JumpIfZero32)
            {
                const auto ipNext = &ip[1] + ip[1].b.s32 + 1;
                if (ipNext[0].op == ByteCodeOp::InternalHasErr && ipNext[1].op == ByteCodeOp::JumpIfZero32)
                {
                    ip[1].b.s32 += ipNext[1].b.s32 + 2;
                    context->setDirtyPass();
                    break;
                }
            }

            // InternalHasErr followed by return
            if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
                ip[2].op == ByteCodeOp::Ret &&
                ip[1].b.s32 == 1 &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                context->bc->out[context->bc->numInstructions - 2].op == ByteCodeOp::Ret)
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
                ip[1].b.s32 = static_cast<int32_t>(&context->bc->out[context->bc->numInstructions - 2] - (ip + 1) - 1);
                setNop(context, ip + 2);
                break;
            }

            // Compact error test
            if (ip[1].op == ByteCodeOp::JumpIfZero32 &&
                ip[0].a.u32 == ip[1].a.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNoError);
                ip[1].a.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::InternalPushErr:
            // Useless pop/push
            if (ip[1].op == ByteCodeOp::InternalPopErr)
            {
                setNop(context, ip);
                setNop(context, ip + 1);
                break;
            }

            // If there's not SetErr between push and pop, then remove them
            {
                auto                 ipScan    = ip + 1;
                auto                 cpt       = 1;
                bool                 hasSetErr = false;
                ByteCodeInstruction* ipEnd     = nullptr;
                while (cpt)
                {
                    if (ipScan[0].op == ByteCodeOp::InternalPushErr)
                        cpt++;
                    if (ipScan[0].op == ByteCodeOp::InternalPopErr)
                    {
                        cpt--;
                        if (!cpt)
                            ipEnd = ipScan;
                    }

                    if (ipScan[0].op == ByteCodeOp::End ||
                        ipScan[0].op == ByteCodeOp::InternalSetErr ||
                        ByteCode::isCall(ipScan))
                    {
                        hasSetErr = true;
                        break;
                    }

                    ipScan++;
                }

                if (!hasSetErr)
                {
                    setNop(context, ip);
                    setNop(context, ipEnd);
                }
            }

            break;

        case ByteCodeOp::MakeConstantSegPointer:
            // Duplicated stack trace
            if (ip[1].op == ByteCodeOp::InternalStackTrace &&
                ip[2].op == ByteCodeOp::MakeConstantSegPointer &&
                ip[3].op == ByteCodeOp::InternalStackTrace &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 2);
                setNop(context, ip + 3);
                break;
            }

            break;

        case ByteCodeOp::InternalStackTrace:
            if (ip[1].op == ByteCodeOp::InternalCatchErr &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::InternalStackTraceConst:
            if (ip[1].op == ByteCodeOp::InternalCatchErr &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::InternalStackTraceConst &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::InternalInitStackTrace:
            if (ip[1].op == ByteCodeOp::InternalInitStackTrace &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DecSPBP &&
                ip[2].op == ByteCodeOp::InternalInitStackTrace &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 2);
                break;
            }

            break;
    }
}

void ByteCodeOptimizer::reduceCallEmptyFct(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ByteCode::isLocalCall(ip))
    {
        const auto destBC = reinterpret_cast<ByteCode*>(ip->a.pointer);
        if (destBC->isEmpty.load() != true)
            return;

        // We eliminate the call to the function that does nothing
        setNop(context, ip);
        if (ip[1].op == ByteCodeOp::IncSPPostCallCond)
            setNop(context, ip + 1);
        if (ip[1].op == ByteCodeOp::IncSPPostCall)
            setNop(context, ip + 1);
        if (ip[2].op == ByteCodeOp::IncSPPostCall)
            setNop(context, ip + 2);

        // Then we can eliminate some instructions related to the function call parameters
        auto backIp = ip;
        if (!backIp->hasFlag(BCI_START_STMT))
        {
            while (backIp != context->bc->out && !ByteCode::isCall(backIp))
            {
                if (backIp->op == ByteCodeOp::PushRVParam ||
                    backIp->op == ByteCodeOp::PushRAParam ||
                    backIp->op == ByteCodeOp::PushRAParam2 ||
                    backIp->op == ByteCodeOp::PushRAParam3 ||
                    backIp->op == ByteCodeOp::PushRAParam4 ||
                    backIp->op == ByteCodeOp::CopySPVaargs ||
                    backIp->op == ByteCodeOp::CopySP)
                {
                    setNop(context, backIp);
                }

                if (backIp->hasFlag(BCI_START_STMT))
                    break;
                backIp--;
            }
        }
    }
}

void ByteCodeOptimizer::reduceMemcpy(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::IntrinsicMemCpy && ip->hasFlag(BCI_IMM_C))
    {
        switch (ip->c.u32)
        {
            case 1:
                SET_OP(ip, ByteCodeOp::MemCpy8);
                break;
            case 2:
                SET_OP(ip, ByteCodeOp::MemCpy16);
                break;
            case 4:
                SET_OP(ip, ByteCodeOp::MemCpy32);
                break;
            case 8:
                SET_OP(ip, ByteCodeOp::MemCpy64);
                break;

            default:
                break;
        }
    }

    else if (ip[0].op == ByteCodeOp::MakeStackPointer &&
             ip[1].op == ByteCodeOp::MakeStackPointer &&
             ip[2].a.u32 == ip[1].a.u32 &&
             ip[2].b.u32 == ip[0].a.u32 &&
             !ip[1].hasFlag(BCI_START_STMT) &&
             !ip[2].hasFlag(BCI_START_STMT))
    {
        switch (ip[2].op)
        {
            case ByteCodeOp::MemCpy8:
                SET_OP(ip + 2, ByteCodeOp::CopyStack8);
                ip[2].a.u32 = ip[1].b.u32;
                ip[2].b.u32 = ip[0].b.u32;
                break;

            case ByteCodeOp::MemCpy16:
                SET_OP(ip + 2, ByteCodeOp::CopyStack16);
                ip[2].a.u32 = ip[1].b.u32;
                ip[2].b.u32 = ip[0].b.u32;
                break;

            case ByteCodeOp::MemCpy32:
                SET_OP(ip + 2, ByteCodeOp::CopyStack32);
                ip[2].a.u32 = ip[1].b.u32;
                ip[2].b.u32 = ip[0].b.u32;
                break;

            case ByteCodeOp::MemCpy64:
                SET_OP(ip + 2, ByteCodeOp::CopyStack64);
                ip[2].a.u32 = ip[1].b.u32;
                ip[2].b.u32 = ip[0].b.u32;
                break;

            default:
                break;
        }
    }

    else if (ip[0].op == ByteCodeOp::MakeStackPointer &&
             ip[1].op == ByteCodeOp::MakeStackPointer &&
             ip[2].a.u32 == ip[0].a.u32 &&
             ip[2].b.u32 == ip[1].a.u32 &&
             !ip[1].hasFlag(BCI_START_STMT) &&
             !ip[2].hasFlag(BCI_START_STMT))
    {
        switch (ip[2].op)
        {
            case ByteCodeOp::MemCpy8:
                SET_OP(ip + 2, ByteCodeOp::CopyStack8);
                ip[2].a.u32 = ip[0].b.u32;
                ip[2].b.u32 = ip[1].b.u32;
                break;

            case ByteCodeOp::MemCpy16:
                SET_OP(ip + 2, ByteCodeOp::CopyStack16);
                ip[2].a.u32 = ip[0].b.u32;
                ip[2].b.u32 = ip[1].b.u32;
                break;

            case ByteCodeOp::MemCpy32:
                SET_OP(ip + 2, ByteCodeOp::CopyStack32);
                ip[2].a.u32 = ip[0].b.u32;
                ip[2].b.u32 = ip[1].b.u32;
                break;

            case ByteCodeOp::MemCpy64:
                SET_OP(ip + 2, ByteCodeOp::CopyStack64);
                ip[2].a.u32 = ip[0].b.u32;
                ip[2].b.u32 = ip[1].b.u32;
                break;

            default:
                break;
        }
    }

    // Copy a constant value (from segment) to the stack
    else if (ip->op == ByteCodeOp::MakeConstantSegPointer &&
             ip[1].op == ByteCodeOp::MakeStackPointer &&
             ByteCode::isMemCpy(ip + 2) &&
             ip->a.u32 == ip[2].b.u32 &&
             !ip[1].hasFlag(BCI_START_STMT) &&
             ip[1].a.u32 == ip[2].a.u32)
    {
        switch (ip[2].op)
        {
            case ByteCodeOp::MemCpy8:
            {
                const auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
                SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer8);
                ip[2].a.u64 = ip[1].b.u64;
                ip[2].b.u64 = *reinterpret_cast<uint8_t*>(ptr);
                ip[2].addFlag(BCI_IMM_B);
                break;
            }
            case ByteCodeOp::MemCpy16:
            {
                const auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
                SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer16);
                ip[2].a.u64 = ip[1].b.u64;
                ip[2].b.u64 = *reinterpret_cast<uint16_t*>(ptr);
                ip[2].addFlag(BCI_IMM_B);
                break;
            }
            case ByteCodeOp::MemCpy32:
            {
                const auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
                SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer32);
                ip[2].a.u64 = ip[1].b.u64;
                ip[2].b.u64 = *reinterpret_cast<uint32_t*>(ptr);
                ip[2].addFlag(BCI_IMM_B);
                break;
            }
            case ByteCodeOp::MemCpy64:
            {
                const auto ptr = context->bc->sourceFile->module->constantSegment.address(ip->b.u32);
                SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer64);
                ip[2].a.u64 = ip[1].b.u64;
                ip[2].b.u64 = *reinterpret_cast<uint64_t*>(ptr);
                ip[2].addFlag(BCI_IMM_B);
                break;
            }

            default:
                break;
        }
    }
}

void ByteCodeOptimizer::reduceAppend(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    const auto opFlags = ByteCode::opFlags(ip->op);

    // Multiple InternalGetTlsPtr, just copy registers, as InternalGetTlsPtr has a function call cost
    if (ip[0].op == ByteCodeOp::InternalGetTlsPtr &&
        ip[1].op == ByteCodeOp::InternalGetTlsPtr &&
        ip[2].op != ByteCodeOp::InternalGetTlsPtr &&
        ip[2].op != ByteCodeOp::IncPointer64 &&
        !ip[1].hasFlag(BCI_START_STMT))
    {
        SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA64);
        ip[1].b.u32 = ip[0].a.u32;
    }

    switch (ip->op)
    {
        case ByteCodeOp::GetIncParam64DeRef8:
        case ByteCodeOp::GetIncFromStack64DeRef8:
            if (ip[1].op == ip[0].op &&
                ip[0].b.u64 == ip[1].b.u64 &&
                ip[0].c.u64 == ip[1].c.u64 &&
                ip[0].d.u64 == ip[1].d.u64 &&
                ip[0].flags == ip[1].flags &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA8);
                ip[1].b.u32 = ip[0].a.u32;
            }

            break;
        case ByteCodeOp::GetIncParam64DeRef16:
        case ByteCodeOp::GetIncFromStack64DeRef16:
            if (ip[1].op == ip[0].op &&
                ip[0].b.u64 == ip[1].b.u64 &&
                ip[0].c.u64 == ip[1].c.u64 &&
                ip[0].d.u64 == ip[1].d.u64 &&
                ip[0].flags == ip[1].flags &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA16);
                ip[1].b.u32 = ip[0].a.u32;
            }

            break;
        case ByteCodeOp::GetIncParam64DeRef32:
        case ByteCodeOp::GetIncFromStack64DeRef32:
            if (ip[1].op == ip[0].op &&
                ip[0].b.u64 == ip[1].b.u64 &&
                ip[0].c.u64 == ip[1].c.u64 &&
                ip[0].d.u64 == ip[1].d.u64 &&
                ip[0].flags == ip[1].flags &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA32);
                ip[1].b.u32 = ip[0].a.u32;
            }

            break;
        case ByteCodeOp::GetIncParam64:
        case ByteCodeOp::GetIncParam64DeRef64:
        case ByteCodeOp::GetIncFromStack64DeRef64:
            if (ip[1].op == ip[0].op &&
                ip[0].b.u64 == ip[1].b.u64 &&
                ip[0].c.u64 == ip[1].c.u64 &&
                ip[0].d.u64 == ip[1].d.u64 &&
                ip[0].flags == ip[1].flags &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA64);
                ip[1].b.u32 = ip[0].a.u32;
            }

            break;
    }

    // A = something followed by B = A
    // make B = something, this gives the opportunity to remove one of them
    if (ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
        opFlags.has(OPF_WRITE_A) &&
        !opFlags.has(OPF_READ_A) &&
        (!opFlags.has(OPF_READ_B) || ip->hasFlag(BCI_IMM_B) || ip->b.u32 != ip->a.u32) &&
        (!opFlags.has(OPF_READ_C) || ip->hasFlag(BCI_IMM_C) || ip->c.u32 != ip->a.u32) &&
        (!opFlags.has(OPF_READ_D) || ip->hasFlag(BCI_IMM_D) || ip->d.u32 != ip->a.u32) &&
        ip->a.u32 == ip[1].b.u32 &&
        !ip[0].hasFlag(BCI_IMM_A) &&
        !ip[1].hasFlag(BCI_START_STMT))
    {
        // List can be extended, as long as the instruction does not have side effects when called twice
        switch (ip->op)
        {
            case ByteCodeOp::CopyRBtoRA64:
            case ByteCodeOp::CopyRTtoRA:
            case ByteCodeOp::MakeStackPointer:
            case ByteCodeOp::GetFromStack8:
            case ByteCodeOp::GetFromStack16:
            case ByteCodeOp::GetFromStack32:
            case ByteCodeOp::GetFromStack64:
            case ByteCodeOp::GetParam64:
            case ByteCodeOp::NegBool:
            case ByteCodeOp::CastBool8:
            case ByteCodeOp::CastBool16:
            case ByteCodeOp::CastBool32:
            case ByteCodeOp::CastBool64:
            case ByteCodeOp::MakeConstantSegPointer:
            case ByteCodeOp::MakeMutableSegPointer:
            case ByteCodeOp::MakeBssSegPointer:
            case ByteCodeOp::MakeCompilerSegPointer:
            case ByteCodeOp::DeRef8:
            case ByteCodeOp::DeRef16:
            case ByteCodeOp::DeRef32:
            case ByteCodeOp::DeRef64:
            {
                const auto s = ip[1].a.u32;
                ip[1]        = ip[0];
                ip[1].a.u32  = s;
                context->setDirtyPass();
                break;
            }

            default:
                break;
        }
    }
}

void ByteCodeOptimizer::reduceFunc(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::DecSPBP:
            if (ip[1].op == ByteCodeOp::CopyRAtoRRRet &&
                ip[2].op == ByteCodeOp::End)
            {
                setNop(context, ip);
                context->bc->stackSize    = 0;
                context->bc->dynStackSize = 0;
                break;
            }
            break;

        case ByteCodeOp::CopyRAtoRRRet:
            if (ip[1].op == ByteCodeOp::CopyRAtoRRRet &&
                ip[0].a.u64 == ip[1].a.u64 &&
                ip[0].hasFlag(BCI_IMM_A) == ip[1].hasFlag(BCI_IMM_A))
            {
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::CopyRAtoRR:
            if (ip[1].op == ByteCodeOp::Ret)
            {
                SET_OP(ip, ByteCodeOp::CopyRAtoRRRet);
                if (!ip[1].hasFlag(BCI_START_STMT))
                    setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::CopyRTtoRA:
            // Swap CopyRTtoRA and IncSPPostCall to put IncSPPostCall right next to the call, which
            // give the opportunity to optimize the call pop
            if (ip[1].op == ByteCodeOp::IncSPPostCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                std::swap(ip[0], ip[1]);
                context->setDirtyPass();
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfRTTrue);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfRTFalse);
                break;
            }

            break;

        case ByteCodeOp::LocalCallPop:
            if (ip[1].op == ByteCodeOp::CopyRTtoRA &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::LocalCallPopRC);
                ip->d.u32 = ip[1].a.u32;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::LocalCallPopRC:
            if (ip[1].op == ByteCodeOp::CopyRTtoRA &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA64);
                ip[1].b.u32 = ip->d.u32;
                break;
            }
            break;

        case ByteCodeOp::LocalCall:
            if (ip[1].op == ByteCodeOp::IncSPPostCall &&
                ip[1].a.u32 == 16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::LocalCallPop16);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::IncSPPostCall &&
                ip[1].a.u32 == 48 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::LocalCallPop48);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::IncSPPostCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::LocalCallPop);
                ip->c.u32 = ip[1].a.u32;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRT2toRARB &&
                ip[2].op == ByteCodeOp::IncSPPostCall &&
                ip[2].a.u32 == 16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::LocalCallPop16);
                setNop(context, ip + 2);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRT2toRARB &&
                ip[2].op == ByteCodeOp::IncSPPostCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::LocalCallPop);
                ip->c.u32 = ip[2].a.u32;
                setNop(context, ip + 2);
                break;
            }
            break;

        case ByteCodeOp::ForeignCall:
            if (ip[1].op == ByteCodeOp::IncSPPostCall &&
                ip[1].a.u32 == 16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::ForeignCallPop16);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::IncSPPostCall &&
                ip[1].a.u32 == 48 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::ForeignCallPop48);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::IncSPPostCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::ForeignCallPop);
                ip->c.u32 = ip[1].a.u32;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::LambdaCall:
            if (ip[1].op == ByteCodeOp::IncSPPostCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::LambdaCallPop);
                ip->c.u32 = ip[1].a.u32;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::PushRAParam:
            if (ip[1].op == ByteCodeOp::PushRAParam &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::PushRAParam2);
                ip->b.u32 = ip[1].a.u32;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::PushRAParam2 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::PushRAParam3);
                ip->b.u32 = ip[1].a.u32;
                ip->c.u32 = ip[1].b.u32;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::PushRAParam3 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::PushRAParam4);
                ip->b.u32 = ip[1].a.u32;
                ip->c.u32 = ip[1].b.u32;
                ip->d.u32 = ip[1].c.u32;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::LambdaCallPop &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LambdaCallPopParam);
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::LocalCallPop &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LocalCallPopParam);
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::ForeignCallPop &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ForeignCallPopParam);
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }

            break;

        case ByteCodeOp::PushRAParam2:
            if (ip[1].op == ByteCodeOp::PushRAParam &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::PushRAParam3);
                ip->c.u32 = ip[1].a.u32;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::PushRAParam2 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::PushRAParam4);
                ip->c.u32 = ip[1].a.u32;
                ip->d.u32 = ip[1].b.u32;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::LocalCallPop16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LocalCallPop16Param2);
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].d.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::LocalCallPop48 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LocalCallPop48Param2);
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].d.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::ForeignCallPop16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ForeignCallPop16Param2);
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].d.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::ForeignCallPop48 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ForeignCallPop48Param2);
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].d.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::PushRAParam3:
            if (ip[1].op == ByteCodeOp::PushRAParam &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::PushRAParam4);
                ip->d.u32 = ip[1].a.u32;
                setNop(context, ip + 1);
                break;
            }
            break;
    }
}

void ByteCodeOptimizer::reduceStack(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::DecSPBP:
            if (ip[1].op == ByteCodeOp::Ret &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                context->bc->stackSize    = 0;
                context->bc->dynStackSize = 0;
                break;
            }
            break;

        case ByteCodeOp::CopyStack8:
            if (ip->a.u32 == ip->b.u32)
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyStack8 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip[1].a.u32 == ip[0].a.u32 + 1 &&
                ip[1].b.u32 == ip[0].b.u32 + 1)
            {
                SET_OP(ip, ByteCodeOp::CopyStack16);
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::CopyStack16:
            if (ip->a.u32 == ip->b.u32)
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyStack16 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip[1].a.u32 == ip[0].a.u32 + 2 &&
                ip[1].b.u32 == ip[0].b.u32 + 2)
            {
                SET_OP(ip, ByteCodeOp::CopyStack32);
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::CopyStack32:
            if (ip->a.u32 == ip->b.u32)
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyStack32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip[1].a.u32 == ip[0].a.u32 + 4 &&
                ip[1].b.u32 == ip[0].b.u32 + 4)
            {
                SET_OP(ip, ByteCodeOp::CopyStack64);
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::CopyStack64:
            if (ip->a.u32 == ip->b.u32)
            {
                setNop(context, ip);
                break;
            }

            break;

        case ByteCodeOp::GetFromStack8:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
                ip[1].b.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyStack8);
                ip[1].b.u64 = ip[0].b.u64;
                break;
            }
            break;

        case ByteCodeOp::GetFromStack16:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
                ip[1].b.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyStack16);
                ip[1].b.u64 = ip[0].b.u64;
                break;
            }
            break;

        case ByteCodeOp::GetFromStack32:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
                ip[1].b.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyStack32);
                ip[1].b.u64 = ip[0].b.u64;
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].b.u32 == 0xFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack8);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].b.u32 == 0xFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack16);
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::GetIncFromStack64:
            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].c.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip->c.s64 += ip[1].b.s64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef8 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef8);
                ip->c.u64 += ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef16);
                ip->c.u64 += ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef32);
                ip->c.u64 += ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef64);
                ip->c.u64 += ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack64:
            if (ip[1].op == ByteCodeOp::DeRef8 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef8);
                ip->c.u64 = ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef16);
                ip->c.u64 = ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef32);
                ip->c.u64 = ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64DeRef64);
                ip->c.u64 = ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].c.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncFromStack64);
                ip->c.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncFromStack64);
                ip[1].a.u32 = ip[1].c.u32;
                ip[1].c.u64 = ip[1].b.u64;
                ip[1].b.u64 = ip[0].b.u64;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
                ip[1].b.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyStack64);
                ip[1].b.u64 = ip[0].b.u64;
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].b.u64 == 0xFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack8);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].b.u64 == 0xFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack16);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].b.u64 == 0xFFFFFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack32);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::MakeStackPointer:
            if (ip[1].op == ByteCodeOp::SetZeroAtPointer8 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetZeroStack8);
                ip[1].a.u32 = ip[1].b.u32 + ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointer16 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetZeroStack16);
                ip[1].a.u32 = ip[1].b.u32 + ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointer32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetZeroStack32);
                ip[1].a.u32 = ip[1].b.u32 + ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::Add64byVB64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u32 += ip[1].b.u32;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointer64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetZeroStack64);
                ip[1].a.u32 = ip[1].b.u32 + ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointerX &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetZeroStackX);
                ip[1].a.u32 = ip[1].c.u32 + ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRAtoRT &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::MakeStackPointerRT);
                ip[1].a.u64 = ip[0].b.u32 + ip[1].b.u64;
                break;
            }

            // MakeStackPointer followed by SetAtPointer, replace with SetAtStackPointer, but
            // leave the MakeStackPointer which will be removed later (?) if no more used
            if (ip[1].op == ByteCodeOp::SetAtPointer8 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer8);
                ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetAtPointer16 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer16);
                ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetAtPointer32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer32);
                ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetAtPointer64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer64);
                ip[1].a.u32 = ip[0].b.u32 + ip[1].c.u32;
                break;
            }

            if (ip[2].op == ByteCodeOp::SetAtPointer8 &&
                ip[0].a.u32 == ip[2].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ByteCode::hasWriteRefToReg(ip + 1, ip[0].a.u32))
            {
                SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer8);
                ip[2].a.u32 = ip[0].b.u32 + ip[2].c.u32;
                break;
            }

            if (ip[2].op == ByteCodeOp::SetAtPointer16 &&
                ip[0].a.u32 == ip[2].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ByteCode::hasWriteRefToReg(ip + 1, ip[0].a.u32))
            {
                SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer16);
                ip[2].a.u32 = ip[0].b.u32 + ip[2].c.u32;
                break;
            }

            if (ip[2].op == ByteCodeOp::SetAtPointer32 &&
                ip[0].a.u32 == ip[2].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ByteCode::hasWriteRefToReg(ip + 1, ip[0].a.u32))
            {
                SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer32);
                ip[2].a.u32 = ip[0].b.u32 + ip[2].c.u32;
                break;
            }

            if (ip[2].op == ByteCodeOp::SetAtPointer64 &&
                ip[0].a.u32 == ip[2].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ByteCode::hasWriteRefToReg(ip + 1, ip[0].a.u32))
            {
                SET_OP(ip + 2, ByteCodeOp::SetAtStackPointer64);
                ip[2].a.u32 = ip[0].b.u32 + ip[2].c.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef8 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetFromStack8);
                ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetFromStack16);
                ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetFromStack32);
                ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetFromStack64);
                ip[1].b.u32 = ip[0].b.u32 + ip[1].c.u32;
                break;
            }

            break;

        case ByteCodeOp::GetIncParam64:
            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].c.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip->d.s64 += ip[1].b.s64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef8 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64DeRef8);
                ip->d.u64 += ip[1].c.s64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64DeRef16);
                ip->d.u64 += ip[1].c.s64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64DeRef32);
                ip->d.u64 += ip[1].c.s64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64DeRef64);
                ip->d.u64 += ip[1].c.s64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef8 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncParam64DeRef8);
                ip[1].d.u64 = ip[0].d.u64 + ip[1].c.u64;
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].c.u64 = ip[0].c.u64;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncParam64DeRef16);
                ip[1].d.u64 = ip[0].d.u64 + ip[1].c.u64;
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].c.u64 = ip[0].c.u64;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncParam64DeRef32);
                ip[1].d.u64 = ip[0].d.u64 + ip[1].c.u64;
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].c.u64 = ip[0].c.u64;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncParam64DeRef64);
                ip[1].d.u64 = ip[0].d.u64 + ip[1].c.u64;
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].c.u64 = ip[0].c.u64;
                break;
            }
            break;

        case ByteCodeOp::GetParam64:
            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[1].b.u32 == 0xFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam8);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[1].b.u32 == 0xFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam16);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].b.u64 == 0xFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam8);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].b.u64 == 0xFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam16);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].b.u64 == 0xFFFFFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam32);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[2].op != ByteCodeOp::SetZeroAtPointer8 &&
                ip[2].op != ByteCodeOp::SetZeroAtPointer16 &&
                ip[2].op != ByteCodeOp::SetZeroAtPointer32 &&
                ip[2].op != ByteCodeOp::SetZeroAtPointer64 &&
                ip[2].op != ByteCodeOp::SetZeroAtPointerX &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].c.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64);
                ip->d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef8 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                ip[1].c.s64 == 0 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam64DeRef8);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                ip[1].c.s64 == 0 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam64DeRef16);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                ip[1].c.s64 == 0 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam64DeRef32);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                ip[1].c.s64 == 0 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam64DeRef64);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef8 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[1].c.s64 > 0 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncParam64DeRef8);
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].d.u64 = ip[1].c.u64;
                ip[1].c.u64 = 0;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[1].c.s64 > 0 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncParam64DeRef16);
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].d.u64 = ip[1].c.u64;
                ip[1].c.u64 = 0;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[1].c.s64 > 0 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncParam64DeRef32);
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].d.u64 = ip[1].c.u64;
                ip[1].c.u64 = 0;
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[1].c.s64 > 0 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::GetIncParam64DeRef64);
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].d.u64 = ip[1].c.u64;
                ip[1].c.u64 = 0;
                break;
            }

            if (ip[1].op == ByteCodeOp::MakeStackPointer &&
                ip[2].op == ByteCodeOp::IncPointer64 &&
                ip[0].a.u32 == ip[2].a.u32 &&
                ip[0].a.u32 != ip[1].a.u32 &&
                ip[2].hasFlag(BCI_IMM_B) &&
                ip[2].a.u32 == ip[2].c.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64);
                ip->d.u64 = ip[2].b.u64;
                setNop(context, ip + 2);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                ip[2].op == ByteCodeOp::IncPointer64 &&
                ip[0].a.u32 == ip[2].a.u32 &&
                ip[0].a.u32 != ip[1].a.u32 &&
                ip[2].hasFlag(BCI_IMM_B) &&
                ip[2].a.u32 == ip[2].c.u32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64);
                ip->d.u64 = ip[2].b.u64;
                setNop(context, ip + 2);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef8 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64DeRef8);
                ip->d.u64 = ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef16 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64DeRef16);
                ip->d.u64 = ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64DeRef32);
                ip->d.u64 = ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::DeRef64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetIncParam64DeRef64);
                ip->d.u64 = ip[1].c.u64;
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::SetZeroStack8:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
                ip[0].a.u32 + 1 == ip[1].a.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[1].a.u32 -= 1;
                ip[1].b.u64 <<= 8;
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer16);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetAtStackPointer8 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer16 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetZeroStack8 ||
                 ip[1].op == ByteCodeOp::SetZeroStack16 ||
                 ip[1].op == ByteCodeOp::SetZeroStack32 ||
                 ip[1].op == ByteCodeOp::SetZeroStack64) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            break;

        case ByteCodeOp::SetZeroStack16:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
                ip[0].a.u32 + 2 == ip[1].a.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[1].a.u32 -= 2;
                ip[1].b.u64 <<= 16;
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer32);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetAtStackPointer16 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetZeroStack16 ||
                 ip[1].op == ByteCodeOp::SetZeroStack32 ||
                 ip[1].op == ByteCodeOp::SetZeroStack64) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            break;

        case ByteCodeOp::SetZeroStack32:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
                ip[0].a.u32 + 4 == ip[1].a.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[1].a.u32 -= 4;
                ip[1].b.u64 <<= 32;
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer64);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack8 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + 4 - 1 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack16 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + 4 - 2 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                ip[1].b.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetZeroStack32 ||
                 ip[1].op == ByteCodeOp::SetZeroStack64) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            break;

        case ByteCodeOp::SetZeroStack64:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack8 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + 8 - 1 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack16 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + 8 - 2 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + 8 - 4 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                ip[1].b.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroStack64 &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            break;

        case ByteCodeOp::SetZeroStackX:
            if (ip[0].b.u32 == 1)
            {
                ip[0].b.u32 = 0;
                SET_OP(ip, ByteCodeOp::SetZeroStack8);
                break;
            }

            if (ip[0].b.u32 == 2)
            {
                ip[0].b.u32 = 0;
                SET_OP(ip, ByteCodeOp::SetZeroStack16);
                break;
            }

            if (ip[0].b.u32 == 4)
            {
                ip[0].b.u32 = 0;
                SET_OP(ip, ByteCodeOp::SetZeroStack32);
                break;
            }

            if (ip[0].b.u32 == 8)
            {
                ip[0].b.u32 = 0;
                SET_OP(ip, ByteCodeOp::SetZeroStack64);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack8 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + ip[0].b.u32 - 1 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack16 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + ip[0].b.u32 - 2 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + ip[0].b.u32 - 4 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                ip[1].b.u32 >= ip[0].a.u32 &&
                ip[1].b.u32 < ip[0].a.u32 + ip[0].b.u32 - 8 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRA);
                break;
            }

            break;

        case ByteCodeOp::CopyRBtoRA8:
            if ((ip[1].op == ByteCodeOp::ClearMaskU32 || ip[1].op == ByteCodeOp::ClearMaskU64) &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::CopyRBtoRA16:
            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[1].b.u32 >= 0xFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].b.u64 >= 0xFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::CopyRBtoRA32:
            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].b.u64 == 0xFFFFFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        // DeRef, by convention, clear the remaining bits. So no need for a cast just after
        case ByteCodeOp::DeRef8:
            if ((ip[1].op == ByteCodeOp::ClearMaskU32 || ip[1].op == ByteCodeOp::ClearMaskU64) &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::DeRef16:
            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[1].b.u32 >= 0xFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].b.u64 >= 0xFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::DeRef32:
            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].b.u64 >= 0xFFFFFFFF &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        // Copy64 followed by cast
        case ByteCodeOp::CopyRBtoRA64:
            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                switch (ip[1].b.u64)
                {
                    case 0xFF:
                        SET_OP(ip, ByteCodeOp::CopyRBtoRA8);
                        setNop(context, ip + 1);
                        break;
                    case 0xFFFF:
                        SET_OP(ip, ByteCodeOp::CopyRBtoRA16);
                        setNop(context, ip + 1);
                        break;
                    case 0xFFFFFFFF:
                        SET_OP(ip, ByteCodeOp::CopyRBtoRA32);
                        setNop(context, ip + 1);
                        break;
                    default:
                        break;
                }
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                switch (ip[1].b.u64)
                {
                    case 0xFF:
                        SET_OP(ip, ByteCodeOp::CopyRBtoRA8);
                        setNop(context, ip + 1);
                        break;
                    case 0xFFFF:
                        SET_OP(ip, ByteCodeOp::CopyRBtoRA16);
                        setNop(context, ip + 1);
                        break;
                    default:
                        break;
                }
                break;
            }
            break;

        case ByteCodeOp::SetImmediate32:
            if (ip[1].op == ByteCodeOp::CopyRBtoRA8 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->a.u32 == ip[1].b.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
                ip[1].b.u32 = ip->b.u8;
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBtoRA16 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->a.u32 == ip[1].b.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
                ip[1].b.u32 = ip->b.u16;
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBtoRA32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->a.u32 == ip[1].b.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
                ip[1].b.u32 = ip->b.u32;
                break;
            }

            if (ip[0].b.u32 == 0)
            {
                SET_OP(ip, ByteCodeOp::ClearRA);
                break;
            }

            break;

        case ByteCodeOp::SetImmediate64:
            if (ip[1].op == ByteCodeOp::CopyRBtoRA8 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->a.u32 == ip[1].b.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate64);
                ip[1].b.u64 = ip->b.u8;
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBtoRA16 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->a.u32 == ip[1].b.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate64);
                ip[1].b.u64 = ip->b.u16;
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBtoRA32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->a.u32 == ip[1].b.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate64);
                ip[1].b.u64 = ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->a.u32 == ip[1].b.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate64);
                ip[1].b.u64 = ip->b.u64;
                break;
            }

            if (ip[0].b.u64 == 0)
            {
                SET_OP(ip, ByteCodeOp::ClearRA);
                break;
            }

            break;

        // Indirect assign to register. Make it direct
        case ByteCodeOp::ClearRA:
            if (ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
                ip[2].op == ByteCodeOp::SetAtPointer64 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                ip[2].c.u32 == 0 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[1].a.u32 == ip[2].a.u32)
            {
                if (ip[2].hasFlag(BCI_IMM_B))
                {
                    SET_OP(ip + 2, ByteCodeOp::SetImmediate64);
                }
                else
                {
                    SET_OP(ip + 2, ByteCodeOp::CopyRBtoRA64);
                }

                ip[2].a.u32 = ip[0].a.u32;
                setNop(context, ip);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
                ip[2].op == ByteCodeOp::SetAtPointer32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                ip[2].c.u32 == 0 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[1].a.u32 == ip[2].a.u32)
            {
                if (ip[2].hasFlag(BCI_IMM_B))
                {
                    ip[2].b.u64 = ip[2].b.u32;
                    SET_OP(ip + 2, ByteCodeOp::SetImmediate64);
                }
                else
                {
                    SET_OP(ip + 2, ByteCodeOp::CopyRBtoRA32);
                }

                ip[2].a.u32 = ip[0].a.u32;
                setNop(context, ip);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
                ip[2].op == ByteCodeOp::SetAtPointer16 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                ip[2].c.u32 == 0 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[1].a.u32 == ip[2].a.u32)
            {
                if (ip[2].hasFlag(BCI_IMM_B))
                {
                    ip[2].b.u64 = ip[2].b.u16;
                    SET_OP(ip + 2, ByteCodeOp::SetImmediate64);
                }
                else
                {
                    SET_OP(ip + 2, ByteCodeOp::CopyRBtoRA16);
                }

                ip[2].a.u32 = ip[0].a.u32;
                setNop(context, ip);
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyRBAddrToRA &&
                ip[2].op == ByteCodeOp::SetAtPointer8 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                ip[2].c.u32 == 0 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[1].a.u32 == ip[2].a.u32)
            {
                if (ip[2].hasFlag(BCI_IMM_B))
                {
                    ip[2].b.u64 = ip[2].b.u8;
                    SET_OP(ip + 2, ByteCodeOp::SetImmediate64);
                }
                else
                {
                    SET_OP(ip + 2, ByteCodeOp::CopyRBtoRA8);
                }

                ip[2].a.u32 = ip[0].a.u32;
                setNop(context, ip);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer8:
            if (ip[0].hasFlag(BCI_IMM_B) && ip[0].b.u8 == 0)
            {
                SET_OP(ip, ByteCodeOp::SetZeroStack8);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyStack8 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer8);
                ip[1].b.u64 = ip[0].b.u8;
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack8 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
                ip[1].b.u64 = ip[0].b.u8;
                ip[1].removeFlag(BCI_IMM_A);
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack8 &&
                !ip->hasFlag(BCI_IMM_B) &&
                ip->b.u32 == ip[1].a.u32 &&
                ip->a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetAtStackPointer8 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer16 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
                ip[1].hasFlag(BCI_IMM_B) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetZeroStack8 ||
                 ip[1].op == ByteCodeOp::SetZeroStack16 ||
                 ip[1].op == ByteCodeOp::SetZeroStack32 ||
                 ip[1].op == ByteCodeOp::SetZeroStack64) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack8 &&
                ip->a.u32 == ip[1].b.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA8);
                ip[1].b.u32 = ip[0].b.u32;
                break;
            }

            break;

        case ByteCodeOp::SetAtStackPointer16:
            if (ip[0].hasFlag(BCI_IMM_B) && ip[0].b.u16 == 0)
            {
                SET_OP(ip, ByteCodeOp::SetZeroStack16);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyStack16 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer16);
                ip[1].b.u64 = ip[0].b.u16;
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack16 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
                ip[1].b.u64 = ip[0].b.u16;
                ip[1].removeFlag(BCI_IMM_A);
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack16 &&
                !ip->hasFlag(BCI_IMM_B) &&
                ip->b.u32 == ip[1].a.u32 &&
                ip->a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetAtStackPointer16 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
                ip[1].hasFlag(BCI_IMM_B) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetZeroStack16 ||
                 ip[1].op == ByteCodeOp::SetZeroStack32 ||
                 ip[1].op == ByteCodeOp::SetZeroStack64) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack16 &&
                ip->a.u32 == ip[1].b.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA16);
                ip[1].b.u32 = ip[0].b.u32;
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer32:
            if (ip[0].hasFlag(BCI_IMM_B) && ip[0].b.u32 == 0)
            {
                SET_OP(ip, ByteCodeOp::SetZeroStack32);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyStack32 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer32);
                ip[1].b.u64 = ip[0].b.u32;
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
                ip[1].b.u64 = ip[0].b.u32;
                ip[1].removeFlag(BCI_IMM_A);
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                !ip->hasFlag(BCI_IMM_B) &&
                ip->b.u32 == ip[1].a.u32 &&
                ip->a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
                ip->hasFlag(BCI_IMM_B) &&
                ip[1].hasFlag(BCI_IMM_B) &&
                ip->a.u32 == ip[1].a.u32 - 4 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer64);
                ip[0].b.u64 |= static_cast<uint64_t>(ip[1].b.u32) << 32;
                setNop(context, ip + 1);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetAtStackPointer32 ||
                 ip[1].op == ByteCodeOp::SetAtStackPointer64) &&
                ip[1].hasFlag(BCI_IMM_B) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if ((ip[1].op == ByteCodeOp::SetZeroStack32 ||
                 ip[1].op == ByteCodeOp::SetZeroStack64) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                ip->a.u32 == ip[1].b.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA32);
                ip[1].b.u32 = ip[0].b.u32;
                break;
            }

            break;

        case ByteCodeOp::SetAtStackPointer64:
            if (ip[0].hasFlag(BCI_IMM_B) && ip[0].b.u64 == 0)
            {
                SET_OP(ip, ByteCodeOp::SetZeroStack64);
                break;
            }

            if (ip[1].op == ByteCodeOp::CopyStack64 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                ip[0].a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetAtStackPointer64);
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
                ip[1].b.u64 = ip[0].b.u32;
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                ip[0].a.u32 + 4 == ip[1].b.u32 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate32);
                ip[1].b.u64 = ip[0].b.u64 >> 32;
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                ip[0].a.u32 == ip[1].b.u32 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::SetImmediate64);
                ip[1].b.u64 = ip[0].b.u64;
                ip[1].addFlag(BCI_IMM_B);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                !ip->hasFlag(BCI_IMM_B) &&
                ip->b.u32 == ip[1].a.u32 &&
                ip->a.u32 == ip[1].b.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroStack64 &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                ip->a.u32 == ip[1].b.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::CopyRBtoRA64);
                ip[1].b.u32 = ip[0].b.u32;
                break;
            }

            break;
    }
}

void ByteCodeOptimizer::reduceStack1(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip[0].op)
    {
        case ByteCodeOp::GetFromStack8:
            if (ip[1].op == ByteCodeOp::SetAtPointer8 &&
                ip[2].op == ByteCodeOp::GetFromStack8 &&
                ip[3].op == ByteCodeOp::SetAtPointer8 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ip[3].hasFlag(BCI_START_STMT) &&
                ip[2].b.u32 == ip[0].b.u32 + 1 &&
                ip[3].c.u32 == ip[1].c.u32 + 1)
            {
                SET_OP(ip, ByteCodeOp::GetFromStack16);
                SET_OP(ip + 1, ByteCodeOp::SetAtPointer16);
                setNop(context, ip + 2);
                setNop(context, ip + 3);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack16:
            if (ip[1].op == ByteCodeOp::SetAtPointer16 &&
                ip[2].op == ByteCodeOp::GetFromStack16 &&
                ip[3].op == ByteCodeOp::SetAtPointer16 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ip[3].hasFlag(BCI_START_STMT) &&
                ip[2].b.u32 == ip[0].b.u32 + 2 &&
                ip[3].c.u32 == ip[1].c.u32 + 2)
            {
                SET_OP(ip, ByteCodeOp::GetFromStack32);
                SET_OP(ip + 1, ByteCodeOp::SetAtPointer32);
                setNop(context, ip + 2);
                setNop(context, ip + 3);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack32:
            if (ip[1].op == ByteCodeOp::SetAtPointer32 &&
                ip[2].op == ByteCodeOp::GetFromStack32 &&
                ip[3].op == ByteCodeOp::SetAtPointer32 &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ip[3].hasFlag(BCI_START_STMT) &&
                ip[2].b.u32 == ip[0].b.u32 + 4 &&
                ip[3].c.u32 == ip[1].c.u32 + 4)
            {
                SET_OP(ip, ByteCodeOp::GetFromStack64);
                SET_OP(ip + 1, ByteCodeOp::SetAtPointer64);
                setNop(context, ip + 2);
                setNop(context, ip + 3);
                break;
            }
            break;
    }
}

void ByteCodeOptimizer::reduceStack2(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip[0].op == ByteCodeOp::SetAtStackPointer8 ||
        ip[0].op == ByteCodeOp::SetAtStackPointer16 ||
        ip[0].op == ByteCodeOp::SetAtStackPointer32 ||
        ip[0].op == ByteCodeOp::SetAtStackPointer64)
    {
        if (ip->flags.has(BCI_IMM_B))
            return;

        auto tst = ip + 1;
        while (tst->op != ByteCodeOp::End)
        {
            if (tst->flags.has(BCI_START_STMT))
                return;

            if (ip[0].op == ByteCodeOp::SetAtStackPointer64 && tst->op == ByteCodeOp::GetFromStack64)
            {
                if (tst->a.u32 == ip->b.u32 && tst->b.u32 == ip->a.u32)
                {
                    setNop(context, tst);
                    return;
                }
            }
            else if (ip[0].op == ByteCodeOp::SetAtStackPointer64 && tst->op == ByteCodeOp::GetFromStack32)
            {
                if (tst->a.u32 == ip->b.u32 && tst->b.u32 == ip->a.u32)
                {
                    SET_OP(tst, ByteCodeOp::ClearMaskU64);
                    tst->b.u64 = 0xFFFFFFFF;
                    return;
                }
            }
            else if (ip[0].op == ByteCodeOp::SetAtStackPointer64 && tst->op == ByteCodeOp::GetFromStack16)
            {
                if (tst->a.u32 == ip->b.u32 && tst->b.u32 == ip->a.u32)
                {
                    SET_OP(tst, ByteCodeOp::ClearMaskU64);
                    tst->b.u64 = 0xFFFF;
                    return;
                }
            }
            else if (ip[0].op == ByteCodeOp::SetAtStackPointer64 && tst->op == ByteCodeOp::GetFromStack8)
            {
                if (tst->a.u32 == ip->b.u32 && tst->b.u32 == ip->a.u32)
                {
                    SET_OP(tst, ByteCodeOp::ClearMaskU64);
                    tst->b.u64 = 0xFF;
                    return;
                }
            }
            else if (ip[0].op == ByteCodeOp::SetAtStackPointer32 && tst->op == ByteCodeOp::GetFromStack32)
            {
                if (tst->a.u32 == ip->b.u32 && tst->b.u32 == ip->a.u32)
                {
                    setNop(context, tst);
                    return;
                }
            }
            else if (ip[0].op == ByteCodeOp::SetAtStackPointer16 && tst->op == ByteCodeOp::GetFromStack16)
            {
                if (tst->a.u32 == ip->b.u32 && tst->b.u32 == ip->a.u32)
                {
                    setNop(context, tst);
                    return;
                }
            }
            else if (ip[0].op == ByteCodeOp::SetAtStackPointer8 && tst->op == ByteCodeOp::GetFromStack8)
            {
                if (tst->a.u32 == ip->b.u32 && tst->b.u32 == ip->a.u32)
                {
                    setNop(context, tst);
                    return;
                }
            }
            else
            {
                switch (tst->op)
                {
                    case ByteCodeOp::GetFromStack8:
                    case ByteCodeOp::GetFromStack16:
                    case ByteCodeOp::GetFromStack32:
                    case ByteCodeOp::GetFromStack64:
                    case ByteCodeOp::DeRef8:
                    case ByteCodeOp::DeRef16:
                    case ByteCodeOp::DeRef32:
                    case ByteCodeOp::DeRef64:
                    case ByteCodeOp::IncSPPostCall:
                    case ByteCodeOp::MakeStackPointer:
                    case ByteCodeOp::MakeConstantSegPointer:
                    case ByteCodeOp::MakeBssSegPointer:
                    case ByteCodeOp::MakeMutableSegPointer:
                        break;
                    default:
                        return;
                }

                if (ByteCode::hasWriteRefToReg(tst, ip->b.u32))
                    return;
            }

            tst++;
        }
    }
}

void ByteCodeOptimizer::reduceStack3(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip[0].op == ByteCodeOp::SetAtStackPointer8 ||
        ip[0].op == ByteCodeOp::SetAtStackPointer16 ||
        ip[0].op == ByteCodeOp::SetAtStackPointer32 ||
        ip[0].op == ByteCodeOp::SetAtStackPointer64)
    {
        auto tst = ip + 1;
        while (tst->op != ByteCodeOp::End)
        {
            if (tst->flags.has(BCI_START_STMT))
                return;

            switch (tst->op)
            {
                case ByteCodeOp::IncSPPostCall:
                case ByteCodeOp::MakeStackPointer:
                case ByteCodeOp::MakeConstantSegPointer:
                case ByteCodeOp::MakeBssSegPointer:
                case ByteCodeOp::MakeMutableSegPointer:
                case ByteCodeOp::ClearMaskU32:
                case ByteCodeOp::ClearMaskU64:
                case ByteCodeOp::CopyRAtoRRRet:
                case ByteCodeOp::CopyRAtoRR:
                case ByteCodeOp::CopyRAtoRT:
                case ByteCodeOp::ClearRA:
                case ByteCodeOp::GetParam8:
                case ByteCodeOp::GetParam16:
                case ByteCodeOp::GetParam32:
                case ByteCodeOp::GetParam64:
                case ByteCodeOp::Ret:
                    break;
                default:
                    return;
            }

            tst++;
        }

        setNop(context, ip);
    }
}

void ByteCodeOptimizer::reduceIncPtr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip[0].op)
    {
        case ByteCodeOp::Mul64byVB64:
            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                !ip[1].hasFlag(BCI_IMM_B) &&
                ip[1].b.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::IncMulPointer64);
                ip[1].d.u64 = ip->b.u64;
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::BinOpMulU64_Safe:
            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_B) &&
                ip[1].b.u32 == ip[0].c.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::IncMulPointer64);
                ip[1].b.u32 = ip->a.u32;
                ip[1].d.u64 = ip->b.u64;
                break;
            }
            break;

        case ByteCodeOp::IncPointer64:
            // followed by another IncPointer64
            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[1].c.u32 == ip->c.u32 &&
                ip[1].c.u32 == ip[1].a.u32 &&
                ip[0].hasFlag(BCI_IMM_B) &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[1].a.u32 = ip[0].a.u32;
                ip[1].b.s64 += ip[0].b.s64;
                setNop(context, ip);
                break;
            }

            if (ip[0].hasFlag(BCI_IMM_B) &&
                ip[0].b.s64 > 0 && // Offset cannot be negative, so zap if inc pointer is negative
                !ip[1].hasFlag(BCI_START_STMT))
            {
                // followed by DeRefStringSlice, set constant to deref
                if (ip[1].op == ByteCodeOp::DeRefStringSlice &&
                    ip->b.s64 + ip[1].c.s64 >= 0 &&
                    ip->b.s64 + ip[1].c.s64 <= 0x7FFFFFFF &&
                    ip->a.u32 == ip->c.u32 &&
                    ip->c.u32 == ip[1].a.u32)
                {
                    ip[1].c.s64 += ip->b.s64;
                    setNop(context, ip);
                    break;
                }

                // followed by SetZeroAtPointerX
                if (ip[1].op == ByteCodeOp::SetZeroAtPointerX &&
                    ip->b.s64 + ip[1].c.s64 >= 0 &&
                    ip->b.s64 + ip[1].c.s64 <= 0x7FFFFFFF &&
                    ip[0].c.u32 == ip[1].a.u32)
                {
                    ip[1].a.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    setNop(context, ip);
                    break;
                }

                // Make DeRef with an offset
                if (ip[1].op == ByteCodeOp::DeRef8 &&
                    ip[0].b.s64 + ip[1].c.s64 >= 0 &&
                    ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF &&
                    ip[0].c.u32 == ip[1].b.u32)
                {
                    if (ip[1].a.u32 == ip[1].b.u32)
                    {
                        ip[1].b.u32 = ip[0].a.u32;
                        ip[1].c.s64 += ip[0].b.s64;
                        setNop(context, ip);
                    }
                    else
                    {
                        ip[1].b.u32 = ip[0].a.u32;
                        ip[1].c.s64 += ip[0].b.s64;
                        if (ip[0].a.u32 != ip[1].a.u32)
                            std::swap(ip[0], ip[1]);
                    }
                    break;
                }

                if (ip[1].op == ByteCodeOp::DeRef16 &&
                    ip[0].b.s64 + ip[1].c.s64 >= 0 &&
                    ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF &&
                    ip[0].c.u32 == ip[1].b.u32)
                {
                    if (ip[1].a.u32 == ip[1].b.u32)
                    {
                        ip[1].b.u32 = ip[0].a.u32;
                        ip[1].c.s64 += ip[0].b.s64;
                        setNop(context, ip);
                    }
                    else
                    {
                        ip[1].b.u32 = ip[0].a.u32;
                        ip[1].c.s64 += ip[0].b.s64;
                        if (ip[0].a.u32 != ip[1].a.u32)
                            std::swap(ip[0], ip[1]);
                    }
                    break;
                }

                if (ip[1].op == ByteCodeOp::DeRef32 &&
                    ip[0].b.s64 + ip[1].c.s64 >= 0 &&
                    ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF &&
                    ip[0].c.u32 == ip[1].b.u32)
                {
                    if (ip[1].a.u32 == ip[1].b.u32)
                    {
                        ip[1].b.u32 = ip[0].a.u32;
                        ip[1].c.s64 += ip[0].b.s64;
                        setNop(context, ip);
                    }
                    else
                    {
                        ip[1].b.u32 = ip[0].a.u32;
                        ip[1].c.s64 += ip[0].b.s64;
                        if (ip[0].a.u32 != ip[1].a.u32)
                            std::swap(ip[0], ip[1]);
                    }
                    break;
                }

                if (ip[1].op == ByteCodeOp::DeRef64 &&
                    ip[0].b.s64 + ip[1].c.s64 >= 0 &&
                    ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF &&
                    ip[0].c.u32 == ip[1].b.u32)
                {
                    if (ip[1].a.u32 == ip[1].b.u32)
                    {
                        ip[1].b.u32 = ip[0].a.u32;
                        ip[1].c.s64 += ip[0].b.s64;
                        setNop(context, ip);
                    }
                    else
                    {
                        ip[1].b.u32 = ip[0].a.u32;
                        ip[1].c.s64 += ip[0].b.s64;
                        if (ip[0].a.u32 != ip[1].a.u32)
                            std::swap(ip[0], ip[1]);
                    }
                    break;
                }

                // IncPointer with src != dst, followed by one SetAtPointer
                // No need to do a nop, the optimizer will remove instruction if unused
                // (safer that the version below)
                if ((ip[1].op == ByteCodeOp::SetAtPointer8 ||
                     ip[1].op == ByteCodeOp::SetAtPointer16 ||
                     ip[1].op == ByteCodeOp::SetAtPointer32 ||
                     ip[1].op == ByteCodeOp::SetAtPointer64) &&
                    ip[0].a.u32 != ip[0].c.u32 &&
                    ip[0].c.u32 == ip[1].a.u32 &&
                    ip[0].b.s64 + ip[1].c.s64 >= 0 &&
                    ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF)
                {
                    ip[1].a.u32 = ip[0].a.u32;
                    ip[1].c.s64 += ip[0].b.s64;
                    context->setDirtyPass();
                    break;
                }

                // Inc Pointer follower by SetAtPointer.
                // Encode the offset in SetAtPointer, but do not remove the IncPointer, as the
                // register can be used later. So we just swap the 2 instructions, and the
                // optimizer will remove the inc pointer if no more necessary

                if ((ip[1].op == ByteCodeOp::SetZeroAtPointer8 ||
                     ip[1].op == ByteCodeOp::SetZeroAtPointer16 ||
                     ip[1].op == ByteCodeOp::SetZeroAtPointer32 ||
                     ip[1].op == ByteCodeOp::SetZeroAtPointer64) &&
                    ip[0].a.u32 == ip[0].c.u32 &&
                    ip[0].a.u32 == ip[1].a.u32 &&
                    ip[0].b.s64 + ip[1].b.s64 >= 0 &&
                    ip[0].b.s64 + ip[1].b.s64 <= 0x7FFFFFFF &&
                    !ip[0].hasFlag(BCI_START_STMT))
                {
                    ip[1].b.s64 += ip[0].b.s64;
                    std::swap(ip[0], ip[1]);
                    context->setDirtyPass();
                    break;
                }

                if ((ip[1].op == ByteCodeOp::SetAtPointer8 ||
                     ip[1].op == ByteCodeOp::SetAtPointer16 ||
                     ip[1].op == ByteCodeOp::SetAtPointer32 ||
                     ip[1].op == ByteCodeOp::SetAtPointer64) &&
                    ip[0].a.u32 == ip[0].c.u32 &&
                    ip[0].a.u32 == ip[1].a.u32 &&
                    ip[0].b.s64 + ip[1].c.s64 >= 0 &&
                    ip[0].b.s64 + ip[1].c.s64 <= 0x7FFFFFFF &&
                    !ip[0].hasFlag(BCI_START_STMT))

                {
                    ip[1].c.s64 += ip[0].b.s64;
                    std::swap(ip[0], ip[1]);
                    context->setDirtyPass();
                    break;
                }
            }
            break;

        case ByteCodeOp::MakeStackPointer:
            // MakeStackPointer Reg, ImmB
            // IncPointer64     Reg, Reg, ImmB
            // We can change the offset of the MakeStackPointer and remove the IncPointer
            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[1].c.u32 == ip[0].a.u32 &&
                ip[1].c.u32 == ip[1].a.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                ip[1].b.s64 > 0)
            {
                ip[0].b.u32 += ip[1].b.u32;
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::CopyRRtoRA:
            // Store offset of CopyRRtoRA directly in the instruction
            if (ip[1].op == ByteCodeOp::IncPointer64 &&
                ip[1].a.u32 == ip[1].c.u32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                ip[1].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u64 += ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointer8 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRR8);
                ip[1].c.u64 = ip[1].b.u64 + ip[0].b.u64;
                ip[1].a.u64 = 0;
                ip[1].b.u64 = 0;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointer16 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRR16);
                ip[1].c.u64 = ip[1].b.u64 + ip[0].b.u64;
                ip[1].a.u64 = 0;
                ip[1].b.u64 = 0;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointer32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRR32);
                ip[1].c.u64 = ip[1].b.u64 + ip[0].b.u64;
                ip[1].a.u64 = 0;
                ip[1].b.u64 = 0;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointer64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRR64);
                ip[1].c.u64 = ip[1].b.u64 + ip[0].b.u64;
                ip[1].a.u64 = 0;
                ip[1].b.u64 = 0;
                break;
            }

            if (ip[1].op == ByteCodeOp::SetZeroAtPointerX &&
                ip[0].a.u32 == ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ClearRRX);
                ip[1].c.u64 += ip[0].b.u64;
                ip[1].a.u64 = 0;
                break;
            }

            break;
    }
}

void ByteCodeOptimizer::reduceCast(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::GetParam8:
            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                (ip[1].b.u32 & 0xFF) == 0xFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                (ip[1].b.u64 & 0xFF) == 0xFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetParam16:
            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                (ip[1].b.u32 & 0xFFFF) == 0xFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                (ip[1].b.u64 & 0xFFFF) == 0xFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetParam32:
            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                (ip[1].b.u32 & 0xFFFFFFFF) == 0xFFFFFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                (ip[1].b.u64 & 0xFFFFFFFF) == 0xFFFFFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::ClearMaskU32:
            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[1].b.u64 = min(ip[0].b.u64, ip[1].b.u64);
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::ClearMaskU64:
            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[1].a.u32 == ip[0].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u64 = min(ip[0].b.u64, ip[1].b.u64);
                setNop(context, ip + 1);
                break;
            }

            if (ip->b.u64 == 0xFFFFFFFF &&
                ip[1].op == ByteCodeOp::CastS32S64 &&
                ip[1].b.u32 == ip[0].a.u32 &&
                ip[1].b.u32 != ip[1].a.u32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ByteCode::swapInstructions(ip, ip + 1);
                context->setDirtyPass();
                break;
            }

            break;

        // GetFromStack8/16/32 clear the other bits by convention, so no need to
        // have a ClearMask after
        case ByteCodeOp::GetFromStack8:
        case ByteCodeOp::GetIncFromStack64DeRef8:
        case ByteCodeOp::GetParam64DeRef8:
        case ByteCodeOp::GetIncParam64DeRef8:
            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                (ip[1].b.u64 & 0xFF) == 0xFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                (ip[1].b.u64 & 0xFF) == 0xFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack16:
        case ByteCodeOp::GetIncFromStack64DeRef16:
        case ByteCodeOp::GetParam64DeRef16:
        case ByteCodeOp::GetIncParam64DeRef16:
            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                (ip[1].b.u64 & 0xFFFF) == 0xFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                (ip[1].b.u64 & 0xFFFF) == 0xFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack32:
        case ByteCodeOp::GetIncFromStack64DeRef32:
        case ByteCodeOp::GetParam64DeRef32:
        case ByteCodeOp::GetIncParam64DeRef32:
            if (ip[1].op == ByteCodeOp::ClearMaskU64 &&
                ip[0].a.u32 == ip[1].a.u32 &&
                (ip[1].b.u64 & 0xFFFFFFFF) == 0xFFFFFFFF &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip + 1);
                break;
            }
            break;
    }
}

void ByteCodeOptimizer::reduceNoOp(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::Add64byVB64:
            if (ip->b.u64 == 0)
            {
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::Mul64byVB64:
            if (ip->b.u64 == 1)
            {
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::DebugNop:
            if (ip[1].op == ByteCodeOp::DebugNop)
            {
                setNop(context, ip + 1);
                break;
            }
            break;

        // Useless pop/push
        case ByteCodeOp::InternalPushErr:
            if (ip[1].op == ByteCodeOp::InternalPopErr)
            {
                setNop(context, ip);
                setNop(context, ip + 1);
                break;
            }
            break;

        // Useless multi return
        case ByteCodeOp::Ret:
            if (ip[1].op == ByteCodeOp::Ret)
            {
                setNop(context, ip);
                break;
            }
            break;

        // Duplicated safety
        case ByteCodeOp::JumpIfNotZero64:
            if (ip[1].op == ByteCodeOp::InternalPanic &&
                ip[2].op == ByteCodeOp::JumpIfNotZero64 &&
                ip[3].op == ByteCodeOp::InternalPanic &&
                ip[0].a.u32 == ip[2].a.u32 &&
                ip[0].b.s32 == 1 &&
                ip[2].b.s32 == 1)
            {
                setNop(context, ip);
                setNop(context, ip + 1);
                break;
            }
            break;
    }

    // Remove operators which do nothing
    if (ip->hasFlag(BCI_IMM_B))
    {
        switch (ip->op)
        {
            case ByteCodeOp::IncPointer64:
                if (ip->b.u64 == 0)
                {
                    if (ip->a.u32 == ip->c.u32)
                        setNop(context, ip);
                    else
                    {
                        SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                        ip->b.u64 = ip->a.u64;
                        ip->a.u64 = ip->c.u64;
                        ip->removeFlag(BCI_IMM_B);
                    }
                }
                break;

            case ByteCodeOp::AffectOpDivEqS8:
            case ByteCodeOp::AffectOpMulEqS8:
                if (ip->b.u8 == 1)
                    setNop(context, ip);
                break;
            case ByteCodeOp::AffectOpDivEqS16:
            case ByteCodeOp::AffectOpMulEqS16:
                if (ip->b.u16 == 1)
                    setNop(context, ip);
                break;
            case ByteCodeOp::AffectOpDivEqS32:
            case ByteCodeOp::AffectOpMulEqS32:
                if (ip->b.u32 == 1)
                    setNop(context, ip);
                break;
            case ByteCodeOp::AffectOpDivEqS64:
            case ByteCodeOp::AffectOpMulEqS64:
                if (ip->b.u64 == 1)
                    setNop(context, ip);
                break;
            case ByteCodeOp::AffectOpPlusEqS8:
            case ByteCodeOp::AffectOpPlusEqS8_Safe:
            case ByteCodeOp::AffectOpPlusEqU8:
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
            case ByteCodeOp::AffectOpMinusEqS8:
            case ByteCodeOp::AffectOpMinusEqS8_Safe:
            case ByteCodeOp::AffectOpMinusEqU8:
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
                if (ip->b.u8 == 0)
                    setNop(context, ip);
                break;
            case ByteCodeOp::AffectOpPlusEqS16:
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
            case ByteCodeOp::AffectOpPlusEqU16:
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
            case ByteCodeOp::AffectOpMinusEqS16:
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
            case ByteCodeOp::AffectOpMinusEqU16:
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
                if (ip->b.u16 == 0)
                    setNop(context, ip);
                break;
            case ByteCodeOp::AffectOpPlusEqS32:
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
            case ByteCodeOp::AffectOpPlusEqU32:
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
            case ByteCodeOp::AffectOpMinusEqS32:
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
            case ByteCodeOp::AffectOpMinusEqU32:
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
                if (ip->b.u32 == 0)
                    setNop(context, ip);
                break;
            case ByteCodeOp::AffectOpPlusEqS64:
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
            case ByteCodeOp::AffectOpPlusEqU64:
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
            case ByteCodeOp::AffectOpMinusEqS64:
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
            case ByteCodeOp::AffectOpMinusEqU64:
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
                if (ip->b.u64 == 0)
                    setNop(context, ip);
                break;

            case ByteCodeOp::AffectOpShiftLeftEqU8:
            case ByteCodeOp::AffectOpShiftLeftEqU16:
            case ByteCodeOp::AffectOpShiftLeftEqU32:
            case ByteCodeOp::AffectOpShiftLeftEqU64:
            case ByteCodeOp::AffectOpShiftLeftEqS8:
            case ByteCodeOp::AffectOpShiftLeftEqS16:
            case ByteCodeOp::AffectOpShiftLeftEqS32:
            case ByteCodeOp::AffectOpShiftLeftEqS64:
            case ByteCodeOp::AffectOpShiftRightEqU8:
            case ByteCodeOp::AffectOpShiftRightEqU16:
            case ByteCodeOp::AffectOpShiftRightEqU32:
            case ByteCodeOp::AffectOpShiftRightEqU64:
            case ByteCodeOp::AffectOpShiftRightEqS8:
            case ByteCodeOp::AffectOpShiftRightEqS16:
            case ByteCodeOp::AffectOpShiftRightEqS32:
            case ByteCodeOp::AffectOpShiftRightEqS64:
                if (ip->b.u32 == 0)
                    setNop(context, ip);
                break;

            default:
                break;
        }
    }

    if (ip->hasFlag(BCI_IMM_B) &&
        !ip->hasFlag(BCI_IMM_A) &&
        !ip->hasFlag(BCI_IMM_C) &&
        ip->a.u32 == ip->c.u32)
    {
        switch (ip->op)
        {
            case ByteCodeOp::BinOpPlusS8:
            case ByteCodeOp::BinOpPlusU8:
            case ByteCodeOp::BinOpPlusS8_Safe:
            case ByteCodeOp::BinOpPlusU8_Safe:
            case ByteCodeOp::BinOpMinusS8:
            case ByteCodeOp::BinOpMinusU8:
            case ByteCodeOp::BinOpMinusS8_Safe:
            case ByteCodeOp::BinOpMinusU8_Safe:
                if (ip->b.u8 == 0)
                    setNop(context, ip);
                break;

            case ByteCodeOp::BinOpPlusS16:
            case ByteCodeOp::BinOpPlusU16:
            case ByteCodeOp::BinOpPlusS16_Safe:
            case ByteCodeOp::BinOpMinusS16_Safe:
            case ByteCodeOp::BinOpMinusS16:
            case ByteCodeOp::BinOpMinusU16:
            case ByteCodeOp::BinOpPlusU16_Safe:
            case ByteCodeOp::BinOpMinusU16_Safe:
                if (ip->b.u16 == 0)
                    setNop(context, ip);
                break;

            case ByteCodeOp::BinOpPlusS32:
            case ByteCodeOp::BinOpPlusU32:
            case ByteCodeOp::BinOpPlusS32_Safe:
            case ByteCodeOp::BinOpPlusU32_Safe:
            case ByteCodeOp::BinOpMinusS32:
            case ByteCodeOp::BinOpMinusU32:
            case ByteCodeOp::BinOpMinusS32_Safe:
            case ByteCodeOp::BinOpMinusU32_Safe:
                if (ip->b.u32 == 0)
                    setNop(context, ip);
                break;

            case ByteCodeOp::BinOpPlusS64:
            case ByteCodeOp::BinOpPlusU64:
            case ByteCodeOp::BinOpPlusS64_Safe:
            case ByteCodeOp::BinOpPlusU64_Safe:
            case ByteCodeOp::BinOpMinusS64:
            case ByteCodeOp::BinOpMinusU64:
            case ByteCodeOp::BinOpMinusS64_Safe:
            case ByteCodeOp::BinOpMinusU64_Safe:
            case ByteCodeOp::IncPointer64:
            case ByteCodeOp::DecPointer64:
                if (ip->b.u64 == 0)
                    setNop(context, ip);
                break;

            case ByteCodeOp::BinOpMulS8:
            case ByteCodeOp::BinOpMulS8_Safe:
            case ByteCodeOp::BinOpDivS8:
                if (ip->b.u8 == 1)
                    setNop(context, ip);
                break;

            case ByteCodeOp::BinOpMulS16:
            case ByteCodeOp::BinOpMulS16_Safe:
            case ByteCodeOp::BinOpDivS16:
                if (ip->b.u16 == 1)
                    setNop(context, ip);
                break;

            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulS32_Safe:
            case ByteCodeOp::BinOpDivS32:
                if (ip->b.u32 == 1)
                    setNop(context, ip);
                break;

            case ByteCodeOp::BinOpMulS64:
            case ByteCodeOp::BinOpMulS64_Safe:
            case ByteCodeOp::BinOpDivS64:
                if (ip->b.u64 == 1)
                    setNop(context, ip);
                break;

            default:
                break;
        }
    }

    if (ip->hasFlag(BCI_IMM_B) &&
        !ip->hasFlag(BCI_IMM_A) &&
        !ip->hasFlag(BCI_IMM_C) &&
        ip->a.u32 != ip->c.u32)
    {
        switch (ip->op)
        {
            case ByteCodeOp::BinOpPlusS8:
            case ByteCodeOp::BinOpPlusU8:
            case ByteCodeOp::BinOpPlusS8_Safe:
            case ByteCodeOp::BinOpPlusU8_Safe:
            case ByteCodeOp::BinOpMinusS8:
            case ByteCodeOp::BinOpMinusU8:
            case ByteCodeOp::BinOpMinusS8_Safe:
            case ByteCodeOp::BinOpMinusU8_Safe:
                if (ip->b.u8 == 0)
                {
                    SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                    const auto s = ip->a.u32;
                    ip->a.u32    = ip->c.u32;
                    ip->b.u32    = s;
                    ip->removeFlag(BCI_IMM_B);
                }
                break;

            case ByteCodeOp::BinOpPlusS16:
            case ByteCodeOp::BinOpPlusU16:
            case ByteCodeOp::BinOpPlusS16_Safe:
            case ByteCodeOp::BinOpPlusU16_Safe:
            case ByteCodeOp::BinOpMinusS16:
            case ByteCodeOp::BinOpMinusU16:
            case ByteCodeOp::BinOpMinusS16_Safe:
            case ByteCodeOp::BinOpMinusU16_Safe:
                if (ip->b.u16 == 0)
                {
                    SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                    const auto s = ip->a.u32;
                    ip->a.u32    = ip->c.u32;
                    ip->b.u32    = s;
                    ip->removeFlag(BCI_IMM_B);
                }
                break;

            case ByteCodeOp::BinOpPlusS32:
            case ByteCodeOp::BinOpPlusU32:
            case ByteCodeOp::BinOpPlusS32_Safe:
            case ByteCodeOp::BinOpPlusU32_Safe:
            case ByteCodeOp::BinOpMinusS32:
            case ByteCodeOp::BinOpMinusU32:
            case ByteCodeOp::BinOpMinusS32_Safe:
            case ByteCodeOp::BinOpMinusU32_Safe:
                if (ip->b.u32 == 0)
                {
                    SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                    const auto s = ip->a.u32;
                    ip->a.u32    = ip->c.u32;
                    ip->b.u32    = s;
                    ip->removeFlag(BCI_IMM_B);
                }
                break;

            case ByteCodeOp::BinOpPlusS64:
            case ByteCodeOp::BinOpPlusU64:
            case ByteCodeOp::BinOpPlusS64_Safe:
            case ByteCodeOp::BinOpPlusU64_Safe:
            case ByteCodeOp::BinOpMinusS64:
            case ByteCodeOp::BinOpMinusU64:
            case ByteCodeOp::BinOpMinusS64_Safe:
            case ByteCodeOp::BinOpMinusU64_Safe:
                if (ip->b.u64 == 0)
                {
                    SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                    const auto s = ip->a.u32;
                    ip->a.u32    = ip->c.u32;
                    ip->b.u32    = s;
                    ip->removeFlag(BCI_IMM_B);
                }
                break;

            case ByteCodeOp::BinOpMulS32:
            case ByteCodeOp::BinOpMulU32:
            case ByteCodeOp::BinOpMulS32_Safe:
            case ByteCodeOp::BinOpMulU32_Safe:
                if (ip->b.u32 == 1)
                {
                    SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                    const auto s = ip->a.u32;
                    ip->a.u32    = ip->c.u32;
                    ip->b.u32    = s;
                    ip->removeFlag(BCI_IMM_B);
                }
                break;

            case ByteCodeOp::BinOpMulU64:
            case ByteCodeOp::BinOpMulS64:
            case ByteCodeOp::BinOpMulU64_Safe:
            case ByteCodeOp::BinOpMulS64_Safe:
                if (ip->b.u64 == 1)
                {
                    SET_OP(ip, ByteCodeOp::CopyRBtoRA64);
                    const auto s = ip->a.u32;
                    ip->a.u32    = ip->c.u32;
                    ip->b.u32    = s;
                    ip->removeFlag(BCI_IMM_B);
                }
                break;

            default:
                break;
        }
    }
}

void ByteCodeOptimizer::reduceSetAt(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (!ip[1].hasFlag(BCI_START_STMT))
    {
        // Reduce SetZeroStack
        uint32_t offset0, offset1;
        auto     size0 = ByteCode::getSetZeroStackSize(ip, offset0);
        if (size0)
        {
            const auto size1 = ByteCode::getSetZeroStackSize(ip + 1, offset1);
            if (size1 && offset0 + size0 == offset1)
            {
                const auto totalSize = size0 + size1;
                switch (totalSize)
                {
                    case 2:
                        SET_OP(ip, ByteCodeOp::SetZeroStack16);
                        break;
                    case 4:
                        SET_OP(ip, ByteCodeOp::SetZeroStack32);
                        break;
                    case 8:
                        SET_OP(ip, ByteCodeOp::SetZeroStack64);
                        break;
                    default:
                        SET_OP(ip, ByteCodeOp::SetZeroStackX);
                        ip->a.u32 = offset0;
                        SWAG_ASSERT(totalSize <= 0x7FFFFFFF);
                        ip->b.u32 = totalSize;
                        break;
                }

                setNop(context, ip + 1);
            }
        }

        // Reduce SetZeroAtPointer
        if (ip->a.u32 == ip[1].a.u32)
        {
            size0 = ByteCode::getSetZeroAtPointerSize(ip, offset0);
            if (size0)
            {
                const auto size1 = ByteCode::getSetZeroAtPointerSize(ip + 1, offset1);
                if (size1 && offset0 + size0 == offset1)
                {
                    const auto totalSize = size0 + size1;
                    switch (totalSize)
                    {
                        case 2:
                            SET_OP(ip, ByteCodeOp::SetZeroAtPointer16);
                            break;
                        case 4:
                            SET_OP(ip, ByteCodeOp::SetZeroAtPointer32);
                            break;
                        case 8:
                            SET_OP(ip, ByteCodeOp::SetZeroAtPointer64);
                            break;
                        default:
                            SET_OP(ip, ByteCodeOp::SetZeroAtPointerX);
                            ip->c.s64 = offset0;
                            SWAG_ASSERT(totalSize <= 0x7FFFFFFF);
                            ip->b.s64 = totalSize;
                            break;
                    }

                    setNop(context, ip + 1);
                }
            }
        }
    }

    switch (ip->op)
    {
        case ByteCodeOp::SetAtPointer8:
            if (ip->a.u32 == ip[1].a.u32 &&
                ip[1].op == ByteCodeOp::SetAtPointer8 &&
                ip->c.u32 + sizeof(uint8_t) == ip[1].c.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->hasFlag(BCI_IMM_B) &&
                ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip, ByteCodeOp::SetAtPointer16);
                ip->b.u64 |= static_cast<uint64_t>(ip[1].b.u32) << 8;
                setNop(context, ip + 1);
                break;
            }

            if (ip->hasFlag(BCI_IMM_B) && ip->b.u8 == 0)
            {
                SET_OP(ip, ByteCodeOp::SetZeroAtPointer8);
                ip->b.u64 = ip->c.u32;
                break;
            }

            break;

        case ByteCodeOp::SetAtPointer16:
            if (ip->a.u32 == ip[1].a.u32 &&
                ip[1].op == ByteCodeOp::SetAtPointer16 &&
                ip->c.u32 + sizeof(uint16_t) == ip[1].c.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->hasFlag(BCI_IMM_B) &&
                ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip, ByteCodeOp::SetAtPointer32);
                ip->b.u64 |= static_cast<uint64_t>(ip[1].b.u32) << 16;
                setNop(context, ip + 1);
                break;
            }

            if (ip->hasFlag(BCI_IMM_B) && ip->b.u16 == 0)
            {
                SET_OP(ip, ByteCodeOp::SetZeroAtPointer16);
                ip->b.u64 = ip->c.u32;
                break;
            }

            break;

        case ByteCodeOp::SetAtPointer32:
            if (ip->a.u32 == ip[1].a.u32 &&
                ip[1].op == ByteCodeOp::SetAtPointer32 &&
                ip->c.u32 + sizeof(uint32_t) == ip[1].c.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->hasFlag(BCI_IMM_B) &&
                ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip, ByteCodeOp::SetAtPointer64);
                ip->b.u64 |= static_cast<uint64_t>(ip[1].b.u32) << 32;
                setNop(context, ip + 1);
                break;
            }

            if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 == 0)
            {
                SET_OP(ip, ByteCodeOp::SetZeroAtPointer32);
                ip->b.u64 = ip->c.u32;
                break;
            }

            break;

        case ByteCodeOp::SetAtPointer64:
            if (ip->hasFlag(BCI_IMM_B) && ip->b.u64 == 0)
            {
                SET_OP(ip, ByteCodeOp::SetZeroAtPointer64);
                ip->b.u64 = ip->c.u32;
                break;
            }

            break;

        case ByteCodeOp::SetAtStackPointer8:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
                ip->a.u32 + sizeof(uint8_t) == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->hasFlag(BCI_IMM_B) &&
                ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer16);
                ip->b.u16 |= ip[1].b.u8 << 8;
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::SetAtStackPointer16:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
                ip->a.u32 + sizeof(uint16_t) == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->hasFlag(BCI_IMM_B) &&
                ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer32);
                ip->b.u32 |= ip[1].b.u16 << 16;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer32:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
                ip->a.u32 + sizeof(uint32_t) == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip->hasFlag(BCI_IMM_B) &&
                ip[1].hasFlag(BCI_IMM_B))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer64);
                ip->b.u64 |= static_cast<uint64_t>(ip[1].b.u32) << 32;
                setNop(context, ip + 1);
                break;
            }

            break;
    }
}

void ByteCodeOptimizer::reduceCmpJump(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    // Instruction followed by jump followed by the exact same instruction, and no stmt start
    // Remove the clone
    if (ByteCode::isJump(ip + 1) &&
        ip[1].op != ByteCodeOp::Jump &&
        !ip[1].hasFlag(BCI_START_STMT) &&
        !ip[2].hasFlag(BCI_START_STMT) &&
        ip[0].op == ip[2].op &&
        ip[0].flags == ip[2].flags &&
        ip[0].a.u64 == ip[2].a.u64 &&
        ip[0].b.u64 == ip[2].b.u64 &&
        ip[0].c.u64 == ip[2].c.u64 &&
        ip[0].d.u64 == ip[2].d.u64)
    {
        switch (ip[2].op)
        {
            case ByteCodeOp::GetParam64:
            case ByteCodeOp::GetParam64DeRef16:
            case ByteCodeOp::GetParam64DeRef32:
            case ByteCodeOp::GetParam64DeRef64:
            case ByteCodeOp::GetIncParam64:
            case ByteCodeOp::GetIncParam64DeRef8:
            case ByteCodeOp::GetIncParam64DeRef16:
            case ByteCodeOp::GetIncParam64DeRef32:
            case ByteCodeOp::GetIncParam64DeRef64:
            case ByteCodeOp::GetIncFromStack64:
            case ByteCodeOp::GetIncFromStack64DeRef8:
            case ByteCodeOp::GetIncFromStack64DeRef16:
            case ByteCodeOp::GetIncFromStack64DeRef32:
            case ByteCodeOp::GetIncFromStack64DeRef64:
            case ByteCodeOp::DeRef8:
            case ByteCodeOp::DeRef16:
            case ByteCodeOp::DeRef32:
            case ByteCodeOp::DeRef64:
            case ByteCodeOp::GetFromStack8x2:
            case ByteCodeOp::GetFromStack16x2:
            case ByteCodeOp::GetFromStack32x2:
            case ByteCodeOp::GetFromStack64x2:
            case ByteCodeOp::GetFromStack8:
            case ByteCodeOp::GetFromStack16:
            case ByteCodeOp::GetFromStack32:
            case ByteCodeOp::GetFromStack64:
            case ByteCodeOp::SetImmediate32:
            case ByteCodeOp::SetImmediate64:
            case ByteCodeOp::GetFromBssSeg8:
            case ByteCodeOp::GetFromBssSeg16:
            case ByteCodeOp::GetFromBssSeg32:
            case ByteCodeOp::GetFromBssSeg64:
            case ByteCodeOp::GetFromCompilerSeg8:
            case ByteCodeOp::GetFromCompilerSeg16:
            case ByteCodeOp::GetFromCompilerSeg32:
            case ByteCodeOp::GetFromCompilerSeg64:
            case ByteCodeOp::GetFromMutableSeg8:
            case ByteCodeOp::GetFromMutableSeg16:
            case ByteCodeOp::GetFromMutableSeg32:
            case ByteCodeOp::GetFromMutableSeg64:
                setNop(context, ip + 2);
                break;
            default:
                break;
        }
    }

    switch (ip->op)
    {
        case ByteCodeOp::ClearMaskU32:
            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[2].op == ByteCodeOp::JumpIfNotEqual32 &&
                ip[0].b.u32 == 0xFF &&
                ip[1].b.u32 == 0xFF &&
                ip[2].a.u32 == ip[0].a.u32 &&
                ip[2].c.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                setNop(context, ip + 1);
                SET_OP(ip + 2, ByteCodeOp::JumpIfNotEqual8);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[2].op == ByteCodeOp::JumpIfNotEqual32 &&
                ip[0].b.u32 == 0xFFFF &&
                ip[1].b.u32 == 0xFFFF &&
                ip[2].a.u32 == ip[0].a.u32 &&
                ip[2].c.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                setNop(context, ip + 1);
                SET_OP(ip + 2, ByteCodeOp::JumpIfNotEqual16);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[2].op == ByteCodeOp::JumpIfEqual32 &&
                ip[0].b.u32 == 0xFF &&
                ip[1].b.u32 == 0xFF &&
                ip[2].a.u32 == ip[0].a.u32 &&
                ip[2].c.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                setNop(context, ip + 1);
                SET_OP(ip + 2, ByteCodeOp::JumpIfEqual8);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearMaskU32 &&
                ip[2].op == ByteCodeOp::JumpIfEqual32 &&
                ip[0].b.u32 == 0xFFFF &&
                ip[1].b.u32 == 0xFFFF &&
                ip[2].a.u32 == ip[0].a.u32 &&
                ip[2].c.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_START_STMT) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                setNop(context, ip);
                setNop(context, ip + 1);
                SET_OP(ip + 2, ByteCodeOp::JumpIfEqual16);
                break;
            }
            break;

        case ByteCodeOp::IncrementRA64:
            if (ip[1].op == ByteCodeOp::JumpIfEqual64 &&
                !ip[0].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip[0].a.u32 == ip[1].a.u32)
            {
                setNop(context, ip);
                SET_OP(ip + 1, ByteCodeOp::IncJumpIfEqual64);
                break;
            }
            break;

        // NegBool followed by jump bool
        case ByteCodeOp::NegBool:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip[0].a.u32 == ip[1].a.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfTrue);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                ip[0].a.u32 == ip[1].a.u32)
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfFalse);
                ip[1].a.u32 = ip->b.u32;
                break;
            }
            break;

        // Cast bool followed by a jump. Change the jump
        case ByteCodeOp::CastBool8:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            break;

        case ByteCodeOp::CastBool16:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            break;

        case ByteCodeOp::CastBool32:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            break;

        case ByteCodeOp::CastBool64:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip->a.u32 == ip[1].a.u32 &&
                !ip[0].hasFlag(BCI_IMM_B) &&
                !ip[1].hasFlag(BCI_IMM_A) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
                ip[1].a.u32 = ip->b.u32;
                break;
            }

            break;

        // Mix compare and jump
        case ByteCodeOp::CompareOpEqual8:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqual8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpEqual16:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqual16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpEqual32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqual32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpEqual64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqual64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpEqualF32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqualF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqualF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpEqualF64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqualF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqualF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        // Mix compare and jump
        case ByteCodeOp::CompareOpNotEqual8:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqual8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpNotEqual16:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqual16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpNotEqual32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqual32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpNotEqual64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqual64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqual64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpNotEqualF32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqualF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqualF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpNotEqualF64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfEqualF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfNotEqualF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerS8:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerS16:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerS32:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerS64:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerU8:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerU16:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerU32:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerU64:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerF32:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerF64:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqU8:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqU16:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqU32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqU64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerU64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqU64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqS8:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqS16:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqS32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqS64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerS64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqS64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqF32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterEqF64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterEqF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterU8:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterU16:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterU32:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterU64:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterS8:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterS16:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterS32:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterS64:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterF32:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpGreaterF64:
            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqU8:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqU16:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqU32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqU64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterU64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqU64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqS8:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS8);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqS16:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS16);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqS32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqS64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterS64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqS64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqF32:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqF32);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;

        case ByteCodeOp::CompareOpLowerEqF64:
            if (ip[1].op == ByteCodeOp::JumpIfFalse &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfGreaterF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            if (ip[1].op == ByteCodeOp::JumpIfTrue &&
                ip[1].a.u32 == ip->c.u32 &&
                ip->a.u32 != ip->c.u32 &&
                (ip->b.u32 != ip->c.u32 || ip->hasFlag(BCI_IMM_B)) &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::JumpIfLowerEqF64);
                ip[1].a.u64 = ip->a.u64;
                ip[1].c.u64 = ip->b.u64;
                ip[1].inheritFlag(ip, BCI_IMM_A);
                ip[1].addFlag(ip->hasFlag(BCI_IMM_B) ? BCI_IMM_C : 0);
                break;
            }

            break;
    }

    // Compare to == 0
    if (ip->hasFlag(BCI_IMM_B) && ip->a.u32 != ip->c.u32)
    {
        switch (ip->op)
        {
            case ByteCodeOp::CompareOpEqual8:
                if (ip->b.u8 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfFalse &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u8 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfTrue &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u8 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfZero8 &&
                    ip[1].a.u32 == ip->c.u32)
                {
                    setNop(context, ip);
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                break;

            case ByteCodeOp::CompareOpEqual16:
                if (ip->b.u16 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfFalse &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u16 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfTrue &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u8 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfZero16 &&
                    ip[1].a.u32 == ip->c.u32)
                {
                    setNop(context, ip);
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                break;

            case ByteCodeOp::CompareOpEqual32:
                if (ip->b.u32 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfFalse &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u32 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfTrue &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u8 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfZero32 &&
                    ip[1].a.u32 == ip->c.u32)
                {
                    setNop(context, ip);
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                break;

            case ByteCodeOp::CompareOpEqual64:
                if (ip->b.u64 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfFalse &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u64 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfTrue &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u64 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfNotZero8 &&
                    ip[1].a.u32 == ip->c.u32)
                {
                    setNop(context, ip);
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
                    ip[1].a.u32 = ip->a.u32;
                }

                break;

            // Compare to != 0
            case ByteCodeOp::CompareOpNotEqual8:
                if (ip->b.u8 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfFalse &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero8);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u8 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfTrue &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero8);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                break;

            case ByteCodeOp::CompareOpNotEqual16:
                if (ip->b.u16 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfFalse &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero16);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u16 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfTrue &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero16);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                break;

            case ByteCodeOp::CompareOpNotEqual32:
                if (ip->b.u32 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfFalse &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero32);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u32 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfTrue &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero32);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                break;

            case ByteCodeOp::CompareOpNotEqual64:
                if (ip->b.u64 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfFalse &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfZero64);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                if (ip->b.u64 == 0 &&
                    ip[1].op == ByteCodeOp::JumpIfTrue &&
                    ip[1].a.u32 == ip->c.u32 &&
                    !ip[1].hasFlag(BCI_START_STMT))
                {
                    SET_OP(ip + 1, ByteCodeOp::JumpIfNotZero64);
                    ip[1].a.u32 = ip->a.u32;
                    break;
                }

                break;

            default:
                break;
        }
    }
}

void ByteCodeOptimizer::reduceLateStack(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::SetAtStackPointer8:
        case ByteCodeOp::SetAtStackPointer16:
        case ByteCodeOp::SetAtStackPointer32:
        case ByteCodeOp::SetAtStackPointer64:
        case ByteCodeOp::SetAtStackPointer8x2:
        case ByteCodeOp::SetAtStackPointer16x2:
        case ByteCodeOp::SetAtStackPointer32x2:
        case ByteCodeOp::SetAtStackPointer64x2:
        case ByteCodeOp::SetZeroStack8:
        case ByteCodeOp::SetZeroStack16:
        case ByteCodeOp::SetZeroStack32:
        case ByteCodeOp::SetZeroStack64:
        case ByteCodeOp::SetZeroStackX:
        case ByteCodeOp::CopyStack8:
        case ByteCodeOp::CopyStack16:
        case ByteCodeOp::CopyStack32:
        case ByteCodeOp::CopyStack64:
            if (ip[1].op == ByteCodeOp::CopyRAtoRRRet || ip[1].op == ByteCodeOp::Ret)
            {
                setNop(context, ip);
                break;
            }
            break;
    }
}

void ByteCodeOptimizer::reduceStackOp(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->op == ByteCodeOp::MakeStackPointer &&
        ip[0].a.u32 == ip[1].a.u32)
    {
        switch (ip[1].op)
        {
            case ByteCodeOp::AffectOpPlusEqS8_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS8_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqS16_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS16_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqS32_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS32_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqS64_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqS64_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU8_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU8_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU16_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU16_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU32_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU32_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqU64_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqU64_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqF32_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                ip[1].c.u32 = 0;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqF32_SSafe);
                break;
            case ByteCodeOp::AffectOpPlusEqF64_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                ip[1].c.u32 = 0;
                SET_OP(ip + 1, ByteCodeOp::AffectOpPlusEqF64_SSafe);
                break;

            case ByteCodeOp::AffectOpMinusEqS8_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS8_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqS16_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS16_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqS32_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS32_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqS64_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqS64_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU8_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU8_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU16_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU16_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU32_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU32_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqU64_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqU64_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqF32_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                ip[1].c.u32 = 0;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqF32_SSafe);
                break;
            case ByteCodeOp::AffectOpMinusEqF64_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                ip[1].c.u32 = 0;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMinusEqF64_SSafe);
                break;

            case ByteCodeOp::AffectOpMulEqS8_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS8_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqS16_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS16_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqS32_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS32_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqS64_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqS64_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU8_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU8_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU16_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU16_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU32_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU32_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqU64_Safe:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqU64_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqF32_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqF32_SSafe);
                break;
            case ByteCodeOp::AffectOpMulEqF64_Safe:
                ip[1].a.u32 = ip->b.u32 + ip[1].c.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpMulEqF64_SSafe);
                break;

            case ByteCodeOp::AffectOpDivEqS8:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqS8_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqS16:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqS16_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqS32:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqS32_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqS64:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqS64_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqU8:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqU8_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqU16:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqU16_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqU32:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqU32_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqU64:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqU64_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqF32:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqF32_SSafe);
                break;
            case ByteCodeOp::AffectOpDivEqF64:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpDivEqF64_SSafe);
                break;

            case ByteCodeOp::AffectOpModuloEqS8:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpModuloEqS8_SSafe);
                break;
            case ByteCodeOp::AffectOpModuloEqS16:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpModuloEqS16_SSafe);
                break;
            case ByteCodeOp::AffectOpModuloEqS32:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpModuloEqS32_SSafe);
                break;
            case ByteCodeOp::AffectOpModuloEqS64:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpModuloEqS64_SSafe);
                break;
            case ByteCodeOp::AffectOpModuloEqU8:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpModuloEqU8_SSafe);
                break;
            case ByteCodeOp::AffectOpModuloEqU16:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpModuloEqU16_SSafe);
                break;
            case ByteCodeOp::AffectOpModuloEqU32:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpModuloEqU32_SSafe);
                break;
            case ByteCodeOp::AffectOpModuloEqU64:
                ip[1].a.u32 = ip->b.u32;
                SET_OP(ip + 1, ByteCodeOp::AffectOpModuloEqU64_SSafe);
                break;
            default:
                break;
        }
    }
}

void ByteCodeOptimizer::reduceForceSafe(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip->dynFlags.has(BCID_SAFETY_OF))
        return;

    switch (ip->op)
    {
        case ByteCodeOp::AffectOpPlusEqS8:
            SET_OP(ip, ByteCodeOp::AffectOpPlusEqS8_Safe);
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            SET_OP(ip, ByteCodeOp::AffectOpPlusEqS16_Safe);
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            SET_OP(ip, ByteCodeOp::AffectOpPlusEqS32_Safe);
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            SET_OP(ip, ByteCodeOp::AffectOpPlusEqS64_Safe);
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            SET_OP(ip, ByteCodeOp::AffectOpPlusEqU8_Safe);
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            SET_OP(ip, ByteCodeOp::AffectOpPlusEqU16_Safe);
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            SET_OP(ip, ByteCodeOp::AffectOpPlusEqU32_Safe);
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            SET_OP(ip, ByteCodeOp::AffectOpPlusEqU64_Safe);
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            SET_OP(ip, ByteCodeOp::AffectOpMinusEqS8_Safe);
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            SET_OP(ip, ByteCodeOp::AffectOpMinusEqU8_Safe);
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            SET_OP(ip, ByteCodeOp::AffectOpMinusEqS16_Safe);
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            SET_OP(ip, ByteCodeOp::AffectOpMinusEqU16_Safe);
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            SET_OP(ip, ByteCodeOp::AffectOpMinusEqS32_Safe);
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            SET_OP(ip, ByteCodeOp::AffectOpMinusEqU32_Safe);
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            SET_OP(ip, ByteCodeOp::AffectOpMinusEqS64_Safe);
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            SET_OP(ip, ByteCodeOp::AffectOpMinusEqU64_Safe);
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            SET_OP(ip, ByteCodeOp::AffectOpMulEqS8_Safe);
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            SET_OP(ip, ByteCodeOp::AffectOpMulEqU8_Safe);
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            SET_OP(ip, ByteCodeOp::AffectOpMulEqS16_Safe);
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            SET_OP(ip, ByteCodeOp::AffectOpMulEqU16_Safe);
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            SET_OP(ip, ByteCodeOp::AffectOpMulEqS32_Safe);
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            SET_OP(ip, ByteCodeOp::AffectOpMulEqU32_Safe);
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            SET_OP(ip, ByteCodeOp::AffectOpMulEqS64_Safe);
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            SET_OP(ip, ByteCodeOp::AffectOpMulEqU64_Safe);
            break;

        case ByteCodeOp::BinOpPlusS8:
            SET_OP(ip, ByteCodeOp::BinOpPlusS8_Safe);
            break;
        case ByteCodeOp::BinOpPlusS16:
            SET_OP(ip, ByteCodeOp::BinOpPlusS16_Safe);
            break;
        case ByteCodeOp::BinOpPlusS32:
            SET_OP(ip, ByteCodeOp::BinOpPlusS32_Safe);
            break;
        case ByteCodeOp::BinOpPlusS64:
            SET_OP(ip, ByteCodeOp::BinOpPlusS64_Safe);
            break;
        case ByteCodeOp::BinOpPlusU8:
            SET_OP(ip, ByteCodeOp::BinOpPlusU8_Safe);
            break;
        case ByteCodeOp::BinOpPlusU16:
            SET_OP(ip, ByteCodeOp::BinOpPlusU16_Safe);
            break;
        case ByteCodeOp::BinOpPlusU32:
            SET_OP(ip, ByteCodeOp::BinOpPlusU32_Safe);
            break;
        case ByteCodeOp::BinOpPlusU64:
            SET_OP(ip, ByteCodeOp::BinOpPlusU64_Safe);
            break;

        case ByteCodeOp::BinOpMinusS8:
            SET_OP(ip, ByteCodeOp::BinOpMinusS8_Safe);
            break;
        case ByteCodeOp::BinOpMinusS16:
            SET_OP(ip, ByteCodeOp::BinOpMinusS16_Safe);
            break;
        case ByteCodeOp::BinOpMinusS32:
            SET_OP(ip, ByteCodeOp::BinOpMinusS32_Safe);
            break;
        case ByteCodeOp::BinOpMinusS64:
            SET_OP(ip, ByteCodeOp::BinOpMinusS64_Safe);
            break;
        case ByteCodeOp::BinOpMinusU8:
            SET_OP(ip, ByteCodeOp::BinOpMinusU8_Safe);
            break;
        case ByteCodeOp::BinOpMinusU16:
            SET_OP(ip, ByteCodeOp::BinOpMinusU16_Safe);
            break;
        case ByteCodeOp::BinOpMinusU32:
            SET_OP(ip, ByteCodeOp::BinOpMinusU32_Safe);
            break;
        case ByteCodeOp::BinOpMinusU64:
            SET_OP(ip, ByteCodeOp::BinOpMinusU64_Safe);
            break;

        case ByteCodeOp::BinOpMulS8:
            SET_OP(ip, ByteCodeOp::BinOpMulS8_Safe);
            break;
        case ByteCodeOp::BinOpMulS16:
            SET_OP(ip, ByteCodeOp::BinOpMulS16_Safe);
            break;
        case ByteCodeOp::BinOpMulS32:
            SET_OP(ip, ByteCodeOp::BinOpMulS32_Safe);
            break;
        case ByteCodeOp::BinOpMulS64:
            SET_OP(ip, ByteCodeOp::BinOpMulS64_Safe);
            break;
        case ByteCodeOp::BinOpMulU8:
            SET_OP(ip, ByteCodeOp::BinOpMulU8_Safe);
            break;
        case ByteCodeOp::BinOpMulU16:
            SET_OP(ip, ByteCodeOp::BinOpMulU16_Safe);
            break;
        case ByteCodeOp::BinOpMulU32:
            SET_OP(ip, ByteCodeOp::BinOpMulU32_Safe);
            break;
        case ByteCodeOp::BinOpMulU64:
            SET_OP(ip, ByteCodeOp::BinOpMulU64_Safe);
            break;
    }
}

// Instruction
// Jump
// Same Instruction
void ByteCodeOptimizer::reduceDupInstr(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (ip[0].op == ByteCodeOp::Nop)
        return;
    if (ip[1].hasFlag(BCI_START_STMT))
        return;

    auto isParam = false;
    switch (ip->op)
    {
        case ByteCodeOp::GetParam8:
        case ByteCodeOp::GetParam16:
        case ByteCodeOp::GetParam32:
        case ByteCodeOp::GetParam64:
        case ByteCodeOp::GetIncParam64:
            isParam = true;
            break;

        case ByteCodeOp::GetFromStack8:
        case ByteCodeOp::GetFromStack16:
        case ByteCodeOp::GetFromStack32:
        case ByteCodeOp::GetFromStack64:
        case ByteCodeOp::GetParam64DeRef8:
        case ByteCodeOp::GetParam64DeRef16:
        case ByteCodeOp::GetParam64DeRef32:
        case ByteCodeOp::GetParam64DeRef64:
        case ByteCodeOp::GetIncFromStack64:
        case ByteCodeOp::GetIncFromStack64DeRef8:
        case ByteCodeOp::GetIncFromStack64DeRef16:
        case ByteCodeOp::GetIncFromStack64DeRef32:
        case ByteCodeOp::GetIncFromStack64DeRef64:
        case ByteCodeOp::GetIncParam64DeRef8:
        case ByteCodeOp::GetIncParam64DeRef16:
        case ByteCodeOp::GetIncParam64DeRef32:
        case ByteCodeOp::GetIncParam64DeRef64:
        case ByteCodeOp::IntrinsicIsConstExprSI:
            break;
        default:
            return;
    }

    auto           ipn  = ip + 1;
    constexpr bool exit = false;
    while (!exit)
    {
        if (ipn->hasFlag(BCI_START_STMT))
            return;
        if (ipn->op == ip->op)
            break;

        if (ByteCode::hasWriteRefToRegA(ipn, ip[0].a.u32))
            return;
        if (ByteCode::hasWriteRefToRegB(ipn, ip[0].a.u32))
            return;
        if (ByteCode::hasWriteRefToRegC(ipn, ip[0].a.u32))
            return;
        if (ByteCode::hasWriteRefToRegD(ipn, ip[0].a.u32))
            return;
        if (ipn->op == ByteCodeOp::End)
            return;

        if (!isParam &&
            !ByteCode::isJump(ipn) &&
            !ipn->hasOpFlag(OPF_REG_ONLY) &&
            ipn->op != ByteCodeOp::Nop)
            return;

        ipn++;
    }

    if (ip[0].flags.maskInvert(BCI_START_STMT) != ipn->flags.maskInvert(BCI_START_STMT))
        return;
    if (ByteCode::hasSomethingInA(ip) && ip[0].a.u64 != ipn->a.u64)
        return;
    if (ByteCode::hasSomethingInB(ip) && ip[0].b.u64 != ipn->b.u64)
        return;
    if (ByteCode::hasSomethingInB(ip) && ip[0].c.u64 != ipn->c.u64)
        return;
    if (ByteCode::hasSomethingInB(ip) && ip[0].d.u64 != ipn->d.u64)
        return;

    setNop(context, ipn);
}

void ByteCodeOptimizer::reduceCopy(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (!(context->contextBcFlags & OCF_HAS_COPY_RBRA))
        return;

    if (ip->op != ByteCodeOp::CopyRBtoRA8 &&
        ip->op != ByteCodeOp::CopyRBtoRA16 &&
        ip->op != ByteCodeOp::CopyRBtoRA32 &&
        ip->op != ByteCodeOp::CopyRBtoRA64)
        return;

    const auto ipn = ip + 1;

    if (ipn->hasFlag(BCI_START_STMT))
        return;

    if (ByteCode::isPushParam(ipn) ||
        ByteCode::isCall(ipn))
        return;

    const auto fl0 = ByteCode::opFlags(ip->op);
    const auto fl1 = ByteCode::opFlags(ipn->op);

    if (fl0.has(OPF_8) && !fl1.has(OPF_8))
        return;
    if (fl0.has(OPF_16) && !fl1.has(OPF_8 | OPF_16))
        return;
    if (fl0.has(OPF_32) && !fl1.has(OPF_8 | OPF_16 | OPF_32))
        return;
    if (fl0.has(OPF_64) && !fl1.has(OPF_8 | OPF_16 | OPF_32 | OPF_64))
        return;

    if (ByteCode::hasReadRefToRegA(ipn, ip->a.u32) &&
        !ByteCode::hasWriteRefToRegA(ipn, ip->a.u32))
    {
        ipn->a.u32 = ip->b.u32;
        context->setDirtyPass();
    }

    if (ByteCode::hasReadRefToRegB(ipn, ip->a.u32) &&
        !ByteCode::hasWriteRefToRegB(ipn, ip->a.u32))
    {
        ipn->b.u32 = ip->b.u32;
        context->setDirtyPass();
    }

    if (ByteCode::hasReadRefToRegC(ipn, ip->a.u32) &&
        !ByteCode::hasWriteRefToRegC(ipn, ip->a.u32))
    {
        ipn->c.u32 = ip->b.u32;
        context->setDirtyPass();
    }

    if (ByteCode::hasReadRefToRegD(ipn, ip->a.u32) &&
        !ByteCode::hasWriteRefToRegD(ipn, ip->a.u32))
    {
        ipn->d.u32 = ip->b.u32;
        context->setDirtyPass();
    }
}

void ByteCodeOptimizer::reduceCall(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::PushRAParam:
            if (ip[1].op == ByteCodeOp::ForeignCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ForeignCallPopParam);
                ip[1].c.u32 = 0;
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::LocalCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LocalCallPopParam);
                ip[1].c.u32 = 0;
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::LambdaCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LambdaCallPopParam);
                ip[1].c.u32 = 0;
                ip[1].d.u32 = ip[0].a.u32;
                setNop(context, ip);
                break;
            }
            break;

        case ByteCodeOp::PushRAParam2:

            if (ip[1].op == ByteCodeOp::ForeignCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::ForeignCallPop0Param2);
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].d.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }

            if (ip[1].op == ByteCodeOp::LocalCall &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip + 1, ByteCodeOp::LocalCallPop0Param2);
                ip[1].c.u32 = ip[0].a.u32;
                ip[1].d.u32 = ip[0].b.u32;
                setNop(context, ip);
                break;
            }

            break;
    }
}

namespace
{
    void reduceStackJumpsOp(ByteCodeOptContext* context, ByteCodeInstruction* ip, ByteCodeOp op0, ByteCodeOp op1)
    {
        bool isDeRef = false;
        bool isZero  = false;

        switch (op0)
        {
            case ByteCodeOp::JumpIfFalse:
            case ByteCodeOp::JumpIfTrue:
            case ByteCodeOp::JumpIfZero8:
            case ByteCodeOp::JumpIfNotZero8:
            case ByteCodeOp::JumpIfZero16:
            case ByteCodeOp::JumpIfNotZero16:
            case ByteCodeOp::JumpIfZero32:
            case ByteCodeOp::JumpIfNotZero32:
            case ByteCodeOp::JumpIfZero64:
            case ByteCodeOp::JumpIfNotZero64:
                isZero = true;
                break;
        }
        switch (op1)
        {
            case ByteCodeOp::JumpIfDeRefEqual8:
            case ByteCodeOp::JumpIfDeRefEqual16:
            case ByteCodeOp::JumpIfDeRefEqual32:
            case ByteCodeOp::JumpIfDeRefEqual64:
            case ByteCodeOp::JumpIfDeRefNotEqual8:
            case ByteCodeOp::JumpIfDeRefNotEqual16:
            case ByteCodeOp::JumpIfDeRefNotEqual32:
            case ByteCodeOp::JumpIfDeRefNotEqual64:
                isDeRef = true;
                break;
        }

        if (isDeRef && (ip[0].a.u32 == ip[0].b.u32))
            return;

        for (int i = 1; i <= 3; i++)
        {
            if (ip[i].op != op0)
                continue;
            if (ip[i].hasFlag(BCI_START_STMT))
                continue;

            // If the second instruction has a deref or a stack access on the second argument, then pass it to the first
            // argument to be able to do the optim in another pass
            if (!isZero &&
                ByteCode::hasOpFlag(op0, OPF_READ_C) &&
                ip[i].c.u32 == ip[0].a.u32 &&
                !ip[i].hasFlag(BCI_IMM_A | BCI_IMM_C) &&
                !ip[i].hasDynFlag(BCID_SWAP))
            {
                ip[i].addDynFlag(BCID_SWAP);
                std::swap(ip[i].a, ip[i].c);
                context->setDirtyPass();
                return;
            }

            if (ip[i].a.u32 == ip[0].a.u32 &&
                !ip[i].hasFlag(BCI_IMM_A))
            {
                for (int j = 1; j < i; j++)
                {
                    if (ip[j].hasFlag(BCI_START_STMT) ||
                        ByteCode::hasWriteRefToReg(ip + j, ip->a.u32) ||
                        (isDeRef && ByteCode::hasWriteRefToReg(ip + j, ip->b.u32)))
                        return;

                    if (!ip[j].hasOpFlag(OPF_REG_ONLY | OPF_REG_READ))
                        return;
                }

                SET_OP(ip + i, op1);
                ip[i].a.u64 = ip[0].b.u64;

                // Store the deref offset in D
                if (isDeRef)
                {
                    ip[i].d.u64 = ip[0].c.u64;
                }

                // If we are testing against zero, put an immediate 0 value in C
                if (isZero)
                {
                    ip[i].addFlag(BCI_IMM_C);
                    ip[i].c.u64 = 0;
                }
                break;
            }
        }
    }
}

void ByteCodeOptimizer::reduceStackJumps(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::GetFromStack8:
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfFalse, ByteCodeOp::JumpIfStackEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfTrue, ByteCodeOp::JumpIfStackNotEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero8, ByteCodeOp::JumpIfStackEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero8, ByteCodeOp::JumpIfStackNotEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual8, ByteCodeOp::JumpIfStackEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual8, ByteCodeOp::JumpIfStackNotEqual8);
            break;

        case ByteCodeOp::GetFromStack16:
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero16, ByteCodeOp::JumpIfStackEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero16, ByteCodeOp::JumpIfStackNotEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual16, ByteCodeOp::JumpIfStackEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual16, ByteCodeOp::JumpIfStackNotEqual16);
            break;

        case ByteCodeOp::GetFromStack32:
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero32, ByteCodeOp::JumpIfStackEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero32, ByteCodeOp::JumpIfStackNotEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual32, ByteCodeOp::JumpIfStackEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual32, ByteCodeOp::JumpIfStackNotEqual32);
            break;

        case ByteCodeOp::GetFromStack64:
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero64, ByteCodeOp::JumpIfStackEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero64, ByteCodeOp::JumpIfStackNotEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual64, ByteCodeOp::JumpIfStackEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual64, ByteCodeOp::JumpIfStackNotEqual64);
            break;

        case ByteCodeOp::DeRef8:
            if (ip->c.u64 >= 0xFFFFFFFF)
                break;
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfFalse, ByteCodeOp::JumpIfDeRefEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfTrue, ByteCodeOp::JumpIfDeRefNotEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero8, ByteCodeOp::JumpIfDeRefEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero8, ByteCodeOp::JumpIfDeRefNotEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual8, ByteCodeOp::JumpIfDeRefEqual8);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual8, ByteCodeOp::JumpIfDeRefNotEqual8);
            break;

        case ByteCodeOp::DeRef16:
            if (ip->c.u64 >= 0xFFFFFFFF)
                break;
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero16, ByteCodeOp::JumpIfDeRefEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero16, ByteCodeOp::JumpIfDeRefNotEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual16, ByteCodeOp::JumpIfDeRefEqual16);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual16, ByteCodeOp::JumpIfDeRefNotEqual16);
            break;

        case ByteCodeOp::DeRef32:
            if (ip->c.u64 >= 0xFFFFFFFF)
                break;
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero32, ByteCodeOp::JumpIfDeRefEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero32, ByteCodeOp::JumpIfDeRefNotEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual32, ByteCodeOp::JumpIfDeRefEqual32);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual32, ByteCodeOp::JumpIfDeRefNotEqual32);
            break;

        case ByteCodeOp::DeRef64:
            if (ip->c.u64 >= 0xFFFFFFFF)
                break;
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfZero64, ByteCodeOp::JumpIfDeRefEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotZero64, ByteCodeOp::JumpIfDeRefNotEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfEqual64, ByteCodeOp::JumpIfDeRefEqual64);
            reduceStackJumpsOp(context, ip, ByteCodeOp::JumpIfNotEqual64, ByteCodeOp::JumpIfDeRefNotEqual64);
            break;
    }
}

void ByteCodeOptimizer::reduceX2(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    switch (ip->op)
    {
        case ByteCodeOp::GetParam64:
            if (ip[1].op == ByteCodeOp::GetParam64 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetParam64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::CopyRBtoRA64:
            if (ip[1].op == ByteCodeOp::CopyRBtoRA64 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::CopyRBtoRA64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::ClearRA:
            if (ip[1].op == ByteCodeOp::ClearRA &&
                ip[2].op == ByteCodeOp::ClearRA &&
                ip[3].op == ByteCodeOp::ClearRA &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT) &&
                !ip[3].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u32 = ip[1].a.u32;
                ip[0].c.u32 = ip[2].a.u32;
                ip[0].d.u32 = ip[3].a.u32;
                SET_OP(ip, ByteCodeOp::ClearRAx4);
                setNop(context, ip + 1);
                setNop(context, ip + 2);
                setNop(context, ip + 3);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearRA &&
                ip[2].op == ByteCodeOp::ClearRA &&
                !ip[1].hasFlag(BCI_START_STMT) &&
                !ip[2].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u32 = ip[1].a.u32;
                ip[0].c.u32 = ip[2].a.u32;
                SET_OP(ip, ByteCodeOp::ClearRAx3);
                setNop(context, ip + 1);
                setNop(context, ip + 2);
                break;
            }

            if (ip[1].op == ByteCodeOp::ClearRA &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].b.u32 = ip[1].a.u32;
                SET_OP(ip, ByteCodeOp::ClearRAx2);
                setNop(context, ip + 1);
                break;
            }

            break;

        case ByteCodeOp::MakeStackPointer:
            if (ip[1].op == ByteCodeOp::MakeStackPointer &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                ip[0].c.u32 = ip[1].a.u32;
                ip[0].d.u32 = ip[1].b.u32;
                SET_OP(ip, ByteCodeOp::MakeStackPointerx2);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer8:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer8 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer8x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                if (ip[1].hasFlag(BCI_IMM_B))
                    ip[0].addFlag(BCI_IMM_D);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer16:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer16x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                if (ip[1].hasFlag(BCI_IMM_B))
                    ip[0].addFlag(BCI_IMM_D);
                setNop(context, ip + 1);
            }
            break;

        case ByteCodeOp::SetAtStackPointer32:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer32x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                if (ip[1].hasFlag(BCI_IMM_B))
                    ip[0].addFlag(BCI_IMM_D);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::SetAtStackPointer64:
            if (ip[1].op == ByteCodeOp::SetAtStackPointer64 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::SetAtStackPointer64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                if (ip[1].hasFlag(BCI_IMM_B))
                    ip[0].addFlag(BCI_IMM_D);
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack8:
            if (ip[1].op == ByteCodeOp::GetFromStack8 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack8x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack16:
            if (ip[1].op == ByteCodeOp::GetFromStack16 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack16x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack32:
            if (ip[1].op == ByteCodeOp::GetFromStack32 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack32x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;

        case ByteCodeOp::GetFromStack64:
            if (ip[1].op == ByteCodeOp::GetFromStack64 &&
                !ip[1].hasFlag(BCI_START_STMT))
            {
                SET_OP(ip, ByteCodeOp::GetFromStack64x2);
                ip[0].c.u64 = ip[1].a.u64;
                ip[0].d.u64 = ip[1].b.u64;
                setNop(context, ip + 1);
                break;
            }
            break;
    }
}

void ByteCodeOptimizer::reduceInvCopy(ByteCodeOptContext* context, ByteCodeInstruction* ip)
{
    if (!ip->dynFlags.has(BCID_SWAP) &&
        !ByteCode::hasWriteRegInA(ip) &&
        !ByteCode::hasWriteRegInB(ip) &&
        ByteCode::hasWriteRegInC(ip) &&
        !ByteCode::hasWriteRegInD(ip) &&
        ip[0].c.u32 == ip[1].b.u32 &&
        !ip[1].hasFlag(BCI_START_STMT))
    {
        if (ip[1].op == ByteCodeOp::CopyRBtoRA32 ||
            ip[1].op == ByteCodeOp::CopyRBtoRA64)
        {
            ip->dynFlags.add(BCID_SWAP);
            ip->c.u32 = ip[1].a.u32;
            std::swap(ip[1].a, ip[1].b);
            context->setDirtyPass();
        }
    }
}
